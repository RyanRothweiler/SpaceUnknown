#pragma once

#ifndef HashCPP
#define HashCPP

#include "Hash.h"

namespace hash {


	// Hash functions --------------------------------------------------------------

	uint32 CalcString(char* Input, uint32 TableSize)
	{
		const int p = 31;
		const int m = 1000009;
		long long hash_value = 0;
		long long p_pow = 1;

		int Len = CharArrayLength(Input);

		for (int i = 0; i < Len; i++) {
			hash_value = (hash_value + (Input[i] - 'a' + 1) * p_pow) % m;
			p_pow = (p_pow * p) % m;
		}
		return (uint32)hash_value % TableSize;
	}

	uint32 CalcAxial(axial Axial, uint32 TableSize)
	{
		uint64 PrimeOne = 19349663;
		uint64 PrimeTwo = 73856093;
		uint64 PrimeThree = 32687;
		uint64 PrimeFour = 69899;

		uint64 Q = 0;
		if (Axial.Q < 0) {
			Q = Axial.Q * PrimeOne * -1;
		} else {
			Q = Axial.Q * PrimeTwo;
		}

		uint64 R = 0;
		if (Axial.R < 0) {
			R = Axial.R * PrimeThree * -1;
		} else {
			R = Axial.R * PrimeFour;
		}

		uint64 Xored = Q ^ R;
		uint32 TableMod = Xored % TableSize;
		return (TableMod);
	}

	//------------------------------------------------------------------------------

	void AllocateTable(hash::table* Table, uint32 Size, uint32 DataSize, memory_arena* Memory)
	{
		Table->Table = (list_head*)ArenaAllocate(Memory, sizeof(list_head) * Size);
		Table->DataSize = DataSize;
		Table->TableSize = Size;
	}

	void Add(hash::table* Table, uint32 Hash, void* Data, real32 DataSize, memory_arena* Memory)
	{
		Hash = Hash % Table->TableSize;

		Assert(Hash < Table->TableSize);
		Assert(DataSize == Table->DataSize);

		list_head* List = &Table->Table[Hash];
		if (!List->Initialized) {
			InitList(List, Table->DataSize);
		}

		AddLink(List, Data, Memory);
	}

	//------------------------------------------------------------------------------
	
	persistent_pointer* GetPersistentPointer(hash::table* Table, uint32 Hash) {

		uint32 HashMod = Hash % Table->TableSize;

		if (!Table->Table[HashMod].Initialized) {
			return GameNull;
		}

		list_link* CurrLink = Table->Table[HashMod].TopLink;
		for (uint32 index = 0; index < Table->Table[HashMod].LinkCount; index++) {
			persistent_pointer* Dat = (persistent_pointer*)CurrLink->Data;
			if (Dat->GUID == Hash) {
				return Dat;
			}
			CurrLink = CurrLink->NextLink;
		}

		return GameNull;
	}

	bool32 Contains_Axial(hash::table* Table, axial Data)
	{
		Assert(Table->DataSize == sizeof(axial));

		uint32 Hash = CalcAxial(Data, Table->TableSize);

		if (!Table->Table[Hash].Initialized) {
			return false;
		}

		list_link* CurrLink = Table->Table[Hash].TopLink;
		for (uint32 index = 0; index < Table->Table[Hash].LinkCount; index++) {
			axial* Dat = (axial*)CurrLink->Data;
			if (Dat->Q == Data.Q && Dat->R == Data.R) {
				return true;
			}
			CurrLink = CurrLink->NextLink;
		}

		return false;
	}

	void Remove_Axial(hash::table* Table, axial Data)
	{
		Assert(Table->DataSize == sizeof(axial));

		uint32 Hash = CalcAxial(Data, Table->TableSize);

		if (!Table->Table[Hash].Initialized) {
			return;
		}

		list_link* CurrLink = Table->Table[Hash].TopLink;
		for (uint32 index = 0; index < Table->Table[Hash].LinkCount; index++) {
			axial* Dat = (axial*)CurrLink->Data;
			if (Dat->Q == Data.Q && Dat->R == Data.R) {
				RemoveLink(&Table->Table[Hash], index);
				return;
			}
			CurrLink = CurrLink->NextLink;
		}
	}

}

#endif
