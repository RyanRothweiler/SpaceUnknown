
void ShipDockUndockStart(ship* Ship, journey_step* JourneyStep, state* State)
{
	if (Ship->Persist->Status == ship_status::docked) {
		Ship->Persist->Status = ship_status::undocking;
	} else {
		Ship->Persist->Status = ship_status::docking;
	}

	JourneyStep->DockUndock.TimeAccum = 0;
}

bool ShipDockUndockStep(ship* Ship, journey_step* JourneyStep, real64 Time, state* State)
{
	JourneyStep->DockUndock.TimeAccum += Time;
	Ship->Persist->Position = per::GetStation(&JourneyStep->DockUndock.Station, State)->Persist->Position;

	if (JourneyStep->DockUndock.TimeAccum >= SecondsToMilliseconds(60.0f)) {

		if (Ship->Persist->Status == ship_status::undocking) {
			StationUndockShip(Ship, State);
		} else {
			StationDockShip(
					per::GetStation(&JourneyStep->DockUndock.Station, State), 
					Ship);
		}

		return true;
	}
	return false;
}


