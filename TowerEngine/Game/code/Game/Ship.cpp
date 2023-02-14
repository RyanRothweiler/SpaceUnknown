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

void ShipMovementStart(ship* Ship, journey_step* JourneyStep, game::state* State)
{
	Ship->Status = ship_status::moving;

	journey_movement* Mov = &JourneyStep->Movement;

	Ship->Velocity = {};
	Mov->StartPosition = Ship->Position;
	Mov->DistFromSidesToCoast = Vector2Distance(Ship->Position, Mov->EndPosition) * 0.5f * Mov->EdgeRatio;
	Mov->DirToEnd = Vector2Normalize(Mov->EndPosition - Ship->Position);
	Mov->FullDistance = Vector2Distance(Mov->EndPosition, Ship->Position) + 1;

	// Update rotation
	vector2 MoveDir = Vector2Normalize(Mov->EndPosition - Ship->Position);
	Ship->Rotation = Vector2AngleBetween(vector2{0, 1}, MoveDir) + PI;
	if (Ship->Position.X < Mov->EndPosition.X) { Ship->Rotation *= -1; }
}

bool32 ShipSimulateMovement(ship* Ship, journey_movement* Mov, real64 TimeMS, game::state* State)
{
	real64 TimeSeconds = TimeMS * 0.001f;

	real64 fuelToUse = Mov->CachedFuelToUse;
	real64 fuelForce = Mov->CachedFuelForce;
	vector2 dirToTargetForce = Mov->CachedDirToTargetForce;
	if (TimeMS != Mov->CachedTime) {
		real64 FuelForceFinal = fuelForcePerGallon + (fuelForcePerGallon * TreeBonusesTotal->FuelForceAddition);

		fuelToUse = Ship->Definition.FuelRateMassPerSecond * TimeSeconds;
		fuelForce = fuelToUse * FuelForceFinal;
		dirToTargetForce = Mov->DirToEnd * fuelForce;

		Mov->CachedTime = TimeMS;
		Mov->CachedFuelForce = fuelForce;
		Mov->CachedFuelToUse = fuelToUse;
		Mov->CachedDirToTargetForce = dirToTargetForce;
	}

	real64 DistToStart = Vector2Distance(Ship->Position, Mov->StartPosition);
	real64 DistToEnd = Vector2Distance(Ship->Position, Mov->EndPosition);

	// close enough
	if (DistToEnd < 0.01f) {
		Ship->Velocity = vector2{0, 0};
		return true;
	}

	// past target
	// Need a 0.1 buffer here because the two distances are equal during the journey, but not always exactly because of rounding errors
	if (Mov->FullDistance < DistToEnd + DistToStart) {
		// Stopping, past destination
		Ship->Velocity = vector2{0, 0};
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
			if (Vector2Length(Ship->Velocity) < 0.001f) {
				Ship->Velocity = vector2{0, 0};
				return true;
			}
		}

		real64 Mass = ShipGetMassTotal(Ship);
		vector2 acceleration = Force / (real64)Mass;
		Ship->Velocity = Ship->Velocity + acceleration;
	} else {

		// If we're simulating, then we can sleep until the ship need to slow down
		// TODO we will need to check for when the simulating ends and wake up then
		if (State->ForwardSimulating) {
			// Figure out when the ship will start slowing down
			real64 CoastDist = Mov->FullDistance - (Mov->DistFromSidesToCoast * 2.0f);
			real64 CoastSpeed = Vector2Length(Ship->Velocity);
			real64 CoastDurationSeconds = CoastDist / CoastSpeed;

			SleepStepper(State, &Ship->Stepper, SecondsToMilliseconds(CoastDurationSeconds));

			// Move ship to the point at which we want it to be when it wakes up. Probably fine maybe?
			vector2 SlowStartingPos = Mov->EndPosition - (Mov->DirToEnd * Mov->DistFromSidesToCoast);
			Ship->Position = SlowStartingPos;
		}
	}

	Ship->Position = Ship->Position + (Ship->Velocity * TimeSeconds);

	return false;
}

bool ShipMovementStep(ship* Ship, journey_step* JourneyStep, real64 Time, game::state* State)
{
	return ShipSimulateMovement(Ship, &JourneyStep->Movement, Time, State);
}

void ShipDockUndockStart(ship* Ship, journey_step* JourneyStep, game::state* State)
{
	if (Ship->Status == ship_status::docked) {
		Ship->Status = ship_status::undocking;
	} else {
		Ship->Status = ship_status::docking;
	}

	JourneyStep->DockUndock.TimeAccum = 0;
}

bool ShipDockUndockStep(ship* Ship, journey_step* JourneyStep, real64 Time, game::state* State)
{
	JourneyStep->DockUndock.TimeAccum += Time;
	Ship->Position = JourneyStep->DockUndock.Station->Position;

	if (JourneyStep->DockUndock.TimeAccum >= SecondsToMilliseconds(60.0f)) {

		if (Ship->Status == ship_status::undocking) {
			StationUndockShip(Ship);
		} else {
			StationDockShip(JourneyStep->DockUndock.Station, Ship);
		}

		return true;
	}
	return false;
}

void ShipStep(void* SelfData, real64 Time, game::state* State)
{
	game::ship* Ship = (game::ship*)SelfData;

	if (Ship->CurrentJourney.InProgress) {

		if (Ship->CurrentJourney.CurrentStep < 0) {
			Ship->CurrentJourney.CurrentStep++;
			Ship->CurrentJourney.Steps[Ship->CurrentJourney.CurrentStep].Start(Ship, &Ship->CurrentJourney.Steps[Ship->CurrentJourney.CurrentStep], State);
		}

		bool32 Finished = Ship->CurrentJourney.Steps[Ship->CurrentJourney.CurrentStep].Step(Ship, &Ship->CurrentJourney.Steps[Ship->CurrentJourney.CurrentStep], Time, State);
		if (Finished) {
			Ship->CurrentJourney.CurrentStep++;
			if (Ship->CurrentJourney.CurrentStep < Ship->CurrentJourney.StepsCount) {
				Ship->CurrentJourney.Steps[Ship->CurrentJourney.CurrentStep].Start(Ship, &Ship->CurrentJourney.Steps[Ship->CurrentJourney.CurrentStep], State);
			} else {

				if (Ship->CurrentJourney.Repeat) {
					Ship->CurrentJourney.Execute();
				} else {
					Ship->CurrentJourney.InProgress = false;
					Ship->CurrentJourney.StepsCount = 0;
					Ship->CurrentJourney.CurrentStep = 0;

					if (Ship->Status != ship_status::docked) {
						Ship->Status = ship_status::idle;
					}
				}
			}
		}
	}
}

void ModuleUpdateAsteroidMiner(void* SelfData, real64 Time, game::state* State)
{
	ship_module* Module = (ship_module*)SelfData;

	Module->Target.Clear();

	bool32 Skip = false;

	// If no cargo space then do nothing
	if (Module->Owner->Hold.MassCurrent == Module->Owner->Hold.MassLimit) { Skip = true; }

	// Can only work when the ship is idle
	if (Module->Owner->Status != ship_status::idle) { Skip = true; }

	if (Skip) {
		Module->ActivationTimerMS = 0.0f;
		return;
	}

	for (int i = 0; i < State->ClustersCount && !Module->Target.HasTarget(); i++) {
		asteroid_cluster* Cluster = &State->Asteroids[i];
		for (int a = 0; a < ArrayCount(Cluster->Asteroids) && !Module->Target.HasTarget(); a++) {
			asteroid* Roid = &Cluster->Asteroids[a];
			if (Roid->Using) {
				real64 Dist = Vector2Distance(Roid->WorldObject.Position, Module->Owner->Position);
				if (Dist < Module->Definition.ActivationRange) {
					Module->Target.Set(Roid);
				}
			}
		}
	}

	if (Module->Target.HasTarget()) {
		Module->ActivationTimerMS += Time;
		if (Module->ActivationTimerMS >= Module->Definition.ActivationTimeMS) {
			Module->ActivationTimerMS = 0.0f;

			// Do module thing

			asteroid* Roid = Module->Target.GetAsteroid();

			int Amount = SubtractAvailable(&Roid->OreCount, 2);
			ItemGive(&Module->Owner->Hold, Roid->OreItem, Amount);

			if (Roid->OreCount <= 0) {
				AsteroidDestroy(Roid, State);
			}

		}
	} else {
		Module->ActivationTimerMS = 0.0f;
	}
}

void ModuleUpdateSalvager(void* SelfData, real64 Time, game::state* State)
{
	ship_module* Module = (ship_module*)SelfData;

	Module->Target.Clear();

	bool32 Skip = false;

	// Can only work when the ship is idle
	if (Module->Owner->Status != ship_status::idle) { Skip = true; }

	if (Skip) {
		Module->ActivationTimerMS = 0.0f;
		return;
	}

	for (int i = 0; i < State->SalvagesCount && !Module->Target.HasTarget(); i++) {
		salvage* Sal = &State->Salvages[i];

		real64 Dist = Vector2Distance(Sal->WorldObject.Position, Module->Owner->Position);
		if (Dist < Module->Definition.ActivationRange) {
			Module->Target.Set(Sal);
		}
	}

	if (Module->Target.HasTarget()) {
		Module->ActivationTimerMS += Time;
		if (Module->ActivationTimerMS >= Module->Definition.ActivationTimeMS) {
			Module->ActivationTimerMS = 0.0f;

			// Do module thing

			int Amount = SubtractAvailable(&Module->Target.GetSalvage()->KnowledgeAmount, 2);
			State->Knowledge += Amount;
			if (Module->Target.GetSalvage()->KnowledgeAmount <= 0) {
				SalvageSpawn(Module->Target.GetSalvage());
			}
		}
	} else {
		Module->ActivationTimerMS = 0.0f;
	}
}

void OnShipSelected(selection* Sel, engine_state* EngineState, game_input* Input)
{
	game::state* State = &EngineState->GameState;
	ship* CurrentShip = Sel->GetShip();

	if (!CurrentShip->IsMoving) {
		CurrentShip->CurrentJourney = {};
	}
}

void CreateDockUndockStep(ship* Ship, station* Station)
{
	journey_step* Step = Ship->CurrentJourney.AddStep();
	Step->Type = journey_step_type::dock_undock;

	Step->Start = &ShipDockUndockStart;
	Step->Step = &ShipDockUndockStep;

	Step->DockUndock.Station = Station;
}

void CreateMovementStep(ship* Ship, vector2 EndPos)
{
	journey_step* MovStep = Ship->CurrentJourney.AddStep();
	MovStep->Type = journey_step_type::movement;

	MovStep->Start = &ShipMovementStart;
	MovStep->Step = &ShipMovementStep;

	MovStep->Movement.EdgeRatio = 0.1f;
	MovStep->Movement.EndPosition = EndPos;
}

void ShipAddModule(ship_module* Dest, ship_module_id ModuleID, ship* Ship, game::state* State)
{
	Dest->Filled = true;
	Dest->Definition = Globals->AssetsList.ShipModuleDefinitions[(int)ModuleID];
	Dest->Owner = Ship;

	game::RegisterStepper(&Dest->Stepper, Dest->Definition.ActivationStepMethod, (void*)(Dest), State);
}

void ShipRemoveModule(ship_module* Module, game::state* State)
{
	Module->Filled = false;
	Module->Definition = {};
	game::UnregisterStepper(&Module->Stepper, State);
}

void ShipSelected(selection* Sel, engine_state* EngineState, game_input* Input)
{
	game::state* State = &EngineState->GameState;
	game::editor_state* EditorState = &EngineState->EditorState;

	ship* CurrentShip = Sel->GetShip();

	vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
	vector2 MouseWorldFlat = vector2{MouseWorld.X, MouseWorld.Y};

	RenderCircle(MouseWorldFlat, vector2{1, 1},
	             COLOR_RED, -1, Globals->GameRenderer);

	bool Showing = true;
	ImGui::Begin("Ship Info", &Showing);

	ImGui::Text("Current Status:");
	ImGui::SameLine();
	ImGui::Text(ship_status_NAME[(int)CurrentShip->Status].Array());

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
				ImGui::Text(string{CurrentShip->Position.X} .Array());

				ImGui::NextColumn();

				ImGui::Text("y");
				ImGui::SameLine();
				ImGui::Text(string{CurrentShip->Position.Y} .Array());

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
		string V = Humanize((int64)(Vector2Length(CurrentShip->Velocity) * UnitToMeters * 1000.0f));
		ImGui::Text("Velocity (kph)");
		ImGui::SameLine();
		ImGui::Text(V.Array());
	}

	ImGui::Dummy(ImVec2(0, 10));

	// Fuel
	{
		string FuelDisp = "Fuel Tank (g) " + string{CurrentShip->FuelTank.GetFuel()} + "/" + string{CurrentShip->FuelTank.MassLimit};
		ImGui::Text(FuelDisp.Array());
		float Progress = (float)(CurrentShip->FuelTank.GetFuel() / CurrentShip->FuelTank.MassLimit);
		ImGui::ProgressBar(Progress);
	}
	ItemDisplayHold("Fuel Tank", &CurrentShip->FuelTank, State, Input,
	                CurrentShip->Status == ship_status::docked,
	                item_hold_filter::stl
	               );

	ImGui::Dummy(ImVec2(0, 10));

	if (ImGui::CollapsingHeader("Modules")) {
		for (int i = 0; i < CurrentShip->Definition.SlotsCount; i++) {
			ship_module* Module = &CurrentShip->EquippedModules[i];

			if (Module->Filled != GameNull) {

				loaded_image* Icon = Globals->AssetsList.ShipModuleIcons[(int)Module->Definition.ID];
				ImGui::Image(
				    (ImTextureID)((int64)Icon->GLID),
				    ImGuiImageSize,
				    ImVec2(0, 0),
				    ImVec2(1, -1),
				    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
				    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
				);

				if (CurrentShip->Status == ship_status::docked &&
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
				float Progress = (float)(Module->ActivationTimerMS / Module->Definition.ActivationTimeMS);
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
				if (CurrentShip->Status == ship_status::docked &&
				        ImGui::BeginDragDropTarget()
				   ) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ImguiItemDraggingID)) {
						item_instance* Inst = State->ItemDragging;

						if (Inst->Definition.IsModule()) {

							ship_module_slot_type DesiredType = Globals->AssetsList.ShipModuleDefinitions[(int)Inst->Definition.ShipModuleID].SlotType;
							if (DesiredType == CurrentShip->Definition.SlotTypes[i]) {
								// Remove item
								Inst->Count = 0;
								CurrentShip->Hold.UpdateMass();

								// Add module
								ShipAddModule(Module, Inst->Definition.ShipModuleID, CurrentShip, State);
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
	                CurrentShip->Status == ship_status::docked,
	                item_hold_filter::any
	               );

	// Journey
	if (ImGui::CollapsingHeader("Commands")) {
		ship_journey* CurrJour = &CurrentShip->CurrentJourney;

		bool32 DockState = (CurrentShip->Status == ship_status::docked);
		station* LastStation = {};
		vector2 JourneyPosCurrent = CurrentShip->Position;

		for (int i = 0; i < CurrentShip->CurrentJourney.StepsCount; i++) {

			string id = "COMMAND_" + string{i};
			ImGui::PushID(id.Array());

			journey_step* Step = &CurrentShip->CurrentJourney.Steps[i];
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
					JourneyPosCurrent = Step->DockUndock.Station->Position;
					LastStation = Step->DockUndock.Station;
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
				RemoveSlideArray((void*)&CurrentShip->CurrentJourney.Steps[0], CurrentShip->CurrentJourney.StepsCount, sizeof(CurrentShip->CurrentJourney.Steps[0]), i);
				CurrentShip->CurrentJourney.StepsCount--;
			}

			ImGui::Separator();
			ImGui::PopID();
		}

		if (!CurrJour->InProgress) {
			if (ImGui::Button("+ Add Step +")) {
				journey_step* Step = CurrentShip->CurrentJourney.AddStep();
				Step->Type = journey_step_type::movement;
			}
			ImGui::Checkbox("Return to start and repeat", &CurrJour->Repeat);
			if (ImGui::Button("Execute")) {

				if (CurrJour->Repeat) {

					// This assumes the next step is a movement step. which won't be true when we add more steps
					if (DockState) {
						CreateDockUndockStep(CurrentShip, LastStation);
					}

					CreateMovementStep(CurrentShip, CurrentShip->Position);
				}

				CurrJour->Execute();
			}

			// Click world to add movement command
			if (Input->MouseLeft.OnUp && !CurrentShip->IsMoving && !Input->MouseMoved()) {

				if (State->Hovering == GameNull) {
					if (CurrentShip->Status == ship_status::docked) {
						CreateDockUndockStep(CurrentShip, CurrentShip->StationDocked);
						CreateMovementStep(CurrentShip, MouseWorldFlat);
					} else if (DockState) {
						CreateDockUndockStep(CurrentShip, LastStation);
						CreateMovementStep(CurrentShip, MouseWorldFlat);
					} else {
						CreateMovementStep(CurrentShip, MouseWorldFlat);
					}

				} else if (State->Hovering->Type == selection_type::station) {
					station* Station = State->Hovering->GetStation();
					CreateMovementStep(CurrentShip, Station->Position);
					CreateDockUndockStep(CurrentShip, Station);
				}
			}
		}
	}

	ImGui::End();

	if (!Showing) { Sel->Clear(); }
}

game::ship* ShipSetup(vector2 Pos, ship_id ID, game::state * State)
{
	for (int i = 0; i < ArrayCount(State->Ships); i++) {
		game::ship* Ship = &State->Ships[i];
		if (!Ship->Using) {

			Ship->Status = ship_status::idle;

			Ship->Using = true;
			Ship->Position = Pos;
			Ship->Size = vector2{5, 5};
			Ship->Definition = Globals->AssetsList.ShipDefinitions[(int)ID];

			Ship->Hold.Setup(Ship->Definition.HoldMass);
			Ship->FuelTank.Setup(Ship->Definition.FuelTankMassLimit);

			ShipAddModule(&Ship->EquippedModules[0], ship_module_id::asteroid_miner, Ship, State);
			ShipAddModule(&Ship->EquippedModules[3], ship_module_id::salvager_i, Ship, State);

			RegisterStepper(&Ship->Stepper, &ShipStep, (void*)Ship, State);

			selectable* Sel = RegisterSelectable(selection_type::ship, &Ship->Position, &Ship->Size, (void*)Ship, State);
			Sel->SelectionUpdate = &ShipSelected;
			Sel->OnSelection = &OnShipSelected;

			ShipUpdateMass(Ship);
			return Ship;
		}
	}

	return GameNull;
}