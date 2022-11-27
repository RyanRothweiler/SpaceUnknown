#define EM_LOG_C_STACK 1

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <emscripten.h>
#include <sys/time.h>
#include <filesystem>
#include <chrono>

#define KEY_ESC 0
#define KEY_TAB 0
#define KEY_BACK 0
#define KEY_LEFT 0
#define KEY_RIGHT 0
#define KEY_UP 0
#define KEY_DOWN 0
#define KEY_PRIOR 0
#define KEY_NEXT 0
#define KEY_HOME 0
#define KEY_END 0
#define KEY_INSERT 0
#define KEY_DELETE 0
#define KEY_SPACE 0
#define KEY_RETURN 0
#define KEY_ESCAPE 0
#define KEY_SHIFT 0

#define WIN_EXPORT

#include "T:/Game/code/Engine/EngineCore.h"
#include "T:/Game/code/Engine/EngineCore.cpp"


void Print(char* Message)
{
	printf("%s", Message);
	printf("\n");
	//fflush();
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
	Print("GetGUID - UNSUPPORTED");
	return 0;
}

void* GetProcAddress(char* ProcName)
{
	Print("GetProcAddress - UNSUPPORTED");
	void* Ret;
	return Ret;
}

string FileTypeDesired;
static int Explore(const char *fpath, const struct stat *sb, int typeflag)
{
	/*
	printf("Getting paths for %s \n", fpath);
	printf("File type %s \n", FileTypeDesired.Array());

	if (typeflag == FTW_F) {
		if (fnmatch(FileTypeDesired.Array(), fpath, FNM_CASEFOLD) == 0) {
			printf("Found file %s \n", fpath);
			//std::cout << "found txt file: " << fpath << std::endl;
		}
	}
	*/
	return 0;
}

// This is recursive and will include all child files also.
// Returns the end of the list
path_list* GetPathsForFileType(char* FileTypeChar, const char* RootDir, memory_arena* Memory, path_list* PathList)
{
	//printf("Getting Paths For Type %s \n", RootDir);

	path_list* NextPath = PathList;

	std::string path = RootDir;
	for (const auto & entry : std::filesystem::directory_iterator(path)) {

		std::string Path = entry.path().string();

		if (entry.is_directory()) {
			//printf("Do Directory %s \n", Path.c_str());
			NextPath = GetPathsForFileType(FileTypeChar, Path.c_str(), Memory, NextPath);
		} else {
			//printf("File %s \n", Path.c_str());

			string MyStr = Path.c_str();
			if (StringEndsWith(MyStr, FileTypeChar)) {
				NextPath->Path = Path.c_str();
				NextPath->Next = (path_list*)ArenaAllocate(Memory, sizeof(path_list));

				//int32 Count = PathListCount(NextPaths);
				//int Count = NextPath->GetCount();
				//printf("		adding file to path %s\n", NextPath->Path.Array());

				NextPath = NextPath->Next;
			}
		}
	}

	return NextPath;
}

bool GlobalRunning = true;

void MakeProgram(shader* Shader)
{
	Print("OGL Make program");
}

void MakeTexture(loaded_image *Image, uint32 *BitmapPixels, bool32 CorrectGamma)
{
	Print("OGL Make Texture");
}

void MakeCubeMap(cube_map* CubeMap, int32 SideWidth, int32 SideHeight, bitmap_pixels* Sides)
{
	Print("OGL make cube map");
}

void MakeHDRTexture(loaded_image* Image, float* Data, int32 Width, int32 Height)
{
	Print("OGL make cubemap");
}

void GetFramebufferCubeMap(camera* Cam, bool32 GenerateMips, bool32 Trilinear)
{
	Print("OGL GetFramebufferCubeMap");
}

void GetFramebufferDepth(camera* Cam, uint32 Width, uint32 Height)
{
	Print("OGL GetFramebufferDepth");
}

void GetFramebuffer(camera* Cam, int32 ColorElementsCount)
{
	Print("OGL GetFramebuffer");
}

void Render (render::api * API,
             camera * ActiveCam, camera * ShadowCam, window_info * WindowInfo,
             renderer * DebugUIRenderer, renderer* UIRenderer, renderer * GameRenderer, shader * GaussianBlurShader
            )
{
	Print("OLG Render");
}

vao CreateVAO()
{
	Print("OGL create VAOs");
	vao VAO = {};
	return VAO;
}

void VAOBind_v2(vao* VAO, v2* Data, uint32 DataCount, int32 Location)
{
	Print("OGL VAOBind_v2");
}

void VAOBind_v3(vao* VAO, v3* Data, uint32 DataCount, int32 Location)
{
	Print("OGL VAOBind_v3");
}

int32 DoPickRender(camera* Camera, vector2 MousePos, window_info WindowInfo)
{
	Print("OGL DoPickRender");
	return 0;
}

void BakeIBL(renderer* EquiRenderer, renderer* ConvRenderer, renderer* PreFilterRenderer, window_info* WindowInfo)
{
	Print("OGL bakeibl");
}

void RenderCameraToBuffer(camera * Camera, renderer * Renderer, window_info WindowInfo)
{
	Print("OGL Render camera to buffer");
}

game_memory GameMemory = {};
game_input GameInput = {};
game_audio_output_buffer GameAudio = {};
window_info WindowInfo = {};

std::chrono::time_point<std::chrono::high_resolution_clock> PrevClock;

void MainLoop()
{
	GameLoop(&GameMemory, &GameInput, &WindowInfo, &GameAudio, "T:/Game/assets/");

	auto CurrClock = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> ClockDiff = CurrClock - PrevClock;
	PrevClock = CurrClock;

	float FPS = 1.0f / (ClockDiff.count());
	printf("FPS %f \n", FPS);
}

int main()
{
	Print("Starting");

	// Limit is 2.14 gigs I guess
	Print("Allocating Memory");
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
	PlatformEm.GetPathsForFileType = &GetPathsForFileType;

	PlatformEm.ScreenDPI = 100;
	PlatformEm.ScreenDPICo = 1.0f;

	GameMemory.PlatformApi = PlatformEm;


	WindowInfo.Width = 512;
	WindowInfo.Height = 512;

	render::api RenderApi = {};
	RenderApi.MakeProgram = &MakeProgram;
	RenderApi.MakeTexture = &MakeTexture;
	RenderApi.MakeCubeMap = &MakeCubeMap;
	RenderApi.MakeHDRTexture = &MakeHDRTexture;
	RenderApi.GetFramebuffer = &GetFramebuffer;
	RenderApi.GetFramebufferDepth = &GetFramebufferDepth;
	RenderApi.GetFramebufferCubeMap = &GetFramebufferCubeMap;
	RenderApi.Render = &Render;
	RenderApi.CreateVAO = &CreateVAO;
	RenderApi.VAOBind_v2 = &VAOBind_v2;
	RenderApi.VAOBind_v3 = &VAOBind_v3;
	RenderApi.DoPickRender = &DoPickRender;
	RenderApi.BakeIBL = &BakeIBL;
	RenderApi.RenderCameraToBuffer = &RenderCameraToBuffer;
	GameMemory.RenderApi = RenderApi;

	PrevClock = std::chrono::high_resolution_clock::now();
	emscripten_set_main_loop(&MainLoop, 0, true);

	// use -ASYNCIFY with this
	/*
	float TargetFPS = 60.0f;
	float TargetFrameSec = 1.0f / TargetFPS;

	while (true) {
		GameLoop(&GameMemory, &GameInput, &WindowInfo, &GameAudio, "T:/Game/assets/");

		auto CurrClock = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> WorkDiff = CurrClock - PrevClock;

		if (WorkDiff.count() > TargetFrameSec) {
			float SleepLen = WorkDiff.count() - TargetFrameSec;
			emscripten_sleep(SleepLen);
		}

		std::chrono::duration<float> TotalFrameDiff = CurrClock - PrevClock;
		float FPS = 1.0f / (TotalFrameDiff.count());
		printf("FPS %f \n", FPS);

		PrevClock = CurrClock;
	}
	*/


	return 0;
}