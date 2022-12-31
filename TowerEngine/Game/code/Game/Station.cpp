station* StationCreate(game::state* State)
{
	station* Station = &State->Stations[State->StationsCount++];
	Assert(ArrayCount(State->Stations) > State->StationsCount);
	return Station;
}