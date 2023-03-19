#pragma once
#ifndef SaveDataCPP
#define SaveDataCPP

uint32 BoilerplateCharactersCount = 3;
const int32 MaxCharCount = 256;

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

void LoadStruct(meta_member* MetaInfo, uint32 MetaInfoCount, char* SourceStart, uint32 SourceSize, void* Destination)
{
	char* Current = SourceStart;
	char* End = SourceStart + SourceSize;

	while (Current < End) {

		char* Start = Current;
		while (Start[0] != ', ') {
			Start++;
		}
		string NameFromFile = BuildString(Current, (uint32)(Start - Current));

		Start++;
		Current = Start;

		while (Start[0] != ', ') { Start++; }
		string ValueStringFromFile = BuildString(Current, (uint32)(Start - Current));

		Start++;
		Current = Start;

		// Move past end line break characters
		while (Current[0] == '\n' || Current[0] == '\r' || Current[0] == '\t' || Current[0] == ', ') { Current++; }

		int32 CurrentMetaIndex = -1;
		for (uint32 MetaCheck = 0; MetaCheck < MetaInfoCount; MetaCheck++) {
			if (MetaInfo[MetaCheck].Name == NameFromFile) {
				CurrentMetaIndex = MetaCheck;
				break;
			}
		}

		if (CurrentMetaIndex >= 0) {
			uint64 CurrentOffset = MetaInfo[CurrentMetaIndex].Offset;

			// NOTE this uses the type currently in the struct, not the type in the file. What happens if there is a missmatch?
			switch (MetaInfo[CurrentMetaIndex].Type) {

				case (meta_member_type::uint32): {
					uint32 FileVal = (uint32)StringToInt32(ValueStringFromFile);
					char* FinalDest = (char*)Destination + CurrentOffset;
					*(uint32*)FinalDest = FileVal;
				} break;

				case (meta_member_type::uint16): {
					uint16 FileVal = (uint16)StringToInt32(ValueStringFromFile);
					char* FinalDest = (char*)Destination + CurrentOffset;
					*(uint16*)FinalDest = FileVal;
				} break;

				case (meta_member_type::uint8): {
					uint8 FileVal = (uint8)StringToInt32(ValueStringFromFile);
					char* FinalDest = (char*)Destination + CurrentOffset;
					*(uint8*)FinalDest = FileVal;
				} break;

				case (meta_member_type::int32): {
					int32 FileVal = (int32)StringToInt32(ValueStringFromFile);
					char* FinalDest = (char*)Destination + CurrentOffset;
					*(int32*)FinalDest = FileVal;
				} break;

				case (meta_member_type::int16): {
					int16 FileVal = (int16)StringToInt32(ValueStringFromFile);
					char* FinalDest = (char*)Destination + CurrentOffset;
					*(int16*)FinalDest = FileVal;
				} break;

				case (meta_member_type::int8): {
					int8 FileVal = (int8)StringToInt32(ValueStringFromFile);
					char* FinalDest = (char*)Destination + CurrentOffset;
					*(int8*)FinalDest = FileVal;
				} break;

				case (meta_member_type::real32): {
					real32 FileVal = (real32)StringToReal64(ValueStringFromFile);
					char* FinalDest = (char*)Destination + CurrentOffset;
					*(real32*)FinalDest = FileVal;
				} break;

				default: {
					// That type isn't supported yet.
					Assert(false);
				}
			}
		} else {
			// Could not fine file member in current meta info.
		}
	}
}

#endif