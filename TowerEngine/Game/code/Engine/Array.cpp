#pragma once

#ifndef ArrayCPP
#define ArrayCPP

namespace Array {

	struct array {
		uint8* Array;
		int32 IndexSize;
		int32 Length;
	};

	void Allocate(array* Array, int32 IndexSize, int32 Length, memory_arena* Memory)
	{
		Array->Length = Length;
		Array->IndexSize = IndexSize;
		Array->Array = (uint8*)ArenaAllocate(Memory, IndexSize * Length);
	}

	void* Get(array* Array, int32 Index)
	{
		Assert(Index > 0 && Index < Array->Length);
		return Array->Array + (Array->IndexSize * Index);
	}


}

#endif