#include <limits>
#define EM_LOG_C_STACK 1

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <emscripten.h>
#include <emscripten/console.h>
#include <emscripten/html5.h>
#include <emscripten/fetch.h>

#include <sys/time.h>
#include <filesystem>
#include <chrono>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <semaphore.h>
#include <pthread.h>

#define WIN_EXPORT
#define CRASH __builtin_trap();

#include "T:/Game/code/Engine/EngineCore.h"
#include "T:/Game/code/Engine/EngineCore.cpp"
#include "T:/Game/code/Platform/render_emscripten_ogles3.cpp"

void Print(char* Message)
{
	printf("%s", Message);
	printf("\n");
}

void PrintError(char* Message) {
	emscripten_console_error(Message);	
}

real64 RandomFloat()
{
	return emscripten_random();
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
	fseek(File, 0, SEEK_SET);

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
	FILE* FileHandle = fopen(FileDestination, "w");
	if (FileHandle != NULL) {
		int32 ElementsWritten = fwrite(Data, DataSize, 1, FileHandle);
		if (ElementsWritten != 1) { 
			fprintf(stderr, "Error writing file. %s \n", FileDestination);
		} else {
			printf("%i bytes written to %s \n", (int)(ElementsWritten * DataSize), FileDestination);
		}
		fclose(FileHandle);
	} else {
		printf("Could not write file %s \n", FileDestination);
	}
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

#define FenceWrite asm volatile("" ::: "memory");
#define FenceRead asm volatile("" ::: "memory");
#define FenceAll FenceWrite FenceRead

struct worker_thread_info {
	int32 ID;
	pthread_t ThreadHandle;
};

void TestGameThread(void* Params, int32 ThreadID)
{
	int32* NumPrinting = (int32*)Params;
	string Disp = string{"thread "} + ThreadID +  string{" : number "} + *NumPrinting + "\n";
	Print(Disp.Array());
}

#undef Status

thread_work WorkQueue[512];
volatile uint64 NextWorkIndex = 0;
volatile uint64 WorkIndexEnd = 0;
sem_t SemaphoreHandle = {};
pthread_mutex_t CountMutex = {};

thread_work* AddWork(game_thread_proc WorkMethod, void* WorkParams)
{
	thread_work* NewWork = &WorkQueue[WorkIndexEnd % ArrayCount(WorkQueue)];
	Assert(NewWork->Status != work_status::waiting && NewWork->Status != work_status::running);

	NewWork->WorkMethod = WorkMethod;
	NewWork->WorkParams = WorkParams;
	NewWork->Status = work_status::waiting;

	FenceAll;

	pthread_mutex_lock(&CountMutex);
	WorkIndexEnd++;
	pthread_mutex_unlock(&CountMutex);

	sem_post(&SemaphoreHandle);

	return NewWork;
}

void *ThreadLoop(void* lpParameter)
{
	worker_thread_info* Info = (worker_thread_info*)lpParameter;

	string Val = "Thread Starting " + string{Info->ID};
	Print(Val.Array());

	while (true) {
		uint64 AwakeWorkIndex = NextWorkIndex;
		if (AwakeWorkIndex < WorkIndexEnd) {

			pthread_mutex_lock(&CountMutex);
			uint64 AtomicWorkIndex = NextWorkIndex;

			if (AtomicWorkIndex == AwakeWorkIndex) {
				NextWorkIndex++;
				pthread_mutex_unlock(&CountMutex);

				FenceAll;

				thread_work * NewWork = &WorkQueue[AtomicWorkIndex % ArrayCount(WorkQueue)];
				NewWork->Status = work_status::running;

				NewWork->WorkMethod(NewWork->WorkParams, Info->ID);
				FenceAll;

				NewWork->Status = work_status::finished;
			} else {
				pthread_mutex_unlock(&CountMutex);
			}
		} else {
			sem_wait(&SemaphoreHandle);
		}
	}

	return NULL;
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
	r64 Num = emscripten_random();
	uint32 n = (uint32)(Num * 4294967.0f);
	return n;
}

void* GetProcAddress(char* ProcName)
{
	Print("GetProcAddress - UNSUPPORTED");
	void* Ret;
	return Ret;
}

// This is recursive and will include all child files also.
// Returns the end of the list
path_list* GetPathsForFileType(char* FileTypeChar, const char* RootDir, memory_arena* Memory, path_list* PathList)
{
	path_list* NextPath = PathList;

	std::string path = RootDir;
	for (const auto & entry : std::filesystem::directory_iterator(path)) {

		std::string Path = entry.path().string();

		if (entry.is_directory()) {
			NextPath = GetPathsForFileType(FileTypeChar, Path.c_str(), Memory, NextPath);
		} else {

			string MyStr = Path.c_str();
			if (StringEndsWith(MyStr, FileTypeChar)) {
				NextPath->Path = Path.c_str();
				NextPath->Next = (path_list*)ArenaAllocate(Memory, sizeof(path_list));
				NextPath = NextPath->Next;
			}
		}
	}

	return NextPath;
}

engine_state* GlobalEngineState = {};

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

void Render(render::api * API,
             camera * ActiveCam, window_info * WindowInfo,
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

network_call_result SendNetworkCall(memory_arena* Memory) { 
	network_call_result Ret = {};

	return Ret;
}

game_memory GameMemory = {};
game_input GameInput = {};
game_audio_output_buffer GameAudio = {};
window_info WindowInfo = {};

std::chrono::time_point<std::chrono::high_resolution_clock> PrevClock;

bool32 MouseLeftState = false;
bool32 MouseRightState = false;
bool32 MouseMiddleState = false;
bool32 KeyboardState[256] = {};
bool32 FunctionKeysDown[10];
bool32 EscapeState = false;

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

EM_BOOL KeyCallback(int eventType, const EmscriptenKeyboardEvent *e, void *userData)
{
	//printf("Keyboard event %i button %s\n", eventType, e->key);

	if (std::strlen(e->key) == 1) {
		if (eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
			if (e->key[0] == 'a') { KeyboardState['a'] = true; } 	if (e->key[0] == 'A') { KeyboardState['A'] = true; }
			if (e->key[0] == 'b') { KeyboardState['b'] = true; } 	if (e->key[0] == 'B') { KeyboardState['B'] = true; }
			if (e->key[0] == 'c') { KeyboardState['c'] = true; } 	if (e->key[0] == 'C') { KeyboardState['C'] = true; }
			if (e->key[0] == 'd') { KeyboardState['d'] = true; } 	if (e->key[0] == 'D') { KeyboardState['D'] = true; }
			if (e->key[0] == 'e') { KeyboardState['e'] = true; } 	if (e->key[0] == 'E') { KeyboardState['E'] = true; }
			if (e->key[0] == 'f') { KeyboardState['f'] = true; } 	if (e->key[0] == 'F') { KeyboardState['F'] = true; }
			if (e->key[0] == 'g') { KeyboardState['g'] = true; } 	if (e->key[0] == 'G') { KeyboardState['G'] = true; }
			if (e->key[0] == 'h') { KeyboardState['h'] = true; } 	if (e->key[0] == 'H') { KeyboardState['H'] = true; }
			if (e->key[0] == 'i') { KeyboardState['i'] = true; } 	if (e->key[0] == 'I') { KeyboardState['I'] = true; }
			if (e->key[0] == 'j') { KeyboardState['j'] = true; } 	if (e->key[0] == 'J') { KeyboardState['J'] = true; }
			if (e->key[0] == 'k') { KeyboardState['k'] = true; } 	if (e->key[0] == 'K') { KeyboardState['K'] = true; }
			if (e->key[0] == 'l') { KeyboardState['l'] = true; } 	if (e->key[0] == 'L') { KeyboardState['L'] = true; }
			if (e->key[0] == 'm') { KeyboardState['m'] = true; } 	if (e->key[0] == 'M') { KeyboardState['M'] = true; }
			if (e->key[0] == 'n') { KeyboardState['n'] = true; } 	if (e->key[0] == 'N') { KeyboardState['N'] = true; }
			if (e->key[0] == 'o') { KeyboardState['o'] = true; } 	if (e->key[0] == 'O') { KeyboardState['O'] = true; }
			if (e->key[0] == 'p') { KeyboardState['p'] = true; } 	if (e->key[0] == 'P') { KeyboardState['P'] = true; }
			if (e->key[0] == 'q') { KeyboardState['q'] = true; } 	if (e->key[0] == 'Q') { KeyboardState['Q'] = true; }
			if (e->key[0] == 'r') { KeyboardState['r'] = true; } 	if (e->key[0] == 'R') { KeyboardState['R'] = true; }
			if (e->key[0] == 's') { KeyboardState['s'] = true; } 	if (e->key[0] == 'S') { KeyboardState['S'] = true; }
			if (e->key[0] == 't') { KeyboardState['t'] = true; } 	if (e->key[0] == 'T') { KeyboardState['T'] = true; }
			if (e->key[0] == 'u') { KeyboardState['u'] = true; } 	if (e->key[0] == 'U') { KeyboardState['U'] = true; }
			if (e->key[0] == 'v') { KeyboardState['v'] = true; } 	if (e->key[0] == 'V') { KeyboardState['V'] = true; }
			if (e->key[0] == 'w') { KeyboardState['w'] = true; } 	if (e->key[0] == 'W') { KeyboardState['W'] = true; }
			if (e->key[0] == 'x') { KeyboardState['x'] = true; } 	if (e->key[0] == 'X') { KeyboardState['X'] = true; }
			if (e->key[0] == 'y') { KeyboardState['y'] = true; } 	if (e->key[0] == 'Y') { KeyboardState['Y'] = true; }
			if (e->key[0] == 'z') { KeyboardState['z'] = true; } 	if (e->key[0] == 'Z') { KeyboardState['Z'] = true; }



		} else if (eventType == EMSCRIPTEN_EVENT_KEYUP) {
			if (e->key[0] == 'a') { KeyboardState['a'] = false; }  	if (e->key[0] == 'A') { KeyboardState['A'] = false; }
			if (e->key[0] == 'b') { KeyboardState['b'] = false; }  	if (e->key[0] == 'B') { KeyboardState['B'] = false; }
			if (e->key[0] == 'c') { KeyboardState['c'] = false; }  	if (e->key[0] == 'C') { KeyboardState['C'] = false; }
			if (e->key[0] == 'd') { KeyboardState['d'] = false; }  	if (e->key[0] == 'D') { KeyboardState['D'] = false; }
			if (e->key[0] == 'e') { KeyboardState['e'] = false; }  	if (e->key[0] == 'E') { KeyboardState['E'] = false; }
			if (e->key[0] == 'f') { KeyboardState['f'] = false; }  	if (e->key[0] == 'F') { KeyboardState['F'] = false; }
			if (e->key[0] == 'g') { KeyboardState['g'] = false; }  	if (e->key[0] == 'G') { KeyboardState['G'] = false; }
			if (e->key[0] == 'h') { KeyboardState['h'] = false; }  	if (e->key[0] == 'H') { KeyboardState['H'] = false; }
			if (e->key[0] == 'i') { KeyboardState['i'] = false; }  	if (e->key[0] == 'I') { KeyboardState['I'] = false; }
			if (e->key[0] == 'j') { KeyboardState['j'] = false; }  	if (e->key[0] == 'J') { KeyboardState['J'] = false; }
			if (e->key[0] == 'k') { KeyboardState['k'] = false; }  	if (e->key[0] == 'K') { KeyboardState['K'] = false; }
			if (e->key[0] == 'l') { KeyboardState['l'] = false; }  	if (e->key[0] == 'L') { KeyboardState['L'] = false; }
			if (e->key[0] == 'm') { KeyboardState['m'] = false; }  	if (e->key[0] == 'M') { KeyboardState['M'] = false; }
			if (e->key[0] == 'n') { KeyboardState['n'] = false; }  	if (e->key[0] == 'N') { KeyboardState['N'] = false; }
			if (e->key[0] == 'o') { KeyboardState['o'] = false; }  	if (e->key[0] == 'O') { KeyboardState['O'] = false; }
			if (e->key[0] == 'p') { KeyboardState['p'] = false; }  	if (e->key[0] == 'P') { KeyboardState['P'] = false; }
			if (e->key[0] == 'q') { KeyboardState['q'] = false; }  	if (e->key[0] == 'Q') { KeyboardState['Q'] = false; }
			if (e->key[0] == 'r') { KeyboardState['r'] = false; }  	if (e->key[0] == 'R') { KeyboardState['R'] = false; }
			if (e->key[0] == 's') { KeyboardState['s'] = false; }  	if (e->key[0] == 'S') { KeyboardState['S'] = false; }
			if (e->key[0] == 't') { KeyboardState['t'] = false; }  	if (e->key[0] == 'T') { KeyboardState['T'] = false; }
			if (e->key[0] == 'u') { KeyboardState['u'] = false; }  	if (e->key[0] == 'U') { KeyboardState['U'] = false; }
			if (e->key[0] == 'v') { KeyboardState['v'] = false; }  	if (e->key[0] == 'V') { KeyboardState['V'] = false; }
			if (e->key[0] == 'w') { KeyboardState['w'] = false; }  	if (e->key[0] == 'W') { KeyboardState['W'] = false; }
			if (e->key[0] == 'x') { KeyboardState['x'] = false; }  	if (e->key[0] == 'X') { KeyboardState['X'] = false; }
			if (e->key[0] == 'y') { KeyboardState['y'] = false; }  	if (e->key[0] == 'Y') { KeyboardState['Y'] = false; }
			if (e->key[0] == 'z') { KeyboardState['z'] = false; }  	if (e->key[0] == 'Z') { KeyboardState['Z'] = false; }
		}
	} else {
		string Key = e->key;

		if (Key == string{"Tab"}) {
			KeyboardState[KEY_TAB] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
		} else if (Key == string{"F1"}) {
			FunctionKeysDown[1] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
			return true;
		} else if (Key == string{"F2"}) {
			FunctionKeysDown[2] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
			return true;
		} else if (Key == string{"F3"}) {
			FunctionKeysDown[3] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
			return true;
		} else if (Key == string{"F4"}) {
			FunctionKeysDown[4] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
			return true;
		} else if (Key == string{"F5"}) {
			FunctionKeysDown[5] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
			return true;
		} else if (Key == string{"F6"}) {
			FunctionKeysDown[6] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
			return true;
		} else if (Key == string{"F7"}) {
			FunctionKeysDown[7] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
			return true;
		} else if (Key == string{"F8"}) {
			FunctionKeysDown[8] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
			return true;
		} else if (Key == string{"F9"}) {
			FunctionKeysDown[9] = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
			return true;
		} else if (Key == string{"Escape"}) {
			EscapeState = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
		}
	}

	/*
	if (ArrayCount(KeyboardState) > (int)e->key) {
		if (eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
			KeyboardState[(int)e->key] = true;
		} else if (eventType == EMSCRIPTEN_EVENT_KEYDOWN) {
			KeyboardState[(int)e->key] = false;
		}
	} else {
		printf("Unknown key %s \n", e->key);
	}
	*/

	return false;
}

EM_BOOL MouseWheelCallback(int eventType, const EmscriptenWheelEvent  *e, void *userData)
{
	if (e->deltaY > 0) {
		GameInput.MouseScrollDelta = -1;
	} else {
		GameInput.MouseScrollDelta = 1;
	}
	return false;
}

EM_BOOL MouseCallback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
	//printf("Mouse event %i button %i x:%i y:%i \n", eventType, e->button, (int)e->clientX, (int)e->clientY);
	if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN) {
		if (e->button == 0) {
			MouseLeftState = true;
		} else if (e->button == 1) {
			MouseMiddleState = true;
		} else if (e->button == 2) {
			MouseRightState = true;
		}
	} else if (eventType == EMSCRIPTEN_EVENT_MOUSEUP) {
		if (e->button == 0) {
			MouseLeftState = false;
		} else if (e->button == 1) {
			MouseMiddleState = false;
		} else if (e->button == 2) {
			MouseRightState = false;
		}
	} else if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE) {
		GameInput.MousePos = vector2{(real64)e->clientX, (real64)e->clientY};
	}

	return false;
}

EGLDisplay GLDisplay = {};
EGLSurface GLSurface = {};

bool FileSystemReady = false;
b32 DidResize = false;

#include "platform_emscripten.h"

static EM_BOOL ResizedCallback(int event_type, const EmscriptenUiEvent *event, void *user_data) {
	DidResize = true;
	return 0;
}

void downloadSucceeded(emscripten_fetch_t *fetch) {
	printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);

	/*
	for (int i = 0; i < fetch->numBytes - 1; i++) {
		printf("%c", fetch->data[i]);
	}
	printf("\n");
	*/

	emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadFailed(emscripten_fetch_t *fetch) {
	printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
	emscripten_fetch_close(fetch); // Also free data on failure.
}

void SendEvent(string EventName, uint32 UserID) {
	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "POST");
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;

	const char* headers[] = {"accept", "application/json", NULL};
	attr.requestHeaders = headers;
	attr.onsuccess = downloadSucceeded;
	attr.onerror = downloadFailed;

	string Token = "5e61c8224851fa1166f68fa342367ad0";
	string UserIDStr = string{UserID};
	string InsertID = string{GetGUID()};
	string Data = string{"[{\"event\":\"" + EventName + "\", \"properties\":{ \"token\":\"" + Token + "\", \"distinct_id\":\"" + UserIDStr + "\", \"$insert_id\":\""} + InsertID + string{"\"} }]"};

	// !!!! not freed !!!! leak 
	char* MalData = (char*)malloc(StringLength(Data));
	MemoryCopy(MalData, Data.Array(), StringLength(Data));

	Print("Sent call");
	Print(Data.Array());

	attr.requestData = MalData;
	attr.requestDataSize = StringLength(Data);

	emscripten_fetch_t *fetch = emscripten_fetch(&attr, "https://api.mixpanel.com/track");
}

/*
void FileSystemDidSync() { 
	printf("updating");
	GlobalEngineState->FileSynced = true;
	printf("done");
}
*/

void MainLoop()
{
	// Update input states
	{
		ProcessInputState(&GameInput.MouseLeft, MouseLeftState);
		ProcessInputState(&GameInput.MouseMiddle, MouseMiddleState);
		ProcessInputState(&GameInput.MouseRight, MouseRightState);
		ProcessInputState(&GameInput.Escape, EscapeState);
		Assert(ArrayCount(FunctionKeysDown) == ArrayCount(GameInput.FunctionKeys));
		for (int index = 0; index < ArrayCount(FunctionKeysDown); index++) {
			ProcessInputState(&GameInput.FunctionKeys[index], FunctionKeysDown[index]);
		}
		for (int index = 0; index < ArrayCount(GameInput.KeyboardInput); index++) {
			ProcessInputState(&GameInput.KeyboardInput[index], KeyboardState[index]);
		}
	}

	// Window resizing
	if (DidResize) {
		DidResize = false;

		double width, height;
		emscripten_get_element_css_size("canvas", &width, &height);

		WindowInfo.Width = (int)width;
		WindowInfo.Height = (int)height;
	}

	if (FileSystemReady) {

		// app start event
		{
			static bool First = true;
			if (First) { 
				First = false;
				uint32 UserID = GetGUID();

				char* FilePath = "SpaceUnknown/UserGUID.dat"; 
				if (FileExists(FilePath)) {
					read_file_result Result = PlatformApi.ReadFile(FilePath, &GameMemory.TransientMemory);
					UserID = *((uint32*)Result.Contents);
					printf("Found existing UserID \n");
				} else {
					WriteFile(FilePath, (void*)(&UserID), sizeof(uint32));
				}

				printf("Send app start event UserID %i \n", UserID);
				SendEvent("app_start", UserID);
			}
		}

		GameLoop(&GameMemory, &GameInput, &WindowInfo, &GameAudio, "T:/Game/assets/");

		engine_state *GameStateFromMemory = (engine_state *)GameMemory.PermanentMemory.Memory;
		state_to_serialize* State = &GameStateFromMemory->StateSerializing;

		GlobalEngineState = GameStateFromMemory;

		GameMemory.RenderApi.Render(&GameMemory.RenderApi, State->ActiveCam, &WindowInfo, &GameStateFromMemory->DebugUIRenderer, &GameStateFromMemory->UIRenderer, &GameStateFromMemory->GameRenderer, &GameStateFromMemory->Assets->GaussianBlurShader);
		eglSwapBuffers(GLDisplay, GLSurface);

		auto CurrClock = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> ClockDiff = CurrClock - PrevClock;
		PrevClock = CurrClock;

		float FPS = 1.0f / (ClockDiff.count());
		GameStateFromMemory->DeltaTimeMS = ClockDiff.count() * 1000.0f;
		GameStateFromMemory->PrevFrameFPS = FPS;

		GameInput.MouseScrollDelta = 0.0f;

		if (GameStateFromMemory->DidSave) {
			GameStateFromMemory->DidSave = false;
			GameStateFromMemory->FileSystemSyncing = true;

			EM_ASM (
				FS.syncfs(false, 
					function(err) {
						console.log("File system synced");
						ccall('FileSystemDidSync', 'v');
					}
				);
			);

		}
	}
}

// https://github.com/emscripten-core/emscripten/blob/main/test/third_party/glbook/Common/esUtil.h
// esCreateWindow flag - RGB color buffer
#define ES_WINDOW_RGB           0
// esCreateWindow flag - ALPHA color buffer
#define ES_WINDOW_ALPHA         1
// esCreateWindow flag - depth buffer
#define ES_WINDOW_DEPTH         2
// esCreateWindow flag - stencil buffer
#define ES_WINDOW_STENCIL       4
// esCreateWindow flat - multi-sample buffer
#define ES_WINDOW_MULTISAMPLE   8


int main()
{
	Print("Starting");


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

	GameMemory.PermanentMemory.Head = (uint8 *)GameMemory.PermanentMemory.Memory + sizeof(engine_state);
	Print("Memory Allocated");

	Print("Setup file system");
    EM_ASM (
        FS.mkdir('/SpaceUnknown');
        FS.mount(IDBFS, {}, '/SpaceUnknown');
        FS.syncfs(true, function (err) {
			assert(!err);
			ccall('FileSystemCreated', 'v');
        });
    );


	Print("Start thread queue");
	worker_thread_info ThreadInfos[4];
	for (int i = 0; i < ArrayCount(ThreadInfos); i++) {
		ThreadInfos[i].ID = i;

		int Succ = pthread_create(&ThreadInfos[i].ThreadHandle, NULL, &ThreadLoop, (void*)(&ThreadInfos[i]));
		if (Succ != 0) {
			string Ret = "Error creating thread " + string{Succ};
			Print(Ret.Array());
			Assert(false);
		}
	}


	/*
	// --------------------
	int one = 1;
	int two = 2;
	int three = 3;
	int four = 4;
	int five = 5;
	int six = 6;
	int seven = 7;
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&two);
	AddWork(TestGameThread, (void*)&three);
	AddWork(TestGameThread, (void*)&four);
	AddWork(TestGameThread, (void*)&five);
	AddWork(TestGameThread, (void*)&six);
	AddWork(TestGameThread, (void*)&seven);
	// --------------------
	*/
	

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
	PlatformEm.SendNetworkCall = &SendNetworkCall;

	PlatformEm.ScreenDPI = 100;
	PlatformEm.ScreenDPICo = 1.0f;

	GameMemory.PlatformApi = PlatformEm;
	PlatformApi = GameMemory.PlatformApi;

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

	emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, MouseCallback);
	emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, MouseCallback);
	emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, MouseCallback);
	emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, MouseCallback);
	emscripten_set_dblclick_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, MouseCallback);

	emscripten_set_wheel_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, MouseWheelCallback);

	emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, KeyCallback);
	emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, KeyCallback);

	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 0, ResizedCallback);

	// full screen
	{
		EmscriptenFullscreenStrategy s;
		memset(&s, 0, sizeof(s));
		s.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH;
		s.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
		s.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
		emscripten_enter_soft_fullscreen("canvas", &s);
	}

	double width, height;
	emscripten_get_element_css_size("canvas", &width, &height);

	WindowInfo.Width = (int)width;
	WindowInfo.Height = (int)height;

	// Create window
	// This is necessary just to set the window size??
	{
		static Display* XDisp = {};

		XDisp = XOpenDisplay(NULL);
		Assert(XDisp != NULL);

		Window Root = DefaultRootWindow(XDisp);

		XSetWindowAttributes swa = {};
		swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;
		Window win = XCreateWindow(
		                 XDisp, Root,
		                 0, 0, WindowInfo.Width, WindowInfo.Height, 0,
		                 CopyFromParent, InputOutput,
		                 CopyFromParent, CWEventMask,
		                 &swa );

		XSetWindowAttributes xattr = {};
		xattr.override_redirect = false;
		XChangeWindowAttributes ( XDisp, win, CWOverrideRedirect, &xattr );

		XWMHints hints = {};
		hints.input = true;
		hints.flags = InputHint;
		XSetWMHints(XDisp, win, &hints);

		// make the window visible on the screen
		XMapWindow (XDisp, win);
		XStoreName (XDisp, win, "Title Here");

		// get identifiers for the provided atom name strings
		Atom wm_state = XInternAtom (XDisp, "_NET_WM_STATE", false);

		XEvent xev = {};
		memset ( &xev, 0, sizeof(xev) );
		xev.type                 = ClientMessage;
		xev.xclient.window       = win;
		xev.xclient.message_type = wm_state;
		xev.xclient.format       = 32;
		xev.xclient.data.l[0]    = 1;
		xev.xclient.data.l[1]    = false;
		XSendEvent (
		    XDisp,
		    DefaultRootWindow(XDisp),
		    false,
		    SubstructureNotifyMask,
		    &xev );

		Print("Created x11 window");
	}

	// Create egl context
	{

		GLuint Flags = ES_WINDOW_RGB;
		EGLint AttribList[] = {
			EGL_DEPTH_SIZE, 24,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,

			//EGL_RED_SIZE,       5,
			//EGL_GREEN_SIZE,     6,
			//EGL_BLUE_SIZE,      5,
			//EGL_ALPHA_SIZE,     (Flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
			//EGL_DEPTH_SIZE,     (Flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
			//EGL_STENCIL_SIZE,   (Flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
			//EGL_SAMPLE_BUFFERS, (Flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
			EGL_NONE
		};

		// Get Display
		GLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		Assert(GLDisplay != EGL_NO_DISPLAY); //eglGetDisplay no display

		// Initialize EGL
		EGLint MajorVersion;
		EGLint MinorVersion;
		if (!eglInitialize(GLDisplay, &MajorVersion, &MinorVersion) ) {
			Assert(false); //eglInitialize failed
		}

		// Get configs
		EGLint NumConfigs;
		if (!eglGetConfigs(GLDisplay, NULL, 0, &NumConfigs) ) {
			Assert(false); //eglGetConfigs
		}

		// Choose config
		EGLConfig Config;
		if (!eglChooseConfig(GLDisplay, AttribList, &Config, 1, &NumConfigs) ) {
			Assert(false); //eglChooseConfig
		}

		// Create a surface
		GLSurface = eglCreateWindowSurface(GLDisplay, Config, NULL, NULL);
		Assert(GLSurface != EGL_NO_SURFACE);

		// Create a GL context
		EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
		EGLContext Context = eglCreateContext(GLDisplay, Config, EGL_NO_CONTEXT, contextAttribs );
		Assert(Context != EGL_NO_CONTEXT);

		// Make the context current
		if ( !eglMakeCurrent(GLDisplay, GLSurface, GLSurface, Context) ) {
			Assert(false); //eglMakeCurrent
		}

		// Verify info
		auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS, GL_SHADING_LANGUAGE_VERSION };
		for (auto name : opengl_info) {
			auto info = glGetString(name);
			printf("OpenGL Info: %s \n", info);
		}
		Print("Created ogl context");
	}

	engine_state *GameStateFromMemory = (engine_state *)GameMemory.PermanentMemory.Memory;
	GameMemory.RenderApi = ogles3::Initialize(WindowInfo, &GameStateFromMemory->OGLProfilerData, &GameMemory.PermanentMemory, &GameMemory.TransientMemory);

	GameStateFromMemory->SaveDataFolder = "SpaceUnknown";

	emscripten_set_main_loop(&MainLoop, 0, true);

	return 0;
}
