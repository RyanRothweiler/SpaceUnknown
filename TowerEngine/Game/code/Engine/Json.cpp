#pragma once

#ifndef JsonCPP
#define JsonCPP

namespace json {

	void MovePastWhitespace(tokenizer *Tokenizer)
	{
		while (*Tokenizer->Position == ' ' ||
		        *Tokenizer->Position == '\n' ||
		        *Tokenizer->Position == '\r' ||
		        *Tokenizer->Position == '\t') {
			Tokenizer->Position++;
		}
	}

	token_type GetNextToken(tokenizer *Tokenizer)
	{
		MovePastWhitespace(Tokenizer);

		token_type TokenType = token_type::end_of_file;
		switch (*Tokenizer->Position) {
			case ('['): { TokenType = token_type::open_bracket; 	} break;
			case (']'): { TokenType = token_type::close_bracket;	} break;
			case ('{'): { TokenType = token_type::open_curly; 		} break;
			case ('}'): { TokenType = token_type::close_curly; 		} break;
			case ('"'): { TokenType = token_type::quote; 			} break;
			case (','): { TokenType = token_type::comma; 			} break;
			case (':'): { TokenType = token_type::colon;			} break;
			case ('*'): { TokenType = token_type::end_of_file; 		} break;
			default: { TokenType = token_type::identifier; 			} break;
		}
		Tokenizer->Position++;
		return (TokenType);
	}

	string GrabUntilToken(tokenizer* Tokenizer, token_type EndToken)
	{
		char* KeyStart = Tokenizer->Position;
		while (GetNextToken(Tokenizer) != EndToken) { }
		char* KeyEnd = Tokenizer->Position;
		KeyEnd -= 1;

		string Identifier = {};
		while (KeyStart < KeyEnd) {
			char NextCharacter = KeyStart[0];
			Identifier = Identifier + NextCharacter;

			KeyStart++;
		}

		return (Identifier);
	}

	json_data GetJson(memory_arena* Memory)
	{
		json_data JsonData = {};
		JsonData.PairsCount = 0;
		JsonData.Pairs = (json_pair *)ArenaAllocate(Memory, sizeof(json_pair) * Max_Pairs_Count);

		return JsonData;
	}

	json_data LoadJsonData(tokenizer* Tokenizer, bool32 IsArray, memory_arena* Memory)
	{
		json_data JsonData = GetJson(Memory);
		token_type NextToken = GetNextToken(Tokenizer);

		while (true) {
			if (NextToken == token_type::open_curly || NextToken == token_type::open_bracket) {

			} else if (NextToken == token_type::close_curly || NextToken == token_type::close_bracket) {
				break;
			} else if (NextToken == token_type::quote) {

				if (!IsArray) {
					JsonData.Pairs[JsonData.PairsCount].Key = GrabUntilToken(Tokenizer, token_type::quote);
					Tokenizer->Position += 1;
				} else {

					// Move back to catch the previous quote
					Tokenizer->Position -= 1;
				}

				token_type Next = GetNextToken(Tokenizer);

				//GrabData(Tokenizer, &JsonData.Pairs[JsonData.PairsCount], Memory);

				if (Next == token_type::quote) {
					// data itself
					
					JsonData.Pairs[JsonData.PairsCount].Data = GrabUntilToken(Tokenizer, token_type::quote);

				} else if (Next == token_type::open_bracket)  {
					// an array

					JsonData.Pairs[JsonData.PairsCount].Child[0] = (json_data*)ArenaAllocate(Memory, sizeof(json_data));

					json_data Child = LoadJsonData(Tokenizer, true, Memory);
					MemoryCopy((char*)JsonData.Pairs[JsonData.PairsCount].Child[0], (char*)(&Child), sizeof(json_data));

				} else if (Next == token_type::open_curly)  {
					// an object

					JsonData.Pairs[JsonData.PairsCount].Child[0] = (json_data*)ArenaAllocate(Memory, sizeof(json_data));

					json_data Child = LoadJsonData(Tokenizer, false, Memory);
					MemoryCopy((char*)JsonData.Pairs[JsonData.PairsCount].Child[0], (char*)(&Child), sizeof(json_data));

				} else {
					Tokenizer->Position -= 1;
					JsonData.Pairs[JsonData.PairsCount].Data = GrabUntilToken(Tokenizer, token_type::comma);
				}

				// make sure that key isn't alraedy used
				/*
				for (int32 KeyIndex = 0; KeyIndex < JsonData.PairsCount; KeyIndex++) {
					if (JsonData.Pairs[KeyIndex].Key == JsonData.Pairs[JsonData.PairsCount].Key) {
						// Duplicate key detected
						//Assert(0);
						// this could be an error
					}
				}
				*/

				JsonData.PairsCount++;
				Assert(JsonData.PairsCount < Max_Pairs_Count);
			}

			NextToken = GetNextToken(Tokenizer);
		}

		return JsonData;
	}

	json_data LoadFile(string Path, memory_arena * Memory)
	{
		read_file_result Result = PlatformApi.ReadFile(Path.CharArray, Memory);

		if (Result.ContentsSize > 0) {
			tokenizer Tokenizer = {};
			Tokenizer.Position = (char*)Result.Contents;
			Tokenizer.End = Tokenizer.Position + Result.ContentsSize;

			token_type NextToken = GetNextToken(&Tokenizer);
			Assert(NextToken == token_type::open_curly);

			return LoadJsonData(&Tokenizer, false, Memory);
		}

		return json_data{};
	}

	json_data Load(char* Start, int32 Count, memory_arena * Memory) {
		tokenizer Tokenizer = {};
		Tokenizer.Position = Start;
		Tokenizer.End = Tokenizer.Position + Count;

		token_type NextToken = GetNextToken(&Tokenizer);
		Assert(NextToken == token_type::open_curly);

		return LoadJsonData(&Tokenizer, false, Memory);
	}

	void AddKeyPair(string Key, string Data, json_data * JsonData)
	{
		for (int32 PairIndex = 0; PairIndex < JsonData->PairsCount; PairIndex++) {
			if (JsonData->Pairs[PairIndex].Key == Key) {
				JsonData->Pairs[PairIndex].Data = Data;
				return;
			}
		}

		json_pair* NewPair = &JsonData->Pairs[JsonData->PairsCount];
		NewPair->Key = Key;
		NewPair->Data = Data;

		Assert(JsonData->PairsCount < Max_Pairs_Count);
		JsonData->PairsCount++;
	}

	void AddKeyPair(string Name, vector2 Data, json_data * JsonData)
	{
		AddKeyPair(Name + string{"_x"}, Real64ToString(Data.X, 7), JsonData);
		AddKeyPair(Name + string{"_y"}, Real64ToString(Data.Y, 7), JsonData);
	}

	string GenerateJsonLine(string Key, string Data, bool32 AddComma)
	{
		// NOTE this is probably a really inefficient way to do this. Probaby a lot of strings and allocations made here for no reason.
		string MaybeComma = "\r";
		if (AddComma) { MaybeComma = ","; }
		string Final = "\r \"" + Key + "\"" + ":" + "\"" + Data + "\"" + MaybeComma;
		return (Final);
	}

	void SaveToFile(json_data * JsonData, string FileLocation)
	{
		char* FinalString = (char *)ArenaAllocate(GlobalTransMem, sizeof(char) * 4096);
		FinalString[0] = '{';
		int32 FinalIndex = 1;

		for (int32 PairIndex = 0; PairIndex < JsonData->PairsCount; PairIndex++) {
			string NewLine = GenerateJsonLine(JsonData->Pairs[PairIndex].Key, JsonData->Pairs[PairIndex].Data, PairIndex != JsonData->PairsCount - 1);
			int32 NewLineLength = StringLength(NewLine);
			for (int32 CharIndex = 0; CharIndex < NewLineLength; CharIndex++) {
				FinalString[FinalIndex] = NewLine.CharArray[CharIndex];
				FinalIndex++;
			}
		}

		FinalString[FinalIndex] = '}';
		FinalIndex++;

		PlatformApi.WriteFile(FileLocation.CharArray, FinalString, sizeof(char) * FinalIndex);
	}

	json_pair* GetPair(string Key, json_data* JsonData)
	{
		split_info Split = SplitString(Key, '.', false, true, GlobalTransMem);

		for (int32 PairIndex = 0; PairIndex < JsonData->PairsCount; PairIndex++) {
			json_pair* Pair = &JsonData->Pairs[PairIndex];

			if (Pair->Key == Split.Strings[0]) {

				if (Split.StringsCount == 1) {
					return Pair;
				} else {

					string NewPath = Split.Strings[1];
					for (uint32 x = 2; x < Split.StringsCount; x++) {
						NewPath = NewPath + "." + Split.Strings[x];
					}

					return GetPair(NewPath, Pair->Child[0]);
				}
			}
		}

		// That key not found
		return GameNull;
	}

	string GetData(string Key, json_data * JsonData)
	{
		json_pair* Pair = GetPair(Key, JsonData);

		if (Pair != GameNull) {
			return Pair->Data;
		}

		// Did not find that pair
		return (EmptyString);
	}

	string GetString(string Key, json_data * Data)
	{
		return (GetData(Key, Data));
	}

	bool32 GetBool(string Key, json_data* Json)
	{
		string Data = GetData(Key, Json);
		if (Data == "1") return true;

		// Default for no key is false
		return false;
	}

	int32 GetInt32(string Key, json_data * Data)
	{
		string DataInString = GetData(Key, Data);
		int32 IntData = StringToInt32(DataInString);
		return (IntData);
	}

	int64 GetInt64(string Key, json_data * Data)
	{
		string DataInString = GetData(Key, Data);
		int64 IntData = StringToInt64(DataInString);
		return (IntData);
	}

	int32 GetInt32Safe(string Key, json_data* Data, int32 Default)
	{
		string DataInString = GetData(Key, Data);
		if (DataInString == EmptyString) {
			return Default;
		}

		int32 IntData = StringToInt32(DataInString);
		return (IntData);
	}

	real64 GetReal64(string Key, json_data* Data)
	{
		string DataInString = GetData(Key, Data);
		real64 Real = StringToReal64(DataInString);
		return (Real);
	}

	vector2 GetVector2(string Key, json_data* Data)
	{
		vector2 Ret;
		Ret.X = GetReal64(Key + "_x", Data);
		Ret.Y = GetReal64(Key + "_y", Data);
		return Ret;
	}

	void WriteStruct(meta_member * MetaInfo, uint32 MetaInfoCount, void* AccData, string FileLoc)
	{
		json_data JsonData = GetJson(GlobalTransMem);

		for (uint32 index = 0; index < MetaInfoCount; index++) {
			char* Start = (char*)AccData;
			Start = Start + MetaInfo[index].Offset;
			uint32 Data = *((uint32 *)Start);

			AddKeyPair(MetaInfo[index].Name, Data, &JsonData);
		}

		SaveToFile(&JsonData, FileLoc);
	}

	void FillStruct(json_data* JsonData, string KeyParent, meta_member * MetaInfo, uint32 MetaInfoCount, void* DataDest)
	{
		for (uint32 index = 0; index < MetaInfoCount; index++) {

			meta_member* InfoCurr = &MetaInfo[index];

			string Key = KeyParent + InfoCurr->Name;
			json_pair * KeyPair = GetPair(Key, JsonData);
			if (KeyPair != GameNull) {

				string DataInString = KeyPair->Data;

				char* FieldDest = (char*)DataDest;
				FieldDest = FieldDest + InfoCurr->Offset;

				switch (InfoCurr->Type) {
					case meta_member_type::uint32: {
						uint32 D = (uint32)StringToInt32(DataInString);
						memcpy((void*)FieldDest, (void*)&D, sizeof(uint32));
					} break;

					case meta_member_type::uint16: {
						uint16 D = (uint16)StringToInt32(DataInString);
						memcpy((void*)FieldDest, (void*)&D, sizeof(uint16));
					} break;

					case meta_member_type::uint8: {
						uint8 D = (uint8)StringToInt32(DataInString);
						memcpy((void*)FieldDest, (void*)&D, sizeof(uint8));
					} break;

					case meta_member_type::int32: {
						int32 D = StringToInt32(DataInString);
						memcpy((void*)FieldDest, (void*)&D, sizeof(int32));
					} break;

					case meta_member_type::int16: {
						int16 D = (int16)StringToInt32(DataInString);
						memcpy((void*)FieldDest, (void*)&D, sizeof(int16));
					} break;

					case meta_member_type::int8: {
						int8 D = (int8)StringToInt32(DataInString);
						memcpy((void*)FieldDest, (void*)&D, sizeof(int8));
					} break;

					case meta_member_type::real32: {
						real32 D = (real32)StringToReal64(DataInString);
						memcpy((void*)FieldDest, (void*)&D, sizeof(real32));
					} break;

					case meta_member_type::real64: {
						real64 D = (real64)StringToReal64(DataInString);
						memcpy((void*)FieldDest, (void*)&D, sizeof(real64));
					} break;

					case meta_member_type::custom: {
						InfoCurr->JsonFillShim(JsonData, InfoCurr->Name + ".", FieldDest);
					} break;

					INVALID_DEFAULT
				}
			}
		}
	}

	void ReadIntoStruct(char* Path, meta_member * MetaInfo, uint32 MetaInfoCount, void* DataDest)
	{
		json_data JsonData = LoadFile(Path, GlobalTransMem);

		json_pair* TestPair = GetPair("FieldHere[1].Droid", &JsonData);

		int x = 0;

		//FillStruct(&JsonData, "", MetaInfo, MetaInfoCount, DataDest);
	}


	void StructMemberFill(struct_string_return* Dest, meta_member* MetaInfo, void* AccData)
	{
		struct {
			void AddString(struct_string_return* Dest, string Input, meta_member* MetaInfo)
			{
				// Add Data
				*Dest->Curr = '"'; Dest->Curr++;

				uint32 DataStringLength = StringLength(Input);
				memcpy((void*)Dest->Curr, (void*)&Input.CharArray, DataStringLength);
				Dest->Curr += DataStringLength;

				*Dest->Curr = '"'; Dest->Curr++;
				*Dest->Curr = ','; Dest->Curr++;
				*Dest->Curr = '\n'; Dest->Curr++;

				Assert(Dest->Curr < Dest->Limit);
			}
		} Locals;

		// Add Name
		*Dest->Curr = '"'; Dest->Curr++;
		uint32 NameStringLength = StringLength(MetaInfo->Name);
		memcpy((void*)Dest->Curr, (void*)&MetaInfo->Name, NameStringLength);
		Dest->Curr += NameStringLength;
		*Dest->Curr = '"'; Dest->Curr++;

		*Dest->Curr = ':'; Dest->Curr++;

		Assert(Dest->Curr < Dest->Limit);

		bool32 IsArray = MetaInfo->ArrayLength > 0;
		int32 DataCount = 1;

		if (IsArray) {
			*Dest->Curr = '['; Dest->Curr++;
			DataCount = MetaInfo->ArrayLength;
		}

		for (int i = 0; i < DataCount; i++) {

			char* Start = (char*)AccData;
			Start = Start + (MetaInfo->Offset + (i * MetaInfo->Size));

			switch (MetaInfo->Type) {
				case meta_member_type::uint32: {
					uint32 Data = *((uint32 *)Start);
					Locals.AddString(Dest, Data, MetaInfo);
				} break;

				case meta_member_type::uint16: {
					uint16 Data = *((uint16 *)Start);
					Locals.AddString(Dest, Data, MetaInfo);
				} break;

				case meta_member_type::uint8: {
					uint8 Data = *((uint8 *)Start);
					Locals.AddString(Dest, Data, MetaInfo);
				} break;

				case meta_member_type::int32: {
					int32 Data = *((int32 *)Start);
					Locals.AddString(Dest, Data, MetaInfo);
				} break;

				case meta_member_type::int16: {
					int16 Data = *((int16 *)Start);
					Locals.AddString(Dest, Data, MetaInfo);
				} break;

				case meta_member_type::int8: {
					int8 Data = *((int8 *)Start);
					Locals.AddString(Dest, Data, MetaInfo);
				} break;

				case meta_member_type::real32: {
					real32 Data = *((real32 *)Start);
					Locals.AddString(Dest, Data, MetaInfo);
				} break;

				case meta_member_type::real64: {
					real64 Data = *((real64 *)Start);
					Locals.AddString(Dest, Data, MetaInfo);
				} break;

				case meta_member_type::custom: {

					*Dest->Curr = '{'; Dest->Curr++;
					*Dest->Curr = '\n'; Dest->Curr++;

					MetaInfo->MetaFillShim(Dest, Start);

					*Dest->Curr = '}'; Dest->Curr++;
					*Dest->Curr = ','; Dest->Curr++;
					*Dest->Curr = '\n'; Dest->Curr++;

				} break;

				INVALID_DEFAULT
			}
		}

		if (IsArray) {
			*Dest->Curr = ']'; Dest->Curr++;
			*Dest->Curr = ','; Dest->Curr++;
			DataCount = MetaInfo->ArrayLength;
		}

	}

	void StructMetaFill(struct_string_return* Dest, meta_member* MetaInfo, uint32 MetaInfoCount, void* AccData)
	{
		for (uint32 index = 0; index < MetaInfoCount; index++) {
			StructMemberFill(Dest, &MetaInfo[index], AccData);
		}
	}

	struct_string_return StructToString(meta_member* MetaInfo, uint32 MetaInfoCount, void* AccData, memory_arena* Memory)
	{
		struct_string_return Ret = {};

		uint32 MaxSize = Megabytes(MaxCharCount);

		Ret.Start = (char*)ArenaAllocate(Memory, MaxSize);
		Ret.Curr = Ret.Start;
		Ret.Limit = Ret.Start + MaxSize;

		*Ret.Curr = '{'; Ret.Curr++;
		*Ret.Curr = '\n'; Ret.Curr++;

		StructMetaFill(&Ret, MetaInfo, MetaInfoCount, AccData);

		*Ret.Curr = '\n'; Ret.Curr++;
		*Ret.Curr = '}'; Ret.Curr++;

		return Ret;
	}

	void WriteStruct(char* Dest, meta_member* MetaInfo, uint32 MetaInfoCount, void* AccData, memory_arena* Memory)
	{
		struct_string_return Ret = StructToString(MetaInfo, MetaInfoCount, AccData, Memory);
		PlatformApi.WriteFile(Dest, Ret.Start, (uint32)(Ret.Curr - Ret.Start));
	}

}

#endif
