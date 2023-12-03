#pragma once

#ifndef GAME_CPP
#define GAME_CPP

#include "Game.h"

#include <chrono>
#include <ctime>

int64 VersionMajor = 0;
int64 VersionMinor = 13;

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

ships_list GetShipsWithinRadius(vector2 Position, r64 Radius, state* State) {
	ships_list Ret = {};

	for (int i = 0; i < State->PersistentData.ShipsCount; i++) {
		Assert(i < ArrayCount(State->Ships));
		if (Vector2Distance(Position, State->Ships[i].Persist->Position) < Radius) {
			Ret.List[Ret.ListCount++] = &State->Ships[i];
			Assert(Ret.ListCount < ArrayCount(Ret.List));
		}
	}		

	return Ret;
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

	// Update ship bonus state
	{
		// Clear old state
		for (int i = 0; i < State->PersistentData.ShipsCount; i++) {
			ship* Ship = &State->Ships[i];
			Ship->IndustrialActivationReductionMinutes = 0;
		}

		// Update state
		for (int i = 0; i < State->PersistentData.ShipsCount; i++) {
			ship* Ship = &State->Ships[i];
			for (int m = 0; m < ArrayCount(Ship->EquippedModules); m++) {
				ship_module* Module = &Ship->EquippedModules[m];

				// forman bonuses
				if (Module->Persist->Filled && Module->Definition.ID == ship_module_id::foreman_i) {
					ships_list ShipsWithin = GetShipsWithinRadius(Ship->Persist->Position, Module->Definition.ActivationRange, State);
					for (int n = 0; n < ShipsWithin.ListCount; n++) {
						ship* SH = ShipsWithin.List[n];
						SH->IndustrialActivationReductionMinutes += Module->Definition.Foreman.ReductionMinutes;
					}
				}
			}
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

	int64 Days = (int64)((r64)Hours / 24.0f);
	Hours -= Days * 24;

	int64 Minutes = (int64)(std::chrono::duration_cast<std::chrono::minutes>(SecondsTotal).count() - (Hours * 60.0f) - (Days * 24.0f * 60.0f));
	int64 Seconds = (int64)(SecondsTotal.count() - (Minutes * 60.0f) - (Hours * 60.0f * 60.0f) - (Days * 24.0f * 60.0f * 60.0f));

	return 
		string{Days} + string{"d "} + 
		string{Hours} + string{"h "} + 
		string{Minutes} + string{"m "} + 
		string{Seconds} + string{"s "};
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
		// Clear any old data
		for (int i = 0; i < ArrayCount(State->PersistentData.SkillNodesIDUnlocked); i++) {
			State->PersistentData.SkillNodesIDUnlocked[i] = {};
		}

		int c = 0;
		for (int i = 0; i < State->SkillNodesCount; i++) {
			skill_node* Node = &State->SkillNodes[i];
			if (Node->Unlocked) {
				State->PersistentData.SkillNodesIDUnlocked[c++] = Node->Persist.ID;
				Assert(c < ArrayCount(State->PersistentData.SkillNodesIDUnlocked));
			}
		}
	}

	save_data::Write((string{SaveDataFolder} + "/SpaceUnknownSave.sus").Array(), &save_file_META[0], ArrayCount(save_file_META), (void*)&State->PersistentData, Root);
	ConsoleLog("Game Saved");
}

item_definition* GetItemDefinition(item_id ItemID)
{
	Assert((int)ItemID < ArrayCount(Globals->AssetsList.ItemDefinitions));
	return &Globals->AssetsList.ItemDefinitions[(int)ItemID];
}

ship_module_slot_definition* GetShipModuleSlotDefinition(ship_module_slot_type Type)
{
	Assert((int)Type < ArrayCount(Globals->AssetsList.ShipModuleSlotDefinitions));
	return &Globals->AssetsList.ShipModuleSlotDefinitions[(int)Type];
}

ship_definition* GetShipDefinition(ship_id ID)
{
	Assert((int)ID < ArrayCount(Globals->AssetsList.ShipDefinitions));
	return &Globals->AssetsList.ShipDefinitions[(int)ID];
}

#include "InfoWindow.cpp"

bool ImGuiItemIcon(item_id ItemID, bool CanDelete) {

	b32 Ret = false;
	ImGui::PushID(item_id_NAME[(int)ItemID].Array());

	loaded_image* Icon = Globals->AssetsList.ItemDefinitions[(int)ItemID].Icon;
	ImGui::Image(
		(ImTextureID)((int64)Icon->GLID),
		ImGuiImageSize,
		ImVec2(0, 0),
		ImVec2(1, -1),
		ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
		ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
	);

	if (ImGui::BeginPopupContextItem("item context menu")) {
		if (ImGui::MenuItem("Info")) { 
			InfoWindow::Show(ItemID);
			ImGui::CloseCurrentPopup();
		}
		if (CanDelete) { 
			if (ImGui::MenuItem("Destroy Item")) { 
				ImGui::CloseCurrentPopup();
				Ret = true;
			}
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();

	return Ret;
}

#include "ChangeLog.cpp"

#include "Persistent.cpp"
#include "Journey.cpp"

void ImGuiSelectionWindowSetup(i32 SelectionIndex) { 
	ImGui::SetNextWindowPos(
			ImVec2(
				(30.0f * (SelectionIndex + 1)) + 50.0f, 
				(30.0f * (SelectionIndex + 1)) + 50.0f
				),
			ImGuiCond_FirstUseEver
		);
	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
}

#include "WorldObject.cpp"
#include "Asteroid.cpp"
#include "Salvage.cpp"
#include "Item.cpp"
#include "Recipe.cpp"
#include "Station.cpp"
#include "Ship.cpp"
#include "SkillTree.cpp"

#include "ModuleAsteroidMiner.cpp"
#include "ModuleSalvager.cpp"

#include "Definitions.cpp"

void LoadGame(state* State)
{
	b32 LoadedFromFile = false;

	State->PersistentData = {};
	TreeBonusesTotal = &State->PersistentData.TreeBonuses;

	if (!save_data::Read((string{SaveDataFolder} + "/SpaceUnknownSave.sus").Array(), (void*)&State->PersistentData, &save_file_META[0], ArrayCount(save_file_META), GlobalTransMem)
			|| !State->PersistentData.Valid
		) {

		ConsoleLog("No saved data file");
		State->PersistentData = {};
		State->PersistentData.Valid = true;
		State->TutorialWindow = true;

		LoadedFromFile = false;

		// Initial state setup
		State->PersistentData.TreeBonuses.ShipLimit = 1;
		State->PersistentData.Knowledge = 20;

		// Station
		station* Station = StationCreate(State);
		Station->Persist->Position.X = 50;
		Station->Persist->Position.Y = 50;

		// Ship
		ship* Ship = ShipCreate(State, ship_id::advent);
		ShipAddModule(&Ship->EquippedModules[0], ship_module_id::asteroid_miner, Ship, State);

		// Ateroids
		AsteroidClusterCreate(vector2{-50, -50}, 15.0f, item_id::venigen, State);
		AsteroidClusterCreate(vector2{-20, 130}, 10.0f, item_id::pyrexium, State);

		// Salvages
		SalvageCreate(vector2{ -110, 40}, State);

	} else {
		LoadedFromFile = true;
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

	// Setup salvages
	for (int i = 0; i < State->PersistentData.SalvagesCount; i++) {
		SalvageSetup(&State->Salvages[i], &State->PersistentData.Salvages[i], State);
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

	if (LoadedFromFile) {
		// Simulate forward missing time
		using std::chrono::duration_cast;
		using std::chrono::system_clock;
		int64 CurrentSinceEpoch = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
		int64 FileSinceEpoch = State->PersistentData.RealTimeSaved;

		i64 HoursLimit = 24;
		r64 TotalMissing = (real64)(CurrentSinceEpoch - FileSinceEpoch);
		State->ForwardSimulatingTimeRemaining = ClampValue(0.0f, HoursToMilliseconds(HoursLimit), TotalMissing);

		State->ForwardSimulatingTimeRemaining = HoursToMilliseconds(5.1234f);

		real64 MissingMSStart = State->ForwardSimulatingTimeRemaining;

		string P = "Simulating " + string{MillisecondsToHours(State->ForwardSimulatingTimeRemaining)} + " h of missing time of total " + string{MillisecondsToHours(TotalMissing)};
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

		//StepUniverse(State, State->ForwardSimulatingTimeRemaining);

		State->ForwardSimulating = false;
	} else {

		// Once evrythig is setup. Initialize more only for first setup.
		ItemGive(&State->Ships[0].FuelTank, item_id::stl, 200);
		ItemGive(&State->Stations[0].Hold, item_id::pyrexium, 10);
		ItemGive(&State->Stations[0].Hold, item_id::sm_salvager_i, 1);
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

	SaveDataFolder = EngineState->SaveDataFolder;
	GlobalSaveDataRoot = save_data::AllocateMember(MEMBER_PAIRS_MAX, GlobalPermMem, false);

	// globalid hashmap
	hash::AllocateTable(&State->PersistentPointerSources, 64, sizeof(persistent_pointer), GlobalPermMem);

	// Load skill nodes
	{
		path_list NodeFiles = {};
		PlatformApi.GetPathsForFileType(".skill_node", EngineState->RootAssetPath.Array(), GlobalTransMem, &NodeFiles);

		path_list* P = &NodeFiles;
		while (StringLength(P->Path) > 0) {
	
			// Manually clear the transient allocations from this
			uint8* MemHead = GlobalTransMem->Head;		
			SkillTreeNodeLoad(P->Path.Array(), State);
			GlobalTransMem->Head = MemHead;

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


	// Create links into persistent data 
	// Does this need to move into load game?
	{
		// Stations
		{
			for (int i = 0; i < State->PersistentData.StationsCount; i++) {
				State->Stations[i].Persist = &State->PersistentData.Stations[i];
				State->Stations[i].Hold.Setup(1000, item_hold_type::fuel_tank, &State->Stations[i].Persist->ItemHold);

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
	float PrevOrtho = EngineState->GameCamera.OrthoZoom;
	EngineState->GameCamera.OrthoZoom = (real32)LerpCurve(ZoomRealMin, ZoomRealMax, Curve, State->Zoom);
	real64 ZoomSpeedAdj = LerpCurve(4.0f, 200.0f, Curve, State->Zoom);

	// Window size might change
	EngineState->GameCamera.Resolution =						vector2{(real64)Window->Width, (real64)Window->Height};
	EngineState->StateSerializing.UICam.Resolution =			vector2{(real64)Window->Width, (real64)Window->Height};
	EngineState->StateSerializing.EditorCamera.Resolution =		vector2{(real64)Window->Width, (real64)Window->Height};

	// save timer
	{
		static real64 SaveTimer = 0;
		SaveTimer += EngineState->DeltaTimeMS;
		if (MillisecondsToSeconds(SaveTimer) > 5.0f || GlobalTriggerSave) {
			GlobalTriggerSave = false;
			EngineState->DidSave = true;
			SaveTimer = 0.0f;

			PlatformApi.ThreadAddWork(&SaveGameThread, (void*)State);
			//SaveGame(State, GlobalSaveDataRoot);
		}
	}

	// tutorial window
	{
		if (State->TutorialWindow) { 
			ImGui::OpenPopup("Help / Tutorial");

			//ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);
			//ImGui::Begin("Help / Tutorial", &State->TutorialWindow);
				
			//ImGui::End();

			ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
			if (ImGui::BeginPopupModal("Help / Tutorial")) {
				ImGui::Text("Welcome to Space Unknown");

				float Spacing = 20;
				ImGui::Dummy(ImVec2(0, Spacing));
				ImGui::TextWrapped("This is an idle space game. The goal is to collect resource to build space ships to collect more resources.");

				ImGui::Dummy(ImVec2(0, Spacing));
				ImGui::TextWrapped("The game runs even while the browser is closed. This is limited to 24 hours.");

				ImGui::Dummy(ImVec2(0, Spacing));
				ImGui::TextWrapped("The game is still in early development.");

				ImGui::Dummy(ImVec2(0, Spacing));
				ImGui::TextWrapped("Save data is saved locally in browser persistent storage. If you clear browser cache you will lose all save data.");

				ImGui::Dummy(ImVec2(0, Spacing));
				ImGui::TextWrapped("Your first goal is equip a salvager module to collect more knowledge from the salvage.");
				ImGui::TextWrapped("Unlock the salvager module in the skill tree, then construct one on the station.");

				ImGui::Dummy(ImVec2(0, Spacing));
				ImGui::TextWrapped("Drag and drop modules onto the ship module window to equip / unqeuip them.");
				ImGui::TextWrapped("Drag and drop items to / from cargo holds to move items between ship and station.");

				ImGui::Dummy(ImVec2(0, Spacing));
				if (ImGui::Button("Close", ImVec2(-1, 0))) {
					ImGui::CloseCurrentPopup();
					State->TutorialWindow = false;
				}
				ImGui::EndPopup();
			}
		}

	} 

	// Editor
	if (!BuildConfig::Release) {
		if (Input->FunctionKeys[1].OnDown) {
			EditorState->EditorMode = !EditorState->EditorMode;
		}

		if (EditorState->EditorMode) {
			static bool Open = true;
			ImGui::Begin("EDITOR", &Open);

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
				ImGui::Checkbox("HideFog", &EditorState->HideFog);

				real64 SimFPS = 15.0f;
				real64 FrameLengthMS = (1.0f / SimFPS) * 1000.0f;
				if (ImGui::Button("1 minute")) {
					for (int i = 0; i < SimFPS * 60.0f; i++) { StepUniverse(State, FrameLengthMS); }
				}
				ImGui::SameLine();
				if (ImGui::Button("1 hour")) {
					State->ForwardSimulating = true;
					i32 Hours = 5;
					for (int i = 0; i < SimFPS * 60.0f * 60.0f * Hours; i++) { StepUniverse(State, FrameLengthMS); }
					State->ForwardSimulating = false;
				}
			}

			ImGui::Separator();
			if (ImGui::Button("Item Window")) { EditorState->ItemWindowOpen = !EditorState->ItemWindowOpen; }
			if (ImGui::Button("Ship Window")) { EditorState->ShipWindowOpen = !EditorState->ShipWindowOpen; }
			if (ImGui::Button("Skill Node Window")) { EditorState->SkillNodeWindowOpen = !EditorState->SkillNodeWindowOpen; }
			ImGui::Separator();

			if (EditorState->ShipWindowOpen) {
				ImGui::Begin("Ship Window");
				for (int i = 0; i < gen_ship_id_count; i++) {
					ImGui::Text("Create New Ship");
					if (ImGui::Button(ship_id_NAME[i].Array())) {
						ship* Ship = ShipCreate(State, (ship_id)i);
							
						// change this to use the correct persistent data index? use the next one?? redo this so its not so confusing
						ShipSetup(Ship, Ship->Persist, State);

						ItemGive(&Ship->FuelTank, item_id::stl, 200);
					}	
				}
				ImGui::End();
			}

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
					NodeMoving->Persist.Position = SnapVector(vector2{MouseWorld.X, MouseWorld.Y}, 3.0f);
				}

				if (EditorState->NodeSelected != GameNull) {
					ImGui::Text("%llu", EditorState->NodeSelected->Persist.ID);

					{
						int Num = (int)EditorState->NodeSelected->Persist.KnowledgeCost;
						ImGui::DragInt("Knowledge Cost", &Num, 1, 0, 1000000, "%i");
						EditorState->NodeSelected->Persist.KnowledgeCost = (int64)Num;
					}

					ImGui::Separator();

					for (int i = 0; i < EditorState->NodeSelected->ChildrenCount; i++) {
						ImGui::Text("%i", EditorState->NodeSelected->Children[i]->Persist.ID);
						ImGui::SameLine();

						ImGui::PushID(i);
						if (ImGui::Button(" - ")) {
							EditorState->NodeSelected->Children[i] = 0;
							RemoveSlideArray((void*)&EditorState->NodeSelected->Children[0], EditorState->NodeSelected->ChildrenCount, sizeof(EditorState->NodeSelected->Children[0]), i);
							EditorState->NodeSelected->ChildrenCount--;
							
							// rebuild persistent skill node children array
							skill_node* NodeSel = EditorState->NodeSelected;
							for (int j = 0; j < ArrayCount(NodeSel->Persist.ChildrenIDs); j++) {
								NodeSel->Persist.ChildrenIDs[j] = 0;
								if (j < NodeSel->ChildrenCount) {
									NodeSel->Persist.ChildrenIDs[j] = NodeSel->Children[j]->Persist.ID;
								}
							}
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

					skill_bonuses* Bonuses = &EditorState->NodeSelected->Persist.BonusAdditions;
					ImGui::DragFloat("FuelForceAddition", &Bonuses->FuelForce, 0.001f);
					ImGui::DragInt("IndustrialActivationTimeMinutes", &Bonuses->IndustrialActivationTimeMinutes);
					ImGui::DragInt("ShipLimit", &Bonuses->ShipLimit, 1);
					ImGui::DragInt("CargoSize", &Bonuses->CargoSize, 1);
					ImGui::DragInt("ShipRadarRangeAddition", &Bonuses->ShipRadarRangeAddition, 1);
					ImGui::DragInt("FuelTankSize", &Bonuses->FuelTankSize, 1);
					if (ImGui::CollapsingHeader("Recipe")) {
						for (int i = 0; i < ArrayCount(Bonuses->RecipeUnlocked); i++) {
							bool B = Bonuses->RecipeUnlocked[i];
							ImGui::Checkbox(recipe_id_NAME[i].Array(), &B);
							Bonuses->RecipeUnlocked[i] = B;
						}
					}

					const char* Options[gen_skill_node_icon_count];
					for (int i = 0; i < gen_skill_node_icon_count; i++) {
						Options[i] = skill_node_icon_NAME[i].Array();
					}

					int v = (int)EditorState->NodeSelected->Persist.Icon;
					ImGui::Combo("Icon", &v, Options, gen_skill_node_icon_count);
					EditorState->NodeSelected->Persist.Icon = (skill_node_icon)v;
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
					Save();
				}

				ImGui::End();
			}
			if (EditorState->ItemWindowOpen) {

				ImGui::Begin("Debug item give");

				EditorState->DebugHold.Persist = &EditorState->DebugHoldPersist;

				for (int i = 0; i < gen_item_id_count; i++) {
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

			ImGui::End();
	
		}
	}

	// Main windows
	{
		ImGui::BeginMainMenuBar();

		switch (State->Scene) {

			case game_scene::universe: {
				if (ImGui::Button("View Skill Tree", ImVec2(200, 0))) {
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
				if (ImGui::Button("View Universe", ImVec2(200, 0))) {
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


		float Spacing = 30;

		ImGui::Dummy(ImVec2(Spacing, 0));
		ImGui::Text("Ship Limit %i/%i", State->ShipsCount, State->PersistentData.TreeBonuses.ShipLimit);

		ImGui::Dummy(ImVec2(Spacing, 0));
		ImGui::Text("Knowledge %i", State->PersistentData.Knowledge);

		ImGui::Dummy(ImVec2(Spacing * 2, 0));
		static bool SettingsOpen = false;
		if (ImGui::Button("Settings")) {
			SettingsOpen = !SettingsOpen;
		}
		if (ImGui::Button("Help")) {
			State->TutorialWindow = true;
		}

		ImGui::EndMainMenuBar();

		// settings window
		if (SettingsOpen) {
			ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
			ImGui::Begin("Settings", &SettingsOpen);

			ImGui::Columns(2);

			ImGui::Text("Version");
			ImGui::NextColumn();

			string V = "v" + string{VersionMajor} + "." + string{VersionMinor};
			ImGui::Text(V.Array());
			ImGui::NextColumn();

			ImGui::Columns(1);

			if (ImGui::Button("Delete Save Data", ImVec2(-1, 0))) {
				ImGui::OpenPopup("Confirm Delete Save");
			}
			if (ImGui::BeginPopupModal("Confirm Delete Save")) {
				static bool Clicked = false;
				if (!Clicked) { 
					ImGui::Text("Delete all save data? This cannot be undone.");

					real32 HW = ImGui::GetWindowWidth() * 0.47f;
					if (ImGui::Button("Yes", ImVec2(HW, 0))) {
						Clicked = true;

						// save syncronously
						State->PersistentData.Valid = false;
						EngineState->DidSave = true;
						EngineState->FileSynced = false;
						SaveGame(State, GlobalSaveDataRoot);
					}
					ImGui::SameLine();
					if (ImGui::Button("No", ImVec2(HW, 0))) {
						ImGui::CloseCurrentPopup();
					}
				} else if (!EngineState->FileSynced) {
					ImGui::Text("Clearing");
				} else {
					ImGui::Text("Save data deleted.");
					ImGui::Text("Plese reload the page.");
				}

				ImGui::EndPopup();
			}
			ImGui::Separator();

			if (ImGui::TreeNode("Change Log")) {
				ImGui::TextWrapped(ChangeLog);
				ImGui::TreePop();
			}

			ImGui::End();

		}
	}

	// Info windows
	{
		InfoWindow::ImGuiRender();
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

						// Can't select other things when issuing movement commands on ship
						!State->AnyShipCreatingMovement &&

						// Clicked
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

					Sel->Current->SelectionUpdate(Sel, EngineState, Input, i);
				}
			}


			// Update displays
			for (int i = 0; i < State->PersistentData.ShipsCount; i++) {
				ship* Ship = &State->Ships[i];

				// Janky but whatever. Would need to use the transform scene hierarchy to improve
				if (Ship->Persist->Status == ship_status::docked) {
					station* StationDocked = per::GetStation(&Ship->Persist->StationDocked, State);
					Ship->Persist->Rotation = StationDocked->Rotation;

					real64 DockRel = (real64)Ship->Persist->DockIndex / (real64)StationDockCount;
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

			// Render Fog
			if (!EditorState->HideFog)
			{
				shader* Shader = assets::GetShader("Fog");

				vector2 Center = vector2{Window->Width * 0.5f, Window->Height * 0.5f};
				vector2 Size = vector2{(real64)Window->Width, (real64)Window->Height};
				float RenderOrder = -10;

				render_command RendCommand = {};
				InitRenderCommand(&RendCommand, 6);
				InitIndexBuffer(&RendCommand);
				RendCommand.Shader = *Shader;

				vector2 HalfSize = vector2{Size.X * 0.5f, Size.Y * 0.5f};

				v3 TopLeft = 		v3{(real32)Center.X - (real32)HalfSize.X, (real32)Center.Y - (real32)HalfSize.Y, (real32)RenderOrder};
				v3 TopRight = 		v3{(real32)Center.X - (real32)HalfSize.X, (real32)Center.Y + (real32)HalfSize.Y, (real32)RenderOrder};
				v3 BottomRight = 	v3{(real32)Center.X + (real32)HalfSize.X, (real32)Center.Y + (real32)HalfSize.Y, (real32)RenderOrder};
				v3 BottomLeft = 	v3{(real32)Center.X + (real32)HalfSize.X, (real32)Center.Y - (real32)HalfSize.Y, (real32)RenderOrder};

				// Vertices
				layout_data* VertexLayout = RendCommand.GetLayout();

				VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), RendCommand.BufferCapacity, GlobalTransMem);
				VertexLayout->Data.Vec3[0] = TopRight;
				VertexLayout->Data.Vec3[1] = BottomRight;
				VertexLayout->Data.Vec3[2] = BottomLeft;
				VertexLayout->Data.Vec3[3] = TopRight;
				VertexLayout->Data.Vec3[4] = BottomLeft;
				VertexLayout->Data.Vec3[5] = TopLeft;

				RendCommand.Uniforms = RendCommand.Shader.Uniforms.Copy(GlobalTransMem);
				RendCommand.Uniforms.SetMat4("model", m4y4Identity());
				RendCommand.Uniforms.SetMat4("projection", m4y4Transpose(EngineState->UIRenderer.Camera->ProjectionMatrix));
				RendCommand.Uniforms.SetMat4("view", m4y4Transpose(EngineState->UIRenderer.Camera->ViewMatrix));

				vector3 WorldPos = {};	
				static v3 PositionsArray[100] = {};
				static float RadiusArray[100] = {};
				int PosCount = 0;

				static_assert(ArrayCount(RadiusArray) < 256, "256 array length hard coded in the shader");
				static_assert(ArrayCount(PositionsArray) < 256, "356 array length hard coded in the shader");

				for (int i = 0; i < State->PersistentData.ShipsCount; i++) {
					ship* Ship = &State->Ships[i];
					vector2 PosScreen = WorldToScreen(
								vector3{Ship->Persist->Position.X, Ship->Persist->Position.Y, 0},
								&EngineState->GameCamera
							);
					PositionsArray[PosCount] = v3{(real32)PosScreen.X, Window->Height - (real32)PosScreen.Y, 0.0f};
					RadiusArray[PosCount] = Ship->GetRadarRadius() / PrevOrtho;

					PosCount++;
					Assert(PosCount < ArrayCount(PositionsArray));
					Assert(PosCount < ArrayCount(RadiusArray));
				}

				i32 StationRadius = 200;

				for (int i = 0; i < State->PersistentData.StationsCount; i++)  {
					station* Station = &State->Stations[i];
					vector2 PosScreen = WorldToScreen(
								vector3{Station->Persist->Position.X, Station->Persist->Position.Y, 0},
								&EngineState->GameCamera
							);
					PositionsArray[PosCount] = v3{(real32)PosScreen.X, Window->Height - (real32)PosScreen.Y, 0.0f};
					RadiusArray[PosCount] = StationRadius / PrevOrtho;

					PosCount++;
					Assert(PosCount < ArrayCount(PositionsArray));
					Assert(PosCount < ArrayCount(RadiusArray));
				}

				RendCommand.Uniforms.SetVec3Array("radiusCenter", &PositionsArray[0], ArrayCount(PositionsArray));
				RendCommand.Uniforms.SetInt("radiusCenterCount", PosCount);
				RendCommand.Uniforms.SetFloatArray("radius", &RadiusArray[0], ArrayCount(RadiusArray));

				// There is ahard coded limit in the fog shader
				Assert(PosCount < 100);

				InsertRenderCommand(&EngineState->UIRenderer, &RendCommand);
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

					color Col = color{1,1,1,0.2f};
					real32 Width = 1;
					if (Node->Unlocked) {
						Width = 5;
						Col = color{0,1,0,0.4f};
					}

					vector2 Dir = vector2{Node->Persist.Position.X, Node->Persist.Position.Y} - 
									vector2{Node->Children[c]->Persist.Position.X, Node->Children[c]->Persist.Position.Y};
					Dir = Vector2Normalize(Dir) * 2.0f;

					vector2 Points[2] = {};
					Points[0] = WorldToScreen(vector3{Node->Persist.Position.X - Dir.X, Node->Persist.Position.Y - Dir.Y, 0}, &EngineState->GameCamera);
					Points[1] = WorldToScreen(vector3{Node->Children[c]->Persist.Position.X + Dir.X, Node->Children[c]->Persist.Position.Y + Dir.Y, 0}, &EngineState->GameCamera);
					render_line Line = {};
					Line.Points = Points;
					Line.PointsCount = ArrayCount(Points);
					RenderLine(Line, Width, Col, &EngineState->UIRenderer, false);
				}

				color Color = COLOR_WHITE;
				vector2 Size = vector2{Node->CircleRadius, Node->CircleRadius};
				if (!Node->Unlocked) {
					Color.A = 0.25f;
				}
				if (SkillNodeCanUnlock(Node)) {
					Color.A = 0.5f;
					Size = Size * 1.1f;
				}

				real32 RadiusMax = 2;
				if (Node->Unlocked) {
					//RadiusMax = 3;
					Color = COLOR_WHITE;
				}
				Node->CircleRadius = Lerp(Node->CircleRadius, RadiusMax, 0.25f);

				recipe_id RecipeUnlocking = Node->RecipeUnlocking();
				if (RecipeUnlocking != recipe_id::none) {

					RenderTextureAll(Node->Persist.Position, Size * 1.4f, Color, 
									Globals->AssetsList.RecipeDefinitions[(int)RecipeUnlocking].Icon->GLID,
									RenderLayerPlanet, m4y4Identity(), Globals->GameRenderer);

				} else {

					RenderTextureAll(Node->Persist.Position, Size * 1.4f, Color, 
									Globals->AssetsList.SkillNodeIcons[(int)Node->Persist.Icon]->GLID,
									RenderLayerPlanet, m4y4Identity(), Globals->GameRenderer);
						
				}
	
				if (Vector2Distance(Node->Persist.Position, MouseWorldFlat) < Node->CircleRadius) {
					EngineState->GameState.NodeHovering = Node;
				}
			}

			static skill_node* NodeSelected = {};

			if (NodeSelected == GameNull && State->NodeHovering != GameNull) {
				if (SkillNodeCanUnlock(State->NodeHovering)) { 
					State->NodeHovering->CircleRadius = 4;
				}

				ImGuiIO& io = ImGui::GetIO();
				if (!io.WantCaptureMouse){ 
					ImGui::SetNextWindowPos(ImVec2((float)Input->MousePos.X + 20, (float)Input->MousePos.Y));
					bool Open = true;
					ImGui::Begin("Info", &Open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
					ImGui::Text("Knowledge Cost %i", State->NodeHovering->Persist.KnowledgeCost);
					SkillTreeImguiDisplayBonuses(State->NodeHovering->Persist.BonusAdditions);
					ImGui::TextColored(ImVec4(1,1,1,0.5f), "Right click for info on modules or ships.");
					ImGui::End();
				}

				// can unlock
				if (SkillNodeCanUnlock(State->NodeHovering) && Input->MouseLeft.OnDown && !EditorState->EditorMode) {
					ImGui::OpenPopup("Unlock");
					NodeSelected = State->NodeHovering;
				}
				if (Input->MouseRight.OnDown && !EditorState->EditorMode) {
					// Probably an error some day!
					// Pick the first recipe unlocked to use for info 
					for (int i = 0; i < ArrayCount(State->NodeHovering->Persist.BonusAdditions.RecipeUnlocked); i++) {
						if (State->NodeHovering->Persist.BonusAdditions.RecipeUnlocked[i]) {
							recipe* Rec = &Globals->AssetsList.RecipeDefinitions[i];
							Assert(Rec->OutputsCount > 0);

							if (Rec->Outputs[0].Type == recipe_member_type::item) { 
								InfoWindow::Show(Rec->Outputs[0].ItemID);
							} else if (Rec->Outputs[0].Type == recipe_member_type::ship) {
								InfoWindow::Show(Rec->Outputs[0].ShipID);
							} else {
								Assert(0);
							}
						}
					}
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
					}

					ImGui::EndPopup();
				}
			}
		}
		break;

		INVALID_DEFAULT;
	}
}

#endif
