#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "T:/Game/code/Engine/EngineCore.h"

void Print(char* Message)
{
	printf("%s", Message);
	printf("\n");
}

real64 RandomFloat()
{
	return rand();
}

int main()
{
	Print("Starting");

	// Limit is 2.14 gigs I guess
	Print("Allocating Memory");
	game_memory GameMemory = {};
	GameMemory.PermanentMemory.Size = Megabytes(512);
	GameMemory.TransientMemory.Size = Megabytes(512);
	GameMemory.ThreadTransientMemory.Size = Megabytes(10);
	GameMemory.TotalSize = GameMemory.PermanentMemory.Size + GameMemory.TransientMemory.Size + GameMemory.ThreadTransientMemory.Size;

	GameMemory.GameMemoryBlock = malloc(GameMemory.TotalSize);

	if (GameMemory.GameMemoryBlock == NULL) {
		// Error allocating memory block
		Assert(0);
	}

	GameMemory.PermanentMemory.Memory = GameMemory.GameMemoryBlock;
	GameMemory.TransientMemory.Memory = (uint8 *)GameMemory.PermanentMemory.Memory + GameMemory.PermanentMemory.Size;
	GameMemory.ThreadTransientMemory.Memory = (uint8 *)GameMemory.TransientMemory.Memory + GameMemory.TransientMemory.Size;

	GameMemory.TransientMemory.EndOfMemory = (uint8 *)GameMemory.TransientMemory.Memory + GameMemory.TransientMemory.Size;
	GameMemory.ThreadTransientMemory.EndOfMemory = (uint8 *)GameMemory.ThreadTransientMemory.Memory + GameMemory.ThreadTransientMemory.Size;
	GameMemory.PermanentMemory.EndOfMemory = (uint8 *)GameMemory.PermanentMemory.Memory + GameMemory.PermanentMemory.Size;

	GameMemory.PermanentMemory.Head = (uint8 *)GameMemory.PermanentMemory.Memory + sizeof(game_state);
	Print("Memory Allocated");

	srand (time(NULL));

	for (int i = 0; i < 100; i++) {
		real64 R = RandomFloat();
		string Test = R;
		Print(Test.Array());
	}

	platform::api PlatformEm = {};
	PlatformEm.RandomFloat = &RandomFloat;

	//PlatformEm.ReadFile = &ReadFile;
	//PlatformEm.FileExists = &FileExists;
	//PlatformEm.WriteFile = &WriteFile;
	//PlatformEm.AppendFile = &AppendFile;
	//PlatformEm.DeleteFile = &PlatformDeleteFile;
	//PlatformEm.GetClockMS = &GetClockMS;
	//PlatformEm.QueryPerformanceCounter = &GetWallClockGame;
	//PlatformEm.PerformanceCounterFrequency = PerformanceCounterFrequency;
	//PlatformEm.GetCycle = &GetCycle;
	//PlatformEm.GetGUID = &GetGUID;
	//PlatformEm.Print = &PlatformPrint;
	//PlatformEm.GetFileWriteTime = &PlatformGetFileWriteTime;
	//PlatformEm.GetProcAddress = &GetProcAddressSafe;
	//PlatformEm.OpenFileExternal = &OpenFileExternal;
	//PlatformEm.GetPathsForFileType = &GetPathsForFileType;
	//PlatformEm.OpenFileDialogue = &OpenFileDialogue;

	PlatformEm.ScreenDPI = 100;
	PlatformEm.ScreenDPICo = 1.0f;

	// Unsupported
	PlatformEm.MakeDirectory = {};
	PlatformEm.ThreadAddWork = {};

	GameMemory.PlatformApi = PlatformEm;


	return 0;
}