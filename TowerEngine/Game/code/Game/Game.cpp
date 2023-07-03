#pragma once

#ifndef GAME_CPP
#define GAME_CPP

#include "Game.h"

#include <chrono>
#include <ctime>


int64 VersionMajor = 0;
int64 VersionMinor = 1;
int64 VersionBuild = 0;

ImGuiWindowFlags SelectionHoverFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing;

void AddStepper(stepper* Stepper, state* State)
{
	Assert(State->SteppersCount < ArrayCount(State->Steppers));
	State->Steppers[State->SteppersCount++] = Stepper;
}

void RegisterStepper(stepper* Stepper,
                     step_func Method,
                     void* SelfData,
                     state* State)
{
	Stepper->Step = Method;
	Stepper->SelfData = SelfData;
	AddStepper(Stepper, State);
}

void UnregisterStepper(stepper* Stepper, state* State)
{
	for (int i = 0; i  < State->SteppersCount; i++) {
		if (Stepper == State->Steppers[i]) {
			RemoveSlideArray((void*)&State->Steppers[0], State->SteppersCount, sizeof(State->Steppers[0]), i);
			State->SteppersCount--;
			return;
		}
	}
}

void StepUniverse(state* State, real64 TimeMS)
{
	// Check for wakeup stepper
	if (State->SleepingSteppers->LinkCount >= 1) {
		stepper_ptr* pt = (stepper_ptr*)GetLinkData(State->SleepingSteppers, 0);
		if (State->PersistentData.UniverseTimeMS >= pt->Stp->WakeupTime) {
			AddStepper(pt->Stp, State);
			RemoveLink(State->SleepingSteppers, 0);
		}
	}

	// Step everything
	for (int i = 0; i < State->SteppersCount; i++) {
		stepper* Stepper = State->Steppers[i];
		Stepper->Step(Stepper->SelfData, TimeMS, State);
	}
}

string ChronoToString(std::chrono::seconds SecondsTotal)
{
	int64 Hours = std::chrono::duration_cast<std::chrono::hours>(SecondsTotal).count();
	int64 Minutes = (int64)(std::chrono::duration_cast<std::chrono::minutes>(SecondsTotal).count() - (Hours * 60.0f));
	int64 Seconds = (int64)(SecondsTotal.count() - (Minutes * 60.0f) - (Hours * 60.0f * 60.0f));

	return string{Hours} + string{"h "} + string{Minutes} + string{"m "} + string{Seconds} + string{"s "};
}

void SleepStepper(state* State, stepper* Stepper, real64 SleepDurationMS)
{
	stepper_ptr Data = {};
	Data.Stp = Stepper;

	Stepper->WakeupTime = State->PersistentData.UniverseTimeMS + SleepDurationMS;
	UnregisterStepper(Stepper, State);

	// Insert into list sorted
	list_link* CurrentLink = State->SleepingSteppers->TopLink;
	int I = 0;
	while (CurrentLink != GameNull) {
		stepper_ptr* Sleeper = (stepper_ptr*)CurrentLink->Data;
		if (Stepper->WakeupTime < Sleeper->Stp->WakeupTime) {
			InsertLink(State->SleepingSteppers, I, (void*)&Data, GlobalPermMem);
			return;
		}

		I++;
		CurrentLink = CurrentLink->NextLink;
	}

	// Add to end, return out before getting here
	AddLink(State->SleepingSteppers, (void*)&Data, GlobalPermMem);
}

void TimeStep(void* SelfData, real64 Time, state* State)
{
	State->PersistentData.UniverseTimeMS += Time;
}

selectable* RegisterSelectable(selection_type Type, vector2* Center, vector2* Size, void* Data, state* State)
{
	selectable* Sel = &State->Selectables[State->SelectablesCount++];
	Assert(ArrayCount(State->Selectables) > State->SelectablesCount);

	Sel->Type = Type;
	Sel->Center = Center;
	Sel->Size = Size;
	Sel->Data = Data;

	return Sel;
}

bool32 GlobalTriggerSave;
void Save()
{
	GlobalTriggerSave = true;
}

void SaveGame(state* State, save_data::member* Root)
{
	// Time
	using std::chrono::duration_cast;
	using std::chrono::system_clock;
	State->PersistentData.RealTimeSaved = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();;

	// Skill nodess
	{
		int c = 0;
		for (int i = 0; i < State->SkillNodesCount; i++) {
			skill_node* Node = &State->SkillNodes[i];
			if (Node->Unlocked) {
				State->PersistentData.SkillNodesIDUnlocked[c++] = Node->Persist.ID;
				Assert(c < ArrayCount(State->PersistentData.SkillNodesIDUnlocked));
			}
		}
	}

	save_data::Write("SpaceUnknownSave.sus", &save_file_META[0], ArrayCount(save_file_META), (void*)&State->PersistentData, Root);
	ConsoleLog("Game Saved");
}

#include "Persistent.cpp"
#include "Journey.cpp"

#include "WorldObject.cpp"
#include "Asteroid.cpp"
#include "Salvage.cpp"
#include "Item.cpp"
#include "Recipe.cpp"
#include "Station.cpp"
#include "Ship.cpp"
#include "SkillTree.cpp"

#include "Definitions.cpp"

void LoadGame(state* State)
{
	State->PersistentData = {};
	TreeBonusesTotal = &State->PersistentData.TreeBonuses;

	if (!save_data::Read("SpaceUnknownSave.sus", (void*)&State->PersistentData, &save_file_META[0], ArrayCount(save_file_META), GlobalTransMem)) {
		State->LoadedFromFile = false;
		ConsoleLog("No saved data file");

		// If no save file, then create initial setup
		if (!State->LoadedFromFile) { 

			// Station
			station* Station = StationCreate(State);
			Station->Persist->Position.X = 50;
			Station->Persist->Position.Y = 50;

			// Ship
			ship* Ship = ShipCreate(State, ship_id::advent);

			// Ateroids
			AsteroidClusterCreate(vector2{0, 0}, 30.0f, item_id::venigen, State);
		}
	} else {
		State->LoadedFromFile = true;
	}

	// Setup stations
	for (int i = 0; i < State->PersistentData.StationsCount; i++) {
		StationSetup(&State->Stations[i], &State->PersistentData.Stations[i], State);
	}

	// Setup ships
	for (int i = 0; i < State->PersistentData.ShipsCount; i++) {
		ShipSetup(&State->Ships[i], &State->PersistentData.Ships[i], State);
	}

	// Setup asteroid clusters
	for (int i = 0; i < State->PersistentData.AsteroidClustersCount; i++) {
		AsteroidClusterSetup(&State->AsteroidClusters[i], &State->PersistentData.AsteroidClusters[i], State);
	}

	// Skill Nodes
	{
		for (int i = 0; i < ArrayCount(State->PersistentData.SkillNodesIDUnlocked); i++) {
			if (State->PersistentData.SkillNodesIDUnlocked[i] > 0) {
				skill_node* Node = SkillTreeNodeFind(State->PersistentData.SkillNodesIDUnlocked[i], State);
				Node->Unlocked = true;
			}
		}
	}

	if (State->LoadedFromFile) {
		// Simulate forward missing time
		using std::chrono::duration_cast;
		using std::chrono::system_clock;
		int64 CurrentSinceEpoch = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
		int64 FileSinceEpoch = State->PersistentData.RealTimeSaved;
		State->ForwardSimulatingTimeRemaining = (real64)(CurrentSinceEpoch - FileSinceEpoch);

		real64 MissingMSStart = State->ForwardSimulatingTimeRemaining;

		string P = "Simulating " + string{State->ForwardSimulatingTimeRemaining} + " ms of missing time";
		ConsoleLog(P.Array());

		State->ForwardSimulating = true;

		int32 Flicker = 2000;

		float SimFPS = 15.0f;
		float TimeStepMS = (1.0f / SimFPS) * 1000.0f;
		while (State->ForwardSimulatingTimeRemaining > TimeStepMS) {
			StepUniverse(State, TimeStepMS);

			State->ForwardSimulatingTimeRemaining -= TimeStepMS;

			Flicker--;
			if (Flicker <= 0) {
				Flicker = 2000;
				real64 PercDone = 100.0f - ((State->ForwardSimulatingTimeRemaining / MissingMSStart) * 100.0f);
				string PercDisp = string{PercDone};
				ConsoleLog(PercDisp);
			}
		}

		StepUniverse(State, State->ForwardSimulatingTimeRemaining);

		State->ForwardSimulating = false;
	}
	ConsoleLog("Finished");
}

save_data::member* GlobalSaveDataRoot;
b32 Saving = false;

void SaveGameThread(void* Params, int32 ThreadID)
{
	if (!Saving) {
		Saving = true;
		SaveGame((state*)Params, GlobalSaveDataRoot);
		Saving = false;
	}
}

void Start(engine_state* EngineState)
{
	state* State = &EngineState->GameState;

	GlobalSaveDataRoot = (save_data::member*)ArenaAllocate(GlobalPermMem, sizeof(save_data::member));

	// globalid hashmap
	hash::AllocateTable(&State->PersistentPointerSources, 64, sizeof(global_id), GlobalPermMem);

	// Load skill nodes
	{
		path_list NodeFiles = {};
		PlatformApi.GetPathsForFileType(".skill_node", EngineState->RootAssetPath.Array(), GlobalTransMem, &NodeFiles);

		path_list* P = &NodeFiles;
		while (StringLength(P->Path) > 0) {
			SkillTreeNodeLoad(P->Path.Array(), State);

			P = P->Next;
		}

		// Update Children after all the node have been loaded
		for (int i = 0; i < State->SkillNodesCount; i++) {
			skill_node* Node = &State->SkillNodes[i];
			for (int c = 0; c < ArrayCount(Node->Persist.ChildrenIDs); c++) {
				if (Node->Persist.ChildrenIDs[c] > 0) {
					Node->AddChild(
					    SkillTreeNodeFind(Node->Persist.ChildrenIDs[c], State)
					);
				}
			}
		}
	}

	State->SleepingSteppers = CreateListFixed(GlobalPermMem, sizeof(stepper_ptr), 100);

	CreateDefinitions();

	RegisterStepper(&State->UniverseTimeStepper, &TimeStep, GameNull, State);

	// Load asteroid images
	Globals->AssetsList.AsteroidImages[0] = assets::GetImage("Asteroid1");
	Globals->AssetsList.AsteroidImages[1] = assets::GetImage("Asteroid2");
	Globals->AssetsList.AsteroidImages[2] = assets::GetImage("Asteroid3");

	// Load salvage images
	Globals->AssetsList.SalvageImages[0] = assets::GetImage("Salvage1");
	Globals->AssetsList.SalvageImages[1] = assets::GetImage("Salvage2");

	//AsteroidCreateCluster(vector2{ -200, 20}, 20.0f, item_id::pyrexium, State);

	// Salvage
	SalvageCreate(State, vector2{ -30, -30});

	// Create links into persistent data 
	{
		// Stations
		{
			for (int i = 0; i < State->PersistentData.StationsCount; i++) {
				State->Stations[i].Persist = &State->PersistentData.Stations[i];
				State->Stations[i].Hold.Setup(1000, &State->Stations[i].Persist->ItemHold);

				ItemHoldUpdateMass(&State->Stations[i].Hold);
			}
		}
	}

	LoadGame(State);
}

const real32 ZoomMin = 0.0f;
const real32 ZoomMax = 1.0f;
const real32 ZoomRealMin = 0.1f;
const real32 ZoomRealMax = 20.0f;

const real32 RenderLayerShip = -2;
const real32 RenderLayerPlanet = -3;
const real32 KeyboardPanSpeed = 0.75f;
const real32 MousePanSpeed = 0.03f;

const real32 MouseZoomSpeed = 0.03f;
const real32 MouseZoomInvert = -1;

void Loop(engine_state* EngineState, window_info* Window, game_input* Input)
{
	state* State = &EngineState->GameState;
	editor_state* EditorState = &EngineState->EditorState;

	State->Zoom = (real32)Lerp(State->Zoom, State->ZoomTarget, 0.5f);
	float Curve = 3.5f;
	EngineState->GameCamera.OrthoZoom = (real32)LerpCurve(ZoomRealMin, ZoomRealMax, Curve, State->Zoom);
	real64 ZoomSpeedAdj = LerpCurve(4.0f, 200.0f, Curve, State->Zoom);

	// save timer
	{
		static real64 SaveTimer = 0;
		SaveTimer += EngineState->DeltaTimeMS;
		if (MillisecondsToSeconds(SaveTimer) > 5.0f || GlobalTriggerSave) {
			GlobalTriggerSave = false;
			SaveTimer = 0.0f;

			PlatformApi.ThreadAddWork(&SaveGameThread, (void*)State);
			//SaveGame(State, GlobalSaveDataRoot);
		}
	}

	// Editor
	{
		if (Input->FunctionKeys[1].OnDown) {
			EditorState->EditorMode = !EditorState->EditorMode;
		}
	}

	// Main windows
	{
		static bool Open = true;
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(Window->Width * 0.15f, -1));
		ImGui::Begin("Main", &Open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

		if (EditorState->EditorMode) {
			ImGui::Text("EDITOR MODE");

			// Mouse world position
			{
				vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
				vector2 MouseWorldFlat = vector2{MouseWorld.X, MouseWorld.Y};
				ImGui::Text(string{MouseWorldFlat.X} .Array());
				ImGui::SameLine();
				ImGui::Text(string{MouseWorldFlat.Y} .Array());
				RenderCircle(MouseWorldFlat, vector2{1, 1},
				             COLOR_RED, -1, Globals->GameRenderer);
			}

			ImGui::Text("Time Boost");
			{
				ImGui::Checkbox("Pause", &EditorState->Paused);

				real64 SimFPS = 60.0f;
				real64 FrameLengthMS = (1.0f / SimFPS) * 1000.0f;
				if (ImGui::Button("1 minute")) {
					for (int i = 0; i < SimFPS * 60.0f; i++) { StepUniverse(State, FrameLengthMS); }
				}
				ImGui::SameLine();
				if (ImGui::Button("1 hour")) {
					for (int i = 0; i < SimFPS * 60.0f * 60.0f; i++) { StepUniverse(State, FrameLengthMS); }
				}
			}

			ImGui::Separator();

			if (ImGui::Button("Item Window")) { EditorState->ItemWindowOpen = !EditorState->ItemWindowOpen; }
			ImGui::SameLine();
			if (ImGui::Button("Skill Node Window")) { EditorState->SkillNodeWindowOpen = !EditorState->SkillNodeWindowOpen; }

			if (EditorState->SkillNodeWindowOpen) {
				ImGui::Begin("Skill Node Window");

				static skill_node* NodeMoving = {};
				static bool32 AddingChild = false;

				// Node selecting
				if (Input->MouseLeft.OnUp && !Input->MouseMoved() && State->NodeHovering != GameNull) {
					if (!AddingChild) {
						EditorState->NodeSelected = State->NodeHovering;
					} else {
						AddingChild = false;
						EditorState->NodeSelected->AddChild(State->NodeHovering);
					}
				}

				// Node dragging
				if (Input->MouseRight.OnUp) {
					NodeMoving = {};
				}
				if (Input->MouseRight.OnDown && State->NodeHovering != GameNull) {
					NodeMoving = State->NodeHovering;
				}
				if (Input->MouseRight.IsDown && Input->MouseMoved() && NodeMoving != GameNull) {
					vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
					NodeMoving->Persist.Position = vector2{MouseWorld.X, MouseWorld.Y};
				}

				if (EditorState->NodeSelected != GameNull) {
					ImGui::Text("%i", EditorState->NodeSelected->Persist.ID);

					{
						int Num = (int)EditorState->NodeSelected->Persist.KnowledgeCost;
						ImGui::DragInt("Knowledge Cost", &Num, 1, 0, 1000000, " % i");
						EditorState->NodeSelected->Persist.KnowledgeCost = (int64)Num;
					}

					ImGui::Separator();

					for (int i = 0; i < EditorState->NodeSelected->ChildrenCount; i++) {
						ImGui::Text("%i", EditorState->NodeSelected->Children[i]->Persist.ID);
						ImGui::SameLine();

						ImGui::PushID(i);
						if (ImGui::Button(" - ")) {
							RemoveSlideArray((void*)&EditorState->NodeSelected->Children[0], EditorState->NodeSelected->ChildrenCount, sizeof(EditorState->NodeSelected->Children[0]), i);
							EditorState->NodeSelected->ChildrenCount--;
						}
						ImGui::PopID();
					}

					if (!AddingChild) {
						if (ImGui::Button(" + Add Child + ")) {
							AddingChild = true;
						}
					} else {
						ImGui::Text("!! CLICK CHILD !!");
					}

					ImGui::Separator();

					ImGui::Text("Bonuses");

					ImGui::DragFloat("FuelForceAddition", &EditorState->NodeSelected->Persist.BonusAdditions.FuelForceAddition, 0.001f);
				}

				ImGui::Dummy(ImVec2(0, 30));

				if (ImGui::Button("Knowledge + 5 ", ImVec2(-1, 0))) {
					State->PersistentData.Knowledge += 5;
				}

				if (ImGui::Button("++ New ++ ", ImVec2(-1, 0))) {
					EditorState->NodeSelected = SkillTreeNodeCreate(State);
				}
				if (ImGui::Button("Save All", ImVec2(-1, 0))) {
					SkillTreeSaveAll(State);
				}
				if (ImGui::Button("Reset All")) {
					for (int i = 0; i < State->SkillNodesCount; i++) {
						State->SkillNodes[i].Unlocked = false;
					}
					SkillTreeSaveAll(State);
				}

				ImGui::End();
			}
			if (EditorState->ItemWindowOpen) {

				ImGui::Begin("Debug item give");

				EditorState->DebugHold.Persist = &EditorState->DebugHoldPersist;

				for (int i = 0; i < (int)item_id::count; i++) {
					item_definition* Def = &Globals->AssetsList.ItemDefinitions[i];

					{

						ImGui::Image(
						    (ImTextureID)((int64)Def->Icon->GLID),
						    ImGuiImageSize,
						    ImVec2(0, 0),
						    ImVec2(1, -1),
						    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
						    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
						);

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
							EditorState->DebugItemDragging.ID = Def->ID;
							EditorState->DebugItemDragging.Count = 1;

							State->ItemDragging = &EditorState->DebugItemDragging;
							State->HoldItemDraggingFrom = &EditorState->DebugHold;

							int D = 0;
							ImGui::SetDragDropPayload(ImguiItemDraggingID, &D, sizeof(D));

							ImGui::Image(
							    (ImTextureID)((int64)Def->Icon->GLID),
							    ImGuiImageSize,
							    ImVec2(0, 0),
							    ImVec2(1, -1),
							    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
							    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
							);

							ImGui::EndDragDropSource();
						}

						ImGui::SameLine();

						ImGui::BeginGroup();
						ImGui::Text(Def->DisplayName.Array());
						ImGui::Text("x1");
						ImGui::EndGroup();
					}


					{
						ImGui::Image(
						    (ImTextureID)((int64)Def->Icon->GLID),
						    ImGuiImageSize,
						    ImVec2(0, 0),
						    ImVec2(1, -1),
						    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
						    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
						);

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
							EditorState->DebugItemDragging.ID = Def->ID;
							EditorState->DebugItemDragging.Count = 10;

							State->ItemDragging = &EditorState->DebugItemDragging;
							State->HoldItemDraggingFrom = &EditorState->DebugHold;

							int D = 0;
							ImGui::SetDragDropPayload(ImguiItemDraggingID, &D, sizeof(D));

							ImGui::Image(
							    (ImTextureID)((int64)Def->Icon->GLID),
							    ImGuiImageSize,
							    ImVec2(0, 0),
							    ImVec2(1, -1),
							    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
							    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
							);

							ImGui::EndDragDropSource();
						}

						ImGui::SameLine();

						ImGui::BeginGroup();
						ImGui::Text(Def->DisplayName.Array());
						ImGui::Text("x10");
						ImGui::EndGroup();
					}
				}

				ImGui::End();
			}

			// Ship simulate performance testing
			if (State->Selections[0].IsShip()) {
				if (ImGui::Button("Test Ship Simulation")) {
					ConsoleLog("Starting Test");
					State->ForwardSimulating = true;

					ship* CurrentShip = State->Selections[0].GetShip();

					uint64 Accum = 0;
					int32 Runs = 10;

					CreateMovementStep(CurrentShip, vector2{50, 50});

					real64 TotalSimTime = 0;

					for (int i = 0; i < Runs; i++ ) {
						uint64 Start = PlatformApi.QueryPerformanceCounter();

						float SimFPS = 30.0f;
						float TimeStepMS = 1.0f / SimFPS;

						vector2 PosOrig = CurrentShip->Persist->Position;
						ItemGive(&CurrentShip->FuelTank, item_id::stl, 1000);
						CurrentShip->Persist->Velocity = {};

						journey::Execute(&CurrentShip->Persist->CurrentJourney);
						journey_methods::Start(CurrentShip->Persist->CurrentJourney.Steps[0].Type)(CurrentShip, &CurrentShip->Persist->CurrentJourney.Steps[0], State);

						while (
							!(journey_methods::Step(CurrentShip->Persist->CurrentJourney.Steps[0].Type)(CurrentShip, &CurrentShip->Persist->CurrentJourney.Steps[0], TimeStepMS, State))
						) {
							TotalSimTime += TimeStepMS;
						}

						CurrentShip->Persist->Position = PosOrig;
						CurrentShip->Persist->Velocity = {};
						CurrentShip->Persist->Status = ship_status::idle;

						uint64 End = PlatformApi.QueryPerformanceCounter();
						uint64 Count = End - Start;
						Accum += Count;

						real64 SimMinutes = MillisecondsToSeconds(TotalSimTime) / 60.0f;
						string Report = "Finished " + string{i + 1} + " / " + string{Runs} + " ->" + string{Count} + " SimMinutes->" + string{SimMinutes};
						ConsoleLog(Report.Array());
					}

					State->ForwardSimulating = false;

					real64 SimMinutes = MillisecondsToSeconds(TotalSimTime) / 60.0f;
					real64 Avg = (real64)Accum / (real64)Runs;
					real64 CyclePerMin = Avg / SimMinutes;
					string Report = "AVG " + string{Avg} + " ||  Total Real Time (m)" + string{SimMinutes} + " || Cycles per SimMin " + string{CyclePerMin};
					ConsoleLog(Report.Array());
				}
			}
		}

		ImGui::Text("Time");
		ImGui::SameLine();

		std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> Time = {};
		Time += std::chrono::milliseconds((int)State->PersistentData.UniverseTimeMS);
		std::chrono::seconds Diff = std::chrono::duration_cast<std::chrono::seconds>(Time.time_since_epoch());
		string Disp = ChronoToString(Diff);
		ImGui::Text(Disp.Array());

		ImGui::Separator();

		switch (State->Scene) {

			case game_scene::universe: {
				ImGui::SliderFloat("Zoom", &State->ZoomTarget, ZoomMin, ZoomMax, " % .3f");

				if (ImGui::Button("Skill Tree", ImVec2(-1, 0))) {
					State->Scene = game_scene::skill_tree;

					State->UniverseOrthoZoom = State->Zoom;
					State->UniverseCamPos = EngineState->GameCamera.Center;
					EngineState->GameCamera.Center = State->SkillTreeCamPos;
					State->Zoom = State->SkillTreeOrthoZoom;
					State->ZoomTarget = State->Zoom;
				}
			}
			break;

			case game_scene::skill_tree: {
				if (ImGui::Button("Universe", ImVec2(-1, 0))) {
					State->Scene = game_scene::universe;

					State->SkillTreeCamPos = EngineState->GameCamera.Center;
					State->SkillTreeOrthoZoom = State->Zoom;
					EngineState->GameCamera.Center = State->UniverseCamPos;
					State->Zoom = State->UniverseOrthoZoom;
					State->ZoomTarget = State->Zoom;
				}
			}
			break;

			INVALID_DEFAULT;
		}

		ImGui::Separator();
		ImGui::Text("Resources");
		ImGui::Text("Knowledge - % i", State->PersistentData.Knowledge);
		ImGui::SameLine();

		ImGui::End();
	}


	if (!EditorState->Paused) {
		StepUniverse(State, EngineState->DeltaTimeMS);
	}

	// camera controls
	{
		// Keyboard
		vector2 CamMoveDir = vector2{0, 0};
		if (Input->KeyboardInput['A'].IsDown || Input->KeyboardInput['a'].IsDown) { CamMoveDir.X = -1; }
		if (Input->KeyboardInput['D'].IsDown || Input->KeyboardInput['d'].IsDown) { CamMoveDir.X = 1; }
		if (Input->KeyboardInput['W'].IsDown || Input->KeyboardInput['w'].IsDown) { CamMoveDir.Y = -1; }
		if (Input->KeyboardInput['S'].IsDown || Input->KeyboardInput['s'].IsDown) { CamMoveDir.Y = 1; }
		CamMoveDir = Vector2Normalize(CamMoveDir);
		EngineState->GameCamera.Center.X += CamMoveDir.X * KeyboardPanSpeed * (ZoomSpeedAdj);
		EngineState->GameCamera.Center.Y += CamMoveDir.Y * KeyboardPanSpeed * (ZoomSpeedAdj);

		// Mouse
		static vector2 MouseStart;
		static vector2 CamStart;
		if (Input->MouseLeft.OnDown) {
			MouseStart = Input->MousePos;

			CamStart.X = EngineState->GameCamera.Center.X;
			CamStart.Y = EngineState->GameCamera.Center.Y;
		}
		if (Input->MouseLeft.IsDown) {
			vector2 Offset = MouseStart - Input->MousePos;
			vector2 P = CamStart + (Offset * MousePanSpeed * ZoomSpeedAdj);
			EngineState->GameCamera.Center.X = P.X;
			EngineState->GameCamera.Center.Y = P.Y;
		}

		State->ZoomTarget = (real32)ClampValue(ZoomMin, ZoomMax, State->ZoomTarget + (Input->MouseScrollDelta * MouseZoomSpeed * MouseZoomInvert));
	}

	// Scene Rendering
	switch (State->Scene) {

		case game_scene::universe: {


			// Selection
			{
				// Find what is hovered
				State->Hovering = {};
				for (int i = 0; i < State->SelectablesCount; i++) {
					selectable* Sel = &State->Selectables[i];
					vector2 TopLeftWorld = *Sel->Center - (*Sel->Size * 0.5f);
					vector2 BottomRightWorld = *Sel->Center + (*Sel->Size * 0.5f);

					rect Bounds = {};
					Bounds.TopLeft = WorldToScreen(vector3{TopLeftWorld.X, TopLeftWorld.Y, 0}, &EngineState->GameCamera);
					Bounds.BottomRight = WorldToScreen(vector3{BottomRightWorld.X, BottomRightWorld.Y, 0}, &EngineState->GameCamera);

					if (RectContains(Bounds, Input->MousePos)) {
						if (State->Hovering == GameNull) {
							State->Hovering = Sel;
						} else if ((int)Sel->Type < (int)State->Hovering->Type) {
							State->Hovering = Sel;
						}
					}
				}

				// Hovering
				if (State->Hovering != GameNull) {

					if (State->Hovering->OnHover != GameNull) {
						State->Hovering->OnHover(State->Hovering, EngineState, Input);
					}

					// can select display
					if (State->Hovering->SelectionUpdate != GameNull) {

						vector2 WorldCenter = *State->Hovering->Center;
						vector2 WorldSize = *State->Hovering->Size;

						vector2 WorldTopLeft = WorldCenter + (WorldSize * 0.5f);
						vector2 WorldBottomRight = WorldCenter - (WorldSize * 0.5f);

						rect R = {};
						R.TopLeft = WorldToScreen(vector3{WorldTopLeft.X, WorldTopLeft.Y, 0}, &EngineState->GameCamera);
						R.BottomRight = WorldToScreen(vector3{WorldBottomRight.X, WorldBottomRight.Y, 0}, &EngineState->GameCamera);

						RenderRectangleOutline(R, 1, COLOR_RED, -1, &EngineState->UIRenderer);
					}
				}

				// Selecting
				if (State->Hovering != GameNull && !State->Hovering->Selected && State->Hovering->SelectionUpdate != GameNull &&
				        Input->MouseLeft.OnUp && !Input->MouseMoved()
				   ) {
					for (int i = 0; i < ArrayCount(State->Selections); i++) {
						selection* Sel = &State->Selections[i];
						if (Sel->None()) {
							Sel->Current = State->Hovering;
							Sel->Current->Selected = true;

							if (Sel->Current->OnSelection != GameNull) {
								Sel->Current->OnSelection(Sel, EngineState, Input);
							}
							break;
						}
					}
				}
			}


			// Call selection update func
			for (int i = 0; i < ArrayCount(State->Selections); i++) {
				selection* Sel = &State->Selections[i];
				if (!Sel->None()) {

					// Render line to selection
					{
						vector2 ObjPos = *Sel->Current->Center;

						vector2 Points[2] = {};
						Points[0] = Sel->Current->InfoWindowPos;
						Points[1] = WorldToScreen(vector3{ObjPos.X, ObjPos.Y, 0}, &EngineState->GameCamera);
						render_line Line = {};
						Line.Points = Points;
						Line.PointsCount = ArrayCount(Points);
						RenderLine(Line, 1.5f, color{1, 1, 1, 0.2f}, &EngineState->UIRenderer, false);
					}

					Sel->Current->SelectionUpdate(Sel, EngineState, Input);
				}
			}


			// Update displays
			for (int i = 0; i < State->PersistentData.ShipsCount; i++) {
				ship* Ship = &State->Ships[i];

				// Janky but whatever. Would need to use the transform scene hierarchy to improve
				if (Ship->Persist->Status == ship_status::docked) {
					station* StationDocked = per::GetStation(&Ship->Persist->StationDocked, State);
					Ship->Persist->Rotation = StationDocked->Rotation;

					int DocksCount = 10;
					real64 DockRel = (real64)StationDocked->Persist->DockedCount / (real64)DocksCount;
					real64 DockRadians = DockRel * (2 * PI);

					real64 DockRadius = StationDocked->Size.X * 0.5f * 0.9f;
					vector2 StationOffset = StationDocked->Persist->Position + vector2 {
						DockRadius * sin(DockRadians),
						DockRadius * cos(DockRadians)
					};

					vector2 NewPos = Vector2RotatePoint(StationOffset, StationDocked->Persist->Position, DockRadians + -StationDocked->Rotation);
					Ship->Persist->Position.X = NewPos.X;
					Ship->Persist->Position.Y = NewPos.Y;
				}
			}

			// Render planets
			RenderCircle(vector2{1200, 200}, vector2{2000, 2000},
			             COLOR_RED, RenderLayerPlanet, Globals->GameRenderer);

			// Render world objects
			for (int i = 0; i < State->WorldObjectsCount; i++)  {
				world_object* Obj = State->WorldObjects[i];

				Obj->Rotation += Obj->RotationRate * EngineState->DeltaTimeMS * 0.0002f;

				m4y4 Model = m4y4Identity();
				Model = Rotate(Model, vector3{0, 0, Obj->Rotation});

				RenderTextureAll( Obj->Position, Obj->Size, ColorPersistToColor(Obj->Color), Obj->Image->GLID,
				                  RenderLayerPlanet, Model, Globals->GameRenderer);
			}

			// Render stations
			for (int i = 0; i < State->PersistentData.StationsCount; i++) {
				station* Station = &State->Stations[i];

				Station->Rotation += (PI / 10.0f) * EngineState->DeltaTimeMS * 0.0002f;

				m4y4 Model = m4y4Identity();
				Model = Rotate(Model, vector3{0, 0, Station->Rotation});

				static loaded_image* Sprite = assets::GetImage("Station");

				RenderTextureAll(
				    Station->Persist->Position,
				    vector2{18.0f, 18.0f},
				    Color255(90.0f, 99.0f, 97.0f, 255.0f),
				    Sprite->GLID,
				    RenderLayerPlanet, Model, Globals->GameRenderer);
			}

			// Render ships
			for (int i = 0; i < State->PersistentData.ShipsCount; i++) {
				ship* Ship = &State->Ships[i];

				{
					vector2 Pos = Ship->Persist->Position;

					m4y4 Model = m4y4Identity();
					Model = Rotate(Model, vector3{0, 0, Ship->Persist->Rotation});

					RenderTextureAll(
					    Pos,
					    Ship->Size,
					    COLOR_WHITE,
					    Ship->Definition.Icon->GLID, RenderLayerShip, Model, Globals->GameRenderer);
				}

				// Render ship module effects
				for (int m = 0; m < ArrayCount(Ship->EquippedModules); m++) {
					ship_module* Module = &Ship->EquippedModules[m];
					if (Module->Persist->Filled && WorldTargetHasTarget(&Module->Persist->Target)) {

						vector2 WorldTargetPos = WorldTargetGetPosition(&Module->Persist->Target, State);

						vector2 Points[2] = {};
						Points[0] = WorldToScreen(vector3{Ship->Persist->Position.X, Ship->Persist->Position.Y, 0}, &EngineState->GameCamera);
						Points[1] = WorldToScreen(vector3{WorldTargetPos.X, WorldTargetPos.Y, 0}, &EngineState->GameCamera);
						render_line Line = {};
						Line.Points = Points;
						Line.PointsCount = ArrayCount(Points);
						RenderLine(Line, 2.0f, color{1, 0, 0, 0.5f}, &EngineState->UIRenderer, false);
					}
				}
			}

		}
		break;

		case game_scene::skill_tree: {

			vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
			vector2 MouseWorldFlat = vector2{MouseWorld.X, MouseWorld.Y};

			EngineState->GameState.NodeHovering = {};

			for (int i = 0; i < State->SkillNodesCount; i++) {
				skill_node* Node = &State->SkillNodes[i];

				// Render skill tree nodes
				for (int c = 0; c < Node->ChildrenCount; c++) {

					real32 Width = 1;
					if (Node->Unlocked) {
						Width = 4;
					}

					vector2 Points[2] = {};
					Points[0] = WorldToScreen(vector3{Node->Persist.Position.X, Node->Persist.Position.Y, 0}, &EngineState->GameCamera);
					Points[1] = WorldToScreen(vector3{Node->Children[c]->Persist.Position.X, Node->Children[c]->Persist.Position.Y, 0}, &EngineState->GameCamera);
					render_line Line = {};
					Line.Points = Points;
					Line.PointsCount = ArrayCount(Points);
					RenderLine(Line, Width, color{1, 1, 1, 0.2f}, &EngineState->UIRenderer, false);
				}

				color Color = {};
				if (!Node->Unlocked) {
					Color = COLOR_WHITE;
					Color.A = 0.1f;
				}
				if (Node->Parent == GameNull || Node->Parent->Unlocked) {
					Color = Color255(15, 87, 34, 1);
				}
				if (Node->Unlocked) {
					Color = Color255(42, 240, 96, 1);
				}

				real32 RadiusMax = 2;
				if (Node->Unlocked) {
					RadiusMax = 3;
				}
				Node->CircleRadius = Lerp(Node->CircleRadius, RadiusMax, 0.25f);
				RenderCircle(Node->Persist.Position, vector2{Node->CircleRadius, Node->CircleRadius}, Color, 2, Globals->GameRenderer);
				if (Vector2Distance(Node->Persist.Position, MouseWorldFlat) < Node->CircleRadius) {
					EngineState->GameState.NodeHovering = Node;
				}
			}

			static skill_node* NodeSelected = {};

			if (NodeSelected == GameNull && State->NodeHovering != GameNull) {
				State->NodeHovering->CircleRadius = 4;

				ImGui::SetNextWindowPos(ImVec2((float)Input->MousePos.X + 20, (float)Input->MousePos.Y));
				bool Open = true;
				ImGui::Begin("Info", &Open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::Text("Knowledge Cost - % i", State->NodeHovering->Persist.KnowledgeCost);
				SkillTreeImguiDisplayBonuses(State->NodeHovering->Persist.BonusAdditions);
				ImGui::End();

				if (!State->NodeHovering->Unlocked && Input->MouseLeft.OnDown && !EditorState->EditorMode) {
					ImGui::OpenPopup("Unlock");
					NodeSelected = State->NodeHovering;
				}
			}

			if (Input->MouseLeft.OnUp) {
				NodeSelected = {};
			}

			if (NodeSelected != GameNull) {
				if (ImGui::BeginPopupModal("Unlock")) {

					ImGui::Text("Spend % i Knowledge to unlock this bonus ? ", NodeSelected->Persist.KnowledgeCost);
					SkillTreeImguiDisplayBonuses(NodeSelected->Persist.BonusAdditions);

					if (NodeSelected->Unlocked) {
						ImGui::Text("UNLOCKED");
					} else {
						ImGui::Separator();
						real32 HW = ImGui::GetWindowWidth() * 0.47f;
						if (State->PersistentData.Knowledge >= NodeSelected->Persist.KnowledgeCost) {
							if (ImGui::Button("Yes", ImVec2(HW, 0))) {
								ImGui::CloseCurrentPopup();
								SkillTreeUnlock(NodeSelected, State);
							}
						} else {
							ImGui::Text("Not enough Knowledge");
						}

						ImGui::SameLine();
						if (ImGui::Button("No", ImVec2(HW, 0))) {
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
				}
			}
		}
		break;

		INVALID_DEFAULT;
	}
}

#endif
