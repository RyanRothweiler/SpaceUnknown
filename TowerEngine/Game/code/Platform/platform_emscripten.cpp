#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <emscripten.h>
#include <sys/time.h>
#include <ftw.h>
#include <fnmatch.h>

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

read_file_result LoadFileData(std::string FileName, memory_arena *Memory)
{
	read_file_result Result = {};

	FILE *File = fopen(FileName.c_str(), "rb");
	if (!File) {
		printf("Couldn't find file %s\n", FileName.c_str());
		return {};
	}

	// get file size
	fseek(File, 0, SEEK_END);
	Result.ContentsSize = ftell(File);
	fseek(File, 0, SEEK_SET);  /* same as rewind(f); */

	// copy data
	Result.Contents = ArenaAllocate(Memory, Result.ContentsSize);
	fread(Result.Contents, Result.ContentsSize, 1, File);

	// close
	fclose(File);

	return (Result);
}

read_file_result ReadFile(char* FileName, memory_arena *Memory)
{
	return LoadFileData(FileName, Memory);
}

bool32 FileExists(char* FileName)
{
	FILE *File = fopen(FileName, "rb");
	return File != NULL;
}

uint64 GetCycle()
{
	timespec now = {};
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (uint64) (now.tv_sec * 1000000000LL + now.tv_nsec);
}

void MakeDirectory(char* Path)
{
	Print("MakeDirectory - UNSUPPORTED");
}

thread_work* AddWork(game_thread_proc WorkMethod, void* WorkParams)
{
	Print("ThreadAddWork - UNSUPPORTED");
	return {};
}

void AppendFile(char *FileDestination, char *Data)
{
	Print("AppendFile - UNSUPPORTED");
}

void DeleteFile(char *Path)
{
	Print("DeleteFile - UNSUPPORTED");
}

void WriteFile(char *FileDestination, void *Data, uint32 DataSize)
{
	Print("WriteFile - UNSUPPORTED");
}

void OpenFileExternal(char* FileDest)
{
	Print("OpenFileExternal - UNSUPPORTED");
}

void OpenFileDialogue(char* Path, int32 MaxPath)
{
	Print("OpenFileDialogue - UNSUPPORTED");
}

uint64 GetWallClockGame()
{
	Print("QueryPerformanceCounter - UNSUPPORTED");
	return 0;
}

uint64 GetFileWriteTime(char* File)
{
	Print("GetFileWriteTime - UNSUPPORTED");
	return 0;
}

// returns time in microseconds
inline uint64
GetWallClock()
{
	// This doesn't do what we want!! Just gives the current time OF milliseconds, not IN milliseconds

	timeval TimeVal = {};
	gettimeofday(&TimeVal, NULL);
	//LOGI("NANO SECONDS %i", (int)TimeVal.tv_usec);
	//return (TimeVal.tv_usec);
	return ((TimeVal.tv_sec * 1000000) + TimeVal.tv_usec);


	/*
	timespec now = {};
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (uint64) (now.tv_sec * 1000000000LL + now.tv_nsec);
	*/
}

uint32 GetGUID()
{
	Print("GetGUID - UNSUPPORTED \n");
	return 0;
}

void* GetProcAddress(char* ProcName)
{
	Print("GetProcAddress - UNSUPPORTED \n");
	void* Ret;
	return Ret;
}

string FileTypeDesired;
static int explore(const char *fpath, const struct stat *sb, int typeflag)
{
	if (typeflag == FTW_F) {
		if (fnmatch("*.txt", fpath, FNM_CASEFOLD) == 0) { ///< it's a .txt file
			std::cout << "found txt file: " << fpath << std::endl;
		}
	}
	return 0;
}

// This is recursive and will include all child files also.
// Returns the end of the list
path_list* GetPathsForFileType(char* FileTypeChar, char* RootDir, memory_arena* Memory, path_list* PathList)
{
	FileTypeDesired = "*" + FileTypeChar;

	/*
	path_list* NextPath = PathList;

	HANDLE hFind = {};
	WIN32_FIND_DATA FileData = {};

	string Path = Root + "*";
	hFind = FindFirstFileA(Path.CharArray, &FileData);

	do {
		if ((FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && FileData.cFileName[0] != '.') {
			string P = Root + FileData.cFileName + "/";
			NextPath = GetPathsForFileType(FileType.Array(), P.Array(), Memory, NextPath);
		} else {
			if (StringEndsWith(FileData.cFileName, FileType)) {
				NextPath->Path = Root + FileData.cFileName;
				NextPath->Next = (path_list*)ArenaAllocate(Memory, sizeof(path_list));
				NextPath = NextPath->Next;
			}

		}
	} while (FindNextFileA(hFind, &FileData) != 0);

	FindClose(hFind);
	return NextPath;
	*/
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

	platform::api PlatformEm = {};
	PlatformEm.PerformanceCounterFrequency = 0;
	PlatformEm.RandomFloat = &RandomFloat;
	PlatformEm.ReadFile = &ReadFile;
	PlatformEm.FileExists = &FileExists;
	PlatformEm.GetCycle = &GetCycle;
	PlatformEm.MakeDirectory = &MakeDirectory;
	PlatformEm.ThreadAddWork = &AddWork;
	PlatformEm.AppendFile = &AppendFile;
	PlatformEm.DeleteFile = &DeleteFile;
	PlatformEm.WriteFile = &WriteFile;
	PlatformEm.OpenFileExternal = &OpenFileExternal;
	PlatformEm.OpenFileDialogue = &OpenFileDialogue;
	PlatformEm.GetClockMS = &GetWallClock;
	PlatformEm.QueryPerformanceCounter = &GetWallClockGame;
	PlatformEm.GetGUID = &GetGUID;
	PlatformEm.Print = &Print;
	PlatformEm.GetFileWriteTime = &GetFileWriteTime;
	PlatformEm.GetProcAddress = &GetProcAddress;

	//PlatformEm.GetPathsForFileType = &GetPathsForFileType;

	PlatformEm.ScreenDPI = 100;
	PlatformEm.ScreenDPICo = 1.0f;

	GameMemory.PlatformApi = PlatformEm;

	return 0;
}