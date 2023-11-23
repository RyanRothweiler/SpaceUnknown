void ShipMovementStart(ship* Ship, journey_step* JourneyStep, state* State)
{
	Ship->Persist->Status = ship_status::moving;

	journey_movement* Mov = &JourneyStep->Movement;

	Ship->Persist->Velocity = {};
	Mov->StartPosition = Ship->Persist->Position;
	Mov->DistFromSidesToCoast = Vector2Distance(Ship->Persist->Position, Mov->EndPosition) * 0.5f * Mov->EdgeRatio;
	Mov->DirToEnd = Vector2Normalize(Mov->EndPosition - Ship->Persist->Position);
	Mov->FullDistance = Vector2Distance(Mov->EndPosition, Ship->Persist->Position) + 1;
	Mov->Cancel = false;

	// Update rotation
	vector2 MoveDir = Vector2Normalize(Mov->EndPosition - Ship->Persist->Position);
	Ship->Persist->Rotation = Vector2AngleBetween(vector2{0, 1}, MoveDir) + PI;
	if (Ship->Persist->Position.X < Mov->EndPosition.X) { Ship->Persist->Rotation *= -1; }
}

bool ShipMovementStep(ship* Ship, journey_step* JourneyStep, real64 Time, state* State)
{
	return ShipSimulateMovement(Ship, &JourneyStep->Movement, Time, State);
}

