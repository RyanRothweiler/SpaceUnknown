#pragma once
#ifndef SaveDataCPP
#define SaveDataCPP

/*
Data is "flattened in to one array"

exmaples of keys
fieldname.foo
fieldname
fieldnamearray.0.fo
fieldnamearray.0.ba
fieldnamearray.1.ba
fieldnamearray.1.ba
*/
namespace save_data {

	enum class direction {
		// put data from class into file
		write,

		// put data from file into class
		read
	};

	direction Direction;

	void AddMembers(member* Root, string KeyParent, meta_member* MI, uint32 MICount, char* Data, memory_arena* TransMem);

	void AddData(member* Root, string KeyParent, string ArrayIndex, meta_member* MI, void* Data, memory_arena* TransMem)
	{
		string KS = KeyParent + MI->Name + ArrayIndex;
		pair* Pair = {};

		if (Direction == direction::write) {
			// if writing, then create a new pair

			// only add pair for data type
			if (MI->Type != meta_member_type::custom) {
				Pair = &Root->Pairs[Root->PairsCount++];
				Assert(Root->PairsCount < MEMBER_PARS_MAX);

				Assert(ArrayCount(Pair->Key) < ArrayCount(KS.CharArray));
				int Count = ArrayCount(Pair->Key);
				for (int i = 0; i < Count; i++) { Pair->Key[i] = KS.Array()[i]; }

				Pair->Type = MI->Type;
			}
		} else if (Direction == direction::read) {
			// if reading, then find a pair that exists

			for (int i = 0; i < Root->PairsCount; i++) {
				if (CharArraysEqual(KS.Array(), (char*)&Root->Pairs[i].Key[0])) {
					Pair = &Root->Pairs[i];
					break;
				}
			}
		}

		switch (MI->Type) {
			case meta_member_type::uint32: {
				if (Direction == direction::write) {
					Pair->Data.ui32 = *(uint32*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					uint32* D = (uint32*)Data;
					*D = Pair->Data.ui32;
				}
			} break;

			case meta_member_type::uint16: {
				if (Direction == direction::write) {
					Pair->Data.ui16 = *(uint16*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					uint16* D = (uint16*)Data;
					*D = Pair->Data.ui16;
				}
			} break;

			case meta_member_type::uint8: {
				if (Direction == direction::write) {
					Pair->Data.ui8 = *(uint8*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					uint8* D = (uint8*)Data;
					*D = Pair->Data.ui8;
				}
			} break;

			case meta_member_type::int32: {
				if (Direction == direction::write) {
					Pair->Data.i32 = *(int32*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					int32* D = (int32*)Data;
					*D = Pair->Data.i32;
				}
			} break;

			case meta_member_type::int16: {
				if (Direction == direction::write) {
					Pair->Data.i16 = *(int16*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					int16* D = (int16*)Data;
					*D = Pair->Data.i16;
				}
			} break;

			case meta_member_type::int8: {
				if (Direction == direction::write) {
					Pair->Data.i8 = *(int8*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					int8* D = (int8*)Data;
					*D = Pair->Data.i8;
				}
			} break;

			case meta_member_type::real32: {
				if (Direction == direction::write) {
					Pair->Data.r32 = *(real32*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					real32* D = (real32*)Data;
					*D = Pair->Data.r32;
				}
			} break;

			case meta_member_type::real64: {
				if (Direction == direction::write) {
					Pair->Data.r64 = *(real64*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					real64* D = (real64*)Data;
					*D = Pair->Data.r64;
				}
			} break;

			case meta_member_type::custom: {
				MI->SaveDataFillShim(Root, KS + ".", (void*)Data, TransMem);
			} break;

			INVALID_DEFAULT
		}
	}

	void AddMembers(member* Root, string KeyParent, meta_member* MI, uint32 MICount, void* Data, memory_arena* TransMem)
	{
		for (uint32 index = 0; index < MICount; index++) {

			meta_member* Member = &MI[index];

			bool32 IsArray = Member->ArrayLength > 0;

			int DataCount = 1;
			if (IsArray) {
				DataCount = Member->ArrayLength;
			}

			for (int i = 0; i < DataCount; i++) {

				char* Start = (char*)Data;
				Start = Start + (Member->Offset + (i * Member->Size));
				//Start = Start + MI->Offset;

				string Key = KeyParent;
				string ArrayIndexKey = "";
				if (IsArray) {
					ArrayIndexKey = ".";
					ArrayIndexKey = ArrayIndexKey + i;
				}

				AddData(Root, Key, ArrayIndexKey, Member, Start, TransMem);
			}
		}
	}

	// NOTE (Ryan) This works because trans mem will be allocated contiguously
	void Write(char* FileDest, meta_member* MetaInfo, uint32 MetaInfoCount, void* Data, memory_arena* TransMem)
	{
		Direction = direction::write;

		member Root = {};
		AddMembers(&Root, "", MetaInfo, MetaInfoCount, Data, TransMem);

		PlatformApi.WriteFile(FileDest, (void*)&Root, sizeof(Root) + (sizeof(pair) * Root.PairsCount));
	}

	void Read(char* FilePath, void* Dest, meta_member* MetaInfo, uint32 MetaInfoCount, memory_arena* TransMem)
	{
		Direction = direction::read;

		read_file_result Result = PlatformApi.ReadFile(FilePath, TransMem);

		member* Root = (member*)Result.Contents;
		AddMembers(Root, "", MetaInfo, MetaInfoCount, Dest, TransMem);
	}
}

#endif