void WorldTargetSet(world_target_persistent* Target, asteroid* Input) { 
	Target->Type = world_target_type::asteroid;
	per::SetAsteroid(&Target->Asteroid, Input);
}

void WorldTargetSet(world_target_persistent* Target, salvage* Input) { 
	Target->Type = world_target_type::salvage;
	Target->Salvage = Input;
}

void WorldTargetClear(world_target_persistent* Target) { 
	Target->Type = world_target_type::none;
}

bool32 WorldTargetHasTarget(world_target_persistent* Target) { 
	return Target->Type != world_target_type::none;
}

asteroid* WorldTargetGetAsteroid(world_target_persistent* Target, state* State) {
	Assert(Target->Type == world_target_type::asteroid);
	return per::GetAsteroid(&Target->Asteroid, State);
}

salvage* WorldTargetGetSalvage(world_target_persistent* Target) {
	Assert(Target->Type == world_target_type::salvage);
	return Target->Salvage;
}

vector2 WorldTargetGetPosition(world_target_persistent* Target, state* State) {
	switch (Target->Type) {
		case world_target_type::asteroid: 
			return per::GetAsteroid(&Target->Asteroid, State)->Persist->WorldObject.Position; 
		break;

		case world_target_type::salvage: return Target->Salvage->WorldObject.Position; break;
		INVALID_DEFAULT
	}
	return {};
}
	 
void ShipUpdateMass(ship* Ship)
{
	Ship->CurrentMassTotal = Ship->Hold.MassCurrent + Ship->FuelTank.MassCurrent + Ship->Definition.Mass;
}

real64 ShipGetMassTotal(ship* Ship)
{
	if (Ship->Hold.MassChanged.DidChange() || Ship->FuelTank.MassChanged.DidChange()) {

		Ship->FuelTank.MassChanged.MarkAccess();
		Ship->Hold.MassChanged.MarkAccess();

		ShipUpdateMass(Ship);
	}

	return Ship->CurrentMassTotal;
}

bool32 ShipSimulateMovement(ship* Ship, journey_movement* Mov, real64 TimeMS, state* State)
{
	real64 TimeSeconds = TimeMS * 0.001f;

	real64 fuelToUse = Mov->CachedFuelToUse;
	real64 fuelForce = Mov->CachedFuelForce;
	vector2 dirToTargetForce = Mov->CachedDirToTargetForce;
	if (TimeMS != Mov->CachedTime || true) {
		real64 FuelForceFinal = fuelForcePerGallon + (fuelForcePerGallon);

		fuelToUse = Ship->Definition.FuelRateMassPerSecond * TimeSeconds;
		fuelForce = fuelToUse * FuelForceFinal;
		dirToTargetForce = Mov->DirToEnd * fuelForce;

		Mov->CachedTime = TimeMS;
		Mov->CachedFuelForce = fuelForce;
		Mov->CachedFuelToUse = fuelToUse;
		Mov->CachedDirToTargetForce = dirToTargetForce;
	}

	real64 DistToStart = Vector2Distance(Ship->Persist->Position, Mov->StartPosition);
	real64 DistToEnd = Vector2Distance(Ship->Persist->Position, Mov->EndPosition);

	// close enough
	if (DistToEnd < 0.01f) {
		Ship->Persist->Velocity = vector2{0, 0};
		return true;
	}

	// past target
	// Need a 0.1 buffer here because the two distances are equal during the journey, but not always exactly because of rounding errors
	if (Mov->FullDistance < DistToEnd + DistToStart) {
		// Stopping, past destination
		Ship->Persist->Velocity = vector2{0, 0};
		return true;
	}

	if (DistToStart < Mov->DistFromSidesToCoast || DistToEnd < Mov->DistFromSidesToCoast) {

		vector2 Force = {};

		// Speed up
		if (DistToStart < Mov->DistFromSidesToCoast) {
			Ship->FuelTank.ConsumeFuel(fuelToUse);
			Force = dirToTargetForce;
		}

		// Slow down
		if (DistToEnd < Mov->DistFromSidesToCoast) {
			Ship->FuelTank.ConsumeFuel(fuelToUse);
			Force = dirToTargetForce * -1.0f;

			// slow enough
			if (Vector2Length(Ship->Persist->Velocity) < 0.001f) {
				Ship->Persist->Velocity = vector2{0, 0};
				return true;
			}
		}

		real64 Mass = ShipGetMassTotal(Ship);
		vector2 acceleration = Force / (real64)Mass;
		Ship->Persist->Velocity = Ship->Persist->Velocity + acceleration;
	} else {

		// If we're simulating, then we can sleep until the ship need to slow down
		if (State->ForwardSimulating) {

			// Stop the sleep when the ships stops coasting, or when the simulating time finishes.
			
			// Figure out when the ship will start slowing down
			real64 CoastDist = Mov->FullDistance - (Mov->DistFromSidesToCoast * 2.0f);
			real64 CoastSpeed = Vector2Length(Ship->Persist->Velocity);
			real64 CoastDurationSeconds = CoastDist / CoastSpeed;

			if (State->ForwardSimulatingTimeRemaining < SecondsToMilliseconds(CoastDurationSeconds)) {
				// Sleep ends before the ship finishes coasting

				vector2 EndPos = Ship->Persist->Position + ((float)MillisecondsToSeconds(State->ForwardSimulatingTimeRemaining) * Ship->Persist->Velocity);
				Ship->Persist->Position = EndPos;

				SleepStepper(State, &Ship->Stepper, State->ForwardSimulatingTimeRemaining);
				return false;

			} else {
				// Sleep ends after ship finishes coasting

				// Move Ship to end of coast
				vector2 SlowStartingPos = Mov->EndPosition - (Mov->DirToEnd * Mov->DistFromSidesToCoast);
				Ship->Persist->Position = SlowStartingPos;

				SleepStepper(State, &Ship->Stepper, SecondsToMilliseconds(CoastDurationSeconds));
				return false;
			}
		}
	}

	Ship->Persist->Position = Ship->Persist->Position + (Ship->Persist->Velocity * TimeSeconds);

	return false;
}

#include "JourneyDockUndock.cpp"
#include "JourneyMovement.cpp"

#include "JourneyGetMethods.cpp"

void ShipStep(void* SelfData, real64 Time, state* State)
{
	ship* Ship = (ship*)SelfData;

	if (Ship->Persist->CurrentJourney.InProgress) {

		journey_step* Step = &Ship->Persist->CurrentJourney.Steps[Ship->Persist->CurrentJourney.CurrentStep];

		if (Ship->Persist->CurrentJourney.CurrentStep < 0) {
			Ship->Persist->CurrentJourney.CurrentStep++;

			Step = &Ship->Persist->CurrentJourney.Steps[Ship->Persist->CurrentJourney.CurrentStep];
			journey_methods::Start(Step->Type)(Ship, Step, State);
		}

		bool32 Finished = (journey_methods::Step(Step->Type)(Ship, Step, Time, State));
		if (Finished) {

			Ship->Persist->CurrentJourney.CurrentStep++;
			Step = &Ship->Persist->CurrentJourney.Steps[Ship->Persist->CurrentJourney.CurrentStep];

			if (Ship->Persist->CurrentJourney.CurrentStep < Ship->Persist->CurrentJourney.StepsCount) {
				journey_methods::Start(Step->Type)(Ship, Step, State);
			} else {

				if (Ship->Persist->CurrentJourney.Repeat) {
					journey::Execute(&Ship->Persist->CurrentJourney);
				} else {
					Ship->Persist->CurrentJourney.InProgress = false;
					Ship->Persist->CurrentJourney.StepsCount = 0;
					Ship->Persist->CurrentJourney.CurrentStep = 0;

					if (Ship->Persist->Status != ship_status::docked) {
						Ship->Persist->Status = ship_status::idle;
					}
				}
			}
		}
	}
}

void ModuleUpdateAsteroidMiner(void* SelfData, real64 Time, state* State)
{
	ship_module* Module = (ship_module*)SelfData;
	ship* Owner = per::GetShip(&Module->Persist->Owner, State);

	WorldTargetClear(&Module->Persist->Target);

	bool32 Skip = false;

	// If no cargo space then do nothing
	if (Owner->Hold.MassCurrent == Owner->Hold.MassLimit) { Skip = true; }

	// Can only work when the ship is idle
	if (Owner->Persist->Status != ship_status::idle) { Skip = true; }

	if (Skip) {
		Module->Persist->ActivationTimerMS = 0.0f;
		return;
	}

	for (int i = 0; i < State->PersistentData.AsteroidClustersCount && !WorldTargetHasTarget(&Module->Persist->Target); i++) {
		asteroid_cluster* Cluster = &State->AsteroidClusters[i];
		for (int a = 0; a < ArrayCount(Cluster->Asteroids) && !WorldTargetHasTarget(&Module->Persist->Target); a++) {
			asteroid* Roid = &Cluster->Asteroids[a];
			if (Roid->Persist->Using) {
				real64 Dist = Vector2Distance(Roid->Persist->WorldObject.Position, Owner->Persist->Position);
				if (Dist < Module->Definition.ActivationRange) {
					WorldTargetSet(&Module->Persist->Target, Roid);
				}
			}
		}
	}

	if (WorldTargetHasTarget(&Module->Persist->Target)) {
		Module->Persist->ActivationTimerMS += Time;
		if (Module->Persist->ActivationTimerMS >= Module->Definition.ActivationTimeMS) {
			Module->Persist->ActivationTimerMS = 0.0f;

			// Do module thing

			asteroid* Roid = WorldTargetGetAsteroid(&Module->Persist->Target, State);

			int Amount = SubtractAvailable(&Roid->Persist->OreCount, 2);
			ItemGive(&Owner->Hold, Roid->Persist->OreItem, Amount);

			if (Roid->Persist->OreCount <= 0) {
				AsteroidDestroy(Roid, State);
			}

		}
	} else {
		Module->Persist->ActivationTimerMS = 0.0f;
	}
}

void ModuleUpdateSalvager(void* SelfData, real64 Time, state* State)
{
	ship_module* Module = (ship_module*)SelfData;
	ship* Owner = per::GetShip(&Module->Persist->Owner, State);

	WorldTargetClear(&Module->Persist->Target);

	bool32 Skip = false;

	// Can only work when the ship is idle
	if (Owner->Persist->Status != ship_status::idle) { Skip = true; }

	if (Skip) {
		Module->Persist->ActivationTimerMS = 0.0f;
		return;
	}

	for (int i = 0; i < State->SalvagesCount && !WorldTargetHasTarget(&Module->Persist->Target); i++) {
		salvage* Sal = &State->Salvages[i];

		real64 Dist = Vector2Distance(Sal->WorldObject.Position, Owner->Persist->Position);
		if (Dist < Module->Definition.ActivationRange) {
			WorldTargetSet(&Module->Persist->Target, Sal);
		}
	}

	if (WorldTargetHasTarget(&Module->Persist->Target)) {
		Module->Persist->ActivationTimerMS += Time;
		if (Module->Persist->ActivationTimerMS >= Module->Definition.ActivationTimeMS) {
			Module->Persist->ActivationTimerMS = 0.0f;

			// Do module thing

			salvage* Sal = WorldTargetGetSalvage(&Module->Persist->Target);
			int Amount = SubtractAvailable(&Sal->KnowledgeAmount, 2);
			State->PersistentData.Knowledge += Amount;
			if (Sal->KnowledgeAmount <= 0) {
				SalvageSpawn(Sal);
			}
		}
	} else {
		Module->Persist->ActivationTimerMS = 0.0f;
	}
}

void OnShipSelected(selection* Sel, engine_state* EngineState, game_input* Input)
{
	state* State = &EngineState->GameState;
	ship* CurrentShip = Sel->GetShip();

	if (CurrentShip->Persist->Status != ship_status::moving) {
		CurrentShip->Persist->CurrentJourney = {};
	}
}

void CreateDockUndockStep(ship* Ship, station* Station)
{
	journey_step* Step = journey::AddStep(&Ship->Persist->CurrentJourney);
	Step->Type = journey_step_type::dock_undock;

	per::SetStation(&Step->DockUndock.Station, Station);
}

void CreateMovementStep(ship* Ship, vector2 EndPos)
{
	journey_step* MovStep = journey::AddStep(&Ship->Persist->CurrentJourney);
	MovStep->Type = journey_step_type::movement;

	MovStep->Movement.EdgeRatio = 0.1f;
	MovStep->Movement.EndPosition = EndPos;
}

void ShipAddModule(ship_module* Dest, ship_module_id ModuleID, ship* Ship, state* State)
{
	Dest->Persist->Filled = true;
	Dest->Persist->Type = ModuleID;

	Dest->Definition = Globals->AssetsList.ShipModuleDefinitions[(int)ModuleID];
	per::SetShip(&Dest->Persist->Owner, Ship);

	RegisterStepper(&Dest->Stepper, Dest->Definition.ActivationStepMethod, (void*)(Dest), State);
}

void ShipRemoveModule(ship_module* Module, state* State)
{
	Module->Persist->Filled = false;
	Module->Definition = {};
	UnregisterStepper(&Module->Stepper, State);
}

void ShipSelected(selection* Sel, engine_state* EngineState, game_input* Input)
{
	state* State = &EngineState->GameState;
	editor_state* EditorState = &EngineState->EditorState;

	ship* CurrentShip = Sel->GetShip();

	vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
	vector2 MouseWorldFlat = vector2{MouseWorld.X, MouseWorld.Y};

	RenderCircle(MouseWorldFlat, vector2{1, 1},
	             COLOR_RED, -1, Globals->GameRenderer);

	bool Showing = true;
	ImGui::Begin("Ship Info", &Showing);

	ImGui::Text("Current Status:");
	ImGui::SameLine();
	ImGui::Text(ship_status_NAME[(int)CurrentShip->Persist->Status].Array());

	ImVec2 window_pos = ImGui::GetWindowPos();
	Sel->Current->InfoWindowPos = vector2{window_pos.x, window_pos.y};

	if (EditorState->EditorMode) {
		if (ImGui::TreeNode("Basic")) {
			// Posititon
			{
				ImGui::PushID("position");
				ImGui::Text("Position");
				ImGui::Columns(2);

				ImGui::Text("x");
				ImGui::SameLine();
				ImGui::Text(string{CurrentShip->Persist->Position.X} .Array());

				ImGui::NextColumn();

				ImGui::Text("y");
				ImGui::SameLine();
				ImGui::Text(string{CurrentShip->Persist->Position.Y} .Array());

				ImGui::Columns(1);
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}

	// Weight
	{
		real64 MassTotal = ShipGetMassTotal(CurrentShip);
		//string ShipWeightDisp = Humanize(MassTotal);
		string ShipWeightDisp = string{MassTotal};

		ImGui::Text("Ship Total Mass (t)");
		ImGui::SameLine();
		ImGui::Text(ShipWeightDisp.Array());
	}

	// Velocity
	{
		string V = Humanize((int64)(Vector2Length(CurrentShip->Persist->Velocity) * UnitToMeters * 1000.0f));
		ImGui::Text("Velocity (kph)");
		ImGui::SameLine();
		ImGui::Text(V.Array());
	}

	ImGui::Dummy(ImVec2(0, 10));

	// Fuel
	{
		real64 FuelCurr = ItemHoldGetFuel(&CurrentShip->FuelTank);

		string FuelDisp = "Fuel Tank (g) " + string{FuelCurr} + "/" + string{CurrentShip->FuelTank.MassLimit};
		ImGui::Text(FuelDisp.Array());

		float Progress = (float)(FuelCurr / CurrentShip->FuelTank.MassLimit);
		ImGui::ProgressBar(Progress);
	}
	ItemDisplayHold("Fuel Tank", &CurrentShip->FuelTank, State, Input,
	                CurrentShip->Persist->Status == ship_status::docked,
	                item_hold_filter::stl
	               );

	ImGui::Dummy(ImVec2(0, 10));

	if (ImGui::CollapsingHeader("Modules")) {
		for (int i = 0; i < CurrentShip->Definition.SlotsCount; i++) {
			ship_module* Module = &CurrentShip->EquippedModules[i];

			if (Module->Persist->Filled != GameNull) {

				loaded_image* Icon = Globals->AssetsList.ShipModuleIcons[(int)Module->Definition.ID];
				ImGui::Image(
				    (ImTextureID)((int64)Icon->GLID),
				    ImGuiImageSize,
				    ImVec2(0, 0),
				    ImVec2(1, -1),
				    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
				    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
				);

				if (CurrentShip->Persist->Status == ship_status::docked &&
				        ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)
				   ) {
					State->ModuleUnequipping = Module;

					int D = 0;
					ImGui::SetDragDropPayload(ImguiShipModuleUnequippingDraggingID, &D, sizeof(D));

					ImGui::Image(
					    (ImTextureID)((int64)Icon->GLID),
					    ImVec2(40, 40),
					    ImVec2(0, 0),
					    ImVec2(1, -1),
					    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
					    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
					);

					ImGui::EndDragDropSource();
				}

				ImGui::SameLine();
				ImGui::BeginGroup();

				ImGui::Text(Module->Definition.DisplayName.Array());
				float Progress = (float)(Module->Persist->ActivationTimerMS / Module->Definition.ActivationTimeMS);
				ImGui::ProgressBar(Progress, ImVec2(-1.0f, 1.0f));

				ImGui::EndGroup();
			} else {

				Assert(ArrayCount(CurrentShip->Definition.SlotTypes) > i);

				ImGui::Image(
				    (ImTextureID)((int64)Globals->AssetsList.ShipModuleTypeIcons[(int)CurrentShip->Definition.SlotTypes[i]]->GLID),
				    ImGuiImageSize,
				    ImVec2(0, 0),
				    ImVec2(1, -1),
				    ImVec4(1.0f, 1.0f, 1.0f, 0.25f),
				    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
				);

				// Equipping ship module
				if (CurrentShip->Persist->Status == ship_status::docked &&
				        ImGui::BeginDragDropTarget()
				   ) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ImguiItemDraggingID)) {
						item_instance_persistent* Inst = State->ItemDragging;
						item_definition* Def = GetItemDefinition(Inst->ID);

						if (Def->IsModule()) {

							ship_module_slot_type DesiredType = Globals->AssetsList.ShipModuleDefinitions[(int)Def->ShipModuleID].SlotType;
							if (DesiredType == CurrentShip->Definition.SlotTypes[i]) {
								// Remove item
								Inst->Count = 0;
								ItemHoldUpdateMass(&CurrentShip->Hold);

								// Add module
								ShipAddModule(Module, Def->ShipModuleID, CurrentShip, State);
							} else {
								//TODO DISPLAY_ERROR
								ConsoleLog("Slots do not match. Display error");
							}
						} else {
							//TODO DISPLAY_ERROR
							ConsoleLog("Item is not module. Display error.");
						}
					}
				}


				ImGui::SameLine();
				ImGui::Text("Empty Slot");
			}

			ImGui::Separator();
		}
	}

	// Cargo
	ItemDisplayHold("Cargo", &CurrentShip->Hold, State, Input,
	                CurrentShip->Persist->Status == ship_status::docked,
	                item_hold_filter::any
	               );

	// Journey
	if (ImGui::CollapsingHeader("Commands")) {
		ship_journey* CurrJour = &CurrentShip->Persist->CurrentJourney;

		bool32 DockState = (CurrentShip->Persist->Status == ship_status::docked);
		station* LastStation = {};
		vector2 JourneyPosCurrent = CurrentShip->Persist->Position;

		for (int i = 0; i < CurrentShip->Persist->CurrentJourney.StepsCount; i++) {

			string id = "COMMAND_" + string{i};
			ImGui::PushID(id.Array());

			journey_step* Step = &CurrentShip->Persist->CurrentJourney.Steps[i];
			switch (Step->Type) {
				case journey_step_type::movement: {
					ImGui::Text("Move");

					// render line
					if (i >= CurrJour->CurrentStep) {
						vector2 Points[2] = {};
						Points[0] = WorldToScreen(vector3{JourneyPosCurrent.X, JourneyPosCurrent.Y, 0}, &EngineState->GameCamera);
						Points[1] = WorldToScreen(vector3{Step->Movement.EndPosition.X, Step->Movement.EndPosition.Y, 0}, &EngineState->GameCamera);
						render_line Line = {};
						Line.Points = Points;
						Line.PointsCount = ArrayCount(Points);
						RenderLine(Line, 1.5f, color{0, 1, 0, 0.2f}, &EngineState->UIRenderer, false);
					}

					JourneyPosCurrent = Step->Movement.EndPosition;
				} break;

				case journey_step_type::dock_undock: {
					JourneyPosCurrent = per::GetStation(&Step->DockUndock.Station, State)->Persist->Position;
					LastStation = per::GetStation(&Step->DockUndock.Station, State);
					if (DockState) {
						ImGui::Text("Undock");
					} else {
						ImGui::Text("Dock");
					}
					DockState = !DockState;
				} break;


				INVALID_DEFAULT
			}

			if (!CurrJour->InProgress && ImGui::Button("-")) {
				RemoveSlideArray((void*)&CurrentShip->Persist->CurrentJourney.Steps[0], CurrentShip->Persist->CurrentJourney.StepsCount, sizeof(CurrentShip->Persist->CurrentJourney.Steps[0]), i);
				CurrentShip->Persist->CurrentJourney.StepsCount--;
			}

			ImGui::Separator();
			ImGui::PopID();
		}

		if (!CurrJour->InProgress) {
			if (ImGui::Button("+ Add Step +")) {
				journey_step* Step = journey::AddStep(&CurrentShip->Persist->CurrentJourney);
				Step->Type = journey_step_type::movement;
			}

			bool r = CurrJour->Repeat;
			ImGui::Checkbox("Return to start and repeat", &r);
			CurrJour->Repeat = r;

			if (ImGui::Button("Execute")) {

				if (CurrJour->Repeat) {

					// This assumes the next step is a movement step. which won't be true when we add more steps
					if (DockState) {
						CreateDockUndockStep(CurrentShip, LastStation);
					}

					CreateMovementStep(CurrentShip, CurrentShip->Persist->Position);
				}

				journey::Execute(CurrJour);
			}

			// Click world to add movement command
			if (Input->MouseLeft.OnUp && CurrentShip->Persist->Status != ship_status::moving && !Input->MouseMoved()) {

				if (State->Hovering == GameNull) {
					if (CurrentShip->Persist->Status == ship_status::docked) {
						CreateDockUndockStep(
								CurrentShip, 
								per::GetStation(&CurrentShip->Persist->StationDocked, State)
						);
						CreateMovementStep(CurrentShip, MouseWorldFlat);
					} else if (DockState) {
						CreateDockUndockStep(CurrentShip, LastStation);
						CreateMovementStep(CurrentShip, MouseWorldFlat);
					} else {
						CreateMovementStep(CurrentShip, MouseWorldFlat);
					}

				} else if (State->Hovering->Type == selection_type::station) {
					station* Station = State->Hovering->GetStation();
					CreateMovementStep(CurrentShip, Station->Persist->Position);
					CreateDockUndockStep(CurrentShip, Station);
				}
			}
		}
	}

	ImGui::End();

	if (!Showing) { Sel->Clear(); }
}

void ShipSetPersist(ship* Ship, ship_persistent* Persist, state* State) {
	Ship->Persist = Persist;

	// Setup modules from the persist
	for (int i = 0; i < ArrayCount(Persist->Modules); i++) {
		Ship->EquippedModules[i].Persist = &Persist->Modules[i];
		Ship->EquippedModules[i].Definition = Globals->AssetsList.ShipModuleDefinitions[(int)Ship->EquippedModules[i].Persist->Type];

//void ShipAddModule(ship_module* Dest, ship_module_id ModuleID, ship* Ship, state* State)

		if (Ship->EquippedModules[i].Persist->Filled) { 
			ship_module* Mod = &Ship->EquippedModules[i];
			RegisterStepper(&Mod->Stepper, Mod->Definition.ActivationStepMethod, (void*)(Mod), State);
		}
	}
}

// Create a new ship
ship* ShipCreate(state* State, ship_id Type) {

	ship* Ship = &State->Ships[State->PersistentData.ShipsCount];
	ShipSetPersist(
			Ship, 
			&State->PersistentData.Ships[State->PersistentData.ShipsCount],
			State
	);

	State->PersistentData.ShipsCount++;
	Assert(ArrayCount(State->PersistentData.Ships) > State->PersistentData.ShipsCount);
	Assert(ArrayCount(State->Ships) > State->PersistentData.ShipsCount);

	Ship->Persist->Type = Type;
	Ship->Persist->Status = ship_status::idle;

	// TODO setup modules
	ShipAddModule(&Ship->EquippedModules[0], ship_module_id::asteroid_miner, Ship, State);
	//ShipAddModule(&Ship->EquippedModules[3], ship_module_id::salvager_i, Ship, State);

	return Ship;
}

// Setup data for exising ship
ship* ShipSetup(ship* Ship, ship_persistent* Persist, state* State)
{
	ShipSetPersist(Ship, Persist, State);

	// Validate module counts
	Assert(ArrayCount(Ship->EquippedModules) == ArrayCount(Persist->Modules));

	Ship->Size = vector2{5, 5};
	Ship->Definition = Globals->AssetsList.ShipDefinitions[(int)Ship->Persist->Type];

	// Setup item holds
	Ship->Hold.Setup(Ship->Definition.HoldMass, &Ship->Persist->ItemHold);
	Ship->FuelTank.Setup(Ship->Definition.FuelTankMassLimit, &Ship->Persist->FuelHold);
	ItemHoldUpdateMass(&Ship->Hold);
	ItemHoldUpdateMass(&Ship->FuelTank);

	RegisterStepper(&Ship->Stepper, &ShipStep, (void*)Ship, State);

	selectable* Sel = RegisterSelectable(selection_type::ship, &Ship->Persist->Position, &Ship->Size, (void*)Ship, State);
	Sel->SelectionUpdate = &ShipSelected;
	Sel->OnSelection = &OnShipSelected;

	ShipUpdateMass(Ship);

	per::AddSource(Ship->Persist->GUID, Ship, State);

	return Ship;
}
