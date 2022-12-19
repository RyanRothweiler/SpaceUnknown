//public static bool SimulateMovement(ref Physics physics, ShipDefinition def, float mass, UniversalPosition targetPositionUniverse, float time, JourneySettings settings)
bool32 ShipSimulateMovement(ship* Ship, vector2 TargetPos, real64 Time)
{
	real64 fuelToUse = Ship->Definition.FuelRateGallonsPerSecond * Time;
	real64 fuelForce = fuelToUse * fuelForcePerGallon;

	// close enough
	if (Vector2Distance(Ship->Position, TargetPos) < 0.01f) {
		Ship->Velocity = vector2{0, 0};
		return false;
	}

	// past target
	// Need a 0.1 buffer here because the two distances are equal during the journey, but not always exactly because of rounding errors
	if (
	    Vector2Distance(Ship->JourneyStartPos, TargetPos) + 0.001f
	    <
	    (Vector2Distance(Ship->Position, TargetPos) + Vector2Distance(Ship->Position, Ship->JourneyStartPos))
	) {
		// Stopping, past destination
		Ship->Velocity = vector2{0, 0};
		return false;
	}

	vector2 Force = {};

	// Speed up
	if (Vector2Distance(Ship->Position, Ship->JourneyStartPos) < Ship->DistFromSidesToCoast) {
		Ship->FuelGallons -= fuelToUse;
		Force = Vector2Normalize(TargetPos - Ship->Position) * fuelForce;
	}

	// Slow down
	if (Vector2Distance(Ship->Position, TargetPos) < Ship->DistFromSidesToCoast) {
		Ship->FuelGallons -= fuelToUse;
		Force = Vector2Normalize(TargetPos - Ship->Position) * fuelForce * -1.0f;

		// slow enough
		if (Vector2Length(Ship->Velocity) < 0.001f) {
			Ship->Velocity = vector2{0, 0};
			return false;
		}
	}

	float mass = 100;

	vector2 acceleration = Force / mass;
	Ship->Velocity = Ship->Velocity + acceleration;
	Ship->Position = Ship->Position + (Ship->Velocity * Time);

	return true;
}

void ShipStep(void* SelfData, real64 Time)
{
	game::ship* Ship = (game::ship*)SelfData;
	if (Ship->Moving) {
		Ship->Moving = ShipSimulateMovement(Ship, Ship->TargetPos, Time);
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

			Ship->Stepper.Step = &ShipStep;
			Ship->Stepper.SelfData = (void*)Ship;
			game::RegisterStepper(&Ship->Stepper, State);

			return Ship;
		}
	}

	return GameNull;
}