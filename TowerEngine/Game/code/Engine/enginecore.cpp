#pragma once
#ifndef ENGINE_CORE
#define ENGINE_CORE

#include "EngineCore.h"

render::api RenderApi;

#include "imgui/imgui.h"
#include "imgui/imgui.cpp"
#include "imgui/imgui_internal.h"
#include "imgui/imconfig.h"
#include "imgui/imstb_rectpack.h"
#include "imgui/imstb_textedit.h"
#include "imgui/imstb_truetype.h"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_widgets.cpp"

#include "Rect.cpp"

// This should be used with the generated system to guarantee safety
int32 StringToEnum(string Input, string* NamesArray, int32 EnumsCount)
{
	for (int i = 0; i < EnumsCount; i++) {
		if (NamesArray[i] == Input) {
			return i;
		}
	}

	ConsoleLog("Could not convert string to enum");
	ConsoleLog(Input.Array());

	return 0;
}

vector2 NearestOnLine(line Line, vector2 Point)
{
	vector2 LineDir = Vector2Normalize(Line.Start - Line.End);
	vector2 LinePerp = Vector2Perp(LineDir);

	// Find intersection between mouse line and axis line. Try both directions.
	real32 CheckDistance = 10000;
	intersection_point P = {};

	P = GetIntersection(Line.Start, Line.End, Point, Point + (LinePerp * CheckDistance));
	if (P.Valid) { return P.Point; }

	P = GetIntersection(Line.Start, Line.End, Point, Point + (LinePerp * -CheckDistance));
	if (P.Valid) { return P.Point; }

	// This should never happen
	Assert(0);
	return {};
}

#include "LinkedList.cpp"
#include "Collision.cpp"

#include "Hash.cpp"
#include "Random.cpp"

#include "Hash.cpp"
#include "Random.cpp"
#include "Range.cpp"

#include "Audio.cpp"
#include "Sorts.cpp"

#include "ParticleSystem.cpp"
#include "Util.cpp"

#include "../Game/AssetList.h"
#include "CSV.cpp"

enum class meta_member_type {
	uint8, uint16, uint32,
	int8, int16, int32, int64,
	real32, real64, bool32,

	// structs / classes / custom types
	custom, enumeration
};

struct meta_member;

#include "SaveData.h"
#include "Json.h"

typedef void(*save_data_fill_shim)(save_data::member* Dest, string KeyParent, void* AccData);

typedef void(*struct_meta_fill_shim_func)(json::struct_string_return* Dest, void* AccData);
typedef void(*json_fill_struct_shim)(json::json_data* JsonData, string KeyParent, void* DataDest);

struct meta_member {
	meta_member_type Type;
	string TypeString;
	string Name;
	uint64 Offset;
	bool32 ArrayLength;
	uint32 Size;

	// Data for custom types
	struct_meta_fill_shim_func MetaFillShim;
	json_fill_struct_shim JsonFillShim;

	save_data_fill_shim SaveDataFillShim;

	meta_member* CustomMetaInfo;
	int32 CustomMetaInfoCount;

};

#include "Json.cpp"
#include "SaveData.cpp"

#include "generated.h"

#include "Editor.h"
#include "Renderer/ShaderLoader.cpp"
#include "ui/UiPanel.h"

struct globals {

	// Here for loading thread. Better way to do this than make them globals??
	string AssetRootDir;
	engine_state* GameState;

	window_info* Window;

	shader_loader ShaderLoader;
	assets_list AssetsList;

	editor::data EditorData;

	uint32 IDSelected;

	renderer* UIRenderer;
	renderer* DebugUIRenderer;
	renderer* GameRenderer;
	camera* ActiveCam;

	int32 GizmoColorStackIndex = 0;
	color GizmoColorStack[10];

	game_input* Input;

	// For debugging purposes
	entity* UnitCube;

	loaded_font OpenSansRegular;
	loaded_font OpenSansBold;

	loaded_font RajMedium;
	loaded_font RajBold;
	loaded_font RajSemiBold;

	game_input* UIUserInput;
	ui_panels* UIPanels;

	font_style FontStyleP;
	font_style FontStyleH1;
	font_style FontStyleH2;
	font_style FontStyleH3;
	font_style FontStyleButton;
	font_style FontStyleDropInfo;
};
globals* Globals;

#include "../Game/AssetList.cpp"

#include "Renderer/Renderer.cpp"
#include "gizmos.cpp"
#include "scene/scene.cpp"
#include "camera/camera.cpp"
#include "Font.cpp"
#include "ui/ui.cpp"

void ProgressBar(vector2 TopLeft,
                 real64 Height, real64 Width, real64 Progress,
                 string Contents, real32 ContentsSize, color ContentsColor,
                 color BGColor, color FGColor
                )
{
	// Experience bar
	real64 LengthCurrent = Width * Progress;

	FontRenderString(&Globals->FontStyleH3, Contents,
	                 TopLeft + vector2{LengthCurrent, 20},
	                 ContentsColor, 0, Globals->UIRenderer);

	// bg
	RenderScreenLineMesh(TopLeft + vector2{0, 10},
	                     TopLeft + vector2{Width, 10},
	                     (real32)Height, BGColor, Globals->UIRenderer);

	// bar
	RenderScreenLineMesh(TopLeft + vector2{0, 10},
	                     TopLeft + vector2{LengthCurrent, 10},
	                     (real32)Height, FGColor, Globals->UIRenderer);

}

#include "ModelLoading/obj.cpp"

#include "imguiHelper.cpp"
#include "Profiler.cpp"

#include "../Game/Game.cpp"

entity* CheckSceneSelection(scene * Scene, renderer * GameRenderer, camera * Cam, window_info * WindowInfo)
{
	// This will prevent selection when moving an object. but we don't do that a whole lot
	//if (Globals->Input->MouseLeft.OnDown && (MovingState == moving_state::none || MovingState == moving_state::translation_choice)) {

	// Check for selection
	if (Globals->Input->TouchStatus[0].OnDown ) {
		Globals->IDSelected = RenderApi.DoPickRender(Cam, Globals->Input->MousePos, *WindowInfo);

		for (uint32 i = 0; i < ArrayCount(Scene->Entities); i++) {

			entity* Entity = Scene->Entities[i];
			if (Entity != GameNull && Entity->ID == Globals->IDSelected) {
				//editor::EntitySelected(Entity, GameState);
				return Entity;
			}
		}
	}

	return GameNull;
}

#include "Editor.cpp"
#include "ui/UiPanel.cpp"

void PollModelReload(game_memory * Memory, game_assets * Assets)
{
	for (int x = 0; x < Assets->ModelPolling.Count; x++) {
		model* Obj = Assets->ModelPolling.Models[x];

		uint64 CurrWriteTime = PlatformApi.GetFileWriteTime(Obj->FileDir.CharArray);

		if (CurrWriteTime != Obj->FileWriteTime) {
			// !!!!!!!!!!!!! this will leak!!!
			//Dae::Load(Obj, Obj->FileDir, Memory, false, Assets);
		}
	}
}

// Initial setup, from loading assets to initializing data
void GameSetup()
{
	GlobalThreadTransMem->Head = (uint8 *)GlobalThreadTransMem->Memory;
	assets::LoadAssets(&Globals->AssetsList, Globals->AssetRootDir, GlobalThreadTransMem);
}

void GameSetupThread(void* Params, int32 ThreadID)
{
	GameSetup();
}

void ImGuiSetEditorTheme()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void ImGuiSetGameTheme()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border]                 = ImVec4(1.0f, 1.0f, 1.0f, 0.29f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.76f, 0.19f, 0.15f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button]                 = ImVec4(0.76f, 0.19f, 0.12f, 0.54f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.44f, 0.10f, 0.07f, 0.54f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.76f, 0.19f, 0.12f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);


	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding                     = ImVec2(8.00f, 5.00f);
	style.FramePadding                      = ImVec2(3.00f, 2.00f);
	//style.CellPadding                       = ImVec2(6.00f, 6.00f);
	style.ItemSpacing                       = ImVec2(6.00f, 3.00f);
	style.ItemInnerSpacing                  = ImVec2(3.00f, 3.00f);
	style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
	style.IndentSpacing                     = 17;
	style.ScrollbarSize                     = 14;
	style.GrabMinSize                       = 10;
	style.WindowBorderSize                  = 1;
	style.ChildBorderSize                   = 0;
	style.PopupBorderSize                   = 0;
	style.FrameBorderSize                   = 0;
	style.TabBorderSize                     = 0;

	style.WindowRounding                    = 0;
	style.ChildRounding                     = 0;
	style.FrameRounding                     = 0;
	style.PopupRounding                     = 0;
	style.ScrollbarRounding                 = 0;
	style.GrabRounding                      = 0;
	//style.LogSliderDeadzone                 = 0;
	style.TabRounding                       = 0;
}

WIN_EXPORT void GameLoop(game_memory * Memory, game_input * GameInput, window_info * WindowInfo, game_audio_output_buffer * AudioBuffer, char* RootAssetPath)
{
	Assert(sizeof(engine_state) <= Memory->PermanentMemory.Size);
	engine_state *GameState = (engine_state *)Memory->PermanentMemory.Memory;
	state_to_serialize* State = &GameState->StateSerializing;
	Assert(GameState);

	MemoryReset(&Memory->TransientMemory);

	// Setup global state
	RenderApi = Memory->RenderApi;
	PlatformApi = Memory->PlatformApi;
	GlobalTransMem = &Memory->TransientMemory;
	GlobalThreadTransMem = &Memory->ThreadTransientMemory;
	GlobalPermMem = &Memory->PermanentMemory;

	Globals = GameState->Globals;

	render::Data = GameState->RenderData;

	GlobalProfilerData = &GameState->ProfilerData;
	GlobalProfilerData->Platform = &PlatformApi;
	GameState->OGLProfilerData.Platform = &PlatformApi;

	// -------------------------------------------------------------------------

	GameState->GameRenderer.RenderCommands = CreateList(GlobalTransMem, sizeof(render_command));
	GameState->DebugUIRenderer.RenderCommands = CreateList(GlobalTransMem, sizeof(render_command));
	GameState->UIRenderer.RenderCommands = CreateList(GlobalTransMem, sizeof(render_command));

	GameState->ClearColor = color{60.0f / 255.0f, 72.0f / 255.0f, 87.0f / 255.0f, 1.0f};

	State->TimeRunningMS += (uint64)GameState->DeltaTimeMS;

	if (GameInput->MouseLeft.OnDown) {
		GameInput->MousePosOnDown = GameInput->MousePos;
	}

	static thread_work* SetupWork = {};

	// Initialization
	if (!Memory->IsInitialized) {
		string AssetRootDir = RootAssetPath;
		GameState->RootAssetPath = AssetRootDir;

		Memory->IsInitialized = true;
		GameState->Assets = (game_assets*)ArenaAllocate(&Memory->PermanentMemory, sizeof(game_assets));
		GameState->Globals = (globals*)ArenaAllocate(&Memory->PermanentMemory, sizeof(globals));
		GameState->RenderData = (render::data*)ArenaAllocate(&Memory->PermanentMemory, sizeof(render::data));

		Globals = GameState->Globals;
		Globals->Window = WindowInfo;
		Globals->Input = GameInput;
		Globals->AssetRootDir = AssetRootDir;
		Globals->GameState = GameState;

		render::Data = GameState->RenderData;

		GizmoPushColor(COLOR_RED);

		AllocateProfiler();

		State->Scene.Init();

		Globals->UIPanels = (ui_panels*)ArenaAllocate(GlobalPermMem, sizeof(ui_panels) * (int)panel_id::count);
		Globals->UIPanels->Setup();
		Globals->UIUserInput = GameInput;

		// Load and setup basic engine resources
		{
			// Load engine resources unit cube
			Dae::Load(&GameState->Assets->UnitCube, AssetRootDir + "UnitCube.dae", true, GlobalTransMem);
			Globals->UnitCube = assets::AllocateEntity("UnitCube");

			// Engine resource shaders
			{
				Globals->ShaderLoader.Load(&Globals->AssetsList.EngineResources.ScreenDrawShader,
				                           AssetRootDir + "Shaders/ScreenDraw.vs",
				                           AssetRootDir + "Shaders/ScreenDraw.fs",
				                           GlobalTransMem);
				RenderApi.MakeProgram(&Globals->AssetsList.EngineResources.ScreenDrawShader);

				Globals->ShaderLoader.Load(&Globals->AssetsList.EngineResources.ScreenDrawTextureShader,
				                           AssetRootDir + "Shaders/ScreenDrawTexture.vs",
				                           AssetRootDir + "Shaders/ScreenDrawTexture.fs",
				                           GlobalTransMem);
				RenderApi.MakeProgram(&Globals->AssetsList.EngineResources.ScreenDrawTextureShader);

				Globals->ShaderLoader.Load(&Globals->AssetsList.EngineResources.ImGuiShader,
				                           AssetRootDir + "Shaders/ImGui.vs",
				                           AssetRootDir + "Shaders/ImGui.fs",
				                           GlobalTransMem);
				RenderApi.MakeProgram(&Globals->AssetsList.EngineResources.ImGuiShader);
			}

			// Create default white image for renderer
			render::Data->WhiteImage = assets::GLLoadPNG(RootAssetPath + string{"SingleWhitePixel.png"}, gl_blend_type::linear, correct_gamma::no, GlobalTransMem);

			// Cameras and some lights. Not all of this is stricly engine resources
			{
				// Init Camera
				InitCamera(&GameState->GameCamera, &GameState->Assets->CamBasicShader,
				           vector2{(real64)WindowInfo->Width, (real64)WindowInfo->Height}, projection::orthographic, 0,
				           0.1f, 10.0f);
				GameState->GameCamera.EulerRotation = vector3{PI * -0.5f, -1.57, 0};

				InitCamera(&State->UICam, &GameState->Assets->CamBasicShader,
				           vector2{(real64)WindowInfo->Width, (real64)WindowInfo->Height}, projection::brokenui, 0,
				           -10.0f, 10.0f);
				State->UICam.EulerRotation = vector3{PI, -0.88f, 0};

				InitCamera(&State->BattleCam, &GameState->Assets->CamBasicShader,
				           vector2{(real64)WindowInfo->Width, (real64)WindowInfo->Height}, projection::perspective, 0,
				           0.1f, 1200.0f);
				State->BattleCam.EulerRotation = vector3{3.14159f, -0.822f, 0};
				State->BattleCam.Center = vector3{ -9.024, 0, 18.0f};

				InitCamera(&State->EditorCamera, &GameState->Assets->CamBasicShader,
				           vector2{(real64)WindowInfo->Width, (real64)WindowInfo->Height}, projection::perspective, 0,
				           0.1f, 1200.0f);

				State->ActiveCam = &GameState->GameCamera;

				// Light camera
				InitCamera(&State->Light.Cam, &GameState->Assets->CamBasicShader,
				           vector2{(real64)WindowInfo->Width, (real64)WindowInfo->Height}, projection::perspective, 0,
				           0.1f, 1200.0f);
				State->Light.Cam.EulerRotation = vector3{2.36f, -1.184f, 0.0f};
				State->Light.Cam.Center = vector3{ -3.5f, 3.6f, 7.8f};

				{
					State->LightTwo.Type = light_type::directional;
					State->LightTwo.Transform.LocalPos = vector3{0, 0, 2};
					State->LightTwo.Transform.LocalRot = vector3{ -0.299, -3.47f, 0.321f};
					State->LightTwo.Color = color{10, 10, 10};
					render::RegisterLight(&State->LightTwo);
				}
			}

			assets::UploadAllQueuedImages(&Globals->AssetsList, GlobalTransMem);
		}

		GameState->GameRenderer.Camera = &GameState->GameCamera;
		GameState->DebugUIRenderer.Camera = &State->UICam;
		GameState->UIRenderer.Camera = &State->UICam;

		Globals->GameRenderer = &GameState->GameRenderer;
		Globals->UIRenderer = &GameState->UIRenderer;
		Globals->DebugUIRenderer = &GameState->DebugUIRenderer;

		// Setup and Load game data
		// Load stuff
		{
			//SetupWork = PlatformApi.ThreadAddWork(GameSetupThread, 0);
			//GameSetup();
			assets::LoadAssets(&Globals->AssetsList, Globals->AssetRootDir, GlobalTransMem);

			// Compile shaders
			for (int i = 0; i < Globals->AssetsList.ShadersCount; i++) {

				ConsoleLog("Compiling shader");
				shader* Shader = &Globals->AssetsList.Shaders[i].Shader;

				RenderApi.MakeProgram(Shader);
			}

			assets::UploadAllQueuedImages(&Globals->AssetsList, GlobalTransMem);
			//assets::UploadAllQueuedEntities(&Globals->AssetsList);
		}

		//PanelStackPush(panel_id::home, &State->PanelStack, State);
		Start(GameState);

#if UNIT_TESTING
		LinkedListUnitTests(&Memory->TransientMemory);

		// Verify type sizes
		Assert(sizeof(uint8) == 1);
		Assert(sizeof(uint16) == 2);
		Assert(sizeof(uint32) == 4);
		Assert(sizeof(uint64) == 8);

		//StringUnitTests(&Memory->TransientMemory);
		RandomUnitTests();

		// Remove slide testing
		{
			int32 Array[10];
			for (int i = 0; i < ArrayCount(Array); i++) {
				Array[i] = i;
			}

			RemoveSlideArray((void*)&Array[0], ArrayCount(Array), sizeof(int32), 1);
			RemoveSlideArray((void*)&Array[0], ArrayCount(Array), sizeof(int32), 5);
		}
#endif
	}

	Globals->ActiveCam = State->ActiveCam;

	// Update camera matricies
	State->ActiveCam->UpdateMatricies();
	State->UICam.UpdateMatricies();

	if (Memory->HotReloaded) {
		Globals->UIPanels->Setup();
	}


	// Editor / mouse touch emulation
	{
		// Toggle on / off
		bool EditorPrev = Globals->EditorData.EditorMode;

		/*
		// Do mouse touch emulation
		editor::SwitchToEditorCam(GameState);

		static int GLID = assets::GetImage("Gizmo_Circle")->GLID;
		RenderTextureCenter(Globals->Input->MousePos,
		                    vector2{8, 8},
		                    color{1, 0, 0, 1},
		                    vector2{}, vector2{1, 1},
		                    GLID,
		                    0,
		                    Globals->DebugUIRenderer);
		*/

		if (Globals->EditorData.EditorMode) {
			// Disable touch input
			Globals->Input->TouchStatus[0].OnDown = false;
			Globals->Input->TouchStatus[0].IsDown = false;
		} else {

			// Disable mouse input
			/*
			Globals->Input->MouseLeft.OnDown = false;
			Globals->Input->MouseLeft.IsDown = false;
			Globals->Input->MouseRight.OnDown = false;
			Globals->Input->MouseRight.IsDown = false;
			Globals->Input->MousePos = vector2{ -1, -1};
			*/
		}
	}

	ui::NextAnimState = 0;

	// imgui init
	if (!Memory->ImguiInitialized) {

		Memory->ImguiInitialized = true;
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = (float)WindowInfo->Width;
		io.DisplaySize.y = (float)WindowInfo->Height;

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
		// Removed for android. ImGui doesn't do anything on android, so what do?
		io.KeyMap[ImGuiKey_Backspace] = KEY_BACK;
		io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = KEY_PRIOR;
		io.KeyMap[ImGuiKey_PageDown] = KEY_NEXT;
		io.KeyMap[ImGuiKey_Home] = KEY_HOME;
		io.KeyMap[ImGuiKey_End] = KEY_END;
		io.KeyMap[ImGuiKey_Insert] = KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
		io.KeyMap[ImGuiKey_Space] = KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = KEY_RETURN;
		io.KeyMap[ImGuiKey_Escape] = KEY_ESCAPE;

		//io.IniFilename = NULL;

		// ImGui create font texture
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA

		loaded_image Result = {};
		Result.Width = width;
		Result.Height = height;
		RenderApi.MakeTexture(&Result, (uint32*)pixels, false);
		io.Fonts->SetTexID((ImTextureID)(intptr_t)Result.GLID);
	}

	// imgui each frame
	{
		// Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = (float)WindowInfo->Width;
		io.DisplaySize.y = (float)WindowInfo->Height;
		io.DeltaTime = 1.0f / 60.0f;

		if (GameInput->MouseScrollDelta != 0) {
			io.MouseWheel += (float)(GameInput->MouseScrollDelta > 0 ? +1.0f : -1.0f);
		}

		// update keyboard input
		for (int index = 0; index < ArrayCount(GameInput->KeyboardInput); index++) {
			io.KeysDown[index] = GameInput->KeyboardInput[index].IsDown ? 1 : 0;

			// clear keyboard after sending to imgui so we don't trigger any of our own hotkeys
			if (io.WantTextInput) {
				GameInput->KeyboardInput[index] = {};
			}
		}

		//char KeyboardState[256] = {};
		// PBYTE KeyboardStatePointer = (PBYTE)(&KeyboardState);
		//GetKeyboardState((char*)&KeyboardState);

		io.AddInputCharacter(GameInput->InputChar);
		GameInput->InputChar = 0;

		ImVec2 mousePos = {};
		mousePos.x = (float)GameInput->MousePos.X;
		mousePos.y = (float)GameInput->MousePos.Y;
		io.MousePos = mousePos;
		io.MouseDown[0] = GameInput->MouseLeft.IsDown != 0;
		io.MouseDown[1] = GameInput->MouseRight.IsDown != 0;
	}

	ImGui::NewFrame();

	// Update touch input
	{
		real64 DragThreshold = 35.0f;
		static bool WentOutThreshold = false;

		for (int i = 0; i < TOUCH_COUNT; i++) {
			touch* Touch = &Globals->Input->Touches[i];

			// Tap gesture only lasts one frame, so rest it
			if (Touch->Gesture == touch_gesture::tap) {
				Touch->Gesture = touch_gesture::none;
			}

			if (Globals->Input->TouchStatus[i].OnDown) {
				Touch->Origin = Globals->Input->TouchPosition[i];
				Touch->Gesture = touch_gesture::none;
			}
			if (Globals->Input->TouchStatus[i].IsDown) {
				Touch->Position = Globals->Input->TouchPosition[i];
				Touch->Delta = Touch->Origin - Globals->Input->TouchPosition[i];

				Touch->FrameDelta = Touch->PositionLastFrame - Touch->Position;
				Touch->PositionLastFrame = Touch->Position;

				Touch->Gesture = touch_gesture::hold;

				if (Vector2Distance(Touch->Delta, {}) > DragThreshold) {
					WentOutThreshold = true;
				}

				if (WentOutThreshold) {
					Touch->Gesture = touch_gesture::drag;
				}
			}
			if (Globals->Input->TouchStatus[i].OnUp) {
				WentOutThreshold = false;

				if (Touch->Gesture == touch_gesture::hold) {
					Touch->Gesture = touch_gesture::tap;
				} else {
					Touch->Gesture = touch_gesture::none;
				}
			}
		}
	}

	// Editor things
	{
		ImGuiStyle& style = ImGui::GetStyle();
		if (!Globals->EditorData.EditorMode) {
			style.Alpha = 0.25f;
		}
		if (Globals->Input->KeyboardInput[KEY_TAB].OnDown) {
			Globals->EditorData.EditorMode = !Globals->EditorData.EditorMode;
		}

		// TODO Don't compile on release builds
		//editor::Update(GameState, WindowInfo);

		style.Alpha = 1.0f;
	}

	if (!BuildConfig::Release) {
		Globals->ShaderLoader.PollReload();
		PollModelReload(Memory, GameState->Assets);
	}

	static bool DemoShowing = true;
	//ImGui::ShowDemoWindow(&DemoShowing);

	// Prevent mouse clicks for game if imgui captures mouse
	if (ImGui::GetIO().WantCaptureMouse) {
		GameInput->MouseScrollDelta = {};

		GameInput->MouseLeft.IsDown = false;
		GameInput->MouseLeft.OnDown = false;
		GameInput->MouseLeft.IsUp = false;
		GameInput->MouseLeft.OnUp = false;
		GameInput->MouseRight.OnDown = false;
		GameInput->MouseRight.IsDown = false;
		GameInput->MouseRight.IsUp = false;
		GameInput->MouseRight.OnUp = false;
	}

	// Camera
	if (Globals->EditorData.EditorMode) {
		editor::SwitchToEditorCam(GameState);

		static bool MouseControlling = false;
		if (GameInput->MouseRight.OnDown) {
			MouseControlling = true;
		}
		if (GameInput->MouseRight.OnUp) {
			MouseControlling = false;
		}

		real64 MouseSpeed = 0.004f;

		if (MouseControlling) {
			static vector2 MouseStart = {};
			static vector3 EulerStart = {};
			if (GameInput->MouseRight.OnDown) {
				MouseStart = GameInput->MousePos;
				EulerStart = State->ActiveCam->EulerRotation;
			}

			State->ActiveCam->EulerRotation.Y += GameInput->MouseDelta.Y * MouseSpeed;
			State->ActiveCam->EulerRotation.X += GameInput->MouseDelta.X * MouseSpeed;

			real64 Bottom = (-PI / 2.0f) + 0.01f;
			real64 Top = -Bottom;
			State->ActiveCam->EulerRotation.Y = ClampValue(Bottom, Top, State->ActiveCam->EulerRotation.Y);

			//Globals->EditorData.EditorMode = !Globals->EditorData.EditorMode;
		}

		if (GameInput->MouseRight.OnDown) {
			GameState->LockCursor = true;
		}
		if (GameInput->MouseRight.OnUp) {
			GameState->LockCursor = false;
		}

		// Fly camera controls
		if (!ImGui::GetIO().WantCaptureMouse && State->ActiveCam == &State->EditorCamera) {

			//real64 KeyboardSpeed = 3;
			real64 KeyboardSpeed = 1;
			if (GameInput->KeyboardInput[KEY_SHIFT].IsDown) {
				//KeyboardSpeed = 1.5f;
			}

			real64 WindowSpeed = 4.0f;

			if (GameInput->KeyboardInput['E'].IsDown) {
				State->ActiveCam->Center.Z += KeyboardSpeed;
				editor::SwitchToEditorCam(GameState);
			}
			if (GameInput->KeyboardInput['Q'].IsDown) {
				State->ActiveCam->Center.Z -= KeyboardSpeed;
				editor::SwitchToEditorCam(GameState);
			}
			if (GameInput->KeyboardInput['A'].IsDown) {

				vector3 Right = Vector3Cross(State->ActiveCam->Forward, vector3{0, 0, 1});
				Right = Vector3Normalize(Right);
				State->ActiveCam->Center = State->ActiveCam->Center - (Right * (real32)KeyboardSpeed);

				editor::SwitchToEditorCam(GameState);
			}
			if (GameInput->KeyboardInput['D'].IsDown) {

				vector3 Right = Vector3Cross(State->ActiveCam->Forward, vector3{0, 0, 1});
				Right = Vector3Normalize(Right);
				State->ActiveCam->Center = State->ActiveCam->Center + (Right * (real32)KeyboardSpeed);

				editor::SwitchToEditorCam(GameState);
			}
			if (GameInput->KeyboardInput['W'].IsDown) {

				if (GameInput->MouseRight.IsDown) {
					State->ActiveCam->Center = State->ActiveCam->Center - (State->ActiveCam->Forward * (real32)KeyboardSpeed);
				} else {
					m4y4 r = RotationZ(-(real32)State->ActiveCam->EulerRotation.X);
					vector3 Dir = Apply4y4(r, vector3{1, 0, 0});
					State->ActiveCam->Center = State->ActiveCam->Center - (Dir * (real32)KeyboardSpeed);
				}

				editor::SwitchToEditorCam(GameState);
			}
			if (GameInput->KeyboardInput['S'].IsDown) {

				if (GameInput->MouseRight.IsDown) {
					State->ActiveCam->Center = State->ActiveCam->Center + (State->ActiveCam->Forward * (real32)KeyboardSpeed);
				} else {
					m4y4 r = RotationZ(-(real32)State->ActiveCam->EulerRotation.X);
					vector3 Dir = Apply4y4(r, vector3{1, 0, 0});
					State->ActiveCam->Center = State->ActiveCam->Center + (Dir * (real32)KeyboardSpeed);
				}

				editor::SwitchToEditorCam(GameState);
			}
		}
	} else {
		State->ActiveCam = &GameState->GameCamera;
	}

	if (GameInput->KeyboardInput['B'].OnDown) {
		Globals->EditorData.GodMode = !Globals->EditorData.GodMode;
	}

	// Panel stuff
	/*
	{
		ui_panel* ActivePanel = State->PanelStack.Panels[State->PanelStack.NextIndex - 1];
		if (ActivePanel->Overlay) {
			Globals->UIUserInput = &EmptyInput;
		}

		// Render active panel and overlay
		if (ActivePanel->Overlay) {

			State->PanelStack.Panels[State->PanelStack.NextIndex - 2]->Render(State, WindowInfo);

			// Overlay background
			RenderRect(rect{0, 0, (real64)WindowInfo->Width, (real64)WindowInfo->Height}, color{0, 0, 0, 0.8f}, 0, Globals->UIRenderer);
		}

		Globals->UIUserInput = GameInput;
		ActivePanel->Render(State, WindowInfo);
	}
	*/

	ImGuiSetGameTheme();
	Loop(GameState, WindowInfo, GameInput);
	ImGuiSetEditorTheme();

	// Keep at the end, so it reports correctly on the frame
	if (Globals->EditorData.ProfilerWindowOpen) {
		profiler::ImGuiProfiler(GameState, Memory, &GameState->ProfilerData, &GameState->OGLProfilerData);
	}
	profiler::InitProfiler(&GameState->ProfilerData);
	profiler::InitProfiler(&GameState->OGLProfilerData);

	// Render imgui
	{
		ImGui::EndFrame();

		ImGui::Render();

		ImDrawData* draw_data = ImGui::GetDrawData();
		ImVec2 pos = draw_data->DisplayPos;
		for (int n = 0; n < draw_data->CmdListsCount; n++) {

			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
			const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

				render_command RendCommand = {};
				InitRenderCommand(&RendCommand, pcmd->ElemCount);

				shader* Shader = &Globals->AssetsList.EngineResources.ImGuiShader;
				if (Shader == GameNull) {
					ConsoleLog("Missing imgui shader");
					continue;
				}

				RendCommand.Shader = *Shader;

				layout_data* VertexLayout = RendCommand.GetLayout();
				VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), RendCommand.BufferCapacity, GlobalTransMem);
				VertexLayout->LayoutInfo->Loc = RenderApi.GetAttribLocation(Shader, render::ShaderVertID);

				layout_data* TextureLayout = RendCommand.GetLayout();
				TextureLayout->Allocate(Shader->GetLayout(render::ShaderTextureCoordsID), RendCommand.BufferCapacity, GlobalTransMem);
				TextureLayout->LayoutInfo->Loc = RenderApi.GetAttribLocation(Shader, render::ShaderTextureCoordsID);

				layout_data* ColorLayout = RendCommand.GetLayout();
				ColorLayout->Allocate(Shader->GetLayout(render::ShaderColorID), RendCommand.BufferCapacity, GlobalTransMem);
				ColorLayout->LayoutInfo->Loc = RenderApi.GetAttribLocation(Shader, render::ShaderColorID);

				RendCommand.ClipRect = vector4{pcmd->ClipRect.x, pcmd->ClipRect.y, pcmd->ClipRect.z, pcmd->ClipRect.w};

				// Copy index buffer
				for (uint32 i = 0; i < pcmd->ElemCount; i++) {
					int32 EI = idx_buffer[i];
					ImColor ImCol = ImColor(vtx_buffer[EI].col);

					VertexLayout->Data.Vec3[i] = v3{vtx_buffer[EI].pos.x, vtx_buffer[EI].pos.y, 0};
					TextureLayout->Data.Vec2[i] = v2{vtx_buffer[EI].uv.x, vtx_buffer[EI].uv.y};
					ColorLayout->Data.Vec4[i] = v4{ImCol.Value.x, ImCol.Value.y, ImCol.Value.z, ImCol.Value.w};

					RendCommand.IndexBuffer[i] = i;
				}

				idx_buffer += pcmd->ElemCount;

				// Copy uniforms
				RendCommand.Uniforms = RendCommand.Shader.Uniforms.Copy(GlobalTransMem);

				RendCommand.Uniforms.SetImage("diffuseTex", (uint64)pcmd->TextureId);
				RendCommand.Uniforms.SetMat4("projection", m4y4Transpose(GameState->DebugUIRenderer.Camera->ProjectionMatrix));
				RendCommand.Uniforms.SetMat4("view", m4y4Transpose(GameState->DebugUIRenderer.Camera->ViewMatrix));

				InsertRenderCommand(&GameState->DebugUIRenderer, &RendCommand);
			}
		}
	}
}

// This must happen after all TIME_BEGIN calls so that COUNTER is exactly the number of TIME_BEGIN calls
// COPIED INTO ogles3.cpp
void AllocateProfiler()
{
	GlobalProfilerData->TimesCount = COUNTER;

	GlobalProfilerData->AccumulatedMethods = (accum_method*)ArenaAllocate(GlobalPermMem, GlobalProfilerData->TimesCount * sizeof(accum_method));
	GlobalProfilerData->AccumulatedMethodsGraphs = (graph_data*)ArenaAllocate(GlobalPermMem, GlobalProfilerData->TimesCount * sizeof(graph_data));
}

#endif
