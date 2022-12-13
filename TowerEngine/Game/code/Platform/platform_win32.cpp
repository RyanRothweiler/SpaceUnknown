#include <dsound.h>
#include <Rpc.h>
#include <stdio.h>
#include <iostream>
#include <Shellapi.h>
#include <Shellscalingapi.h>
#include <commdlg.h>

// logging ---------------------------------------------------------------------
void
DebugLine(char *Output)
{
	OutputDebugString(Output);
	OutputDebugString("\n");
}

#define LOG(Message) { DebugLine(Message); }
//------------------------------------------------------------------------------

#include "../Engine/EngineCore.h"
#include <gl\gl.h>

// Intel intrinsics for random generation
#include "immintrin.h"
#include "intrin.h"
#include "Winnt.h"


bool32 GlobalRunning = true;

global_variable real64 GlobalScrollPos = 0.0f;
global_variable real64 PrevGlobalScrollPos;

global_variable float DisplayPPI = 105.0f / 100.0f;

global_variable game_input *GlobalGameInput;

int64 PerformanceCounterFrequency;


// -----------------------------------------------------------------------------
// This is opengl specific. Really will need to put this into its own ogl file to support multiple rendering backends
// Trid to put this into renderer_win32_ogles3.cpp but had some trouble getting the dlls correctly. And then
// 		the pixel formats didn't work correctly. Maybe because it was in a dll?? idk
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

// See https://www.opengl.org/registry/specs/ARB/wgl_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_FULL_ACCELERATION_ARB               0x2027

#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB        0x20A9

#define WGL_RED_BITS_ARB                        0x2015
#define WGL_GREEN_BITS_ARB                      0x2017
#define WGL_BLUE_BITS_ARB                       0x2019
#define WGL_ALPHA_BITS_ARB                      0x201B
#define WGL_DEPTH_BITS_ARB                      0x2022

typedef HGLRC WINAPI wgl_create_context_attribs_arb(HDC hDC, HGLRC hShareContext,
        const int *attribList);

typedef BOOL WINAPI wgl_get_pixel_format_attrib_iv_arb(HDC hdc,
        int iPixelFormat,
        int iLayerPlane,
        UINT nAttributes,
        const int *piAttributes,
        int *piValues);

typedef BOOL WINAPI wgl_get_pixel_format_attrib_fv_arb(HDC hdc,
        int iPixelFormat,
        int iLayerPlane,
        UINT nAttributes,
        const int *piAttributes,
        FLOAT *pfValues);

typedef BOOL WINAPI wgl_choose_pixel_format_arb(HDC hdc,
        const int *piAttribIList,
        const FLOAT *pfAttribFList,
        UINT nMaxFormats,
        int *piFormats,
        UINT *nNumFormats);

wgl_create_context_attribs_arb *wglCreateContextAttribsARB;
wgl_choose_pixel_format_arb *wglChoosePixelFormatARB;
// -----------------------------------------------------------------------------

struct hot_dll {
	char* Path;
	char* PathTemp;
	HMODULE CurrentDLL;

	bool32 ProcsNeedLink;
};

struct game_code {
	hot_dll DLL;

	game_update_and_render *GameLoop;
};

struct render_code {
	hot_dll DLL;

	render::api(*Init)(window_info WindowInfo, platform::api* PlatApi, profiler_data* ProfilerData, memory_arena* PermMem, memory_arena* TransMem);
};

game_code GameCode;
render_code RenderCode;

void* GetProcAddressSafe(char* ProcName)
{
	void* Func = wglGetProcAddress(ProcName);
	if (Func == 0 ||
	        (Func == (void*)0x1) || (Func == (void*)0x2) || (Func == (void*)0x3) ||
	        (Func == (void*) - 1) ) {
		// Problems getting the proc address
		Assert(0);
	}

	return (Func);
}

real64 RandomFloat()
{
	static short fallback = 0;
	unsigned short rand;
	if (_rdrand16_step(&rand) == 0) {
		// Didn't get a random number
		rand = (fallback++);
	}

	return rand / 65536.0f;
}

void
DebugLine(int64 *Output)
{
	char NumChar[MAX_PATH] = {};
	IntToCharArray(*Output, NumChar);

	char FinalOutput[MAX_PATH] = {};
	ConcatCharArrays(NumChar, "\n", FinalOutput);
	OutputDebugString(FinalOutput);
}

void
DebugLine(int64 Output)
{
	DebugLine(&Output);
}

void
ConcatIntChar(char *CharInput, int64 IntInput, char *CharOutput)
{
	char IntInputAsChar[MAX_PATH] = {};
	IntToCharArray(IntInput, IntInputAsChar);
	ConcatCharArrays(IntInputAsChar, CharInput, CharOutput);
}

real64
CheckStickDeadzone(short Value, SHORT DeadZoneThreshold)
{
	real64 Result = 0;

	if (Value < -DeadZoneThreshold) {
		Result = (real64)(Value + DeadZoneThreshold) / (32768.0f - DeadZoneThreshold);
	} else if (Value > DeadZoneThreshold) {
		// this explicit number is pulled from my ass.
		Result = (real64)(Value  + DeadZoneThreshold) / (47467.0f - DeadZoneThreshold);
	}

	return (Result);
}

void OpenFileDialogue(char* Path, int32 MaxPath)
{
	OPENFILENAME ofn = {};
	ofn.lStructSize  = sizeof(ofn);
	ofn.hwndOwner    = NULL;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter  = NULL; // Allow anything
	ofn.lpstrFile    = Path;
	ofn.nMaxFile     = MaxPath;
	ofn.lpstrTitle   = "Select a File, yo!";
	ofn.Flags        = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	GetOpenFileNameA(&ofn);
}

void
ProcessInputState(input_state *ButtonProcessing, bool32 NewState)
{
	if (NewState) {
		if (ButtonProcessing->IsDown) {
			ButtonProcessing->OnDown = false;
		} else {
			ButtonProcessing->IsDown = true;
			ButtonProcessing->OnDown = true;
			ButtonProcessing->IsUp = false;
			ButtonProcessing->OnUp = false;
		}
	} else {
		if (ButtonProcessing->IsUp) {
			ButtonProcessing->OnUp = false;
		} else {
			ButtonProcessing->Claimed = false;
			ButtonProcessing->IsUp = true;
			ButtonProcessing->OnUp = true;
			ButtonProcessing->IsDown = false;
			ButtonProcessing->OnDown = false;
		}
	}
}

void
ProcessTriggerInput(input_state *Trigger, int32 TriggerValue)
{
	if (TriggerValue > 200) {
		ProcessInputState(Trigger, true);
	} else {
		ProcessInputState(Trigger, false);
	}
}

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

void
LoadDirectSound(HWND WindowHandle, win32_audio_output *SoundOutput, LPDIRECTSOUNDBUFFER *SoundSecondaryBuffer)
{
	HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
	if (DSoundLibrary) {
		direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

		LPDIRECTSOUND DirectSound;
		if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0))) {
			WAVEFORMATEX WaveFormat = {};
			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;

			// NOTE this is forced mono
			WaveFormat.nChannels = 1;

			WaveFormat.nSamplesPerSec = SoundOutput->SamplesPerSecond;
			WaveFormat.wBitsPerSample = 16;
			WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
			WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
			WaveFormat.cbSize = 0;

			if (SUCCEEDED(DirectSound->SetCooperativeLevel(WindowHandle, DSSCL_PRIORITY))) {
				DSBUFFERDESC BufferDescription = {};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0))) {
					if (SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat))) {
						// NOTE primary format format has been set
					} else {
						// diagnostics
					}
				} else {
					// diagnostics
				}
			} else {
				// diagnostics
			}

			//secondary buffer stuff

			DSBUFFERDESC BufferDescription = {};
			BufferDescription.dwSize = sizeof(BufferDescription);
			BufferDescription.dwFlags = 0;
			BufferDescription.dwBufferBytes = SoundOutput->SecondaryBufferSize;
			BufferDescription.lpwfxFormat = &WaveFormat;

			if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, SoundSecondaryBuffer, 0))) {
				// NOTE the secondary sound buffer was created successfully
			} else {
				// diagnostics
			}
		}
	} else {
		// didn't load the library
		// diagnostics
	}
}

void
FillSoundOutput(game_audio_output_buffer *GameAudio, win32_audio_output *SoundOutput,
                DWORD ByteToLock, DWORD BytesToWrite, LPDIRECTSOUNDBUFFER SoundSecondaryBuffer)
{
	VOID *Region1;
	DWORD Region1Size;
	VOID *Region2;
	DWORD Region2Size;

	if (SUCCEEDED(SoundSecondaryBuffer->Lock(ByteToLock, BytesToWrite,
	              &Region1, &Region1Size,
	              &Region2,  &Region2Size,
	              0))) {
		DWORD Region1SampleCount = Region1Size / SoundOutput->BytesPerSample;
		int16 *DestSample = (int16 *)Region1;
		int16 *SourceSamples = GameAudio->Samples;
		for (DWORD SampleIndex = 0;
		        SampleIndex < Region1SampleCount;
		        SampleIndex++) {
			*DestSample++ = *SourceSamples++;
			*DestSample++ = *SourceSamples++;
			SoundOutput->RunningSampleIndex++;
		}

		DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
		DestSample = (int16 *)Region2;
		for (DWORD SampleIndex = 0;
		        SampleIndex < Region2SampleCount;
		        SampleIndex++) {
			*DestSample++ = *SourceSamples++;
			*DestSample++ = *SourceSamples++;
			SoundOutput->RunningSampleIndex++;
		}

		SoundSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
	}
}

inline LARGE_INTEGER
GetWallClock()
{
	LARGE_INTEGER Count;
	QueryPerformanceCounter(&Count);
	return (Count);
}

uint64 GetWallClockGame()
{
	LARGE_INTEGER Clock = GetWallClock();
	return Clock.QuadPart;
}

inline real32
GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End, uint64 PerfCountFrequency)
{
	real32 Result = ((real32)(End.QuadPart - Start.QuadPart) / (real32)PerfCountFrequency);
	return (Result);
}

void
SaveState(char *FileName, game_memory *GameMemory)
{
	DebugLine("Saving State");

	// char FinalFileName[MAX_PATH] = {};
	// ConcatCharArrays("Saved States/", FileName, FinalFileName);
	HANDLE FileHandle = CreateFileA(FileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	// NOTE if totalsize if greater than 4gb then we must write a for loop to loop over the write file.
	DWORD BytesWritten = {};
	bool32 Success = WriteFile(FileHandle, GameMemory->GameMemoryBlock, (DWORD)GameMemory->TotalSize, &BytesWritten, 0);

	Assert(Success)
	CloseHandle(FileHandle);

	DebugLine("Save Complete");
}

void
LoadState(char *FileName, game_memory *GameMemory)
{
	DebugLine("Loading State");
	// char FinalFileName[MAX_PATH] = {};
	// ConcatCharArrays("Saved States/", FileName, FinalFileName);
	HANDLE FileHandle = CreateFileA(FileName, FILE_SHARE_READ,  0, 0, OPEN_EXISTING, 0, 0);
	DWORD BytesRead;
	bool32 Success = ReadFile(FileHandle, GameMemory->GameMemoryBlock, (DWORD)GameMemory->TotalSize, &BytesRead, 0);

	if (!Success) {
		DebugLine("Loading State File Failed");
		Assert(0);
	}
	CloseHandle(FileHandle);
	DebugLine("Load Complete");
}

// This is recursive and will include all child files also.
// Returns the end of the list
path_list* GetPathsForFileType(char* FileTypeChar, const char* RootChar, memory_arena* Memory, path_list* PathList)
{
	string FileType = FileTypeChar;
	string Root = RootChar;

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
}

read_file_result
LoadFileData(char *FileName, memory_arena *Memory)
{
	read_file_result Result = {};

	HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (FileHandle != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER FileSize;
		if (GetFileSizeEx(FileHandle, &FileSize)) {
			uint32 FileSize32 = (uint32)FileSize.QuadPart;
			Result.Contents = ArenaAllocate(Memory, FileSize32);
			// Result.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (Result.Contents) {
				DWORD BytesRead;
				if (ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0) &&
				        (FileSize32 == BytesRead)) {
					// File read successfully
					Result.ContentsSize = FileSize32;
				} else {
					VirtualFree(Result.Contents, 0, MEM_RELEASE);
					Result.Contents = 0;
				}
			} else {

			}
		} else {

		}

		CloseHandle(FileHandle);
	} else {

	}

	return (Result);
}

bool32 FileExists(char* FilePath)
{
	if (GetFileAttributes(FilePath) == INVALID_FILE_ATTRIBUTES) {
		return (false);
	}
	return (true);
}

read_file_result ReadFile(char *Path, memory_arena *Memory)
{
	read_file_result Result = LoadFileData(Path, Memory);
	return (Result);
}

void WriteFile(char *FileDestination, void *Data, uint32 DataSize)
{
	HANDLE FileHandle = CreateFileA(FileDestination, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	Assert(FileHandle != INVALID_HANDLE_VALUE);

	DWORD BytesWritten = {};
	bool32 Success = WriteFile(FileHandle, Data, DataSize, &BytesWritten, 0);

	Assert(Success);
	CloseHandle(FileHandle);
}

uint64 GetClockMS()
{
	// This gives CURRENT ms not total ms
	// Is that something we really need?? Assert here and find out


	SYSTEMTIME SystemTime = {};
	GetSystemTime(&SystemTime);
	return ((uint64)SystemTime.wMilliseconds);
}

uint32 GetGUID()
{
	UUID uuid = {};
	if (UuidCreate(&uuid) == RPC_S_OK) {
		return uuid.Data1;
	} else {
		// Error creating uuid
		return 0;
	}
}

void MakeDirectory(char* Path)
{
	CreateDirectoryA(Path, NULL);
}

void PlatformDeleteFile(char *Path)
{
	DeleteFile(Path);
}

void PlatformPrint(char* Message)
{
	// This will print to console in headless mode
	std::cout << Message << std::flush;
	std::cout << '\n' << std::flush;

	// This will print to visual studio / debugger in window mode
	DebugLine(Message);
}

uint64 GetCycle()
{
	return (__rdtsc());
}

void AppendFile (char *FileDestination, char *Data)
{
	printf(Data);
	FILE* stream;
	errno_t error = fopen_s(&stream, FileDestination, "a");

	if (stream != 0) {
		if (error == 0) {
			fprintf(stream, "%s", Data);
		} else {
			DebugLine("Error appending opening file.");
		}

		fclose(stream);
	} else {
		DebugLine("Error appending opening file.");
	}
}

inline FILETIME
GetFileWriteTime(string fileName)
{
	FILETIME LastWriteTime = {};

	WIN32_FILE_ATTRIBUTE_DATA Data;
	if (GetFileAttributesEx(fileName.CharArray, GetFileExInfoStandard, &Data)) {
		LastWriteTime = Data.ftLastWriteTime;
	}

	return (LastWriteTime);
}

void CheckHotDll(hot_dll* DLL)
{
	FILETIME NewCodeWriteTime = GetFileWriteTime(DLL->Path);
	FILETIME TempWriteTime = GetFileWriteTime(DLL->PathTemp);

	if (NewCodeWriteTime.dwLowDateTime != 0) {

		FreeLibrary(DLL->CurrentDLL);

		CopyFile(DLL->Path, DLL->PathTemp, FALSE);
		DeleteFile(DLL->Path);
		DLL->CurrentDLL = LoadLibraryA(DLL->PathTemp);
		DLL->ProcsNeedLink = true;
	}

	if (DLL->CurrentDLL == NULL) {
		DLL->CurrentDLL = LoadLibraryA(DLL->PathTemp);
		DLL->ProcsNeedLink = true;
	}
}

void
CheckSaveState(char *FilePath, input_state *ButtonChecking, bool32 SelectIsDown, game_memory *GameMemory)
{
	if (ButtonChecking->OnDown && SelectIsDown) {
		SaveState(FilePath, GameMemory);
	}
	if (ButtonChecking->OnDown && !SelectIsDown) {
		LoadState(FilePath, GameMemory);
	}
}


internal LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Message) {
		case WM_CLOSE: {
			GlobalRunning = false;
			Result = DefWindowProcA(Window, Message, WParam, LParam);
		}
		break;

		case WM_DESTROY: {
			GlobalRunning = false;
			Result = DefWindowProcA(Window, Message, WParam, LParam);
		}
		break;

		case WM_CHAR: {
			int x = 0;
		}
		break;

		default: {
			Result = DefWindowProcA(Window, Message, WParam, LParam);
		}
		break;
	}

	return (Result);
}

uint64 PlatformGetFileWriteTime(char* File)
{
	FILETIME time = GetFileWriteTime(File);
	ULARGE_INTEGER large = {};
	large.LowPart = time.dwLowDateTime;
	large.HighPart = time.dwHighDateTime;
	return large.QuadPart;
}

void CheckAllDLLs(game_memory* GameMemory, window_info WindowInfo, platform::api* PlatformWin, bool32 Headless)
{
	CheckHotDll(&GameCode.DLL);
	if (GameCode.DLL.ProcsNeedLink) {
		GameCode.DLL.ProcsNeedLink = false;

		GameCode.GameLoop = (game_update_and_render *)GetProcAddress(GameCode.DLL.CurrentDLL, "GameLoop");

		GameMemory->ImguiInitialized = false;
		GameMemory->HotReloaded = true;
	}

	// Use empty render methods for headless mode
	if (Headless) {
		//GameMemory->RenderApi = render::GetEmptyApi();
	} else {
		CheckHotDll(&RenderCode.DLL);
		if (RenderCode.DLL.ProcsNeedLink) {
			RenderCode.DLL.ProcsNeedLink = false;

			engine_state *GameStateFromMemory = (engine_state *)GameMemory->PermanentMemory.Memory;

			RenderCode.Init = (render::api(*)(window_info WindowInfo, platform::api * PlatApi, profiler_data*, memory_arena*, memory_arena*))GetProcAddress(RenderCode.DLL.CurrentDLL, "Init");
			GameMemory->RenderApi = RenderCode.Init(WindowInfo, PlatformWin, &GameStateFromMemory->OGLProfilerData, &GameMemory->PermanentMemory, &GameMemory->TransientMemory);
		}
	}
}

HWND WindowGlobal;

void OpenFileExternal(char* FileDest)
{
	ShellExecute(
	    WindowGlobal,
	    "open",
	    FileDest,
	    NULL,
	    NULL,
	    SW_SHOWNORMAL
	);
}

#define FenceWrite _WriteBarrier(); _mm_sfence();
#define FenceRead _ReadBarrier(); _mm_lfence();
#define FenceAll FenceWrite FenceRead

void TestGameThread(void* Params, int32 ThreadID)
{
	int32* NumPrinting = (int32*)Params;
	string Disp = string{"thread "} + ThreadID +  string{" : number "} + *NumPrinting + "\n";
	LOG(Disp.Array());
}

struct worker_thread_info {
	int32 ID;
	DWORD WinHandle;
	HANDLE SemaphoreHandle;
};

thread_work WorkQueue[512];
volatile uint64 NextWorkIndex = 0;
volatile uint64 WorkIndexEnd = 0;
HANDLE SemaphoreHandle = {};

thread_work* AddWork(game_thread_proc WorkMethod, void* WorkParams)
{
	thread_work* NewWork = &WorkQueue[WorkIndexEnd % ArrayCount(WorkQueue)];
	Assert(NewWork->Status != work_status::waiting && NewWork->Status != work_status::running);

	NewWork->WorkMethod = WorkMethod;
	NewWork->WorkParams = WorkParams;
	NewWork->Status = work_status::waiting;

	FenceAll;

	WorkIndexEnd++;
	ReleaseSemaphore(SemaphoreHandle, 1, 0);

	return NewWork;
}

DWORD ThreadLoop(void* lpParameter)
{
	worker_thread_info* Info = (worker_thread_info*)lpParameter;
	while (true) {
		uint64 OriginalNextIndex = NextWorkIndex;
		if (OriginalNextIndex < WorkIndexEnd) {

			uint64 ActualIndex = InterlockedCompareExchange(
			                         &NextWorkIndex,
			                         OriginalNextIndex + 1,
			                         OriginalNextIndex
			                     );
			if (ActualIndex == OriginalNextIndex) {
				FenceAll;

				thread_work * NewWork = &WorkQueue[ActualIndex % ArrayCount(WorkQueue)];
				NewWork->Status = work_status::running;

				NewWork->WorkMethod(NewWork->WorkParams, Info->ID);
				FenceAll;

				NewWork->Status = work_status::finished;
			}
		} else {
			WaitForSingleObjectEx(Info->SemaphoreHandle, INFINITE, false);
		}
	}
	return 0;
}

// TODO There are a few if(!Window) in here. Would be good to remove those.
int32 Run(int32 WindowWidth, int32 WindowHeight, HWND Window, bool32 Headless)
{
	window_info WindowInfo = {};
	// monitor is 105
	// device is 538
	WindowInfo.Width = WindowWidth;
	WindowInfo.Height = WindowHeight;
	// SetWindowPos(Window, HWND_TOP, 0, 0, WindowInfo.Width, WindowInfo.Height, NULL);

	{
		LARGE_INTEGER FrequencyLong;
		QueryPerformanceFrequency(&FrequencyLong);
		PerformanceCounterFrequency = FrequencyLong.QuadPart;
	}

	LARGE_INTEGER FlipWallClock = GetWallClock();

	// Probably need to get this from hardware instead of pulling a number out of my ass
	int32 GameUpdateHz = 60;
	real64 TargetSecondsElapsedPerFrame = 1.0f / (real64)GameUpdateHz;

	game_input GameInput = {};
	GlobalGameInput = &GameInput;


	LPVOID BaseAddress = (LPVOID)Gigabytes(2);
	game_memory GameMemory = {};
	GameMemory.PermanentMemory.Size = Megabytes(1024);
	GameMemory.TransientMemory.Size = Megabytes(1024);
	GameMemory.ThreadTransientMemory.Size = Megabytes(1024);
	GameMemory.TotalSize = GameMemory.PermanentMemory.Size + GameMemory.TransientMemory.Size + GameMemory.ThreadTransientMemory.Size;

	GameMemory.GameMemoryBlock = VirtualAlloc(BaseAddress, (SIZE_T)GameMemory.TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (GameMemory.GameMemoryBlock == NULL) {
		// Error allocating memory block
		DWORD Error = GetLastError();
		Assert(0);
	}

	GameMemory.PermanentMemory.Memory = GameMemory.GameMemoryBlock;
	GameMemory.TransientMemory.Memory = (uint8 *)GameMemory.PermanentMemory.Memory + GameMemory.PermanentMemory.Size;
	GameMemory.ThreadTransientMemory.Memory = (uint8 *)GameMemory.TransientMemory.Memory + GameMemory.TransientMemory.Size;

	GameMemory.TransientMemory.EndOfMemory = (uint8 *)GameMemory.TransientMemory.Memory + GameMemory.TransientMemory.Size;
	GameMemory.ThreadTransientMemory.EndOfMemory = (uint8 *)GameMemory.ThreadTransientMemory.Memory + GameMemory.ThreadTransientMemory.Size;
	GameMemory.PermanentMemory.EndOfMemory = (uint8 *)GameMemory.PermanentMemory.Memory + GameMemory.PermanentMemory.Size;

	GameMemory.PermanentMemory.Head = (uint8 *)GameMemory.PermanentMemory.Memory + sizeof(engine_state);


	platform::api PlatformWin = {};
	PlatformWin.MakeDirectory = &MakeDirectory;
	PlatformWin.ReadFile = &ReadFile;
	PlatformWin.FileExists = &FileExists;
	PlatformWin.WriteFile = &WriteFile;
	PlatformWin.AppendFile = &AppendFile;
	PlatformWin.DeleteFile = &PlatformDeleteFile;
	PlatformWin.GetClockMS = &GetClockMS;
	PlatformWin.QueryPerformanceCounter = &GetWallClockGame;
	PlatformWin.PerformanceCounterFrequency = PerformanceCounterFrequency;
	PlatformWin.GetCycle = &GetCycle;
	PlatformWin.GetGUID = &GetGUID;
	PlatformWin.Print = &PlatformPrint;
	PlatformWin.GetFileWriteTime = &PlatformGetFileWriteTime;
	PlatformWin.GetProcAddress = &GetProcAddressSafe;
	PlatformWin.OpenFileExternal = &OpenFileExternal;
	PlatformWin.GetPathsForFileType = &GetPathsForFileType;
	PlatformWin.OpenFileDialogue = &OpenFileDialogue;
	PlatformWin.RandomFloat = &RandomFloat;
	PlatformWin.ThreadAddWork = &AddWork;
	PlatformWin.ScreenDPI = 100;
	PlatformWin.ScreenDPICo = 1.0f;

	// Create threads
	worker_thread_info ThreadInfos[4];
	SemaphoreHandle = CreateSemaphoreEx(0, 0, ArrayCount(ThreadInfos), "semaphore_name", 0, SEMAPHORE_ALL_ACCESS);
	for (int i = 0; i < ArrayCount(ThreadInfos); i++) {
		ThreadInfos[i].ID = i;
		ThreadInfos[i].SemaphoreHandle = SemaphoreHandle;
		HANDLE Thread = CreateThread(0, 0, ThreadLoop, (void*)(&ThreadInfos[i]), 0, &ThreadInfos[i].WinHandle);
		CloseHandle(Thread);
	}

	LARGE_INTEGER PreviousFrameCount = GetWallClock();

	win32_audio_output SoundOutput = {};
	// NOTE I don't know why this * 2 is necessary. There is a bug somewhere with the audio that this fixes.
	// There are twice as many samples as we expect?
	SoundOutput.SamplesPerSecond = 48000 * 2;
	SoundOutput.RunningSampleIndex = 0;
	SoundOutput.BytesPerSample = sizeof(int16) * 2;
	SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample;
	bool SoundIsInit = false;

	int16 *AudioSamplesMemory = (int16 *)VirtualAlloc(NULL, SoundOutput.SecondaryBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	//LPDIRECTSOUNDBUFFER SoundSecondaryBuffer;
	//LoadDirectSound(Window, &SoundOutput, &SoundSecondaryBuffer);
	//SoundSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

	GameCode = {};
	GameCode.DLL.Path = "EngineCore.dll";
	GameCode.DLL.PathTemp = "EngineCore_TEMP.dll";

	RenderCode = {};
	RenderCode.DLL.Path = "render_win32_ogles3.dll";
	RenderCode.DLL.PathTemp = "render_win32_ogles3_TEMP.dll";


	bool32 KeyboardDown[256] = {};
	bool32 MouseLeftDown = false;
	bool32 MouseMiddleDown = false;
	bool32 MouseRightDown = false;
	bool32 NumberKeysDown[10] = {};


	//RECT ScreenSize = {};
	//GetWindowRect(Window, &ScreenSize);
	//WindowInfo.Width = (ScreenSize.right - ScreenSize.left);
	//WindowInfo.Height = (ScreenSize.bottom - ScreenSize.top);

	GameMemory.PlatformApi = PlatformWin;

	CheckAllDLLs(&GameMemory, WindowInfo, &PlatformWin, Headless);

	/*
	int one = 1;
	int two = 2;
	int three = 3;
	int four = 4;
	int five = 5;
	int six = 6;
	int seven = 7;
	AddWork(SemaphoreHandle, TestGameThread, (void*)&one);
	AddWork(SemaphoreHandle, TestGameThread, (void*)&two);
	AddWork(SemaphoreHandle, TestGameThread, (void*)&three);
	AddWork(SemaphoreHandle, TestGameThread, (void*)&four);
	AddWork(SemaphoreHandle, TestGameThread, (void*)&five);
	AddWork(SemaphoreHandle, TestGameThread, (void*)&six);
	AddWork(SemaphoreHandle, TestGameThread, (void*)&seven);
	*/

	GlobalRunning = true;
	while (GlobalRunning) {

		uint64 BeginCounter = __rdtsc();

		if (Window != GameNull) {
			MSG WindowMessage = {};
			while (PeekMessage(&WindowMessage, Window,  0, 0, PM_REMOVE)) {
				switch (WindowMessage.message) {
					case (WM_LBUTTONDOWN): {
						MouseLeftDown = true;
					}
					break;

					case (WM_MBUTTONDOWN): {
						MouseMiddleDown = true;
					}
					break;

					case (WM_RBUTTONDOWN): {
						MouseRightDown = true;
					}
					break;

					case (WM_LBUTTONUP): {
						MouseLeftDown = false;
					}
					break;

					case (WM_MBUTTONUP): {
						MouseMiddleDown = false;
					}
					break;

					case (WM_RBUTTONUP): {
						MouseRightDown = false;
					}
					break;

					case (WM_MOUSEWHEEL): {
						real32 ZDelta = GET_WHEEL_DELTA_WPARAM(WindowMessage.wParam);
						GlobalScrollPos = ZDelta;
					}
					break;

					case WM_CHAR: {
						// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
						if (WindowMessage.wParam > 0 && WindowMessage.wParam < 0x10000) {
							GameInput.InputChar = (unsigned short)WindowMessage.wParam;
						}
					}
					break;

					case (WM_SYSKEYDOWN): case (WM_KEYDOWN): {

						// this is so we also get WM_CHAR messages
						TranslateMessage(&WindowMessage);
						DispatchMessage(&WindowMessage);

						KeyboardDown[WindowMessage.wParam] = true;

					}
					break;

					case (WM_SYSKEYUP): case (WM_KEYUP): {

						// this is so we also get WM_CHAR messages
						TranslateMessage(&WindowMessage);
						DispatchMessage(&WindowMessage);

						KeyboardDown[WindowMessage.wParam] = false;
					}
					break;

					default: {
						TranslateMessage(&WindowMessage);
						DispatchMessage(&WindowMessage);
					}
					break;
				}
			}
		}
		for (int index = 0; index < 256; index++) {
			ProcessInputState(&GameInput.KeyboardInput[index], KeyboardDown[index]);
		}

		ProcessInputState(&GameInput.MouseLeft, MouseLeftDown);
		ProcessInputState(&GameInput.MouseRight, MouseRightDown);
		ProcessInputState(&GameInput.MouseMiddle, MouseMiddleDown);


		CheckAllDLLs(&GameMemory, WindowInfo, &PlatformWin, Headless);


		// update the window width, it might change been changed
		/*
		ScreenSize = {};
		GetWindowRect(Window, &ScreenSize);
		WindowInfo.Width = ScreenSize.right - ScreenSize.left;
		WindowInfo.Height = ScreenSize.bottom - ScreenSize.top;
		*/


		// Save state stuffs
#if 0
		CheckSaveState("StateSlot1.ts", &GameInput.R1, GameInput.Select.IsDown, &GameMemory);

		// if (GameInput.MouseLeft.OnDown)
		// {
		// 	SaveState("TestSaveState.ts", &GameMemory);
		// }
		// if (GameInput.MouseRight.OnDown)
		// {
		// 	LoadState("TestSaveState.ts", &GameMemory);
		// }
#endif


		LARGE_INTEGER AudioWallClock = GetWallClock();
		real32 FromBeginToAudioSeconds = GetSecondsElapsed(FlipWallClock, AudioWallClock, PerformanceCounterFrequency);

		game_audio_output_buffer GameAudio = {};

		/*
		DWORD PlayCursor;
		DWORD WriteCursor;
		DWORD BytesToWrite = 0;
		DWORD ByteToLock = 0;
		if (SoundSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor) == DS_OK) {
			ByteToLock = (SoundOutput.RunningSampleIndex * SoundOutput.BytesPerSample) % SoundOutput.SecondaryBufferSize;

			real32 BytesPerSecond = ((real32)(SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample));
			DWORD ExpectedSoundBytesPerFrame = (int)(BytesPerSecond / GameUpdateHz);

			DWORD TargetCursor = 0;
			TargetCursor = (WriteCursor + ExpectedSoundBytesPerFrame) % SoundOutput.SecondaryBufferSize;

			if (ByteToLock > TargetCursor) {
				BytesToWrite = (SoundOutput.SecondaryBufferSize - ByteToLock) + TargetCursor;
			} else {
				BytesToWrite = TargetCursor - ByteToLock;
			}

			GameAudio.SamplesPerSecond = SoundOutput.SamplesPerSecond;
			GameAudio.SampleCount = BytesToWrite / SoundOutput.BytesPerSample;
			GameAudio.Samples = AudioSamplesMemory;
			GameAudio.BytesPerSample = SoundOutput.BytesPerSample;
		}
		*/

		engine_state *GameStateFromMemory = (engine_state *)GameMemory.PermanentMemory.Memory;

		// Getting mouse position
		if (Window != GameNull) {
			POINT CursorInfo = {};
			GetCursorPos(&CursorInfo);
			ScreenToClient(Window, &CursorInfo);

			vector2 PrevPos = GameInput.MousePos;
			GameInput.MousePos = vector2{(real64)(CursorInfo.x), (real64)(CursorInfo.y)};

			static bool FirstLock = true;
			static vector2 LockPos = {};
			if (GameStateFromMemory->LockCursor) {
				if (FirstLock) {
					LockPos = GameInput.MousePos;
				}
				FirstLock = false;


				POINT NewCursor = {};
				NewCursor.x = (LONG)LockPos.X;
				NewCursor.y = (LONG)LockPos.Y;
				ClientToScreen(Window, &NewCursor);
				SetCursorPos(NewCursor.x, NewCursor.y);
				GameInput.MouseDelta = vector2{(int)LockPos.X - GameInput.MousePos.X, (int)LockPos.Y - GameInput.MousePos.Y};
			} else {
				FirstLock = true;
				GameInput.MouseDelta = vector2{PrevPos.X - GameInput.MousePos.X, PrevPos.Y - GameInput.MousePos.Y};
			}

			// Emulate touches
			GameInput.TouchPosition[0] = GameInput.MousePos;
			ProcessInputState(&GameInput.TouchStatus[0], GameInput.MouseLeft.IsDown);
		}

		// SYSTEMTIME SystemTime = {};
		// GetSystemTime(&SystemTime);
		// GameStateFromMemory->StateSerializing.RandomPackage.RandomGenState += SystemTime.wMilliseconds + SystemTime.wSecond + SystemTime.wMinute +
		//         SystemTime.wDay + SystemTime.wMonth + SystemTime.wYear;
		// if (GameStateFromMemory->StateSerializing.RandomPackage.RandomGenState > 100000) {
		// 	GameStateFromMemory->StateSerializing.RandomPackage.RandomGenState = 0;
		// }

		GameInput.MouseScrollDelta = 0.0f;
		GameInput.ScrollClaimed = false;
		if (GlobalScrollPos != 0.0f) {
			GameInput.MouseScrollDelta = GlobalScrollPos;
			GlobalScrollPos = 0.0f;
		}

		uint64 LogicCyclesBegin = __rdtsc();

		GameCode.GameLoop(&GameMemory, &GameInput, &WindowInfo, &GameAudio, "T:/Game/Assets/");

		// FillSoundOutput(&GameAudio, &SoundOutput, ByteToLock, BytesToWrite, SoundSecondaryBuffer);
		//GameStateFromMemory = (engine_state *)GameMemory.PermanentMemory.Memory;
		GameStateFromMemory->LogicCycles = __rdtsc() - LogicCyclesBegin;
		GameMemory.HotReloaded = false;


		state_to_serialize* State = &GameStateFromMemory->StateSerializing;
		if (BuildVarAutomatedTesting) {
			if (State->TestingDone) {
				char* ResultsPath = "T:/Crossbow/build/TestingResults.txt";
				PlatformDeleteFile(ResultsPath);
				AppendFile(ResultsPath, "guddderanother \n");
				return 0;
			}
		}

		uint64 RenderingCyclesBegin = __rdtsc();
		if (GlobalRunning) {
			GameMemory.RenderApi.Render(&GameMemory.RenderApi, State->ActiveCam, &State->Light.Cam, &WindowInfo,
			                            &GameStateFromMemory->DebugUIRenderer, &GameStateFromMemory->UIRenderer, &GameStateFromMemory->GameRenderer,
			                            &GameStateFromMemory->Assets->GaussianBlurShader);
		}
		if (Window != GameNull) {
			HDC DC = GetDC(Window);

			if (DC == NULL) {
				int x = 0;
			}

			//SwapBuffers(DC);
			if (!wglSwapLayerBuffers(DC, WGL_SWAP_MAIN_PLANE)) {
				DWORD dw = GetLastError();
				int x = 0;
			}

			if (DC == NULL) {
				int x = 0;
			}

			ReleaseDC(Window, DC);

			if (DC == NULL) {
				int x = 0;
			}
		}
		GameStateFromMemory->CyclesPlatformRendering = __rdtsc() - RenderingCyclesBegin;


		/*
		{
			static int proc = 0;
			static int procPrev = 0;
			proc = GetCurrentProcessorNumber();
			if (procPrev != proc) {
				//int x = 0;x
				DebugLine("SWITCH");
			}
			procPrev = proc;
		}
		*/


		LARGE_INTEGER WorkFrameCount = GetWallClock();
		int64 ElapsedFrameCount = WorkFrameCount.QuadPart - PreviousFrameCount.QuadPart;

		real64 SecondsElapsedForWork = (real64)ElapsedFrameCount / (real64)PerformanceCounterFrequency;
		real64 SecondsElapsedForFrame = SecondsElapsedForWork;

		uint64 TSCBeforeWait = __rdtsc();
		while (SecondsElapsedForFrame < TargetSecondsElapsedPerFrame) {
			LARGE_INTEGER NewWorkFrameCount = GetWallClock();
			SecondsElapsedForFrame = (((real64)NewWorkFrameCount.QuadPart - PreviousFrameCount.QuadPart) /
			                          (real64)PerformanceCounterFrequency);
		}
		GameStateFromMemory->CyclesUsedWaiting = __rdtsc() - TSCBeforeWait;
		WorkFrameCount = GetWallClock();

		ElapsedFrameCount = WorkFrameCount.QuadPart - PreviousFrameCount.QuadPart;
		real64 MSThisFrame = ((1000.0f * (real64)ElapsedFrameCount) / (real64)PerformanceCounterFrequency);
		GameStateFromMemory->DeltaTimeMS = MSThisFrame;

		int64 FPS = PerformanceCounterFrequency / ElapsedFrameCount;
		char charFPS[MAX_PATH] = {};
		ConcatIntChar(FPS, " FPS", charFPS);
		//DebugLine(charFPS);
		GameStateFromMemory->PrevFrameFPS = FPS;

		PreviousFrameCount = WorkFrameCount;
		GameMemory.ElapsedCycles = PreviousFrameCount.QuadPart;

		FlipWallClock = GetWallClock();

		GameStateFromMemory->TotalCyclesUsedLastFrame = __rdtsc() - BeginCounter;
	}

	return 0;
}

/*
int main()
{
	printf("Starting automated testing in headless mode \n");
	return Run(0, 0, GameNull, true);
}
*/


int CALLBACK
WinMain(_In_ HINSTANCE Instance,
        _In_opt_ HINSTANCE PrevInstance,
        _In_ LPSTR CommandLine,
        _In_ int ShowCode)
{

	int32 WindowWidth = 1600;
	int32 WindowHeight = 900;

	WNDCLASSA WindowClass = {};

	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
	//    WindowClass.hIcon;
	WindowClass.lpszClassName = "TowerWindowClass";
	if (RegisterClassA(&WindowClass)) {
		HWND Window =
		    CreateWindowExA(
		        0,
		        WindowClass.lpszClassName,
		        "White Mocha",
		        WS_VISIBLE | WS_BORDER | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
		        10,
		        10,
		        WindowWidth,
		        WindowHeight,
		        0,
		        0,
		        Instance,
		        0);
		if (Window) {
			WindowGlobal = Window;

			// Setup opengl window
			{
				// Use dummy dc to get the proc addresses that we need for the real one
				{
					WNDCLASSA GLWindowLoaderClass = {};

					GLWindowLoaderClass.lpfnWndProc = DefWindowProcA;
					GLWindowLoaderClass.hInstance = GetModuleHandle(0);
					GLWindowLoaderClass.lpszClassName = "HandmadeWGLLoader";

					if (RegisterClassA(&GLWindowLoaderClass)) {
						HWND GLWindowLoader = CreateWindowExA(
						                          0,
						                          GLWindowLoaderClass.lpszClassName,
						                          "Handmade Hero",
						                          0,
						                          CW_USEDEFAULT,
						                          CW_USEDEFAULT,
						                          CW_USEDEFAULT,
						                          CW_USEDEFAULT,
						                          0,
						                          0,
						                          GLWindowLoaderClass.hInstance,
						                          0);


						HDC DummyContext = GetDC(GLWindowLoader);

						PIXELFORMATDESCRIPTOR DesiredPixelFormat = {};
						DesiredPixelFormat.nSize = sizeof(DesiredPixelFormat);
						DesiredPixelFormat.nVersion = 1;
						DesiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
						DesiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
						DesiredPixelFormat.cColorBits = 32;
						DesiredPixelFormat.cAlphaBits = 8;
						DesiredPixelFormat.cDepthBits = 24;
						DesiredPixelFormat.cStencilBits = 8;
						DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

						int SuggestedPixelFormatIndex = ChoosePixelFormat(DummyContext, &DesiredPixelFormat);
						if (!SuggestedPixelFormatIndex) {
							Assert(0);
						}
						//PIXELFORMATDESCRIPTOR SuggestedPixelFormat = {};
						//DescribePixelFormat(DummyContext, SuggestedPixelFormatIndex, sizeof(SuggestedPixelFormat), &SuggestedPixelFormat);
						if (!SetPixelFormat(DummyContext, SuggestedPixelFormatIndex, &DesiredPixelFormat)) {
							Assert(0);
						}

						HGLRC DummyOpenGLRC = wglCreateContext(DummyContext);
						if (wglMakeCurrent(DummyContext, DummyOpenGLRC)) {
							wglChoosePixelFormatARB = (wgl_choose_pixel_format_arb *)GetProcAddressSafe("wglChoosePixelFormatARB");
							wglCreateContextAttribsARB = (wgl_create_context_attribs_arb *)GetProcAddressSafe("wglCreateContextAttribsARB");

							wglMakeCurrent(DummyContext, 0);
						} else {
							Assert(0);
						}

						wglDeleteContext(DummyOpenGLRC);
						ReleaseDC(GLWindowLoader, DummyContext);
						DestroyWindow(GLWindowLoader);
					}
				}


				// Init open gl
				HDC DeviceContext = GetDC(Window);


				int IntAttribList[] = {
					WGL_DRAW_TO_WINDOW_ARB, 			GL_TRUE, // 0
					WGL_SUPPORT_OPENGL_ARB, 			GL_TRUE, // 2
					WGL_DOUBLE_BUFFER_ARB, 				GL_TRUE, // 3
					WGL_ACCELERATION_ARB, 				WGL_FULL_ACCELERATION_ARB, // 1
					WGL_PIXEL_TYPE_ARB, 				WGL_TYPE_RGBA_ARB, // 4
					//WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, 	GL_TRUE, // 5
					WGL_COLOR_BITS_ARB,         		32,
					WGL_DEPTH_BITS_ARB,         		24,
					WGL_STENCIL_BITS_ARB,       		8,
					0,
				};


				GLuint ExtendedPick = 0;
				int SuggestedPixelFormatIndex = 0;
				wglChoosePixelFormatARB(DeviceContext, IntAttribList, 0, 1, &SuggestedPixelFormatIndex, &ExtendedPick);
				if (!ExtendedPick) {
					Assert(0);
				}

				PIXELFORMATDESCRIPTOR SuggestedPixelFormat = {};
				DescribePixelFormat(DeviceContext, SuggestedPixelFormatIndex, sizeof(SuggestedPixelFormat), &SuggestedPixelFormat);
				if (!SetPixelFormat(DeviceContext, SuggestedPixelFormatIndex, &SuggestedPixelFormat)) {
					Assert(0);
				}

				int Win32OpenGLAttribs[] = {
					WGL_CONTEXT_MAJOR_VERSION_ARB, 	3,
					WGL_CONTEXT_MINOR_VERSION_ARB, 	3,
					//WGL_CONTEXT_FLAGS_ARB, 			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
					WGL_CONTEXT_PROFILE_MASK_ARB, 	WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
					0,
				};
				HGLRC OpenGLRC = wglCreateContextAttribsARB(DeviceContext, 0, Win32OpenGLAttribs);
				if (wglMakeCurrent(DeviceContext, OpenGLRC)) {
					//const GLubyte *glVersion = glGetString(GL_VERSION);
				} else {
					Assert(0);
				}

				ReleaseDC(Window, DeviceContext);

			}


			// Hide title bar
			SetWindowLong(Window, GWL_STYLE, 0);
			ShowWindow(Window, SW_SHOW); //display window

			Run(WindowWidth, WindowHeight, Window, false);
		}

		DestroyWindow(Window);
	}

	return 1;
}