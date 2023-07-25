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

	void AddMembers(member* Root, string KeyParent, meta_member* MI, uint32 MICount, char* Data);

	void AddData(member* Root, string KeyParent, string ArrayIndex, meta_member* MI, void* Data)
	{
		string KS = KeyParent + MI->Name + ArrayIndex;
		int64 KeyHashFull = StringHash(KS);
		int64 KeyHash = KeyHashFull % MEMBER_PARS_MAX;
		pair* Pair = {};

		if (Direction == direction::write) {
			// if writing, then create a new pair

			// only add pair for data type
			if (MI->Type != meta_member_type::custom) {

				b32 Found = false;
				for (int i = 0; i < ArrayCount(Root->Pairs[0]); i++) {
					if (!Root->Pairs[KeyHash][i].Used) {
						Pair = &Root->Pairs[KeyHash][i];
						Found = true;
						break;
					}
				}
				Assert(Found);

				Pair->Key = KeyHashFull;
				Pair->Type = MI->Type;
				Pair->Used = true;
			}
		} else if (Direction == direction::read) {
			// if reading, then find a pair that exists

			b32 Found = false;
			for (int i = 0; i < ArrayCount(Root->Pairs[0]); i++) {
				if (Root->Pairs[KeyHash][i].Key == KeyHashFull) {
					Pair = &Root->Pairs[KeyHash][i];
					Found = true;
				}
			}

			// Save data didn't have that key
			if (!Found) {
				//return;
			}
			//Assert(Found);

			/*
			for (int i = 0; i < Root->PairsCount; i++) {
				if (Root->Pairs[i].Key == KeyHash) {
					Pair = &Root->Pairs[i];
					break;
				}
			}
			*/
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

			case meta_member_type::bool32: {
				if (Direction == direction::write) {
					Pair->Data.b32 = *(bool32*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					bool32* D = (bool32*)Data;
					*D = Pair->Data.b32;
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

			case meta_member_type::int8: {
				if (Direction == direction::write) {
					Pair->Data.i8 = *(int8*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					int8* D = (int8*)Data;
					*D = Pair->Data.i8;
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

			case meta_member_type::int32: {
				if (Direction == direction::write) {
					Pair->Data.i32 = *(int32*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					int32* D = (int32*)Data;
					*D = Pair->Data.i32;
				}
			} break;

			case meta_member_type::int64: {
				if (Direction == direction::write) {
					Pair->Data.i64 = *(int64*)Data;
				} else if (Direction == direction::read && Pair != GameNull) {
					int64* D = (int64*)Data;
					*D = Pair->Data.i64;
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

			case meta_member_type::enumeration: {
				int64 NumData = 0;

				if (Direction == direction::write) {
					if (MI->Size == 1) {
						char d = *(char*)Data;
						Pair->Data.i64 = d;
					} else if (MI->Size == 2)  {
						int16 d = *(int16*)Data;
						Pair->Data.i64 = d;
					} else if (MI->Size == 4)  {
						int32 d = *(int32*)Data;
						Pair->Data.i64 = d;
					} else if (MI->Size == 8)  {
						int64 d = *(int64*)Data;
						Pair->Data.i64 = d;
					} else {
						// Unknown enum size
						Assert(false);
					}
				} else if (Direction == direction::read && Pair != GameNull) {

					if (MI->Size == 1) {
						char Source = (char)Pair->Data.i64;
						char* Dest = (char*)Data;
						*Dest = Source;
					} else if (MI->Size == 2)  {
						int16 Source = (int16)Pair->Data.i64;
						int16* Dest = (int16*)Data;
						*Dest = Source;
					} else if (MI->Size == 4)  {
						int32 Source = (int32)Pair->Data.i64;
						int32* Dest = (int32*)Data;
						*Dest = Source;
					} else if (MI->Size == 8)  {
						int64 Source = (int64)Pair->Data.i64;
						int64* Dest = (int64*)Data;
						*Dest = Source;
					} else {
						// Unknown enum size
						Assert(false);
					}
				}

			} break;

			case meta_member_type::custom: {
				MI->SaveDataFillShim(Root, KS + ".", (void*)Data);
			} break;

			INVALID_DEFAULT
		}
	}

	void AddMembers(member* Root, string KeyParent, meta_member* MI, uint32 MICount, void* Data)
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

				AddData(Root, Key, ArrayIndexKey, Member, Start);
			}
		}
	}

	// NOTE (Ryan) This works because trans mem will be allocated contiguously
	void Write(char* FileDest, meta_member* MetaInfo, uint32 MetaInfoCount, void* Data, member* Root)
	{
		Direction = direction::write;
		ClearMemory((uint8*)Root, sizeof(member));
		AddMembers(Root, "", MetaInfo, MetaInfoCount, Data);

		PlatformApi.WriteFile(FileDest, (void*)Root, sizeof(member));
	}

	// Returns if successful
	bool32 Read(char* FilePath, void* Dest, meta_member* MetaInfo, uint32 MetaInfoCount, memory_arena* TransMem)
	{
		Direction = direction::read;

		read_file_result Result = PlatformApi.ReadFile(FilePath, TransMem);

		if (Result.ContentsSize > 0) {
			member* Root = (member*)Result.Contents;
			AddMembers(Root, "", MetaInfo, MetaInfoCount, Dest);

			return true;
		}

		return false;
	}
}

#endif
