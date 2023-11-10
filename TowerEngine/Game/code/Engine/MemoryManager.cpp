#pragma once

#ifndef MemoryManagerCPP
#define MemoryManagerCPP

#define TOWER_DEBUG_MEMORY false

#if TOWER_DEBUG_MEMORY
#define ArenaAllocate(...) ArenaAllocate_Name(AT, __VA_ARGS__)
#endif

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

void ClearMemory(uint8* Mem, uint64 Size)
{
	for (uint64 Index = 0; Index < Size; Index++) {
		Mem[Index] = 0x00;
	}
}


void* ArenaAllocate(memory_arena *Memory, uint64 Size, bool32 ClearMem = false)
{
	// Require 4 byte alignment
	if ((Size & 3) != 0) {
		Size = RoundMultiple(Size, 4);
	}

#if TOWER_DEBUG_MEMORY
	if (Memory == GlobalTransMem) {

		real64 Perc = 1.0f - (real64)(Memory->EndOfMemory - Memory->Head) / (real64)Memory->Size;

		char Buf[256];
		sprintf_s(Buf, "%f", Perc);

		PlatformApi.Print("Trans Allocation");
		PlatformApi.Print(Source);
		PlatformApi.Print(&Buf[0]);
		printf("testing %i", 1);
		PlatformApi.Print("");
	}
#endif

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

	if (ClearMem) {
		ClearMemory((uint8*)CurrMemoryPos, Size);
	}

	return (CurrMemoryPos);
}

void MemoryCopy(char* Destination, char* Source, uint64 Size)
{
	for (uint64 Index = 0; Index < Size; Index++) {
		Destination[Index] = Source[Index];
	}
}

void MemoryReset(memory_arena* Memory) {
	Memory->Head = (uint8 *)Memory->Memory;
}

#endif
