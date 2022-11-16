#pragma once

#ifndef JsonCPP
#define JsonCPP

namespace json {

	/*
	TODO: Features not yet implemented here

	- Reading floats

	- Check for duplicate keys on load
	- add if key exists method
	- save and load a struct

	*/

	struct json_data;

	struct json_pair {
		string Key;

		union {
			string Data;
			json_data* Child;
		};

		//int32 ChildrenNodesCount;
	};

	struct json_data {
		int32 PairsCount;
		json_pair* Pairs;
	};

	enum class token_type {
		close_curly,
		open_curly,
		quote,
		comma,
		colon,
		identifier,
		end_of_file
	};

	struct tokenizer {
		char *Position;
	};

	const int32 Max_Pairs_Count = 100;

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
			case ('{'): { TokenType = token_type::open_curly; 	} break;
			case ('}'): { TokenType = token_type::close_curly; 	} break;
			case ('"'): { TokenType = token_type::quote; 		} break;
			case (','): { TokenType = token_type::comma; 		} break;
			case (':'): { TokenType = token_type::colon;		} break;
			case ('*'): { TokenType = token_type::end_of_file; 	} break;
			default: { TokenType = token_type::identifier; 		} break;
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

	json_data LoadJsonData(tokenizer* Tokenizer, memory_arena* Memory)
	{
		json_data JsonData = GetJson(Memory);

		token_type NextToken = GetNextToken(Tokenizer);

		while (true) {
			if (NextToken == token_type::open_curly) {

			} else if (NextToken == token_type::close_curly) {
				break;
			} else if (NextToken == token_type::quote) {

				JsonData.Pairs[JsonData.PairsCount].Key = GrabUntilToken(Tokenizer, token_type::quote);
				Tokenizer->Position += 1;

				token_type Next = GetNextToken(Tokenizer);

				if (Next == token_type::quote) {
					JsonData.Pairs[JsonData.PairsCount].Data = GrabUntilToken(Tokenizer, token_type::quote);
				} else if (Next == token_type::open_curly)  {

					JsonData.Pairs[JsonData.PairsCount].Child = (json_data*)ArenaAllocate(Memory, sizeof(json_data));

					json_data Child = LoadJsonData(Tokenizer, Memory);
					MemoryCopy((char*)JsonData.Pairs[JsonData.PairsCount].Child, (char*)(&Child), sizeof(json_data));

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

			token_type NextToken = GetNextToken(&Tokenizer);
			Assert(NextToken == token_type::open_curly);

			return LoadJsonData(&Tokenizer, Memory);
		}

		return json_data{};
	}

	string GenerateJsonLine(string Key, string Data, bool32 AddComma)
	{
		// NOTE this is probably a really inefficient way to do this. Probaby a lot of strings and allocations made here for no reason.
		string MaybeComma = "\r";
		if (AddComma) { MaybeComma = ","; }
		string Final = "\r \"" + Key + "\"" + ":" + "\"" + Data + "\"" + MaybeComma;
		return (Final);
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

					return GetPair(NewPath, Pair->Child);
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
		if (Data == "true") return true;

		// Default for no key is false
		return false;
	}

	int32 GetInt32(string Key, json_data * Data)
	{
		string DataInString = GetData(Key, Data);
		int32 IntData = StringToInt32(DataInString);
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

	real64 GetReal64(string Key, json_data * Data)
	{
		string DataInString = GetData(Key, Data);
		real64 Real = StringToReal64(DataInString);
		return (Real);
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
}

#endif