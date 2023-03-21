#pragma once
#ifndef SaveDataCPP
#define SaveDataCPP

namespace save_data {

	void AddMembers(member* Root, string KeyParent, meta_member* MI, uint32 MICount, void* Data, memory_arena* TransMem);

	void AddData(member* Root, string KeyParent, meta_member* MI, void* Data, memory_arena* TransMem)
	{
		Root->Pairs[Root->PairsCount] = (pair*)ArenaAllocate(TransMem, sizeof(pair));
		pair* Pair = Root->Pairs[Root->PairsCount];
		Root->PairsCount++;
		Assert(Root->PairsCount < MEMBER_PARS_MAX);

		string KS = KeyParent + MI->Name;
		Assert(ArrayCount(Pair->Key) < ArrayCount(KS.CharArray));

		int Count = ArrayCount(Pair->Key);
		for (int i = 0; i < Count; i++) { Pair->Key[i] = KS.Array()[i]; }

		Pair->Type = MI->Type;

		char* Start = (char*)Data;
		//Start = Start + (MI->Offset + (i * MetaInfo->Size));
		Start = Start + MI->Offset;

		switch (MI->Type) {
			case meta_member_type::uint32: {
				Pair->Data.ui32 = *(uint32*)Start;
			} break;

			case meta_member_type::uint16: {
				Pair->Data.ui16 = *(uint16*)Start;
			} break;

			case meta_member_type::uint8: {
				Pair->Data.ui8 = *(uint8*)Start;
			} break;

			case meta_member_type::int32: {
				Pair->Data.i32 = *(int32*)Start;
			} break;

			case meta_member_type::int16: {
				Pair->Data.i16 = *(int16*)Start;
			} break;

			case meta_member_type::int8: {
				Pair->Data.i8 = *(int8*)Start;
			} break;

			case meta_member_type::real32: {
				Pair->Data.r32 = *(real32*)Start;
			} break;

			case meta_member_type::real64: {
				Pair->Data.r64 = *(real64*)Start;
			} break;

			case meta_member_type::custom: {
				//typedef void(*save_data_fill_shim)(save_data::member * Dest, string KeyParent, void* AccData, memory_arena * Memory);
				MI->SaveDataFillShim(Root, KS + ".", Start, TransMem);
			} break;

			INVALID_DEFAULT
		}
	}

	void AddMembers(member* Root, string KeyParent, meta_member* MI, uint32 MICount, void* Data, memory_arena* TransMem)
	{
		for (uint32 index = 0; index < MICount; index++) {
			AddData(Root, KeyParent, &MI[index], Data, TransMem);
		}
	}

	void Write(char* FileDest, meta_member* MetaInfo, uint32 MetaInfoCount, void* Data, memory_arena* TransMem)
	{
		member Root = {};
		AddMembers(&Root, "", MetaInfo, MetaInfoCount, Data, TransMem);

		int x = 0;
	}
}

#endif