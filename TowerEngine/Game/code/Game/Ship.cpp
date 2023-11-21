

void WorldTargetSet(world_target_persistent* Target, asteroid* Input) { 
	Target->Type = world_target_type::asteroid;
	per::SetAsteroid(&Target->Asteroid, Input);
}

void WorldTargetSet(world_target_persistent* Target, salvage* Input) { 
	Target->Type = world_target_type::salvage;
	per::SetSalvage(&Target->Salvage, Input);
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

salvage* WorldTargetGetSalvage(world_target_persistent* Target, state* State) {
	Assert(Target->Type == world_target_type::salvage);
	return per::GetSalvage(&Target->Salvage, State);
}

vector2 WorldTargetGetPosition(world_target_persistent* Target, state* State) {
	switch (Target->Type) {
		case world_target_type::asteroid: 
			return per::GetAsteroid(&Target->Asteroid, State)->Persist->WorldObject.Position; 
		break;

		case world_target_type::salvage: 
			return per::GetSalvage(&Target->Salvage, State)->Persist->WorldObject.Position;
		break;

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

void ConsumeFuel(item_hold* Hold, r64 FuelAmount) {
	Hold->ConsumeFuel(FuelAmount);
	ItemHoldUpdateMass(Hold);
}

// returns if finished
bool32 ShipSimulateMovement(ship* Ship, journey_movement* Mov, real64 TimeMS, state* State)
{
	real64 TimeSeconds = TimeMS * 0.001f;

	real64 fuelToUse = Mov->CachedFuelToUse;
	real64 fuelForce = Mov->CachedFuelForce;
	vector2 dirToTargetForce = Mov->CachedDirToTargetForce;

	r64 finalFuelForcePerGallon = fuelForcePerGallon;
	finalFuelForcePerGallon += fuelForcePerGallon * TreeBonusesTotal->FuelForce;

	// NOTE reevaluate this caching optimizations.
	// If its enabled then there will be a bug here when fuelForcePerGallon changes.
	// Cache should be invalidated then.

	if (TimeMS != Mov->CachedTime || true) {
		real64 FuelForceFinal = finalFuelForcePerGallon;

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
	if (Mov->FullDistance < DistToEnd + DistToStart) {
		// Stopping, past destination
		Ship->Persist->Velocity = vector2{0, 0};
		return true;
	}

	if (DistToStart < Mov->DistFromSidesToCoast || DistToEnd < Mov->DistFromSidesToCoast) {

		vector2 Force = {};

		// Speed up
		if (DistToStart < Mov->DistFromSidesToCoast) {
			ConsumeFuel(&Ship->FuelTank, fuelToUse); 
			Force = dirToTargetForce;
		}

		// Slow down
		if (DistToEnd < Mov->DistFromSidesToCoast) {
			ConsumeFuel(&Ship->FuelTank, fuelToUse); 
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

ship_journey_estimate ShipEstimateJourney(ship* Ship, state* State) {

	ship_journey_estimate Ret = {};

	ship_persistent DummyPersist = *Ship->Persist;
	item_hold_persistent DummyItemHold = {};

	ship DummyShip = *Ship;
	DummyShip.Persist = &DummyPersist;
	DummyShip.FuelTank.Persist = &DummyItemHold;

	r64 InitialFuel = Ship->FuelTank.GetFuelLevel();
	DummyShip.FuelTank.SetFuelLevel(InitialFuel);

	float SimFPS = 15.0f;
	float TimeStepMS = (1.0f / SimFPS) * 1000.0f;

	for (int i = 0; i < Ship->Persist->CurrentJourney.StepsCount; i++) {
		journey_step* Step = &Ship->Persist->CurrentJourney.Steps[i];

		switch (Step->Type) {
			case journey_step_type::movement: {
				ShipMovementStart(&DummyShip, Step, State);
				while (!ShipSimulateMovement(&DummyShip, &Step->Movement, TimeStepMS, State)) {
					Ret.DurationMS += TimeStepMS;
				}
			} break;

			case journey_step_type::dock_undock: {
				Ret.DurationMS += DockTimeMS;
			} break;
		}
	}
	
	Ret.FuelUsage = InitialFuel - DummyShip.FuelTank.GetFuelLevel();
	return Ret;
}

void ShipStep(void* SelfData, real64 Time, state* State)
{
	ship* Ship = (ship*)SelfData;

	// Update item cargo mod from modules.
	// Maybe do something different so this isn't updated every frame idk
	Ship->Hold.MassLimitMod = 0;
	for (int i = 0; i < ArrayCount(Ship->EquippedModules); i++) {
		if (Ship->EquippedModules[i].Persist != GameNull) {
			Ship->Hold.MassLimitMod += Ship->EquippedModules[i].Definition.CargoAddition;
		}
	}

	// Update ship journey
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
					journey::Clear(&Ship->Persist->CurrentJourney);

					if (Ship->Persist->Status != ship_status::docked) {
						Ship->Persist->Status = ship_status::idle;
					}
				}
			}
		}
	}
}

// Time needed for the module to activate
r64 ModuleGetActivationTime(ship_module_definition* ModDef, ship* Ship) {

	r64 ReductionPerc = 0; 
	if (ModDef->SlotType == ship_module_slot_type::industrial) { 
		ReductionPerc = (r64)ModDef->ActivationTimeMS * (Ship->Definition.Prospector.IndustrialActivationReductionPerc / 100.0f); 
	}

	r64 Val = ModDef->ActivationTimeMS - ReductionPerc; 
	r64 ReductionMS = 0;

	switch (ModDef->SlotType) {
		case ship_module_slot_type::industrial: {

			// skill tree
			ReductionMS += MinutesToMilliseconds(TreeBonusesTotal->IndustrialActivationTimeMinutes);

			// outside influence
			ReductionMS += MinutesToMilliseconds(Ship->IndustrialActivationReductionMinutes);
		}
	}

	Val = Val - ReductionMS;
	Val = ClampValue((r64)0, (r64)ModDef->ActivationTimeMS, (r64)Val);

	return Val;
}

void OnShipSelected(selection* Sel, engine_state* EngineState, game_input* Input)
{
	state* State = &EngineState->GameState;
	ship* CurrentShip = Sel->GetShip();

	if (CurrentShip->Persist->Status == ship_status::idle) {
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

	if (Dest->Definition.ActivationStepMethod != GameNull) {
		RegisterStepper(&Dest->Stepper, Dest->Definition.ActivationStepMethod, (void*)(Dest), State);
	}
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
	ship_journey* CurrJour = &CurrentShip->Persist->CurrentJourney;

	vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
	vector2 MouseWorldFlat = vector2{MouseWorld.X, MouseWorld.Y};

	// Render world effects
	{
		// Module effects
		if (CurrentShip->Persist->Status != ship_status::docked) {
			for (int m = 0; m < ArrayCount(CurrentShip->EquippedModules); m++) {
				ship_module* Module = &CurrentShip->EquippedModules[m];
				if (Module->Persist->Filled) {

					color Col = Module->Definition.ActivationRangeDisplayColor;
					Col.A = 0.5f;

					RenderMeshCircleOutline(CurrentShip->Persist->Position, (r32)Module->Definition.ActivationRange, 0.3f, 64, Col, -2, Globals->GameRenderer);
				}
			}
		}

		// Render journey lines
		{
			vector2 JourneyPosCurrent = CurrentShip->Persist->Position;

			for (int i = 0; i < CurrentShip->Persist->CurrentJourney.StepsCount; i++) {

				journey_step* Step = &CurrentShip->Persist->CurrentJourney.Steps[i];
				switch (Step->Type) {
					case journey_step_type::movement: {

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
					} break;


					INVALID_DEFAULT
				}
			}
		}
	}

	bool Showing = true;

	ImGui::PushID(CurrentShip->Persist->GUID);
	string ID = string{"Ship Info###"} + string{CurrentShip->Persist->GUID};
	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
	ImGui::Begin(ID.Array(), &Showing);

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

	if (!State->CreatingMovement) {

		// Ship info
		{
			ImGui::Columns(2);

			ImGui::Text("Current Status");
			ImGui::NextColumn();

			ImGui::Text(ship_status_NAME[(int)CurrentShip->Persist->Status].Array());
			ImGui::NextColumn();
			
			// Weight
			{
				ImGui::Text("Ship Total Mass (t)");
				ImGui::NextColumn();

				real64 MassTotal = ShipGetMassTotal(CurrentShip);
				//string ShipWeightDisp = Humanize(MassTotal);
				string ShipWeightDisp = string{MassTotal};
				ImGui::Text(ShipWeightDisp.Array());
				ImGui::NextColumn();
			}

			// Velocity
			{
				ImGui::Text("Velocity (kph)");
				ImGui::NextColumn();

				string V = Humanize((int64)(Vector2Length(CurrentShip->Persist->Velocity) * UnitToMeters * 1000.0f));
				ImGui::Text(V.Array());
				ImGui::NextColumn();
			}

			// Hull type
			{
				ImGui::Text("Hull Type");
				ImGui::NextColumn();

				string Disp = CurrentShip->Definition.DisplayName;
				ImGui::Text(Disp.Array());
				ImGui::SameLine();
				if (ImGui::Button("Info")) {
					InfoWindow::Show(CurrentShip->Definition.ID);
				}
				ImGui::NextColumn();
			}

			ImGui::Columns(1);
		}

		ImGui::Dummy(ImVec2(0, 10));

		// Fuel
		{
			real64 FuelCurr = ItemHoldGetFuel(&CurrentShip->FuelTank);

			string FuelDisp = "Fuel Tank (g) " + string{FuelCurr} + "/" + string{CurrentShip->FuelTank.GetMassLimit()};
			ImGui::Text(FuelDisp.Array());

			float Progress = (float)(FuelCurr / CurrentShip->FuelTank.GetMassLimit());
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

					ImGuiItemIcon(Module->Definition.ItemID, false);

					if (CurrentShip->Persist->Status == ship_status::docked &&
							ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)
					   ) {
						State->ModuleUnequipping = Module;

						int D = 0;
						ImGui::SetDragDropPayload(ImguiShipModuleUnequippingDraggingID, &D, sizeof(D));

						loaded_image* Icon = Globals->AssetsList.ShipModuleIcons[(int)Module->Definition.ID];
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
					float Progress = (float)(Module->Persist->ActivationTimerMS / ModuleGetActivationTime(&Module->Definition, CurrentShip));
					ImGui::ProgressBar(Progress, ImVec2(-1.0f, 1.0f));

					if (Module->Definition.ActivationTimeMS > 0) {
						if (Module->Persist->ActivationTimerMS > 0) {
							r64 MSRemaining = ModuleGetActivationTime(&Module->Definition, CurrentShip) - Module->Persist->ActivationTimerMS;
							r64 MinutesRemaining = (r64)MillisecondsToMinutes(MSRemaining);
							ImGui::Text("%.2lf minutes remaining ", MinutesRemaining);
						} else {
 							ImGui::TextColored(ImVec4(1,1,1,0.5f), "idle");
						}
					}

					ImGui::EndGroup();
				} else {

					Assert(ArrayCount(CurrentShip->Definition.SlotTypes) > i);

					ship_module_slot_definition* SlotDef = GetShipModuleSlotDefinition(CurrentShip->Definition.SlotTypes[i]);
					ImGui::Image(
						(ImTextureID)((int64)SlotDef->Icon->GLID),
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

		if (!CurrJour->InProgress) {
			if (ImGui::Button("Move Ship", ImVec2(-1, 0))) {
				State->CreatingMovement = true;
			}
		} else {
			ImGui::Text("Journey in progress");

			ImGui::Separator();

			r64 TimeLeftMS = CurrJour->UniverseTimeEndMS - State->PersistentData.UniverseTimeMS;
			r64 TimeLeftMinutes = MillisecondsToMinutes(TimeLeftMS);
			ImGui::Text("%.2f minutes remaining ", TimeLeftMinutes);

			r64 PercProgress = TimeLeftMS / CurrJour->Estimate.DurationMS;
			ImGui::ProgressBar((float)PercProgress, ImVec2(-1.0f, 1.0f));

			//journey::ImGuiDrawSteps(CurrJour, CurrentShip, State);
		}
	} else {
		// Create movement window

		if (CurrJour->StepsCount == 0) {
			ImGui::TextWrapped("- Click anywhere on map to queue movement command.");
			ImGui::TextWrapped("- Click on a station to move the ship and dock there.");
			ImGui::TextWrapped("- If the ship is already docked, then it will automatically undock before next movement.");
			ImGui::Dummy(ImVec2(0, 20));
		}

		bool32 DockState = (CurrentShip->Persist->Status == ship_status::docked);
		station* LastStation = {};

		ImGui::Columns(2);

		ImGui::Text("Time Estimate"); 
		ImGui::NextColumn();

		ImGui::Text("%.2f (min)", MillisecondsToMinutes(CurrJour->Estimate.DurationMS));
		ImGui::NextColumn();

		ImGui::Text("Fuel Usage");
		ImGui::NextColumn();

		ImGui::Text("%.2f", CurrJour->Estimate.FuelUsage);
		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::Separator();

		journey::ImGuiDrawSteps(CurrJour, CurrentShip, State);
		if (!journey::CanAddSteps(CurrJour)) { 
			string Disp = string{"Limit "} + string{journey::StepsLimit} + string{" steps per journey."} ;
			ImGui::TextColored(ImVec4(1,0,0,1), Disp.Array()); 
		}

		ImGui::Columns(2);
		if (ImGui::Button("Cancel", ImVec2(-1, 0))) {
			State->CreatingMovement = false;
			journey::Clear(CurrJour);
		}
		ImGui::NextColumn();

		// Have a journey set?
		if (CurrJour->StepsCount > 0) {


			// Enough fuel?
			if (CurrentShip->FuelTank.GetFuelLevel() >= CurrJour->Estimate.FuelUsage) {

				// Button clicked?
				if (ImGui::Button("Execute", ImVec2(-1, 0))) { 
					State->CreatingMovement = false;

					if (CurrJour->Repeat) {

						// This assumes the next step is a movement step. which won't be true when we add more steps
						if (DockState) {
							CreateDockUndockStep(CurrentShip, LastStation);
						}

						CreateMovementStep(CurrentShip, CurrentShip->Persist->Position);
					}

					CurrJour->UniverseTimeEndMS = State->PersistentData.UniverseTimeMS + CurrJour->Estimate.DurationMS;
					journey::Execute(CurrJour);
					Save();
				}
			} else {
				ImGui::TextWrapped("Not Enough Fuel");
			}
		}
		ImGui::Columns(1);

		// Click world to add movement command
		if (Input->MouseLeft.OnUp && CurrentShip->Persist->Status != ship_status::moving && !Input->MouseMoved() && 
				journey::CanAddSteps(&CurrentShip->Persist->CurrentJourney)
			) {

			if (State->Hovering != NULL && State->Hovering->Type == selection_type::station) {
				station* Station = State->Hovering->GetStation();
				CreateMovementStep(CurrentShip, Station->Persist->Position);
				CreateDockUndockStep(CurrentShip, Station);
			} else {

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
			}

			CurrJour->Estimate = ShipEstimateJourney(CurrentShip, State);
		}
		
	}

	ImGui::End();
	ImGui::PopID();

	if (!Showing) { Sel->Clear(); }
}

void ShipSetPersist(ship* Ship, ship_persistent* Persist, state* State) {
	Ship->Persist = Persist;

	// Setup modules from the persist
	for (int i = 0; i < ArrayCount(Persist->Modules); i++) {
		Ship->EquippedModules[i].Persist = &Persist->Modules[i];
		Ship->EquippedModules[i].Definition = Globals->AssetsList.ShipModuleDefinitions[(int)Ship->EquippedModules[i].Persist->Type];

		if (Ship->EquippedModules[i].Persist->Filled) { 
			ship_module* Mod = &Ship->EquippedModules[i];
			if (Mod->Definition.ActivationStepMethod != GameNull) {
				RegisterStepper(&Mod->Stepper, Mod->Definition.ActivationStepMethod, (void*)(Mod), State);
			}
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
	Ship->Persist->GUID = PlatformApi.GetGUID();

	ShipAddModule(&Ship->EquippedModules[0], ship_module_id::asteroid_miner, Ship, State);

	return Ship;
}

// Setup data for exising ship
ship* ShipSetup(ship* Ship, ship_persistent* Persist, state* State)
{
	State->ShipsCount++;
	ShipSetPersist(Ship, Persist, State);

	// Validate module counts
	Assert(ArrayCount(Ship->EquippedModules) == ArrayCount(Persist->Modules));

	Ship->Definition = Globals->AssetsList.ShipDefinitions[(int)Ship->Persist->Type];
	Ship->Size = vector2{Ship->Definition.Size, Ship->Definition.Size};

	// Setup item holds
	Ship->Hold.Setup(Ship->Definition.HoldMass, item_hold_type::ship_cargo, &Ship->Persist->ItemHold);
	Ship->FuelTank.Setup(Ship->Definition.FuelTankMassLimit, item_hold_type::fuel_tank, &Ship->Persist->FuelHold);
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
