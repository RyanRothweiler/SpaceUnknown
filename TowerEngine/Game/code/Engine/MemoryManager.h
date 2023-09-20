#pragma once

#ifndef MemoryManagerH
#define MemoryManagerH

struct memory_arena {
	uint64 Size;

	// Pointer to the start of the arena. That is different than the start of new usable memory. Which is held by Head.
	void *Memory;

	// Head is NOT the head of this mem block. Head is the start of the next unallocated space.
	// This is only natural since I'm not freeing any memory
	uint8 *Head;
	uint8 *CurrentTops;

	uint8 *EndOfMemory;
};

#endif