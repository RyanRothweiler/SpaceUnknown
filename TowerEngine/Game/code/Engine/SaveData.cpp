#pragma once
#ifndef SaveDataCPP
#define SaveDataCPP

uint32 BoilerplateCharactersCount = 3;

string StructMemberToString(meta_member* MetaInfo, void* AccData, memory_arena* Memory)
{
	string DataOutput = {};

	char* Start = (char*)AccData;
	Start = Start + MetaInfo->Offset;

	switch (MetaInfo->Type) {
		case meta_member_type::uint32: {
			uint32 Data = *((uint32 *)Start);
			DataOutput = Data;
		} break;

		case meta_member_type::uint16: {
			uint16 Data = *((uint16 *)Start);
			DataOutput = Data;
		} break;

		case meta_member_type::uint8: {
			uint8 Data = *((uint8 *)Start);
			DataOutput = Data;
		} break;

		case meta_member_type::int32: {
			int32 Data = *((int32 *)Start);
			DataOutput = Data;
		} break;

		case meta_member_type::int16: {
			int16 Data = *((int16 *)Start);
			DataOutput = Data;
		} break;

		case meta_member_type::int8: {
			int8 Data = *((int8 *)Start);
			DataOutput = Data;
		} break;

		case meta_member_type::real32: {
			real32 Data = *((real32 *)Start);
			DataOutput = Data;
		} break;

		case meta_member_type::custom: {
			struct_string_return Ret = MetaInfo->ToStringFunc(MetaInfo->CustomMetaInfo, MetaInfo->CustomMetaInfoCount, Start, Memory);

			int x = 0;
		} break;

		default : {
			// That type isn't supported yet.
			Assert(false);
		} break;
	}

	return DataOutput;
}

uint32 StructGuessSize(meta_member* MetaInfo, uint32 MetaInfoCount)
{
	uint32 MaxSize = 0;
	for (uint32 index = 0; index < MetaInfoCount; index++) {

		MaxSize += StringLength(MetaInfo[index].Name);

		// Three for the separating commas end line break characters
		MaxSize += BoilerplateCharactersCount;

		switch (MetaInfo[index].Type) {
			case meta_member_type::uint32: {
				MaxSize += 10;
			} break;

			case meta_member_type::uint16: {
				MaxSize += 5;
			} break;

			case meta_member_type::uint8: {
				MaxSize += 3;
			} break;

			case meta_member_type::int32: {
				MaxSize += 10;
			} break;

			case meta_member_type::int16: {
				MaxSize += 5;
			} break;

			case meta_member_type::int8: {
				MaxSize += 3;
			} break;

			case meta_member_type::real32: {
				MaxSize += 20;
			} break;

			default : {
				//That type isn't supported yet.
				//Assert(false);
				MaxSize += 10000;
			} break;
		}
	}

	return MaxSize;
}

// This just saves / loads structs as comma separated
struct_string_return StructToString(meta_member* MetaInfo, uint32 MetaInfoCount, void* AccData, memory_arena* Memory)
{
	uint32 MaxSize = StructGuessSize(MetaInfo, MetaInfoCount);

	uint32 AccSize = 0;
	char* Output = (char*)ArenaAllocate(Memory, MaxSize);
	char* NextChar = Output;

	for (uint32 index = 0; index < MetaInfoCount; index++) {

		string DataOutput = StructMemberToString(&MetaInfo[index], AccData, Memory);

		uint32 DataStringLength = StringLength(DataOutput);
		uint32 NameStringLength = StringLength(MetaInfo[index].Name);
		AccSize += DataStringLength + NameStringLength + BoilerplateCharactersCount;

		// Add Name
		memcpy((void*)NextChar, (void*)&MetaInfo[index].Name, NameStringLength);
		NextChar += NameStringLength;
		*NextChar = ','; NextChar++;

		// Add Data
		memcpy((void*)NextChar, (void*)&DataOutput.CharArray, DataStringLength);
		NextChar += DataStringLength;
		*NextChar = ','; NextChar++;
		// *NextChar = '\n'; NextChar++;
	}

	struct_string_return Ret = {};
	Ret.Data = Output;
	Ret.DataLength = NextChar - Output;
	return Ret;
}

void LoadStruct(meta_member* MetaInfo, uint32 MetaInfoCount, char* SourceStart, uint32 SourceSize, void* Destination)
{
	char* Current = SourceStart;
	char* End = SourceStart + SourceSize;

	while (Current < End) {

		char* Start = Current;
		while (Start[0] != ',') {
			Start++;
		}
		string NameFromFile = BuildString(Current, (uint32)(Start - Current));

		Start++;
		Current = Start;

		while (Start[0] != ',') { Start++; }
		string ValueStringFromFile = BuildString(Current, (uint32)(Start - Current));

		Start++;
		Current = Start;

		// Move past end line break characters
		while (Current[0] == '\n' || Current[0] == '\r' || Current[0] == '\t' || Current[0] == ',') { Current++; }

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