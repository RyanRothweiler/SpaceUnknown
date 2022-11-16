#pragma once
#ifndef ENGINE_CORE
#define ENGINE_CORE

//#include "httplib.h"
#include "EngineCore.h"
#include "Generated.h"

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
#include "Math.cpp"

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
#include "Grid.cpp"
#include "Range.cpp"

#include "Json.cpp"

#include "Audio.cpp"
#include "Sorts.cpp"

#include "SaveData.cpp"

#include "ParticleSystem.cpp"
#include "Util.cpp"

#include "../Game/AssetList.h"
#include "CSV.cpp"

#include "Editor.h"
#include "Renderer/ShaderLoader.cpp"
#include "ui/UiPanel.h"

struct globals {

	// Here for loading thread. Better way to do this than make them globals??
	string AssetRootDir;
	game_state* GameState;

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

void BakeIBL()
{
	game_state* GameState = Globals->GameState;
	state_to_serialize * State = &GameState->StateSerializing;

	/*
	// Load cubemap
	{
		// NOTE we get dangling warning here when compiling android. Is that true? Is this an issue?
		char* Files[6] = {
			(AssetRootDir + "Skybox/space/right.png").CharArray,
			(AssetRootDir + "Skybox/space/left.png").CharArray,
			(AssetRootDir + "Skybox/space/top.png").CharArray,
			(AssetRootDir + "Skybox/space/bottom.png").CharArray,
			(AssetRootDir + "Skybox/space/front.png").CharArray,
			(AssetRootDir + "Skybox/space/back.png").CharArray
		};
		Assets->SpaceCubeMap = GLLoadCubeMap(Files, TransMemory);
	}
	*/

	// Cubemaps
	{
		// NOTE we get dangling warning here when compiling android. Is that true? Is this an issue?
		char* Files[6] = {
			(Globals->AssetRootDir + "Skybox/space/right.png").CharArray,
			(Globals->AssetRootDir + "Skybox/space/left.png").CharArray,
			(Globals->AssetRootDir + "Skybox/space/top.png").CharArray,
			(Globals->AssetRootDir + "Skybox/space/bottom.png").CharArray,
			(Globals->AssetRootDir + "Skybox/space/front.png").CharArray,
			(Globals->AssetRootDir + "Skybox/space/back.png").CharArray
		};
		Globals->AssetsList.SpaceCubeMap = assets::GLLoadCubeMap(Files, GlobalTransMem);
	}

	// HDR
	{
		Globals->AssetsList.HDRImage = assets::GLLoadHDRTexture((Globals->AssetRootDir + "HDR_IBL/Arches_E_PineTree/Arches_E_PineTree_3k.hdr").CharArray);
		//Assets->HDRImage = GLLoadHDRTexture((AssetRootDir + "HDR_IBL/Ditch_River/Ditch-River_2k.hdr").CharArray);
		//Assets->HDRImage = GLLoadHDRTexture((AssetRootDir + "HDR_IBL/Theatre_Seating/Theatre-Side_2k.hdr").CharArray);
	}

	// load brdf lut
	GameState->BRDFlut = assets::GLLoadPNG(Globals->AssetRootDir + "ibl_brdf_lut.png", gl_blend_type::linear, correct_gamma::no, GlobalTransMem);

	// Setup Cameras
	camera EquiCam = {};
	camera DiffuseConvCam = {};
	camera SpecularConvCam = {};

	InitCamera(&EquiCam, 			GameNull, vector2{512, 512}, 	projection::perspective, 90, 0.1f, 1200.0f);
	InitCamera(&DiffuseConvCam, 	GameNull, vector2{32, 32}, 		projection::perspective, 90, 0.1f, 1200.0f);
	InitCamera(&SpecularConvCam, 	GameNull, vector2{128, 128}, 	projection::perspective, 90, 0.1f, 1200.0f);

	renderer EquiRenderer = {};
	renderer ConvRenderer = {};
	renderer PreFilterRenderer = {};

	RenderApi.GetFramebufferCubeMap(&EquiCam, false, true);
	RenderApi.GetFramebufferCubeMap(&DiffuseConvCam, false, false);
	RenderApi.GetFramebufferCubeMap(&SpecularConvCam, true, true);

	// Create materials
	material EquiMat = {};
	material IBLConvMat = {};
	material PreFilterMat = {};

	EquiMat.Create(assets::GetShader("EquiToCube"), GlobalPermMem);
	EquiMat.Uniforms.SetImage("equirectangularMap", Globals->AssetsList.HDRImage.GLID);

	IBLConvMat.Create(assets::GetShader("IBLConv"), GlobalPermMem);
	//IBLConvMat.Uniforms.SetImage("envMap", EquiCam.TextureColorbuffers[0]);
	IBLConvMat.Uniforms.SetImage("envMap", Globals->AssetsList.SpaceCubeMap.GLID);

	PreFilterMat.Create(assets::GetShader("PreFilter"), GlobalPermMem);
	PreFilterMat.Uniforms.SetImage("environmentMap", Globals->AssetsList.SpaceCubeMap.GLID);

	// Load models
	entity EquiToCubemapCube = {};
	entity IBLConvCube = {};
	entity PreFilterCube = {};

	Dae::Load(&EquiToCubemapCube, Globals->AssetRootDir + "UnitCube.dae", true, GlobalTransMem);
	Dae::Load(&IBLConvCube, Globals->AssetRootDir + "UnitCube.dae", true, GlobalTransMem);
	Dae::Load(&PreFilterCube, Globals->AssetRootDir + "UnitCube.dae", true, GlobalTransMem);

	// Set material
	EquiToCubemapCube.Material = &EquiMat;
	IBLConvCube.Material = &IBLConvMat;
	PreFilterCube.Material = &PreFilterMat;

	// Upload to gpu
	EquiToCubemapCube.VAO = RenderApi.CreateVAO();
	assets::UploadEntity(&EquiToCubemapCube);

	IBLConvCube.VAO = RenderApi.CreateVAO();
	assets::UploadEntity(&IBLConvCube);

	PreFilterCube.VAO = RenderApi.CreateVAO();
	assets::UploadEntity(&PreFilterCube);

	// Setup and Add entities to render list
	{
		// equi to cubemap
		{
			EquiRenderer.RenderCommands = CreateList(GlobalTransMem, sizeof(render_command));
			EquiRenderer.Camera = &EquiCam;

			EquiToCubemapCube.Transform.Update(m4y4Identity());
			RenderEntity(&EquiToCubemapCube, &EquiRenderer, 100);
		}

		// diffuse convolude
		{
			ConvRenderer.RenderCommands = CreateList(GlobalTransMem, sizeof(render_command));
			ConvRenderer.Camera = &DiffuseConvCam;

			vector3 VecRot = vector3{ -PI / 2, PI, 0};
			quat QuatRot = {};
			QuatRot.FromEuler(VecRot);
			m4y4 Rot = QuatRot.ToMatrix();

			IBLConvCube.Transform.Update(Rot);
			RenderEntity(&IBLConvCube, &ConvRenderer, 100);

		}

		// prefilter / specular convolude
		{
			PreFilterRenderer.RenderCommands = CreateList(GlobalTransMem, sizeof(render_command));
			PreFilterRenderer.Camera = &SpecularConvCam;

			vector3 VecRot = vector3{ -PI / 2, PI, 0};
			quat QuatRot = {};
			QuatRot.FromEuler(VecRot);
			m4y4 Rot = QuatRot.ToMatrix();

			PreFilterCube.Transform.Update(Rot);
			RenderEntity(&PreFilterCube, &PreFilterRenderer, 100);
		}
	}

	// Actually do the bake render
	RenderApi.BakeIBL(&EquiRenderer, &ConvRenderer, &PreFilterRenderer, Globals->Window);

	render::Data->IrradianceMap = ConvRenderer.Camera->TextureColorbuffers[0];
	render::Data->PrefilterMap = PreFilterRenderer.Camera->TextureColorbuffers[0];
	render::Data->BRDFlut = GameState->BRDFlut.GLID;

	// skybox cube
	{
		State->SkyboxMaterial.Create(assets::GetShader("Skybox"), GlobalPermMem);
		State->SkyboxMaterial.Uniforms.SetImage("skybox", EquiCam.TextureColorbuffers[0]);

		State->SkyboxMaterial.Uniforms.SetImage("skybox", Globals->AssetsList.SpaceCubeMap.GLID);
		//State->SkyboxMaterial.Uniforms.SetImage("skybox", State->DiffuseConvCam.TextureColorbuffers[0]);
		//State->SkyboxMaterial.Uniforms.SetImage("skybox", State->SpecularConvCam.TextureColorbuffers[0]);
		GameState->Assets->UnitCube.Material = &State->SkyboxMaterial;

		GameState->Assets->UnitCube.VAO = RenderApi.CreateVAO();
		assets::UploadMesh(&GameState->Assets->UnitCube.VAO, GameState->Assets->UnitCube.Children[0], assets::GetShader("Skybox"));
	}
}

// Initial setup, from loading assets to initializing data
void GameSetup()
{
	game_state* GameState = Globals->GameState;
	state_to_serialize * State = &GameState->StateSerializing;

	GlobalThreadTransMem->Head = (uint8 *)GlobalThreadTransMem->Memory;

	assets::LoadAssets(&Globals->AssetsList, Globals->AssetRootDir, GlobalThreadTransMem);
}

void SavePlayerData()
{

}

void GameSetupThread(void* Params, int32 ThreadID)
{
	GameSetup();
}

void GameLoop(game_memory * Memory, game_input * GameInput, window_info * WindowInfo, game_audio_output_buffer * AudioBuffer, char* RootAssetPath)
{
	Assert(sizeof(game_state) <= Memory->PermanentMemory.Size);
	game_state *GameState = (game_state *)Memory->PermanentMemory.Memory;
	state_to_serialize* State = &GameState->StateSerializing;
	Assert(GameState);

	Memory->TransientMemory.Head = (uint8 *)Memory->TransientMemory.Memory;

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

	static thread_work* SetupWork = {};

	/*
	{
		//https://httpbin.org/#/HTTP_Methods/get_get

		httplib::Client cli("http://httpbin.org/get");

		httplib::Headers headers = {
			{ "content-type", "application/json" }
		};
		if (auto res = cli.Get("/hi")) {
			if (res->status == 200) {
				std::cout << res->body << std::endl;
			}
		} else {
			auto err = res.error();
			std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
		}
	}
	*/


	// Initialization
	if (!Memory->IsInitialized) {
		string AssetRootDir = RootAssetPath;

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
				Globals->ShaderLoader.Load(&GameState->Assets->GaussianBlurShader,
				                           AssetRootDir + "Shaders/GaussianBlur.vs",
				                           AssetRootDir + "Shaders/GaussianBlur.fs",
				                           GlobalTransMem);
				RenderApi.MakeProgram(&GameState->Assets->GaussianBlurShader);

				Globals->ShaderLoader.Load(&GameState->Assets->CamBasicShader,
				                           AssetRootDir + "Shaders/CamBasic.vs",
				                           AssetRootDir + "Shaders/CamBasic.fs",
				                           GlobalTransMem);
				RenderApi.MakeProgram(&GameState->Assets->CamBasicShader);

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

				Globals->ShaderLoader.Load(&Globals->AssetsList.EngineResources.FontSDFShader,
				                           AssetRootDir + "Shaders/FontSDF.vs",
				                           AssetRootDir + "Shaders/FontSDF.fs",
				                           GlobalTransMem);
				RenderApi.MakeProgram(&Globals->AssetsList.EngineResources.FontSDFShader);

				assets::LoadFontMSDF(&Globals->AssetsList.EngineResources.DefaultFont, AssetRootDir + "Fonts/OpenSans/OpenSans_Bold", GlobalTransMem, &Globals->AssetsList);
				Globals->AssetsList.EngineResources.DefaultFont.LineHeight = 1.361f;
				Globals->AssetsList.EngineResources.DefaultFontStyle.Font = &Globals->AssetsList.EngineResources.DefaultFont;
				Globals->AssetsList.EngineResources.DefaultFontStyle.SizePoints = 15.0f;
			}

			// Create default white image for renderer
			render::Data->WhiteImage = assets::GLLoadPNG(RootAssetPath + string{"SingleWhitePixel.png"}, gl_blend_type::linear, correct_gamma::no, GlobalTransMem);

			// Cameras and some lights. Not all of this is stricly engine resources
			{
				// Init Camera
				InitCamera(&State->GameCamera, &GameState->Assets->CamBasicShader,
				           vector2{(real64)WindowInfo->Width, (real64)WindowInfo->Height}, projection::perspective, 0,
				           0.1f, 500.0f);

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

				State->ActiveCam = &State->GameCamera;

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

			// Setup renderer backup shader
			/*
			{
				BackupShader = {};
				RenderApi.MakeProgram(&BackupShader, BackupVertexShader, BackupFragmentShader, GlobalThreadTransMem);
				Assert(BackupShader.Valid);
			}
			*/

			assets::UploadAllQueuedImages(&Globals->AssetsList, GlobalTransMem);
		}

		GameState->GameRenderer.Camera = &State->GameCamera;
		GameState->DebugUIRenderer.Camera = &State->UICam;
		GameState->UIRenderer.Camera = &State->UICam;

		Globals->GameRenderer = &GameState->GameRenderer;
		Globals->UIRenderer = &GameState->UIRenderer;
		Globals->DebugUIRenderer = &GameState->DebugUIRenderer;

		// Setup and Load game data
		SetupWork = PlatformApi.ThreadAddWork(GameSetupThread, 0);
		//GameSetup();

		//PanelStackPush(panel_id::home, &State->PanelStack, State);

#if UNIT_TESTING
		LinkedListUnitTests(&Memory->TransientMemory);

		// Verify type sizes
		Assert(sizeof(uint8) == 1);
		Assert(sizeof(uint16) == 2);
		Assert(sizeof(uint32) == 4);
		Assert(sizeof(uint64) == 8);

		StringUnitTests(&Memory->TransientMemory);
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

	// Initial Loading
	{
		static bool32 IsLoading = true;
		static int32 LoadingStage = 0;

		static string StageTitleString = "";
		static string StageProgressString = "";

		static real32 StageProgress;

		static bool32 StageEnter[100] = {};
		static char* StageTitles[100] = {
			"Loading Data",
			"Baking IBL",
			"Compiling Shaders",
			"Uploading Images",
			"Uploading Entities",
		};
		static int32 LoadingStagesTotal = 5;

		struct local {
			static void HandleStageEnter()
			{
				if (!StageEnter[LoadingStage]) {
					StageEnter[LoadingStage] = true;

					ConsoleLog(StageTitleString);
					StageProgress = 0.0f;
				}
			}

			static void AdvanceStage()
			{
				LoadingStage++;
				StageProgress = 0.0f;

				if (LoadingStage >= LoadingStagesTotal) {
					IsLoading = false;
				} else {
					assets::CurrentLoadingStepDisplay = "";
					StageProgressString = "";
					StageTitleString = StageTitles[LoadingStage];
				}
			}
		};

		if (IsLoading && SetupWork != GameNull) {

			switch (LoadingStage) {

				case 0: {

					local::HandleStageEnter();

					StageProgress = (real32)assets::CurrentLoadingStep / (real32)assets::KnownLoadingSteps;
					StageProgressString = assets::CurrentLoadingStepDisplay;

					if (SetupWork->Status == work_status::finished) {
						local::AdvanceStage();
					}
				}
				break;

				case 1: {
					local::HandleStageEnter();

					BakeIBL();

					local::AdvanceStage();
				}
				break;

				case 2: {
					local::HandleStageEnter();

					static int Index = 0;

					if (Index < Globals->AssetsList.ShadersCount) {
						StageProgress = (real32)Index / (real32)Globals->AssetsList.ShadersCount;

						ConsoleLog("Compiling shader");
						shader* Shader = &Globals->AssetsList.Shaders[Index].Shader;
						StageProgressString = Shader->VertPath + " / " + Shader->FragPath;

						RenderApi.MakeProgram(Shader);

						Index++;
					} else {
						local::AdvanceStage();
					}
				}
				break;

				case 3: {
					local::HandleStageEnter();

					static int32 TotalCount = Globals->AssetsList.ImagesToLoadCount;

					if (Globals->AssetsList.ImagesToLoadCount > 0) {
						StageProgress = 1.0f - ((real32)Globals->AssetsList.ImagesToLoadCount / (real32)TotalCount);

						loaded_image* Image = Globals->AssetsList.ImagesToLoad[Globals->AssetsList.ImagesToLoadCount - 1];
						StageProgressString = Image->FilePath;

						assets::UploadOneImage(&Globals->AssetsList, GlobalTransMem);
					} else {
						local::AdvanceStage();
					}
				}
				break;

				case 4: {
					local::HandleStageEnter();

					static int32 TotalCount = Globals->AssetsList.EntitiesToUploadCount;

					if (Globals->AssetsList.EntitiesToUploadCount > 0) {
						StageProgress = 1.0f - ((real32)Globals->AssetsList.EntitiesToUploadCount / (real32)TotalCount);

						entity* Entity = Globals->AssetsList.EntitiesToUpload[Globals->AssetsList.EntitiesToUploadCount - 1];
						StageProgressString = Entity->Name;

						assets::UploadOneEntity(&Globals->AssetsList);
					} else {
						local::AdvanceStage();
					}
				}
				break;
			};

			// Render progress bar
			{
				real32 StepMin = (real32)LoadingStage / (real32)LoadingStagesTotal;
				real32 StepMax = (real32)(LoadingStage + 1) / (real32)LoadingStagesTotal;

				real32 StepAmount = 1.0f / LoadingStagesTotal;
				real32 TotalProgress = StepMin + (StepAmount * StageProgress);

				rect LoadingBar = {};
				LoadingBar.BottomRight.X = WindowInfo->Width * TotalProgress;
				LoadingBar.BottomRight.Y = WindowInfo->Height;
				color BarCol = Icon_BlueDark;
				BarCol.A = 0.3f;
				RenderRect(LoadingBar, BarCol, 0, Globals->UIRenderer);

				real32 Left = 10;

				color DescriptionColor = COLOR_ORANGE;
				DescriptionColor.A = 0.5f;
				FontRenderString(&Globals->AssetsList.EngineResources.DefaultFontStyle, "LOADING",
				                 vector2{Left, WindowInfo->Height * 0.25f},
				                 DescriptionColor, 0, Globals->UIRenderer);

				FontRenderString(Globals->AssetsList.EngineResources.DefaultFontStyle.Font,
				                 StageTitleString,
				                 vector2{Left, (WindowInfo->Height * 0.6666f) + 100.0f},
				                 20.0f, COLOR_ORANGE, 0, Globals->UIRenderer);

				FontRenderString(Globals->AssetsList.EngineResources.DefaultFontStyle.Font,
				                 StageProgressString,
				                 vector2{Left, WindowInfo->Height * 0.6666f},
				                 10.0f, DescriptionColor, 0, Globals->UIRenderer);

				real32 DispProg = (TotalProgress * 100.0f);
				string ProgressDisp = DispProg + string{"%"};
				//string ProgressDisp = "50%";
				FontRenderString(Globals->AssetsList.EngineResources.DefaultFontStyle.Font,
				                 ProgressDisp,
				                 vector2{Left, WindowInfo->Height * 0.5f},
				                 60.0f, COLOR_ORANGE, 0, Globals->UIRenderer);
			}

			return;
		}
	}


	if (Memory->HotReloaded) {
		Globals->UIPanels->Setup();
	}


	// Editor / mouse touch emulation
	{
		// Toggle on / off
		bool EditorPrev = Globals->EditorData.EditorMode;
		if (Globals->Input->KeyboardInput[VK_TAB].OnDown) {
			Globals->EditorData.EditorMode = !Globals->EditorData.EditorMode;
		}

		// Do mouse touch emulation
		if (Globals->EditorData.EditorMode) {
			editor::SwitchToEditorCam(GameState);

			static int GLID = assets::GetImage("Gizmo_Circle")->GLID;
			RenderTextureCenter(Globals->Input->MousePos,
			                    vector2{8, 8},
			                    color{1, 0, 0, 1},
			                    vector2{}, vector2{1, 1},
			                    GLID,
			                    0,
			                    Globals->DebugUIRenderer);

			// Disable touch input
			Globals->Input->TouchStatus[0].OnDown = false;
			Globals->Input->TouchStatus[0].IsDown = false;
		} else {

			// Disable mouse input
			Globals->Input->MouseLeft.OnDown = false;
			Globals->Input->MouseLeft.IsDown = false;
			Globals->Input->MouseRight.OnDown = false;
			Globals->Input->MouseRight.IsDown = false;
			Globals->Input->MousePos = vector2{ -1, -1};
		}
	}

	if (BuildVarAutomatedTesting) {
		State->TestingDone = true;
		ConsoleLog("Running!!");
	}

	// Render skybox
	{
		vector3 VecRot = vector3{0, 0, 0};
		quat QuatRot = {};
		QuatRot.FromEuler(VecRot);
		m4y4 Rot = QuatRot.ToMatrix();
		RenderSkybox(GameState->Assets->UnitCube.Children[0]->Model, &State->SkyboxMaterial, &GameState->GameRenderer, &GameState->Assets->UnitCube.VAO, Rot);
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
		io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		io.KeyMap[ImGuiKey_Tab] = VK_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
		io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
		io.KeyMap[ImGuiKey_Home] = VK_HOME;
		io.KeyMap[ImGuiKey_End] = VK_END;
		io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
		io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
		io.KeyMap[ImGuiKey_Space] = VK_SPACE;
		io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;


		// ImGui create font texture
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA

		loaded_image Result = {};
		Result.Width = width;
		Result.Height = height;
		RenderApi.MakeTexture(&Result, (uint32*)pixels, true);
		io.Fonts->TexID = (void *)(intptr_t)Result.GLID;

		ImGui::StyleColorsDark();
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

		if (ImGui::GetIO().WantCaptureMouse) {
			GameInput->MouseScrollDelta = {};
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
		// io.MouseDown[1] = GameInput->MouseRight.IsDown != 0;
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
		// TODO Don't compile on release builds
		editor::Update(GameState, WindowInfo);
	}

	Globals->ShaderLoader.PollReload();
	PollModelReload(Memory, GameState->Assets);

	static bool DemoShowing = true;
	//ImGui::ShowDemoWindow(&DemoShowing);

	// Prevent mouse clicks for game if imgui captures mouse
	if (ImGui::GetIO().WantCaptureMouse) {
		GameInput->MouseLeft.IsDown = false;
		GameInput->MouseLeft.OnDown = false;
		//GameInput->MouseRight.OnDown = false;
		//GameInput->MouseRight.IsDown = false;
	}

	// Camera
	{
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

			real32 Bottom = (-PI / 2.0f) + 0.01f;
			real32 Top = -Bottom;
			State->ActiveCam->EulerRotation.Y = ClampValue(Bottom, Top, State->ActiveCam->EulerRotation.Y);

			editor::SwitchToEditorCam(GameState);
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
			if (GameInput->KeyboardInput[VK_SHIFT].IsDown) {
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

	}



	if (Globals->EditorData.GodMode) {
		SceneUpdate(&State->DebugScene, &GameState->GameRenderer);
	}
	if (GameInput->KeyboardInput['B'].OnDown) {
		Globals->EditorData.GodMode = !Globals->EditorData.GodMode;
	}


	State->Light.Cam.Forward = vector3{0.013f, 0.542f, 0.83f};
	State->Light.Cam.UpdateMatricies();


	if (Globals->UIPanels == GameNull) {
		ConsoleLog("Missing panels!!");
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

	// Scenes
	SceneUpdate(&State->GalaxyScene, &GameState->GameRenderer);

	// Keep at the end, so it reports correctly on the frame
	if (Globals->EditorData.ProfilerWindowOpen) {
		profiler::ImGuiProfiler(GameState, Memory, &GameState->ProfilerData, &GameState->OGLProfilerData);
	}
	profiler::InitProfiler(&GameState->ProfilerData);
	profiler::InitProfiler(&GameState->OGLProfilerData);


	// Render imgui
	{

		ImGuiStyle& style = ImGui::GetStyle();
		style.Alpha = 0.25f;
		if (Globals->EditorData.EditorMode) {
			style.Alpha = 1.0f;
		}


		ImGui::EndFrame();

		if (!BuildVarRelease) {
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

					shader* Shader = assets::GetShader("ImGui");
					if (Shader == GameNull) continue;

					RendCommand.Shader = *Shader;

					layout_data* VertexLayout = RendCommand.GetLayout();
					VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), RendCommand.BufferCapacity, GlobalTransMem);
					layout_data* TextureLayout = RendCommand.GetLayout();
					TextureLayout->Allocate(Shader->GetLayout(render::ShaderTextureCoordsID), RendCommand.BufferCapacity, GlobalTransMem);
					layout_data* ColorLayout = RendCommand.GetLayout();
					ColorLayout->Allocate(Shader->GetLayout(render::ShaderColorID), RendCommand.BufferCapacity, GlobalTransMem);

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