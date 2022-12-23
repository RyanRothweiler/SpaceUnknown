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
	real64 mass = Ship->Definition.Mass;

	vector2 acceleration = Force / mass;
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

void ModuleUpdate(void* SelfData, real64 Time, game::state* State)
{
	ship_module* Module = (ship_module*)SelfData;

	Module->Target = GameNull;

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

			Ship->Definition.FuelRateGallonsPerSecond = 1.0f;
			Ship->Definition.Mass = 100;

			Ship->Modules[0].Definition.ActivationTimeMS = SecondsToMilliseconds(60.0f * 2.0f);
			Ship->Modules[0].Definition.ActivationRange = 40.0f;
			Ship->Modules[0].Owner = Ship;
			game::RegisterStepper(&Ship->Modules[0].Stepper, &ModuleUpdate, (void*)(&Ship->Modules[0]), State);
			Ship->ModulesCount++;

			game::RegisterStepper(&Ship->Stepper, &ShipStep, (void*)Ship, State);

			return Ship;
		}
	}

	return GameNull;
}