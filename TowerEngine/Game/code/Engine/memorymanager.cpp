#pragma once

#ifndef MemoryManagerCPP
#define MemoryManagerCPP

uint64 RoundMultiple(uint64 numToRound, uint64 multiple)
{
	if (multiple == 0) {
		return numToRound;
	}

	int64 remainder = numToRound % multiple;
	if (remainder == 0) {
		return numToRound;
	}

	return numToRound + multiple - remainder;
}

void* ArenaAllocate(memory_arena *Memory, uint64 Size)
{
	// Require 4 byte alignment
	if ((Size & 3) != 0) {
		Size = RoundMultiple(Size, 4);
	}

	void *CurrMemoryPos = Memory->Head;
	Memory->Head += Size;

	// Check if we've run out of memory
	if (Memory->Head > Memory->EndOfMemory) {
		int64 Excess = Memory->Head - Memory->EndOfMemory;
		int64 ExcessGB = BytesToGigabytes(Excess);
		int64 ExcessMB = BytesToMegabytes(Excess);
		int64 ExcessKB = BytesToKilobytes(Excess);
		Assert(false);
	}

	/*
		// clear the memory
		for (uint64 Offset = 0;
		        Offset < Size;
		        Offset++) {
			// NOTE commented this out, is it necessary?
			uint8 *NextLoc = (uint8 *)CurrMemoryPos + Offset;
			NextLoc = '\0';
		}
		*/

	return (CurrMemoryPos);
}

void ThreadPushAllocate(memory_arena* Arena, uint64 Size)
{

}

void ThreadPopAllocate(memory_arena* Arena, uint64 Size)
{

}

void ClearMemory(uint8* Mem, uint64 Size)
{
	for (uint64 Index = 0; Index < Size; Index++) {
		Mem[Index] = 0x00;
	}
}

void MemoryCopy(char* Destination, char* Source, uint64 Size)
{
	for (uint64 Index = 0; Index < Size; Index++) {
		Destination[Index] = Source[Index];
	}
}

#endif