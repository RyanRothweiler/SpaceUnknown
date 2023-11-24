
const float DockTimeMS = SecondsToMilliseconds(60);

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
	station* Station = per::GetStation(&JourneyStep->DockUndock.Station, State);

	JourneyStep->DockUndock.TimeAccum += Time;
	Ship->Persist->Position = Station->Persist->Position;

	if (JourneyStep->DockUndock.TimeAccum >= DockTimeMS) {

		if (Ship->Persist->Status == ship_status::undocking) {
			StationUndockShip(Ship, State);
		} else {
			StationDockShip(Station, Ship);
		}

		return true;
	}
	return false;
}


