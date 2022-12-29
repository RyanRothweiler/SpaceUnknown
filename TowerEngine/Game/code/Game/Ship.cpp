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

bool32 ShipSimulateMovement(ship* Ship, vector2 TargetPos, real64 TimeMS)
{
	real64 TimeSeconds = TimeMS * 0.001f;

	real64 fuelToUse = Ship->Definition.FuelRateGallonsPerSecond * TimeSeconds;
	real64 fuelForce = fuelToUse * fuelForcePerGallon;

	real64 DistToEnd = Vector2Distance(Ship->Position, TargetPos);
	real64 DistToStart = Vector2Distance(Ship->Position, Ship->CurrentJourney.StartPosition);

	// close enough
	if (DistToEnd < 0.01f) {
		Ship->Velocity = vector2{0, 0};
		return false;
	}

	// past target
	// Need a 0.1 buffer here because the two distances are equal during the journey, but not always exactly because of rounding errors
	if (
	    Vector2Distance(Ship->CurrentJourney.StartPosition, TargetPos) + 0.001f
	    <
	    (DistToEnd + DistToStart)
	) {
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

	vector2 acceleration = Force / (real64)Mass;
	Ship->Velocity = Ship->Velocity + acceleration;
	Ship->Position = Ship->Position + (Ship->Velocity * TimeSeconds);

	return true;
}

void ShipStep(void* SelfData, real64 Time, game::state* State)
{
	game::ship* Ship = (game::ship*)SelfData;
	if (Ship->IsMoving) {
		Ship->IsMoving = ShipSimulateMovement(Ship, Ship->CurrentJourney.EndPosition, Time);

		if (!Ship->IsMoving) {
			Ship->CurrentJourney = {};
		}
	}
}

void ShipMove(ship* Ship, ship_journey Journey)
{
	Ship->IsMoving = true;
	Ship->CurrentJourney = Journey;

	Ship->CurrentJourney.DistFromSidesToCoast =
	    Vector2Distance(Ship->Position, Ship->CurrentJourney.EndPosition) * 0.5f * Journey.EdgeRatio;

	Ship->CurrentJourney.DirToEnd = Vector2Normalize(Journey.EndPosition - Ship->Position);

	// Update rotation
	vector2 MoveDir = Vector2Normalize(Ship->CurrentJourney.EndPosition - Ship->Position);
	Ship->Rotation = Vector2AngleBetween(vector2{0, 1}, MoveDir) + PI;
	if (Ship->Position.X < Ship->CurrentJourney.EndPosition.X) { Ship->Rotation *= -1; }
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

game::ship* ShipSetup(game::state* State, vector2 Pos)
{
	for (int i = 0; i < ArrayCount(State->Ships); i++) {
		game::ship* Ship = &State->Ships[i];
		if (!Ship->Using) {

			Ship->Using = true;
			Ship->Position = Pos;
			Ship->Size = vector2{6, 6};
			Ship->Definition = Globals->AssetsList.Definition_Ship_First;
			Ship->FuelGallons = Ship->Definition.FuelTankGallons;

			Ship->Modules[0].Definition = Globals->AssetsList.Definition_Module_AsteroidMiner;
			Ship->Modules[0].Owner = Ship;
			game::RegisterStepper(&Ship->Modules[0].Stepper, &ModuleUpdate, (void*)(&Ship->Modules[0]), State);
			Ship->ModulesCount++;

			game::RegisterStepper(&Ship->Stepper, &ShipStep, (void*)Ship, State);

			ShipUpdateMass(Ship);
			return Ship;
		}
	}

	return GameNull;
}