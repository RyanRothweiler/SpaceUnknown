#pragma once

#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#define internal static
#define global_variable static

#define ID_LINE __LINE__
#define ID_FILE __FILE__
#define ID_METHOD __func__
#define COUNTER __COUNTER__

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)


//#if DEBUG
#define Assert(Expression) if (!(Expression)) {LOG("ASSERT"); LOG(#Expression); LOG(AT); *(int *)0 = 0;}
//#define Assert(Expression) if (!(Expression)) { LOG(":: ASSERT "); LOG(#Expression); LOG("\n:: LINE "); LOG(ID_LINE); LOG(":: FILE "); LOG(ID_FILE); LOG("\n:: FUNC "); LOG(ID_METHOD); LOG("\n"); *(int *)0 = 0; }
//#define Assert(Expression) if (!(Expression)) { LOG(":: ASSERT "); LOG(#Expression); LOG("\n:: LINE "); LOG(":: FILE "); LOG(ID_FILE); LOG("\n:: FUNC "); LOG(ID_METHOD); LOG("\n"); *(int *)0 = 0; }
//#else
//	#define Assert(Expression)
//	#define AssertM(Expression, Message)
//#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#include "Types.cpp"
#include "../preprocessor.h"

#define ARRAY_SIZE(Array, Type) sizeof(Array) / sizeof(Type)
#define MAX_VALUE(a) (((unsigned long long)1 << (sizeof(a) * CHAR_BIT)) - 1)

#define GameNull 0

// -----------------------------------------------------------------------------
// build variables
// -----------------------------------------------------------------------------
#if AUTOMATED_TESTING
	const bool BuildVarAutomatedTesting = true;
#else
	const bool BuildVarAutomatedTesting = false;
#endif

#if RELEASE
	const bool BuildVarRelease = true;
#else
	const bool BuildVarRelease = false;
#endif
// -----------------------------------------------------------------------------

struct read_file_result {
	uint32 ContentsSize;
	void *Contents;
};

#include "GameConstants.cpp"
#include "MemoryManager.h"

memory_arena* GlobalPermMem;
memory_arena* GlobalTransMem;
memory_arena* GlobalThreadTransMem;

struct path_list;

enum class work_status {null, waiting, running, finished};
typedef void (*game_thread_proc)(void* Params, int32 ThreadID);

struct thread_work {
	game_thread_proc WorkMethod;
	void* WorkParams;
	work_status Status;
};

namespace platform {
	struct api {
		void(*MakeDirectory)(char* Path);
		read_file_result(*ReadFile)(char *Path, memory_arena *Memory);
		bool32(*FileExists)(char *Path);
		void(*WriteFile)(char *FileDestination, void *Data, uint32 DataSize);
		void(*AppendFile)(char *FileDestination, char *Data);
		void(*DeleteFile)(char *Path);
		uint64(*GetClockMS)();
		uint64(*GetCycle)();
		uint32(*GetGUID)();
		void(*Print)(char* Message);
		uint64(*GetFileWriteTime)(char* File);
		void*(*GetProcAddress)(char* Name);
		void(*OpenFileExternal)(char* File);
		path_list*(*GetPathsForFileType)(char* FileType, char* Root, memory_arena* Memory, path_list* PathList);
		void(*OpenFileDialogue)(char* Path, int32 MaxPathLength);
		thread_work*(*ThreadAddWork)(game_thread_proc WorkMethod, void* WorkParams);

		// Between 0 and 1
		real64(*RandomFloat)();

		uint64(*QueryPerformanceCounter)();
		uint64 PerformanceCounterFrequency;

		// Android
		read_file_result(*AndroidReadFile)(char *Path, memory_arena *Memory);

		real64 ScreenDPI;
		real64 ScreenDPICo;
		real64 ScreenTypePoints()
		{
			// one point exuals 1/72 of an inch
			return (ScreenDPI / 72.0f) * ScreenDPICo;
		}
	};
}

platform::api PlatformApi;

#include "Logging.cpp"
#include "MemoryManager.cpp"
#include "String.cpp"

void LogToConsole(char* Tag, string Message, int32 LineNumber, char* FileName)
{
	LogToConsole(Tag, Message.Array(), LineNumber, FileName);
}

// Preprocessor member type ----------------------------------------------------
enum class meta_member_type {
	uint32, uint16, uint8,
	int32, int16, int8, int64,
	real32, real64,

	// is this needed???
	battle_monster_pointer,
};

struct meta_member {
	meta_member_type Type;
	string Name;
	uint64 Offset;
	bool32 ArrayLength;
};

struct s_void {
	int64 Type;
	bool32 IsAllocated;
	int64 DataSize;
	void* Data;

	void Clear()
	{
		if (IsAllocated) {
			ClearMemory((uint8*)Data, DataSize);
		}
	}
};

void SafeVoidAlloc(s_void* SafeVoid, memory_arena* Memory, int64 Size, int32 Type)
{
	SafeVoid->IsAllocated = true;
	SafeVoid->Type = Type;
	SafeVoid->DataSize = Size;
	SafeVoid->Data = ArenaAllocate(Memory, Size);
	ClearMemory((uint8*)SafeVoid->Data, Size);
};

// -----------------------------------------------------------------------------

#include "Color.cpp"
#include "Array.cpp"

struct line {
	vector2 Start, End;
};

// Should probably go in some renederer class ----------------------------------

enum class gl_blend_type {
	nearest, linear
};

struct loaded_image {
	string FilePath;
	bool32 CorrectGamma;
	gl_blend_type BlendType;

	int Width;
	int Height;

	uint32 GLID;

	// If this image was loaded successfully
	bool32 Valid;
};

// -----------------------------------------------------------------------------

struct path_list {
	string Path;
	path_list* Next;

	int32 GetCount()
	{
		int32 Count = 0;

		if (Next != GameNull) {
			Count++;
			path_list* P = Next;

			while (P->Next != GameNull) {
				Count++;
				P = P->Next;
			}
		}

		return Count;
	}
};

#include "FixedAllocator.cpp"

struct sprite_sheet {
	uint8 ColumnCount;
	uint8 RowCount;
	uint8 Padding;
	loaded_image Image;
};

struct window_info {
	uint32 Width;
	uint32 Height;
};

#include "Rect.h"

#define TRANSFORM_MAX_CHILDREN 100

struct transform {

	m4y4 FinalWorld;

	vector3 LocalPos;
	vector3 LocalRot;
	vector3 LocalScale;

	transform* Children[TRANSFORM_MAX_CHILDREN];
	int32 ChildrenCount;

	void AddChild(transform* Trans)
	{
		Children[ChildrenCount] = Trans;
		ChildrenCount++;
		Assert(ChildrenCount < TRANSFORM_MAX_CHILDREN);
	}

	void Update(m4y4 Parent)
	{
		quat Q = {};
		Q.FromEuler(LocalRot);
		m4y4 Rotation = Q.ToMatrix();

		// Questionable
		if (LocalScale.X == 0 && LocalScale.Y == 0 && LocalScale.Z == 0) {
			LocalScale = vector3{1, 1, 1};
		}

		m4y4 Scale = {
			{
				{(real32)LocalScale.X, 	0,  				0, 					0},
				{0, 				(real32)LocalScale.Y, 	0, 					0},
				{0, 				0, 					(real32)LocalScale.Z, 	0},
				{0, 				0, 					0, 					1},
			}
		};
		m4y4 Translate = {
			{
				{1, 	0,  	0, 		(real32)LocalPos.X},
				{0, 	1, 		0, 		(real32)LocalPos.Y},
				{0, 	0, 		1, 		(real32)LocalPos.Z},
				{0, 	0, 		0, 		1},
			}
		};

		m4y4 Model = MatrixMultiply(Scale, Rotation);
		Model = MatrixMultiply(Model, Translate);

		FinalWorld = MatrixMultiply(Model, Parent);

		for (int x = 0; x < ChildrenCount; x++) {
			Children[x]->Update(FinalWorld);
		}
	}
};


#include "LinkedList.h"
#include "Hash.h"
#include "Profiler.h"
#include "Renderer/Renderer.h"
#include "Font.h"


struct game_memory {
	bool32 IsInitialized;
	bool32 ImguiInitialized;

	uint64 TotalSize;
	void *GameMemoryBlock;

	// NOTE required to be cleared to 0 on startup / allocation
	memory_arena PermanentMemory;

	// NOTE this transient memory head is reset to the top of transient storage at the beginning of the game loop
	// Anything that needs to stick around for more tha one game loop should go in PermanentStorage
	memory_arena TransientMemory;

	memory_arena ThreadTransientMemory;

	// This is a subset arena of PermanentMemory
	memory_arena AssetsMemory;

	int64 ElapsedCycles;

	bool32 HotReloaded;

	render::api RenderApi;
	platform::api PlatformApi;
};

struct entity {
	uint32 ID;
	transform Transform;
	string Name;

	vao VAO;

	// Model Only --------------------------------------------------------------
	model* Model;

	material* Material;
	string MaterialID;
	// -------------------------------------------------------------------------


	// Skinned mesh renderer. Bones --------------------------------------------
	skin_controller SkinController;
	armature* Armature;
	// -------------------------------------------------------------------------

	aabb AABBLocal;
	real32 BoundingRadius;

	entity* Children[100];
	int32 ChildrenCount;

	bool32 Wireframe;

	void AddChild(entity* Child)
	{
		Transform.AddChild(&Child->Transform);
		Children[ChildrenCount] = Child;

		Assert(ChildrenCount < ArrayCount(Children));
		ChildrenCount++;
	}

	aabb GetScaledAABB()
	{
		aabb Ret = AABBLocal;
		Ret.XMax = Ret.XMax * Transform.LocalScale.X;
		Ret.XMin = Ret.XMin * Transform.LocalScale.X;

		Ret.YMax = Ret.YMax * Transform.LocalScale.Y;
		Ret.YMin = Ret.YMin * Transform.LocalScale.Y;

		Ret.ZMax = Ret.ZMax * Transform.LocalScale.Z;
		Ret.ZMin = Ret.ZMin * Transform.LocalScale.Z;
		return Ret;
	}
};

struct loaded_sound {
	uint32 SampleCount;
	uint32 ChannelCount;
	int16 *Samples[2];
};

struct game_audio_output_buffer {
	// NOTE this running sample index is only for creating the sinwave. We don't need this otherwise. Remove this eventually.
	uint32 RunningSampleIndex;

	int SamplesPerSecond;
	int SampleCount;
	uint32 BytesPerSample;
	int16 *Samples;
};

struct win32_audio_output {
	int SamplesPerSecond;
	int BytesPerSample;
	int SecondaryBufferSize;
	uint32 RunningSampleIndex;
};

struct input_state {
	bool32 OnDown;
	bool32 OnUp;
	bool32 IsDown;
	bool32 IsUp;

	bool32 Claimed;
};

enum class touch_gesture {
	none,
	tap, // finger has tapped, and lifted without going ouside the drag threshold
	drag, // finger has tappend, is down, and is outside the drag threshold
	hold // finger has tapped, and is down, and is inside the drag threshold
};

struct touch {
	touch_gesture Gesture;
	vector2 Position;
	vector2 Origin;
	vector2 Delta; // Change from origin
	vector2 FrameDelta; // Change from last frames position
	vector2 PositionLastFrame;
};

// Input for touchscreens. Only support two touces for now
#define TOUCH_COUNT 2

struct game_input {
	vector2 MousePos;
	vector2 MouseDelta;
	vector2 MousePosOnDown;

	real64 MouseScrollDelta;
	bool32 ScrollClaimed;

	// this is set by the platform, and needs to be consumed by the game each frame
	short InputChar;

	input_state MouseLeft;
	input_state MouseRight;
	input_state MouseMiddle;

	input_state KeyboardInput[256];

	input_state TouchStatus[TOUCH_COUNT];
	vector2 TouchPosition[TOUCH_COUNT];

	touch Touches[TOUCH_COUNT]; // This is update in the game layer. Just provides easier convenience for interacting with the touch data
};

game_input EmptyInput = {};

#include "Range.h"
#include "Hex.cpp"
#include "Timer.h"

#define ImageRotationsCount 8

struct game_assets {

	// Engine resources
	shader GaussianBlurShader;
	shader CamBasicShader;

	entity UnitCube;

	model_polling ModelPolling;
};

#include "Timer.cpp"
#include "scene/scene.h"
#include "ui/ui.h"

struct monster_instance;

struct ui_panel;
struct panel_stack {
	ui_panel* Panels[100];
	uint32 NextIndex;

	monster_instance* Monster;

	game_input* UIInput;
};

const vector3 CamPlayerOffset = vector3{ -10.0f, 0, 25.0f};
const real64 PlayerSpeed = 0.3f;
const real32 PlayerRadius = 1.4f;

struct nav_mesh {
	struct face {
		vector3 PointOne, PointTwo, PointThree;
	};

	face* Faces;
	int32 FacesCount;
};

// NOTE pointers in here will break state saving / loading
struct state_to_serialize {

	bool32 TestingDone;

	ui::anim_state BackpackAnimState;
	ui::anim_state FirstAbilityAnimState;
	ui::anim_state SecondAbilityAnimState;
	ui::anim_state CloseAnimState;

	light Light;
	light LightTwo;

	material SkyboxMaterial;

	bool32 ShowingBackpack;

	camera EditorCamera;
	camera GameCamera;
	camera UICam;
	camera BattleCam;
	camera* ActiveCam;

	scene GalaxyScene;
	scene DebugScene;

	// TODO this scene should probably just be removed / phased out
	scene Scene;

	bool32 AudioPaused;
	bool32 AudioMuted;

	uint64 TimeRunningMS;

	vector3 GalaxyCamTargetPos;
	vector3 GalaxyCamTargetRot;
	bool GalaxyDetailShowing = false;

	panel_stack PanelStack;
};

struct globals;

struct game_state {
	shader BackupShader;

	game_assets* Assets;
	state_to_serialize StateSerializing;

	// TODO move ClearColor into renderer
	color ClearColor;

	renderer GameRenderer;
	renderer UIRenderer;
	renderer DebugUIRenderer;
	renderer MonsterPreviewRenderer;

	// ibl
	loaded_image BRDFlut;

	// gif recording
	bool32 IsGifRecording;
	uint32 RecordingFrame;

	bool32 DebuggerPaused;

	loaded_sound TestSound;
	uint64 TestSoundSampleIndex;

	real64 DeltaTimeMS;
	int64 PrevFrameFPS;

	real64 BuildingInfoScroll;

	bool32 LockCursor;

	// This is the total cycles used in the previous frame. This includes the platform layer.
	uint64 TotalCyclesUsedLastFrame;
	// This is the number of cycles which were used doing nothing, effectively waiting just to hit a target frame rate.
	uint64 CyclesUsedWaiting;
	// The number of cycles spent rendering the opengl data on the platform layer.
	uint64 CyclesPlatformRendering;
	// Total cycles spent on game logic
	uint64 LogicCycles;

	globals* Globals;
	render::data* RenderData;

	profiler_data ProfilerData;
	profiler_data OGLProfilerData;
};

// Game Loop
#define GAME_LOOP(name) void name(game_memory *Memory, game_input *GameInput, window_info *WindowInfo, game_audio_output_buffer *AudioBuffer, char* RootAssetPath)
typedef GAME_LOOP(game_update_and_render);

// Called before the window closes
#define GAME_WINDOW_CLOSING(name) void name(game_memory *Memory)
typedef GAME_WINDOW_CLOSING(game_window_closing);
GAME_WINDOW_CLOSING(GameWindowClosingStub) { }


#endif