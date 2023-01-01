void ShipUpdateMass(ship* Ship)
{
	Ship->CurrentCargoMass = 0;

	// Add cargo weightt
	for (int i = 0; i < ArrayCount(Ship->Cargo); i++) {
		if (Ship->Cargo[i].Count > 0) {
			Ship->CurrentCargoMass += Ship->Cargo[i].Definition.Mass * Ship->Cargo[i].Count;
		}
	}

	Ship->CurrentMassTotal = Ship->CurrentCargoMass + Ship->Definition.Mass;
}

bool32 ShipSimulateMovement(ship* Ship, real64 TimeMS)
{
	/*
	real64 TimeSeconds = TimeMS * 0.001f;

	real64 fuelToUse = Ship->Definition.FuelRateGallonsPerSecond * TimeSeconds;
	real64 fuelForce = fuelToUse * fuelForcePerGallon;

	real64 DistToEnd = Vector2Distance(Ship->Position, Ship->CurrentJourney.EndPosition);
	real64 DistToStart = Vector2Distance(Ship->Position, Ship->CurrentJourney.StartPosition);

	// close enough
	if (DistToEnd < 0.01f) {
		Ship->Velocity = vector2{0, 0};
		return false;
	}

	// past target
	// Need a 0.1 buffer here because the two distances are equal during the journey, but not always exactly because of rounding errors
	real64 D = Vector2Distance(Ship->CurrentJourney.StartPosition, Ship->CurrentJourney.EndPosition) + 1;
	real64 T = DistToEnd + DistToStart;
	if (D < T) {
		// Stopping, past destination
		Ship->Velocity = vector2{0, 0};
		return false;
	}

	vector2 Force = {};
	vector2 DirToTargetForce = Ship->CurrentJourney.DirToEnd * fuelForce;

	// Speed up
	if (DistToStart < Ship->CurrentJourney.DistFromSidesToCoast) {
		Ship->FuelGallons -= fuelToUse;
		Force = DirToTargetForce;
	}

	// Slow down
	if (DistToEnd < Ship->CurrentJourney.DistFromSidesToCoast) {
		Ship->FuelGallons -= fuelToUse;
		Force = DirToTargetForce * -1.0f;

		// slow enough
		if (Vector2Length(Ship->Velocity) < 0.001f) {
			Ship->Velocity = vector2{0, 0};
			return false;
		}
	}

	// Get cargo mass too
	int64 Mass = Ship->CurrentMassTotal;

	if (Force.X != 0 || Force.Y != 0) {
		vector2 acceleration = Force / (real64)Mass;
		Ship->Velocity = Ship->Velocity + acceleration;
	}
	Ship->Position = Ship->Position + (Ship->Velocity * TimeSeconds);
	*/

	return true;
}

void ShipStep(void* SelfData, real64 Time, game::state* State)
{
	game::ship* Ship = (game::ship*)SelfData;
	if (Ship->IsMoving) {
		Ship->IsMoving = ShipSimulateMovement(Ship, Time);

		if (!Ship->IsMoving) {
			Ship->CurrentJourney = {};
		}
	}
}

void ShipMove(ship* Ship, ship_journey Journey)
{
	Ship->IsMoving = true;
	Ship->CurrentJourney = Journey;

	/*
	Ship->CurrentJourney.DistFromSidesToCoast =
	    Vector2Distance(Ship->Position, Ship->CurrentJourney.EndPosition) * 0.5f * Journey.EdgeRatio;

	Ship->CurrentJourney.DirToEnd = Vector2Normalize(Journey.EndPosition - Ship->Position);

	// Update rotation
	vector2 MoveDir = Vector2Normalize(Ship->CurrentJourney.EndPosition - Ship->Position);
	Ship->Rotation = Vector2AngleBetween(vector2{0, 1}, MoveDir) + PI;
	if (Ship->Position.X < Ship->CurrentJourney.EndPosition.X) { Ship->Rotation *= -1; }
	*/
}

// Add item to a stack without exceeding the cargo mass limit
void ShipStackGive(ship* Ship, item_instance* Inst, item_definition Def, int32 Count)
{
	int64 NewMass = Ship->CurrentCargoMass + (Def.Mass * Count);
	if (NewMass <= Ship->Definition.CargoMassLimit) {
		Inst->Count += Count;
	} else {
		int64 MassAvail = Ship->Definition.CargoMassLimit - Ship->CurrentCargoMass;
		int32 CountCanGiv = (int32)(MassAvail / Def.Mass);
		Inst->Count += CountCanGiv;
	}
}

void ShipGiveItem(ship* Ship, item_id ItemID, int32 Count)
{
	item_definition Def = GetItemDefinition(ItemID);

	if (Def.Stackable) {
		// Add to existing stack
		for (int i = 0; i < ArrayCount(Ship->Cargo); i++) {
			if (Ship->Cargo[i].Count > 0 && Ship->Cargo[i].Definition.ID == ItemID) {
				ShipStackGive(Ship, &Ship->Cargo[i], Def, Count);
				goto end;
			}
		}

		// Make new stack
		for (int i = 0; i < ArrayCount(Ship->Cargo); i++) {
			if (Ship->Cargo[i].Count <= 0) {

				Ship->Cargo[i].Count = 0;
				Ship->Cargo[i].Definition = Def;
				ShipStackGive(Ship, &Ship->Cargo[i], Def, Count);
				goto end;
			}
		}

		ConsoleLog("Ship cargo full");
		goto end;
	}

	// Not stackable, so make new stacks
	{
		// Verify we have space
		if (Ship->CurrentCargoMass + Def.Mass > Ship->Definition.CargoMassLimit) { return; }

		// Give
		for (int c = 0; c < Count; c++) {
			for (int i = 0; i < ArrayCount(Ship->Cargo); i++) {
				if (Ship->Cargo[i].Count <= 0) {
					Ship->Cargo[i].Count = 1;
					Ship->Cargo[i].Definition = Def;
					goto end;
				}
			}
		}
	}

end:
	ShipUpdateMass(Ship);
}

void ModuleUpdate(void* SelfData, real64 Time, game::state* State)
{
	ship_module* Module = (ship_module*)SelfData;

	Module->Target = GameNull;

	// if no cargo space then do nothing
	if (Module->Owner->CurrentCargoMass == Module->Owner->Definition.CargoMassLimit) { return; }

	for (int i = 0; i < State->ClustersCount && Module->Target == GameNull; i++) {
		asteroid_cluster* Cluster = &State->Asteroids[i];
		for (int a = 0; a < ArrayCount(Cluster->Asteroids) && Module->Target == GameNull; a++) {
			asteroid* Roid = &Cluster->Asteroids[a];
			if (Roid->Using) {
				real64 Dist = Vector2Distance(Roid->Position, Module->Owner->Position);
				if (Dist < Module->Definition.ActivationRange) {
					Module->Target = Roid;
				}
			}
		}
	}

	if (Module->Target != GameNull) {
		Module->ActivationTimerMS += Time;
		if (Module->ActivationTimerMS >= Module->Definition.ActivationTimeMS) {
			Module->ActivationTimerMS = 0.0f;
			Module->Target->Using = false;

			// Do module thing
			ShipGiveItem(Module->Owner, item_id::venigen, 2);
		}
	} else {
		Module->ActivationTimerMS = 0.0f;
	}
}

void OnShipSelected(engine_state* EngineState, game_input* Input)
{
	game::state* State = &EngineState->GameState;
	ship* CurrentShip = State->Selection.GetShip();

	if (!CurrentShip->IsMoving) {
		CurrentShip->CurrentJourney = {};
	}
}

void ShipSelected(engine_state* EngineState, game_input* Input)
{
	game::state* State = &EngineState->GameState;
	game::editor_state* EditorState = &EngineState->EditorState;

	ship* CurrentShip = State->Selection.GetShip();

	vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
	vector2 MouseWorldFlat = vector2{MouseWorld.X, MouseWorld.Y};

	RenderCircle(MouseWorldFlat, vector2{1, 1},
	             COLOR_RED, -1, Globals->GameRenderer);

	// ship current movement line
	/*
	if (CurrentShip->IsMoving) {
		vector2 Points[2] = {};
		Points[0] = WorldToScreen(vector3{CurrentShip->CurrentJourney.EndPosition.X, CurrentShip->CurrentJourney.EndPosition.Y, 0}, &EngineState->GameCamera);
		Points[1] = WorldToScreen(vector3{CurrentShip->Position.X, CurrentShip->Position.Y, 0}, &EngineState->GameCamera);
		render_line Line = {};
		Line.Points = Points;
		Line.PointsCount = ArrayCount(Points);
		RenderLine(Line, 2.0f, color{56.0f / 255.0f, 255.0f / 255.0f, 248.0f / 255.0f, 0.5f}, &EngineState->UIRenderer, false);
	}
	*/

	bool Showing = true;
	ImGui::Begin("Ship Info", &Showing);

	ImVec2 window_pos = ImGui::GetWindowPos();
	State->Selection.Current->InfoWindowPos = vector2{window_pos.x, window_pos.y};

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
		string ShipWeightDisp = Humanize(CurrentShip->CurrentMassTotal);

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
		string FuelDisp = "Fuel Tank (g) " + string{CurrentShip->FuelGallons} + "/" + string{CurrentShip->Definition.FuelTankGallons};
		ImGui::Text(FuelDisp.Array());
		float Progress = (float)(CurrentShip->FuelGallons / CurrentShip->Definition.FuelTankGallons);
		ImGui::ProgressBar(Progress);

	}

	ImGui::Dummy(ImVec2(0, 10));

	if (ImGui::CollapsingHeader("Modules")) {
		for (int i = 0; i < CurrentShip->ModulesCount; i++) {
			ship_module* Module = &CurrentShip->Modules[i];

			ImGui::Text(Module->Definition.DisplayName.Array());
			float Progress = (float)(Module->ActivationTimerMS / Module->Definition.ActivationTimeMS);
			ImGui::ProgressBar(Progress, ImVec2(-1.0f, 1.0f));
		}
	}

	// Cargo
	{
		int64 CargoWeight = (int64)CurrentShip->CurrentCargoMass;
		string CargoTitle = "Cargo (" + string{CargoWeight} + "/" + string{(int64)CurrentShip->Definition.CargoMassLimit} + ")(t)###CARGO";
		if (ImGui::CollapsingHeader(CargoTitle.Array())) {
			for (int i = 0; i < ArrayCount(CurrentShip->Cargo); i++) {
				item_instance* Item = &CurrentShip->Cargo[i];
				if (Item->Count > 0) {
					ImGui::Text(Item->Definition.DisplayName.Array());
					ImGui::SameLine();
					ImGui::Text("x");
					ImGui::SameLine();
					ImGui::Text(string{Item->Count} .Array());
				}
			}
		}
	}

	// Journey
	if (ImGui::CollapsingHeader("Commands")) {
		for (int i = 0; i < CurrentShip->CurrentJourney.StepsCount; i++) {
			journey_step* Step = &CurrentShip->CurrentJourney.Steps[i];
			switch (Step->Type) {
				case journey_step_type::movement: {
					ImGui::Text("Movement Step");
				} break;
				INVALID_DEFAULT
			}
			ImGui::Separator();
		}
		if (ImGui::Button("+ Add Step +")) {
			journey_step* Step = CurrentShip->CurrentJourney.AddStep();
			Step->Type = journey_step_type::movement;
		}

		// Click world to add movement command
		if (Input->MouseLeft.OnUp && !CurrentShip->IsMoving && !Input->MouseMoved()) {
			journey_step* Step = CurrentShip->CurrentJourney.AddStep();
			Step->Type = journey_step_type::movement;


			/*
			CurrentShip->CurrentJourney.EdgeRatio = 0.1f;
			CurrentShip->CurrentJourney.StartPosition = CurrentShip->Position;
			CurrentShip->CurrentJourney.EndPosition = MouseWorldFlat;
			*/
		}
	}

	// Journey / movement stuff
	/*
	{
		static bool32 DoCalc = false;

		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 10));
		ImGui::Text("Issue Movement Command");

		if (
		    !CurrentShip->IsMoving &&
		    CurrentShip->CurrentJourney.EndPosition.X != 0 &&
		    CurrentShip->CurrentJourney.EndPosition.Y != 0
		) {


			// Line to taget destination
			{
				vector2 Points[2] = {};
				Points[0] = WorldToScreen(vector3{CurrentShip->CurrentJourney.EndPosition.X, CurrentShip->CurrentJourney.EndPosition.Y, 0}, &EngineState->GameCamera);
				Points[1] = WorldToScreen(vector3{CurrentShip->Position.X, CurrentShip->Position.Y, 0}, &EngineState->GameCamera);
				render_line Line = {};
				Line.Points = Points;
				Line.PointsCount = ArrayCount(Points);
				RenderLine(Line, 1.5f, color{0, 1, 0, 0.2f}, &EngineState->UIRenderer, false);
			}

			// Journey settings
			{
				static float DurationMS = 0.0f;
				static real64 FuelUsage = 0.0f;

				if (ImGui::SliderFloat("Fuel Usage", &CurrentShip->CurrentJourney.EdgeRatio, 0.1f, 1.0f, "%.2f")) {
					DoCalc = true;
				}

				if (DoCalc) {
					DoCalc = false;

					DurationMS = 0.0f;

					float SimFPS = 30.0f;
					float TimeStepMS = 1.0f / SimFPS;

					vector2 PosOrig = CurrentShip->Position;
					real64 FuelOrig = CurrentShip->FuelGallons;
					real64 RotOrig = CurrentShip->Rotation;
					CurrentShip->Velocity = {};
					ShipMove(CurrentShip, CurrentShip->CurrentJourney);

					while (ShipSimulateMovement(CurrentShip, TimeStepMS)) {
						DurationMS += TimeStepMS;
					}

					FuelUsage = FuelOrig - CurrentShip->FuelGallons;

					CurrentShip->Position = PosOrig;
					CurrentShip->Velocity = {};
					CurrentShip->FuelGallons = FuelOrig;
					CurrentShip->Rotation = RotOrig;
					CurrentShip->IsMoving = false;
				}

				float DurationMinutes = DurationMS / 1000.0f / 60.0f;

				ImGui::Text("Journey Minutes");
				ImGui::SameLine();
				ImGui::Text(string{DurationMinutes} .Array());

				ImGui::Text("Fuel Usage");
				ImGui::SameLine();
				ImGui::Text(string{FuelUsage} .Array());

				if (FuelUsage < CurrentShip->FuelGallons) {
					if (ImGui::Button("Execute Movement", ImVec2(-1.0f, 0.0f))) {
						SaveGame(State);
						ShipMove(CurrentShip, CurrentShip->CurrentJourney);
					}
				} else {
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Not Enough Fuel");
				}
			}
		} else {
			ImGui::Text("Click world to set target destination");
		}

		if (Input->MouseLeft.OnUp && !CurrentShip->IsMoving && !Input->MouseMoved()) {
			DoCalc = true;

			CurrentShip->CurrentJourney.EdgeRatio = 0.1f;
			CurrentShip->CurrentJourney.StartPosition = CurrentShip->Position;
			CurrentShip->CurrentJourney.EndPosition = MouseWorldFlat;
		}
	}
	*/

	ImGui::End();

	if (!Showing) { State->Selection.Clear(); }
}

game::ship* ShipSetup(game::state* State, vector2 Pos)
{
	for (int i = 0; i < ArrayCount(State->Ships); i++) {
		game::ship* Ship = &State->Ships[i];
		if (!Ship->Using) {

			Ship->Using = true;
			Ship->Position = Pos;
			Ship->Size = vector2{5, 5};
			Ship->Definition = Globals->AssetsList.Definition_Ship_First;
			Ship->FuelGallons = Ship->Definition.FuelTankGallons;

			Ship->Modules[0].Definition = Globals->AssetsList.Definition_Module_AsteroidMiner;
			Ship->Modules[0].Owner = Ship;
			game::RegisterStepper(&Ship->Modules[0].Stepper, &ModuleUpdate, (void*)(&Ship->Modules[0]), State);
			Ship->ModulesCount++;

			game::RegisterStepper(&Ship->Stepper, &ShipStep, (void*)Ship, State);
			game::RegisterSelectable(selection_type::ship, &Ship->Position, &Ship->Size, (void*)Ship, State,
			                         &ShipSelected, &OnShipSelected
			                        );

			ShipUpdateMass(Ship);
			return Ship;
		}
	}

	return GameNull;
}