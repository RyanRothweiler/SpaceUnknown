
#include <initializer_list>
#include <atomic>

#include <jni.h>
#include <errno.h>
#include <stdio.h>
#include <cstdio>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <pthread.h>
#include <semaphore.h>

#include <unistd.h>
#include <stdlib.h>

#include <android/api-level.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/configuration.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
//------------------------------------------------------------------------------

#include "../Engine/EngineCore.h"

// -------
// These should be removed at some point. This is because we're also including editor code
#define VK_ESCAPE 0
#define VK_BACK 0
#define VK_TAB 0
#define VK_LEFT 0
#define VK_RIGHT 0
#define VK_UP 0
#define VK_DOWN 0
#define VK_PRIOR 0
#define VK_NEXT 0
#define VK_HOME 0
#define VK_END 0
#define VK_INSERT 0
#define VK_DELETE 0
#define VK_SPACE 0
#define VK_RETURN 0
#define VK_ESCAPE 0
#define VK_SHIFT 0
// -------

// This is included manually because we don't do dll hot loading on android
#include "../Engine/EngineCore.cpp"

#include "render_android_ogles3.cpp"

#include "T:/Game/code/engine/Generated.h"

AAssetManager* GlobalAssetManager;
bool32 GlobalRunning = true;
game_input* GlobalGameInput;

const char* GlobalInternalDataPath;

vector2 GlobalPointerPos[2];
bool32 GlobalPointerState[2] = {};

bool32 GlobalTriggerAssetReload;

// monitor is 105
// device is 538
// float GlobalObjScale = 538.0f / 105.0f;
// float DisplayPPI = 538.0f / 100.0f;

struct engine {
	struct android_app* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
};

void AutoHideNavBar(struct android_app* state)
{
	JNIEnv* env{};
	state->activity->vm->AttachCurrentThread(&env, NULL);

	jclass activityClass = env->FindClass("android/app/NativeActivity");
	jmethodID getWindow = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");

	jclass windowClass = env->FindClass("android/view/Window");
	jmethodID getDecorView = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

	jclass viewClass = env->FindClass("android/view/View");
	jmethodID setSystemUiVisibility = env->GetMethodID(viewClass, "setSystemUiVisibility", "(I)V");

	jobject window = env->CallObjectMethod(state->activity->clazz, getWindow);

	jobject decorView = env->CallObjectMethod(window, getDecorView);

	jfieldID flagFullscreenID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
	jfieldID flagHideNavigationID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
	jfieldID flagImmersiveStickyID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");

	const int flagFullscreen = env->GetStaticIntField(viewClass, flagFullscreenID);
	const int flagHideNavigation = env->GetStaticIntField(viewClass, flagHideNavigationID);
	const int flagImmersiveSticky = env->GetStaticIntField(viewClass, flagImmersiveStickyID);
	const int flag = flagFullscreen | flagHideNavigation | flagImmersiveSticky;

	env->CallVoidMethod(decorView, setSystemUiVisibility, flag);

	state->activity->vm->DetachCurrentThread();
}

/**
* Initialize an EGL context for the current display.
*/
static int engine_init_display(struct engine* engine)
{
	EGLint w, h, format;

	// initialize OpenGL ES and EGL
	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	const EGLint attrib_list[] = {
		// this specifically requests an Open GL ES 2 renderer
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
		EGL_DEPTH_SIZE, 24,

		// (ommiting other configs regarding the color channels etc...
		EGL_NONE
	};

	EGLConfig config;
	EGLint num_configs;
	eglChooseConfig(display, attrib_list, &config, 1, &num_configs);

	// ommiting other codes

	const EGLint context_attrib_list[] = {
		// request a context using Open GL ES 2.0
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};
	EGLContext context = eglCreateContext(display, config, NULL, context_attrib_list);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	* As soon as we picked a EGLConfig, we can safely reconfigure the
	* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	EGLSurface surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	// Verify info
	auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
	for (auto name : opengl_info) {
		auto info = glGetString(name);
		LOGI("OpenGL Info: %s", info);
	}

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;

	return 0;
}

/**
* Tear down the EGL context currently associated with the display.
*/
static void engine_term_display(struct engine* engine)
{
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
* Process the next input event.
*/
static int32_t HandleInput(struct android_app* app, AInputEvent* NextEvent)
{
	bool32 DebugTouches = false;

	if (AInputEvent_getType(NextEvent) == AINPUT_EVENT_TYPE_MOTION &&
	        AInputEvent_getSource(NextEvent) == AINPUT_SOURCE_TOUCHSCREEN) {
		int32 BaseAction = AMotionEvent_getAction(NextEvent);
		int32 Action = BaseAction & AMOTION_EVENT_ACTION_MASK;

		// NOTE the Action already had the pointer info removed
		int32 PointerIndex = (BaseAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
		int32 PointerID = AMotionEvent_getPointerId(NextEvent, PointerIndex);
		int32 PointerCount = AMotionEvent_getPointerCount(NextEvent);

		if (DebugTouches) {
			LOGI("POINTER INDEX %i : ACTION %i : POINT ID %i : POINTER COUNT %i", PointerIndex, Action, PointerID, PointerCount);
		}


		switch (Action) {
			case (AMOTION_EVENT_ACTION_UP): {
				if (DebugTouches) {
					LOGI("UP : %i", PointerID);
				}

				GlobalPointerState[PointerID] = false;
				return 1;
			}
			break;

			case (AMOTION_EVENT_ACTION_POINTER_UP): {
				if (DebugTouches) {
					LOGI("UP : %i", PointerID);
				}

				GlobalPointerState[PointerID] = false;
				return 1;
			}
			break;

			case (AMOTION_EVENT_ACTION_MOVE): {
				if (DebugTouches) {
					LOGI("MOVE : %i", PointerIndex);
				}

				float XPos = AMotionEvent_getX(NextEvent, 0);
				float YPos = AMotionEvent_getY(NextEvent, 0);
				GlobalPointerPos[0] = vector2{XPos, YPos};

				if (PointerCount > 1) {
					if (DebugTouches) {
						LOGI("DOUBLE MOVE");
					}

					float XPos = AMotionEvent_getX(NextEvent, 1);
					float YPos = AMotionEvent_getY(NextEvent, 1);
					GlobalPointerPos[1] = vector2{XPos, YPos};
				}
				return 1;
			}
			break;

			case (AMOTION_EVENT_ACTION_DOWN): {
				if (DebugTouches) {
					LOGI("DOWN : %i", PointerID);
				}

				float XPos = AMotionEvent_getX(NextEvent, PointerIndex);
				float YPos = AMotionEvent_getY(NextEvent, PointerIndex);
				GlobalPointerPos[PointerID] = vector2{XPos, YPos};

				GlobalPointerState[PointerID] = true;
				return 1;
			}
			break;

			// NOTE idk why, but the second touch is always registered as a pointer
			case (AMOTION_EVENT_ACTION_POINTER_DOWN): {
				if (DebugTouches) {
					LOGI("POINTER DOWN : %i", PointerID);
				}

				float XPos = AMotionEvent_getX(NextEvent, PointerIndex);
				float YPos = AMotionEvent_getY(NextEvent, PointerIndex);
				GlobalPointerPos[PointerID] = vector2{XPos, YPos};

				GlobalPointerState[PointerID] = true;
				return 1;
			}
			break;
		}
	}

	return 0;
}

uint64 PlatformGetFileWriteTime(char* File)
{
	return 0;
}

void MakeDirectory(char* Path)
{

}

uint64 GetCycle()
{
	timespec now = {};
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (uint64) (now.tv_sec * 1000000000LL + now.tv_nsec);
}

uint32 GetGUID()
{
	return 0;
}

void* GetProcAddress(char* ProcName)
{
	void* Ret;
	return Ret;
}

real64 RandomFloat()
{
	return (rand() % 10000) / 10000.0f;
}

/**
* Process the next main command.
*/
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd) {
		case APP_CMD_SAVE_STATE: {
			// The system has asked us to save our current state.  Do so.
		}
		break;

		case APP_CMD_INIT_WINDOW: {
			// The window is being shown, get it ready.
			if (engine->app->window != NULL) {
				engine_init_display(engine);
			}
		}
		break;

		case APP_CMD_TERM_WINDOW: {
			// The window is being hidden or closed, clean it up.
			engine_term_display(engine);
		}
		break;

		case APP_CMD_GAINED_FOCUS: {
			GlobalTriggerAssetReload = true;
		}
		break;

		case APP_CMD_LOST_FOCUS: {
			// Save game or something
		}
		break;
	}
}


// Maybe something better here is possible. We really shouldn't need to pregenerate the folder structure for this.
/*
// https://android.googlesource.com/platform/frameworks/base.git/+/1cadc07dd1e3711fb1e57548038e3026682c5ef8/native/android/asset_manager.cpp#135
struct AAssetDir {
	AssetDir* mAssetDir;
	size_t mCurFileIndex;
	String8 mCachedFileName;
	AAssetDir(AssetDir* dir) : mAssetDir(dir), mCurFileIndex(0) { }
	~AAssetDir() { delete mAssetDir; }
};

path_list* GetPathsForFileType(string FileType, string Root, memory_arena* Memory, path_list* PathList)
{
	path_list* NextPath = PathList;

	AAssetDir* Dir = AAssetManager_openDir(GlobalAssetManager, Root.CharArray);
	size_t FilesCount = Dir->mAssetDir->getFileCount();
	int FileIndex = 0;

	while (FileIndex < FilesCount) {
		int x = 0;
		FileIndex++;
		char* FileName = Dir->mAssetDir->getFileName(FileIndex);
	}

	return NextPath;
}
*/

path_list* AddFolders(string FileType, string Path, memory_arena* Memory, path_list* PathList)
{
	path_list* NextPath = PathList;

	AAssetDir* Dir = AAssetManager_openDir(GlobalAssetManager, Path.CharArray);

	const char* FileNameC = AAssetDir_getNextFileName(Dir);
	if (FileNameC == GameNull) { return NextPath; }

	char* FileName = strdup(FileNameC);
	while (true) {

		int32 SourceLen = CharArrayLength(FileName);
		int32 EndLen = StringLength(FileType);

		bool32 EndsWith = true;

		if (SourceLen == 0 || EndLen == 0) { EndsWith = false; }
		if (EndLen > SourceLen) { EndsWith = false; }
		if (EndsWith) {
			for (int x = 0; x < EndLen; x++) {
				char S = FileName[SourceLen - 1 - x];
				char En = FileType.CharArray[EndLen - 1 - x];
				if (S != En) {
					EndsWith = false;
					break;
				}
			}
		}

		if (EndsWith) {
			string Final = Path + FileName;;
			NextPath->Path = Final;
			NextPath->Next = (path_list*)ArenaAllocate(Memory, sizeof(path_list));
			NextPath = NextPath->Next;
		}

		FileNameC = AAssetDir_getNextFileName(Dir);
		if (FileNameC == GameNull) { return NextPath; }
		FileName = strdup(FileNameC);
	}

	return NextPath;
}

// NOTE this searches all folders in assets. Maybe fix that or change the name of this
path_list* GetPathsForFileType(char* FileTypeChar, char* RootChar, memory_arena* Memory, path_list* PathList)
{
	string FileType = FileTypeChar;
	string Root = RootChar;

	path_list* NextPath = PathList;
	//NextPath = AddFolders(FileType, Root, Memory, NextPath);

	for (int i = 0; i < ArrayCount(AssetsFolderStructure); i++) {
		string Path = AssetsFolderStructure[i];
		NextPath = AddFolders(FileType, Path, Memory, NextPath);
	}

	return NextPath;
}

read_file_result
LoadFileData(char *FileName, memory_arena * Memory)
{
	// LOGI("Loading File %s", FilePath.CharArray);
	read_file_result Result = {};

	AAsset* AssetHandle = AAssetManager_open(GlobalAssetManager, FileName, AASSET_MODE_UNKNOWN);
	if (!AssetHandle) {
		LOGI("Could not find file %s", FileName);
	} else {
		Result.ContentsSize = AAsset_getLength(AssetHandle);
		// Result.Contents = (void*)AAsset_getBuffer(AssetHandle);
		void* Buffer = ArenaAllocate(Memory, Result.ContentsSize);
		AAsset_read(AssetHandle, Buffer, Result.ContentsSize);
		Result.Contents = Buffer;

		AAsset_close(AssetHandle);
	}

	return (Result);
}

bool32 FileExists(char* FilePath)
{
	AAsset* AssetHandle = AAssetManager_open(GlobalAssetManager, FilePath, 0);
	return (AssetHandle != NULL);
}

void WriteFile(char *FileDestination, void *Data, uint32 DataSize)
{
	string DataPath = (char*)GlobalInternalDataPath;
	FILE* FileHandle = std::fopen((DataPath +  "/" + FileDestination).CharArray, "w");
	if (FileHandle != NULL) {
		int32 CountWritten = std::fwrite(Data, DataSize, 1, FileHandle);
		LOGI("%i written to %s", (int)CountWritten, (DataPath + "/" + FileDestination).CharArray);
	} else {
		LOGI("Could not write file %s", FileDestination);
	}
	std::fclose(FileHandle);
}

void AppendFile (char *FileDestination, char *Data)
{
	FILE* fh = std::fopen(FileDestination, "a");
	if (fh == 0) {
		std::fprintf(fh, "%s", Data);
	} else {
		LOGI("Error appending opening file.");
	}
	std::fclose(fh);
}

void DeleteFile(char* FileDest)
{
	std::remove(FileDest);
}

read_file_result ReadFileExternal(char *Path, memory_arena *Memory)
{

	/*
	 // FILE* FileHandle = std::fopen((DataPath + FileLoading).CharArray, "r");
	 // void* Buffer = ArenaAllocate(Memory, sizeof(int32));
	 // std::fread(Buffer, sizeof(int32), 1, NextFile);
	 // int32* Number = (int32*)Buffer;
	 // LOGI("Char %i", Number[0]);
	 int32 FileHandle = open((DataPath +  "/" + Path).CharArray, O_RDWR);
	 struct stat FileStats = {};
	 fstat(FileHandle, &FileStats);
	 LOGI("FILE SIZE %i", (int)FileStats.st_size);
	 */

	// // fstat

	read_file_result Result = {};



	string DataPath = (char*)GlobalInternalDataPath;
	string FullPath = DataPath + "/" + Path;
	int FileHandle = open(FullPath.CharArray, O_RDWR);
	if (FileHandle != -1) {
		struct stat FileStats = {};
		fstat(FileHandle, &FileStats);

		Result.Contents = ArenaAllocate(Memory, FileStats.st_size);
		read(FileHandle, Result.Contents, FileStats.st_size);
		Result.ContentsSize = FileStats.st_size;
	} else {
		LOGI("Could not open file %s", FullPath.CharArray);
		Assert(0);
		// Could not open file
	}
	close(FileHandle);

	return (Result);
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

void PlatformPrint(char* Message)
{
	LOGI("%s", Message);
}

void
ProcessInputState(input_state * ButtonProcessing, bool32 NewState)
{
	if (NewState) {
		if (ButtonProcessing->IsDown) {
			ButtonProcessing->OnDown = false;
		} else {
			ButtonProcessing->IsDown = true;
			ButtonProcessing->OnDown = true;
			ButtonProcessing->IsUp = false;
			ButtonProcessing->OnUp = false;
			ButtonProcessing->Claimed = false;
		}
	} else {
		if (ButtonProcessing->IsUp) {
			ButtonProcessing->OnUp = false;
		} else {
			ButtonProcessing->IsUp = true;
			ButtonProcessing->OnUp = true;
			ButtonProcessing->IsDown = false;
			ButtonProcessing->OnDown = false;
		}
	}
}

// Threading -------------------------------------------------------------------
// https://learning.oreilly.com/library/view/android-native-development/9781849691505/ch06s02.html#ch06lvl3sec14

//#define FenceWrite _WriteBarrier(); _mm_sfence();
//#define FenceRead _ReadBarrier(); _mm_lfence();
//#define FenceAll FenceWrite FenceRead
#define FenceAll asm volatile ("" : : : "memory"); std::atomic_thread_fence(std::memory_order_acq_rel);


void TestGameThread(void* Params, int32 ThreadID)
{
	sleep(2);
	int32* NumPrinting = (int32*)Params;
	string Disp = string{"thread "} + ThreadID +  string{" : number "} + *NumPrinting + "\n";
	LOG(Disp.Array());
}

struct worker_thread_info {
	int32 ID;
	pthread_t ThreadHandle;
};

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
	LOG(Val.Array());

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
//------------------------------------------------------------------------------

/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(struct android_app * state)
{

	LOGI("<----------------------->");

	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = HandleInput;
	engine.app = state;

	AutoHideNavBar(state);

	window_info WindowInfo = {};

	game_memory GameMemory = {};
	GameMemory.PermanentMemory.Size = Megabytes(512);
	GameMemory.TransientMemory.Size = Megabytes(512);
	GameMemory.ThreadTransientMemory.Size = Megabytes(1024);
	GameMemory.TotalSize = GameMemory.PermanentMemory.Size + GameMemory.TransientMemory.Size + GameMemory.ThreadTransientMemory.Size;

	GameMemory.GameMemoryBlock = malloc((size_t)GameMemory.TotalSize);
	Assert(GameMemory.GameMemoryBlock);

	GameMemory.PermanentMemory.Memory = GameMemory.GameMemoryBlock;
	GameMemory.TransientMemory.Memory = (uint8 *)GameMemory.PermanentMemory.Memory + GameMemory.PermanentMemory.Size;
	GameMemory.ThreadTransientMemory.Memory = (uint8 *)GameMemory.TransientMemory.Memory + GameMemory.TransientMemory.Size;

	GameMemory.TransientMemory.EndOfMemory = (uint8 *)GameMemory.TransientMemory.Memory + GameMemory.TransientMemory.Size;
	GameMemory.ThreadTransientMemory.EndOfMemory = (uint8 *)GameMemory.ThreadTransientMemory.Memory + GameMemory.ThreadTransientMemory.Size;
	GameMemory.PermanentMemory.EndOfMemory = (uint8 *)GameMemory.PermanentMemory.Memory + GameMemory.PermanentMemory.Size;
	GameMemory.PermanentMemory.Head = (uint8 *)GameMemory.PermanentMemory.Memory + sizeof(engine_state);

	// Create threads
	worker_thread_info ThreadInfos[4] = {};
	SemaphoreHandle = {};

	// Create semaphor
	{
		int Success = sem_init(&SemaphoreHandle, 0, 0);
		if (Success != 0) {
			string Ret = "Error creating semaphore " + string{Success};
			LOG(Ret.Array());
			Assert(false);
		}
	}

	// Create mutex
	{
		int Success = pthread_mutex_init(&CountMutex, NULL);
		if (Success != 0) {
			string Ret = "Error creating mutex " + string{Success};
			LOG(Ret.Array());
			Assert(false);
		}
	}

	// Start threads
	for (int i = 0; i < ArrayCount(ThreadInfos); i++) {
		ThreadInfos[i].ID = i;

		int Success = pthread_create(&ThreadInfos[0].ThreadHandle, NULL, ThreadLoop, (void*)(&ThreadInfos[i]));
		if (Success != 0) {
			string Ret = "Error creating thread " + string{Success};
			LOG(Ret.Array());
			Assert(false);
		}
	}

	/*
	sleep(10);

	int one = 1;
	int two = 2;
	int three = 3;
	int four = 4;
	int five = 5;
	int six = 6;
	int seven = 7;
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&one);
	AddWork(TestGameThread, (void*)&two);
	AddWork(TestGameThread, (void*)&three);
	AddWork(TestGameThread, (void*)&four);
	AddWork(TestGameThread, (void*)&five);
	AddWork(TestGameThread, (void*)&six);
	AddWork(TestGameThread, (void*)&seven);

	while (true) {

	}
	*/


	platform::api PlatformApi = {};
	PlatformApi.ReadFile = &LoadFileData;
	PlatformApi.GetPathsForFileType = &GetPathsForFileType;
	PlatformApi.FileExists = &FileExists;
	PlatformApi.WriteFile = &WriteFile;
	PlatformApi.AppendFile = &AppendFile;
	PlatformApi.DeleteFile = &DeleteFile;
	PlatformApi.GetClockMS = &GetWallClock;
	PlatformApi.Print = &PlatformPrint;
	PlatformApi.RandomFloat = &RandomFloat;
	PlatformApi.ThreadAddWork = &AddWork;


	// These are just stubs and don't do anything yet
	PlatformApi.GetFileWriteTime = &PlatformGetFileWriteTime;
	PlatformApi.MakeDirectory = &MakeDirectory;
	PlatformApi.GetCycle = &GetCycle;
	PlatformApi.GetGUID = &GetGUID;
	PlatformApi.GetProcAddress = &GetProcAddress;


	// screen dpi
	{
		AConfiguration* config = AConfiguration_new();
		AConfiguration_fromAssetManager(config, state->activity->assetManager);

		PlatformApi.ScreenDPI = AConfiguration_getDensity(config);
		PlatformApi.ScreenDPICo = 0.35f; // This should be adjustable by user, but this number feels right to me.


		AConfiguration_delete(config);
	}

	GameMemory.PlatformApi = PlatformApi;


	game_input GameInput = {};
	GlobalGameInput = &GameInput;
	game_audio_output_buffer AudioBuffer = {};

	int32 GameUpdateHz = 60;
	real64 TargetSecondsElapsedPerFrame = 1.0f / (real64)GameUpdateHz;
	uint64 PrevFrameTime = GetWallClock();

	GlobalRunning = true;
	while (GlobalRunning) {
		GlobalInternalDataPath = state->activity->internalDataPath;

		uint64 BeginCounter = GetWallClock();

		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;
		while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {

			// Process this event.
			if (source != NULL) {
				source->process(state, source);
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0) {
				engine_term_display(&engine);
				return;
			}
		}

		if (state->window != NULL) {

			// This doesn't work. Hoping to use it to get available memory. Do we need java for that??
			/*
			{

				JNIEnv* env = state->activity->env;
				JavaVM* jvm = state->activity->vm;

				jvm->AttachCurrentThread(&env, NULL);

				// Find the Java class - provide package ('.' replaced to '/') and class name

				jclass jniTestClass = env->FindClass("com/example/ryanlib/RyanTest");

				// Find the Java method - provide parameters inside () and return value (see table below for an explanation of how to encode them)
				jmethodID getAnswerMethod = env->GetStaticMethodID(jniTestClass, "Fucker", "()I");

				// Calling the method
				int x = (int)env->CallStaticIntMethod(jniTestClass, getAnswerMethod);

				int z = 0;
			}
			 */


			engine_state *GameStateFromMemory = (engine_state *)GameMemory.PermanentMemory.Memory;

			WindowInfo.Width = ANativeWindow_getWidth(state->window);
			WindowInfo.Height = ANativeWindow_getHeight(state->window);

			if (GameMemory.RenderApi.MakeProgram == GameNull) {
				GameMemory.RenderApi = ogles3::Initialize(WindowInfo, &GameStateFromMemory->OGLProfilerData, &GameMemory.PermanentMemory, &GameMemory.TransientMemory);
			}

			GameInput.TouchPosition[0] = vector2{0, 0};
			for (int32 TouchIndex = 0; TouchIndex < 2; TouchIndex++) {
				//LOGI("STATE %i : %i", TouchIndex, GlobalPointerState[TouchIndex]);
				ProcessInputState(&GameInput.TouchStatus[TouchIndex], GlobalPointerState[TouchIndex]);
				GameInput.TouchPosition[TouchIndex] = GlobalPointerPos[TouchIndex];
			}

			GlobalAssetManager = state->activity->assetManager;

			uint64 LogicCyclesBegin = GetWallClock();
			GameLoop(&GameMemory, &GameInput, &WindowInfo, &AudioBuffer, "");
			GameStateFromMemory->LogicCycles = GetWallClock() - LogicCyclesBegin;

			uint64 RenderingCyclesBegin = GetWallClock();
			state_to_serialize* State = &GameStateFromMemory->StateSerializing;
			GameMemory.RenderApi.Render(&GameMemory.RenderApi, State->ActiveCam, &State->Light.Cam, &WindowInfo, &GameStateFromMemory->DebugUIRenderer, &GameStateFromMemory->UIRenderer, &GameStateFromMemory->GameRenderer, &GameStateFromMemory->Assets->GaussianBlurShader);

			if (GlobalTriggerAssetReload) {
				GlobalTriggerAssetReload = false;
				//GameStateFromMemory->FlagTriggerAssetReload = true;
			}

			eglSwapBuffers(engine.display, engine.surface);
			GameStateFromMemory->CyclesPlatformRendering = GetWallClock() - RenderingCyclesBegin;

			uint64 TimeElapsedMicroSeconds = GetWallClock() - PrevFrameTime;
			double SecondsForFrame = MicrosecondsToSeconds(TimeElapsedMicroSeconds);

			uint64 TSCBeforeWait = GetWallClock();
			while (SecondsForFrame < TargetSecondsElapsedPerFrame) {
				TimeElapsedMicroSeconds = GetWallClock() - PrevFrameTime;
				SecondsForFrame = MicrosecondsToSeconds(TimeElapsedMicroSeconds);
			}

			GameStateFromMemory->CyclesUsedWaiting = GetWallClock() - TSCBeforeWait;
			GameStateFromMemory->DeltaTimeMS = SecondsToMilliseconds(SecondsForFrame);

			// -------- adhoc rendering performance metrics
			float Average = 0;
			{
				static real32 History[300] = {};
				static real32 HistorySum = 0;
				static int HistoryIndex = 0;

				// Loop
				HistoryIndex++;
				if (HistoryIndex == ArrayCount(History)) {
					HistoryIndex = 0;
				}


				HistorySum -= History[HistoryIndex];
				History[HistoryIndex] = GameStateFromMemory->CyclesPlatformRendering;
				HistorySum += GameStateFromMemory->CyclesPlatformRendering;

				Average = HistorySum / ArrayCount(History);
			}
			// --------


			real64 FPS = 1.0f / SecondsForFrame;
			GameStateFromMemory->PrevFrameFPS = FPS;
			LOGI("FPS %f     average %u 	curr %u", FPS, (uint)Average, (uint)GameStateFromMemory->CyclesPlatformRendering);
			PrevFrameTime = GetWallClock();

			GameStateFromMemory->TotalCyclesUsedLastFrame = GetWallClock() - BeginCounter;
		}
	}
}