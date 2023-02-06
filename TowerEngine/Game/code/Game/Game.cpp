#pragma once

#ifndef GAME_CPP
#define GAME_CPP

#include "Game.h"

#include <chrono>
#include <ctime>

int64 VersionMajor = 0;
int64 VersionMinor = 0;
int64 VersionBuild = 0;

namespace game {

	void AddStepper(stepper* Stepper, game::state* State)
	{
		Assert(State->SteppersCount < ArrayCount(State->Steppers));
		State->Steppers[State->SteppersCount++] = Stepper;
	}

	void RegisterStepper(stepper* Stepper,
	                     step_func Method,
	                     void* SelfData,
	                     game::state* State)
	{
		Stepper->Step = Method;
		Stepper->SelfData = SelfData;
		AddStepper(Stepper, State);
	}

	void UnregisterStepper(stepper* Stepper, game::state* State)
	{
		for (int i = 0; i  < State->SteppersCount; i++) {
			if (Stepper == State->Steppers[i]) {
				RemoveSlideArray((void*)&State->Steppers[0], State->SteppersCount, sizeof(State->Steppers[0]), i);
				State->SteppersCount--;
				return;
			}
		}
	}

	void StepUniverse(game::state* State, real64 TimeMS)
	{
		// Check for wakeup stepper
		if (State->SleepingSteppers->LinkCount >= 1) {
			stepper_ptr* pt = (stepper_ptr*)GetLinkData(State->SleepingSteppers, 0);
			if (State->UniverseTime.TimeMS >= pt->Stp->WakeupTime) {
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

	void SleepStepper(game::state* State, stepper* Stepper, real64 SleepDurationMS)
	{
		stepper_ptr Data = {};
		Data.Stp = Stepper;

		Stepper->WakeupTime = State->UniverseTime.TimeMS + SleepDurationMS;
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

	void TimeStep(void* SelfData, real64 Time, game::state* State)
	{
		game::universe_time* UT = (game::universe_time*)SelfData;
		UT->TimeMS += Time;
	}

	void RegisterSelectable(selection_type Type, vector2* Center, vector2* Size, void* Data, game::state* State,
	                        selection_update_func OnSelectionUpdate,
	                        selection_on_func OnSelection)
	{
		selectable* Sel = &State->Selectables[State->SelectablesCount++];
		Assert(ArrayCount(State->Selectables) > State->SelectablesCount);

		Sel->Type = Type;
		Sel->Center = Center;
		Sel->Size = Size;
		Sel->Data = Data;
		Sel->SelectionUpdate = OnSelectionUpdate;
		Sel->OnSelection = OnSelection;
	}

	void SaveGame(game::state* State)
	{
		json::json_data JsonOut = json::GetJson(GlobalTransMem);
		int32 DecimalCount = 7;

		using std::chrono::duration_cast;
		using std::chrono::system_clock;
		int64 SinceEpoch = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();

		AddKeyPair("real_time_saved", string{SinceEpoch} .Array(), &JsonOut);
		AddKeyPair("simulation_time", string{State->UniverseTime.TimeMS}, &JsonOut);

		// Ships
		for (int i = 0; i < ArrayCount(State->Ships); i++) {
			if (State->Ships[i].Using) {
				ship* Ship = &State->Ships[i];

				json::AddKeyPair("ship_" + string{i} + "_position_x", Real64ToString(Ship->Position.X, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_position_y", Real64ToString(Ship->Position.Y, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_velocity_x", Real64ToString(Ship->Velocity.X, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_velocity_y", Real64ToString(Ship->Velocity.Y, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_is_moving", Real64ToString(Ship->IsMoving, DecimalCount), &JsonOut);
				//json::AddKeyPair("ship_" + string{i} + "_fuel", Real64ToString(Ship->FuelGallons, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_rotation", Real64ToString(Ship->Rotation, DecimalCount), &JsonOut);

				/*
				json::AddKeyPair("ship_" + string{i} + "_journey_end_x", Real64ToString(Ship->CurrentJourney.EndPosition.X, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_journey_end_y", Real64ToString(Ship->CurrentJourney.EndPosition.Y, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_journey_start_x", Real64ToString(Ship->CurrentJourney.StartPosition.X, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_journey_start_y", Real64ToString(Ship->CurrentJourney.StartPosition.Y, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_journey_dist_from_sides_to_coast", Real64ToString(Ship->CurrentJourney.DistFromSidesToCoast, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_journey_edge_ratio", Real64ToString(Ship->CurrentJourney.EdgeRatio, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_journey_dir_to_end_x", Real64ToString(Ship->CurrentJourney.DirToEnd.X, DecimalCount), &JsonOut);
				json::AddKeyPair("ship_" + string{i} + "_journey_dir_to_end_y", Real64ToString(Ship->CurrentJourney.DirToEnd.Y, DecimalCount), &JsonOut);
				*/

				// Add method for saving a hold
				/*
				for (int c = 0; c < ArrayCount(Ship->Cargo); c++) {
					if (Ship->Cargo[c].Count > 0) {

						string DefID = item_id_NAME[(int)Ship->Cargo[c].Definition.ID];

						json::AddKeyPair("ship_" + string{i} + "_cargo_" + string{c} + "_id", DefID, &JsonOut);
						json::AddKeyPair("ship_" + string{i} + "_cargo_" + string{c} + "_count", string{Ship->Cargo[c].Count}, &JsonOut);
					}
				}
				*/
			}
		}

		// Asteroids
		for (int i = 0; i < State->ClustersCount; i++) {
			asteroid_cluster* Cluster = &State->Asteroids[i];
			for (int c = 0; c < ArrayCount(Cluster->Asteroids); c++) {
				if (Cluster->Asteroids[c].Using) {
					asteroid* Roid = &Cluster->Asteroids[c];

					json::AddKeyPair("cluster_" + string{i} + "_asteroid_" + string{c} + "_position_x", Real64ToString(Roid->Position.X, DecimalCount), &JsonOut);
					json::AddKeyPair("cluster_" + string{i} + "_asteroid_" + string{c} + "_position_y", Real64ToString(Roid->Position.Y, DecimalCount), &JsonOut);
					json::AddKeyPair("cluster_" + string{i} + "_asteroid_" + string{c} + "_size", Real64ToString(Roid->Size, DecimalCount), &JsonOut);
				}
			}
		}

		json::SaveToFile(&JsonOut, "SaveGame.sus");
		ConsoleLog("Game Saved");
	}

#include "Definitions.cpp"
#include "Asteroid.cpp"
#include "Salvage.cpp"
#include "Item.cpp"
#include "Recipe.cpp"
#include "Station.cpp"
#include "Ship.cpp"
#include "SkillTree.cpp"

	void LoadGame(game::state* State)
	{
		json::json_data JsonIn = json::LoadFile("SaveGame.sus", GlobalTransMem);

		if (JsonIn.PairsCount == 0) {
			ConsoleLog("No saved data file");
			return;
		}

		State->UniverseTime.TimeMS = json::GetReal64("simulation_time", &JsonIn);

		// Ships
		for (int i = 0; i < ArrayCount(State->Ships); i++) {
			json::json_pair* TestPair = GetPair("ship_" + string{i} + "_position_x", &JsonIn);
			if (TestPair != GameNull) {
				ship* Ship = &State->Ships[i];
				Ship->Using = true;

				Ship->Position.X = json::GetReal64("ship_" + string{i} + "_position_x", &JsonIn);
				Ship->Position.Y = json::GetReal64("ship_" + string{i} + "_position_y", &JsonIn);
				Ship->Velocity.X = json::GetReal64("ship_" + string{i} + "_velocity_x", &JsonIn);
				Ship->Velocity.Y = json::GetReal64("ship_" + string{i} + "_velocity_y", &JsonIn);
				Ship->IsMoving = json::GetBool("ship_" + string{i} + "_is_moving", &JsonIn);
				//Ship->FuelGallons = json::GetReal64("ship_" + string{i} + "_fuel", &JsonIn);
				Ship->Rotation = json::GetReal64("ship_" + string{i} + "_rotation", &JsonIn);

				/*
				Ship->CurrentJourney.EndPosition.X = json::GetReal64("ship_" + string{i} + "_journey_end_x", &JsonIn);
				Ship->CurrentJourney.EndPosition.Y = json::GetReal64("ship_" + string{i} + "_journey_end_y", &JsonIn);
				Ship->CurrentJourney.StartPosition.X = json::GetReal64("ship_" + string{i} + "_journey_start_x", &JsonIn);
				Ship->CurrentJourney.StartPosition.Y = json::GetReal64("ship_" + string{i} + "_journey_start_y", &JsonIn);
				Ship->CurrentJourney.DistFromSidesToCoast = json::GetReal64("ship_" + string{i} + "_journey_dist_from_sides_to_coast", &JsonIn);
				Ship->CurrentJourney.EdgeRatio = (real32)json::GetReal64("ship_" + string{i} + "_journey_edge_ratio", &JsonIn);
				Ship->CurrentJourney.DirToEnd.X = json::GetReal64("ship_" + string{i} + "_journey_dir_to_end_x", &JsonIn);
				Ship->CurrentJourney.DirToEnd.Y = json::GetReal64("ship_" + string{i} + "_journey_dir_to_end_y", &JsonIn);
				*/

				// method to load an item_hold
				/*
				for (int c = 0; c < ArrayCount(Ship->Cargo); c++) {
					json::json_pair* CargoTestPair = GetPair("ship_" + string{i} + "_cargo_" + string{c} + "_id", &JsonIn);
					if (CargoTestPair != GameNull) {

						string IDStr = json::GetString("ship_" + string{i} + "_cargo_" + string{c} + "_id", &JsonIn);
						item_id ItemID = (item_id)StringToEnum(IDStr, &item_id_NAME[0], ArrayCount(item_id_NAME));
						item_definition Def = GetItemDefinition(ItemID);
						Ship->Cargo[c].Definition = Def;

						Ship->Cargo[c].Count = (int32)json::GetInt64("ship_" + string{i} + "_cargo_" + string{c} + "_count", &JsonIn);;
					}
				}
				*/

				ShipUpdateMass(Ship);
			}
		}

		// Asteroids
		for (int i = 0; i < State->ClustersCount; i++) {
			asteroid_cluster* Cluster = &State->Asteroids[i];
			for (int c = 0; c < ArrayCount(Cluster->Asteroids); c++) {
				json::json_pair* TestPair = GetPair("cluster_" + string{i} + "_asteroid_" + string{c} + "_position_x", &JsonIn);
				if (TestPair != GameNull) {
					asteroid* Roid = &Cluster->Asteroids[c];
					InitAsteroid(Roid);

					Roid->Position.X = json::GetReal64("cluster_" + string{i} + "_asteroid_" + string{c} + "_position_x", &JsonIn);
					Roid->Position.Y = json::GetReal64("cluster_" + string{i} + "_asteroid_" + string{c} + "_position_y", &JsonIn);
				}
			}
		}

		using std::chrono::duration_cast;
		using std::chrono::system_clock;
		int64 CurrentSinceEpoch = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
		int64 FileSinceEpoch = json::GetInt64("real_time_saved", &JsonIn);
		real64 MissingMS = (real64)(CurrentSinceEpoch - FileSinceEpoch);
		real64 MissingMSStart = MissingMS;

		string P = "Simulating " + string{MissingMS} + " ms of missing time";
		ConsoleLog(P.Array());

		State->ForwardSimulating = true;

		int32 Flicker = 2000;

		float SimFPS = 15.0f;
		float TimeStepMS = (1.0f / SimFPS) * 1000.0f;
		while (MissingMS > SimFPS) {
			StepUniverse(State, TimeStepMS);
			MissingMS -= TimeStepMS;

			Flicker--;
			if (Flicker <= 0) {
				Flicker = 2000;
				real64 PercDone = 100.0f - ((MissingMS / MissingMSStart) * 100.0f);
				string PercDisp = string{PercDone};
				ConsoleLog(PercDisp);
			}
		}
		StepUniverse(State, MissingMS);

		State->ForwardSimulating = false;
		ConsoleLog("Finished");
	}

	void Start(engine_state* EngineState)
	{
		game::state* State = &EngineState->GameState;

		// Load skill nodes
		{
			path_list NodeFiles = {};
			PlatformApi.GetPathsForFileType(".skill_node", EngineState->RootAssetPath.Array(), GlobalTransMem, &NodeFiles);

			path_list* P = &NodeFiles;
			while (StringLength(P->Path) > 0) {
				json::json_data json = json::LoadFile(P->Path, GlobalTransMem);
				SkillTreeNodeLoad(&json, State);

				P = P->Next;
			}

			// Update Children after all the node have been loaded
			for (int i = 0; i < State->SkillNodesCount; i++) {
				skill_node* Node = &State->SkillNodes[i];
				for (int c = 0; c < ArrayCount(Node->SavedChildrenIDs); c++) {
					if (StringLength(Node->SavedChildrenIDs[c]) > 0) {
						Node->AddChild(SkillTreeNodeFind(Node->SavedChildrenIDs[c], State));
					}
				}
			}
		}

		State->SleepingSteppers = CreateListFixed(GlobalPermMem, sizeof(stepper_ptr), 100);

		CreateDefinitions();

		RegisterStepper(&State->UniverseTime.Stepper, &TimeStep, (void*)(&State->UniverseTime), State);

		// Load asteroid images
		Globals->AssetsList.AsteroidImages[0] = assets::GetImage("Asteroid1");
		Globals->AssetsList.AsteroidImages[1] = assets::GetImage("Asteroid2");
		Globals->AssetsList.AsteroidImages[2] = assets::GetImage("Asteroid3");

		// Load salvage images
		Globals->AssetsList.SalvageImages[0] = assets::GetImage("Salvage1");
		Globals->AssetsList.SalvageImages[1] = assets::GetImage("Salvage2");

		ShipSetup(State, vector2{0, 0});
		AsteroidCreateCluster(vector2{0, 0}, 30.0f, State);

		// Salvage
		SalvageCreate(State, vector2{ -30, -30});

		// Station
		station* Station = StationCreate(State);
		Station->Position.X = 50;
		Station->Position.Y = 50;

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
		game::state* State = &EngineState->GameState;
		game::editor_state* EditorState = &EngineState->EditorState;

		TreeBonusesTotal = &State->TreeBonusesTotal;

		State->Zoom = (real32)Lerp(State->Zoom, State->ZoomTarget, 0.5f);
		float Curve = 3.5f;
		EngineState->GameCamera.OrthoZoom = (real32)LerpCurve(ZoomRealMin, ZoomRealMax, Curve, State->Zoom);
		real64 ZoomSpeedAdj = LerpCurve(4.0f, 200.0f, Curve, State->Zoom);

		// save timer
		{
			static real64 SaveTimer = 0;
			SaveTimer += EngineState->DeltaTimeMS;
			if (MillisecondsToSeconds(SaveTimer) > 5.0f) {
				SaveTimer = 0.0f;
				SaveGame(State);
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
						NodeMoving->Position = vector2{MouseWorld.X, MouseWorld.Y};
					}

					if (EditorState->NodeSelected != GameNull) {
						ImGui::Text(EditorState->NodeSelected->ID.Array());

						{
							int Num = (int)EditorState->NodeSelected->KnowledgeCost;
							ImGui::DragInt("Knowledge Cost", &Num, 1, 0, 1000000, "%i");
							EditorState->NodeSelected->KnowledgeCost = (int64)Num;
						}

						ImGui::Separator();

						for (int i = 0; i < EditorState->NodeSelected->ChildrenCount; i++) {
							ImGui::Text(EditorState->NodeSelected->Children[i]->ID.Array());
							ImGui::SameLine();

							ImGui::PushID(i);
							if (ImGui::Button("-")) {
								RemoveSlideArray((void*)&EditorState->NodeSelected->Children[0], EditorState->NodeSelected->ChildrenCount, sizeof(EditorState->NodeSelected->Children[0]), i);
								EditorState->NodeSelected->ChildrenCount--;
							}
							ImGui::PopID();
						}

						if (!AddingChild) {
							if (ImGui::Button("+ Add Child + ")) {
								AddingChild = true;
							}
						} else {
							ImGui::Text("!! CLICK CHILD !!");
						}

						ImGui::Separator();

						ImGui::Text("Bonuses");

						ImGui::DragFloat("FuelForceAddition", &EditorState->NodeSelected->BonusAdditions.FuelForceAddition, 0.001f);
					}

					ImGui::Dummy(ImVec2(0, 30));

					if (ImGui::Button("Knowledge +5 ", ImVec2(-1, 0))) {
						State->Knowledge += 5;
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

					for (int i = 0; i < (int)item_id::count; i++) {
						item_definition Def = Globals->AssetsList.ItemDefinitions[i];

						{

							ImGui::Image(
							    (ImTextureID)((int64)Def.Icon->GLID),
							    ImGuiImageSize,
							    ImVec2(0, 0),
							    ImVec2(1, -1),
							    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
							    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
							);

							if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
								EditorState->DebugItemDragging.Definition = Def;
								EditorState->DebugItemDragging.Count = 1;

								State->ItemDragging = &EditorState->DebugItemDragging;
								State->HoldItemDraggingFrom = &EditorState->DebugHold;

								int D = 0;
								ImGui::SetDragDropPayload(ImguiItemDraggingID, &D, sizeof(D));

								ImGui::Image(
								    (ImTextureID)((int64)Def.Icon->GLID),
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
							ImGui::Text(Def.DisplayName.Array());
							ImGui::Text("x1");
							ImGui::EndGroup();
						}


						{
							ImGui::Image(
							    (ImTextureID)((int64)Def.Icon->GLID),
							    ImGuiImageSize,
							    ImVec2(0, 0),
							    ImVec2(1, -1),
							    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
							    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
							);

							if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
								EditorState->DebugItemDragging.Definition = Def;
								EditorState->DebugItemDragging.Count = 10;

								State->ItemDragging = &EditorState->DebugItemDragging;
								State->HoldItemDraggingFrom = &EditorState->DebugHold;

								int D = 0;
								ImGui::SetDragDropPayload(ImguiItemDraggingID, &D, sizeof(D));

								ImGui::Image(
								    (ImTextureID)((int64)Def.Icon->GLID),
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
							ImGui::Text(Def.DisplayName.Array());
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

							vector2 PosOrig = CurrentShip->Position;
							ItemGive(&CurrentShip->FuelTank, item_id::stl, 1000);
							CurrentShip->Velocity = {};

							CurrentShip->CurrentJourney.Execute();
							CurrentShip->CurrentJourney.Steps[0].Start(CurrentShip, &CurrentShip->CurrentJourney.Steps[0], State);

							while (
							    !CurrentShip->CurrentJourney.Steps[0].Step(CurrentShip, &CurrentShip->CurrentJourney.Steps[0], TimeStepMS, State)
							) {
								TotalSimTime += TimeStepMS;
							}

							CurrentShip->Position = PosOrig;
							CurrentShip->Velocity = {};
							CurrentShip->IsMoving = false;

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
			Time += std::chrono::milliseconds((int)State->UniverseTime.TimeMS);
			std::chrono::seconds Diff = std::chrono::duration_cast<std::chrono::seconds>(Time.time_since_epoch());
			string Disp = ChronoToString(Diff);
			ImGui::Text(Disp.Array());

			ImGui::Separator();

			switch (State->Scene) {

				case scene::universe: {
					ImGui::SliderFloat("Zoom", &State->ZoomTarget, ZoomMin, ZoomMax, " % .3f");

					if (ImGui::Button("Skill Tree", ImVec2(-1, 0))) {
						State->Scene = scene::skill_tree;
					}
				}
				break;

				case scene::skill_tree: {
					if (ImGui::Button("Universe", ImVec2(-1, 0))) {
						State->Scene = scene::universe;
					}
				}
				break;

				INVALID_DEFAULT;
			}

			ImGui::Separator();
			ImGui::Text("Resources");
			ImGui::Text("Knowledge - % i", State->Knowledge);
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

			case scene::universe: {


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

					// Hover display
					if (State->Hovering != GameNull) {

						vector2 WorldCenter = *State->Hovering->Center;
						vector2 WorldSize = *State->Hovering->Size;

						vector2 WorldTopLeft = WorldCenter + (WorldSize * 0.5f);
						vector2 WorldBottomRight = WorldCenter - (WorldSize * 0.5f);

						rect R = {};
						R.TopLeft = WorldToScreen(vector3{WorldTopLeft.X, WorldTopLeft.Y, 0}, &EngineState->GameCamera);
						R.BottomRight = WorldToScreen(vector3{WorldBottomRight.X, WorldBottomRight.Y, 0}, &EngineState->GameCamera);

						RenderRectangleOutline(R, 1, COLOR_RED, -1, &EngineState->UIRenderer);
					}

					// Selecting
					if (State->Hovering != GameNull && !State->Hovering->Selected && Input->MouseLeft.OnUp && !Input->MouseMoved()) {
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
				for (int i = 0; i < ArrayCount(State->Ships); i++) {
					ship* Ship = &State->Ships[i];
					if (Ship->Using) {

						// Janky but whatever. Would need to use the transform scene hierarchy to improve
						if (Ship->Status == ship_status::docked) {
							Ship->Rotation = Ship->StationDocked->Rotation;

							int DocksCount = 10;
							real64 DockRel = (real64)(Ship->StationDocked->DockedCount) / (real64)DocksCount;
							real64 DockRadians = DockRel * (2 * PI);

							real64 DockRadius = Ship->StationDocked->Size.X * 0.5f * 0.9f;
							vector2 StationOffset = Ship->StationDocked->Position + vector2 {
								DockRadius * sin(DockRadians),
								DockRadius * cos(DockRadians)
							};

							vector2 NewPos = Vector2RotatePoint(StationOffset, Ship->StationDocked->Position, DockRadians + -Ship->StationDocked->Rotation);
							Ship->Position.X = NewPos.X;
							Ship->Position.Y = NewPos.Y;
						}
					}
				}

				// Render planets
				RenderCircle(vector2{1200, 200}, vector2{2000, 2000},
				             COLOR_RED, RenderLayerPlanet, Globals->GameRenderer);

				// Render asteroids
				for (int i = 0; i < State->ClustersCount; i++) {
					asteroid_cluster* Clust = &State->Asteroids[i];
					for (int a = 0; a < ArrayCount(Clust->Asteroids); a++) {
						if (Clust->Asteroids[a].Using) {

							Clust->Asteroids[a].Rotation += Clust->Asteroids[a].RotationRate * EngineState->DeltaTimeMS * 0.0002f;

							m4y4 Model = m4y4Identity();
							Model = Rotate(Model, vector3{0, 0, Clust->Asteroids[a].Rotation});

							RenderTextureAll(
							    Clust->Asteroids[a].Position,
							    vector2{Clust->Asteroids[a].Size, Clust->Asteroids[a].Size},
							    Color255(79.0f, 60.0f, 48.0f, 1.0f),
							    Clust->Asteroids[a].Image->GLID,
							    RenderLayerPlanet, Model, Globals->GameRenderer);
						}
					}
				}

				// Render salvage
				for (int i = 0; i < State->SalvagesCount; i++) {
					salvage* Salvage = &State->Salvages[i];

					//Clust->Asteroids[a].Rotation += Clust->Asteroids[a].RotationRate * EngineState->DeltaTimeMS * 0.0002f;

					//m4y4 Model = m4y4Identity();
					//Model = Rotate(Model, vector3{0, 0, Clust->Asteroids[a].Rotation});

					RenderTextureAll(
					    Salvage->Position,
					    vector2{10.0f, 10.0f},
					    Color255(197.0f, 214.0f, 208.0f, 1.0f),
					    Salvage->Image->GLID,
					    RenderLayerPlanet, m4y4Identity(), Globals->GameRenderer);
				}

				// Render stations
				for (int i = 0; i < State->StationsCount; i++) {
					station* Station = &State->Stations[i];

					Station->Rotation += (PI / 10.0f) * EngineState->DeltaTimeMS * 0.0002f;

					m4y4 Model = m4y4Identity();
					Model = Rotate(Model, vector3{0, 0, Station->Rotation});

					static loaded_image* Sprite = assets::GetImage("Station");

					RenderTextureAll(
					    Station->Position,
					    vector2{18.0f, 18.0f},
					    Color255(90.0f, 99.0f, 97.0f, 255.0f),
					    Sprite->GLID,
					    RenderLayerPlanet, Model, Globals->GameRenderer);
				}

				// Render ships
				for (int i = 0; i < ArrayCount(State->Ships); i++) {
					ship* Ship = &State->Ships[i];
					if (Ship->Using) {

						vector2 Pos = Ship->Position;

						m4y4 Model = m4y4Identity();
						Model = Rotate(Model, vector3{0, 0, Ship->Rotation});

						static loaded_image* ShipImage = assets::GetImage("Ship");
						RenderTextureAll(
						    Pos,
						    Ship->Size,
						    COLOR_WHITE,
						    ShipImage->GLID, RenderLayerShip, Model, Globals->GameRenderer);
					}

					// Render ship module effects
					for (int m = 0; m < ArrayCount(Ship->EquippedModules); m++) {
						ship_module* Module = &Ship->EquippedModules[m];
						if (Module->Filled && Module->Target != GameNull) {
							vector2 Points[2] = {};
							Points[0] = WorldToScreen(vector3{Ship->Position.X, Ship->Position.Y, 0}, &EngineState->GameCamera);
							Points[1] = WorldToScreen(vector3{Module->Target->Position.X, Module->Target->Position.Y, 0}, &EngineState->GameCamera);
							render_line Line = {};
							Line.Points = Points;
							Line.PointsCount = ArrayCount(Points);
							RenderLine(Line, 1.5f, color{1, 0, 0, 0.2f}, &EngineState->UIRenderer, false);
						}
					}
				}

			}
			break;

			case scene::skill_tree: {

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
						Points[0] = WorldToScreen(vector3{Node->Position.X, Node->Position.Y, 0}, &EngineState->GameCamera);
						Points[1] = WorldToScreen(vector3{Node->Children[c]->Position.X, Node->Children[c]->Position.Y, 0}, &EngineState->GameCamera);
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
					RenderCircle(Node->Position, vector2{Node->CircleRadius, Node->CircleRadius}, Color, 2, Globals->GameRenderer);
					if (Vector2Distance(Node->Position, MouseWorldFlat) < Node->CircleRadius) {
						EngineState->GameState.NodeHovering = Node;
					}
				}

				static skill_node* NodeSelected = {};

				if (NodeSelected == GameNull && State->NodeHovering != GameNull) {
					State->NodeHovering->CircleRadius = 4;

					ImGui::SetNextWindowPos(ImVec2((float)Input->MousePos.X + 20, (float)Input->MousePos.Y));
					bool Open = true;
					ImGui::Begin("Info", &Open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
					ImGui::Text("Knowledge Cost - % i", State->NodeHovering->KnowledgeCost);
					SkillTreeImguiDisplayBonuses(State->NodeHovering->BonusAdditions);
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

						ImGui::Text("Spend % i Knowledge to unlock this bonus ? ", NodeSelected->KnowledgeCost);
						SkillTreeImguiDisplayBonuses(NodeSelected->BonusAdditions);

						if (NodeSelected->Unlocked) {
							ImGui::Text("UNLOCKED");
						} else {
							ImGui::Separator();
							real32 HW = ImGui::GetWindowWidth() * 0.47f;
							if (State->Knowledge >= NodeSelected->KnowledgeCost) {
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
}

#endif