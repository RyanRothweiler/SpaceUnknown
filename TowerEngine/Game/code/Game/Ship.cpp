int64 ShipGetCargoMass(ship* Ship)
{
	int64 Mass = 0;

	// Add cargo weightt
	for (int i = 0; i < ArrayCount(Ship->Cargo); i++) {
		if (Ship->Cargo[i].Count > 0) {
			Mass += Ship->Cargo[i].Definition.Mass * Ship->Cargo[i].Count;
		}
	}

	return Mass;
}

int64 ShipGetMass(ship* Ship)
{
	int64 Mass = Ship->Definition.Mass;
	Mass += ShipGetCargoMass(Ship);
	return Mass;
}

//public static bool SimulateMovement(ref Physics physics, ShipDefinition def, float mass, UniversalPosition targetPositionUniverse, float time, JourneySettings settings)
bool32 ShipSimulateMovement(ship* Ship, vector2 TargetPos, real64 Time)
{
	real64 TimeSeconds = Time * 0.001f;

	real64 fuelToUse = Ship->Definition.FuelRateGallonsPerSecond * TimeSeconds;
	real64 fuelForce = fuelToUse * fuelForcePerGallon;

	// close enough
	if (Vector2Distance(Ship->Position, TargetPos) < 0.01f) {
		Ship->Velocity = vector2{0, 0};
		return false;
	}

	// past target
	// Need a 0.1 buffer here because the two distances are equal during the journey, but not always exactly because of rounding errors
	if (
	    Vector2Distance(Ship->CurrentJourney.StartPosition, TargetPos) + 0.001f
	    <
	    (Vector2Distance(Ship->Position, TargetPos) + Vector2Distance(Ship->Position, Ship->CurrentJourney.StartPosition))
	) {
		// Stopping, past destination
		Ship->Velocity = vector2{0, 0};
		return false;
	}

	vector2 Force = {};

	// Speed up
	if (Vector2Distance(Ship->Position, Ship->CurrentJourney.StartPosition) < Ship->CurrentJourney.DistFromSidesToCoast) {
		Ship->FuelGallons -= fuelToUse;
		Force = Vector2Normalize(TargetPos - Ship->Position) * fuelForce;
	}

	// Slow down
	if (Vector2Distance(Ship->Position, TargetPos) < Ship->CurrentJourney.DistFromSidesToCoast) {
		Ship->FuelGallons -= fuelToUse;
		Force = Vector2Normalize(TargetPos - Ship->Position) * fuelForce * -1.0f;

		// slow enough
		if (Vector2Length(Ship->Velocity) < 0.001f) {
			Ship->Velocity = vector2{0, 0};
			return false;
		}
	}

	// Get cargo mass too
	int64 Mass = ShipGetMass(Ship);

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

	// Update rotation
	vector2 MoveDir = Vector2Normalize(Ship->CurrentJourney.EndPosition - Ship->Position);
	Ship->Rotation = Vector2AngleBetween(vector2{0, 1}, MoveDir) + PI;
	if (Ship->Position.X < Ship->CurrentJourney.EndPosition.X) { Ship->Rotation *= -1; }
}

// Add item to a stack without exceeding the cargo mass limit
void ShipStackGive(ship* Ship, item_instance* Inst, item_definition Def, int32 Count)
{
	int64 NewMass = ShipGetCargoMass(Ship) + (Def.Mass * Count);
	if (NewMass <= Ship->Definition.CargoMassLimit) {
		Inst->Count += Count;
	} else {
		int64 MassAvail = Ship->Definition.CargoMassLimit - ShipGetCargoMass(Ship);
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
				return;
			}
		}

		// Make new stack
		for (int i = 0; i < ArrayCount(Ship->Cargo); i++) {
			if (Ship->Cargo[i].Count <= 0) {

				Ship->Cargo[i].Count = 0;
				Ship->Cargo[i].Definition = Def;
				ShipStackGive(Ship, &Ship->Cargo[i], Def, Count);
				return;
			}
		}

		ConsoleLog("Ship cargo full");
		return;
	}

	// Not stackable, so make new stacks
	{
		// Verify we have space
		if (ShipGetCargoMass(Ship) + Def.Mass > Ship->Definition.CargoMassLimit) { return; }

		// Give
		for (int c = 0; c < Count; c++) {
			for (int i = 0; i < ArrayCount(Ship->Cargo); i++) {
				if (Ship->Cargo[i].Count <= 0) {
					Ship->Cargo[i].Count = 1;
					Ship->Cargo[i].Definition = Def;
					break;
				}
			}
		}
	}
}

void ModuleUpdate(void* SelfData, real64 Time, game::state* State)
{
	ship_module* Module = (ship_module*)SelfData;

	Module->Target = GameNull;

	// if no cargo space then do nothing
	if (ShipGetCargoMass(Module->Owner) == Module->Owner->Definition.CargoMassLimit) { return; }

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

			Ship->Modules[0].Definition = Globals->AssetsList.Definition_Module_AsteroidMiner;
			Ship->Modules[0].Owner = Ship;
			game::RegisterStepper(&Ship->Modules[0].Stepper, &ModuleUpdate, (void*)(&Ship->Modules[0]), State);
			Ship->ModulesCount++;

			game::RegisterStepper(&Ship->Stepper, &ShipStep, (void*)Ship, State);

			return Ship;
		}
	}

	return GameNull;
}