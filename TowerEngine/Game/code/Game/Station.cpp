void StationSelected(selection* Sel, engine_state* EngineState, game_input* Input)
{
	game::state* State = &EngineState->GameState;
	game::editor_state* EditorState = &EngineState->EditorState;

	station* CurrentStation = Sel->GetStation();

	bool Open = true;
	ImGui::Begin("Station Info", &Open);

	ItemDisplayHold(&CurrentStation->Hold, State, Input, true);

	ImVec2 window_pos = ImGui::GetWindowPos();
	Sel->Current->InfoWindowPos = vector2{window_pos.x, window_pos.y};
	ImGui::End();

	if (!Open) { Sel->Clear(); }
}

void StationDockShip(station* Station, ship* Ship)
{
	int DockIndex = Station->DockedCount;
	Station->DockedCount++;

	int DocksCount = 10;
	real64 DockRel = (real64)DockIndex / (real64)DocksCount;
	real64 DockRadians = DockRel * (2 * PI);

	real64 DockRadius = Station->Size.X * 0.5f * 0.9f;
	vector2 P = Station->Position + vector2 {
		DockRadius * sin(DockRadians),
		DockRadius * cos(DockRadians)
	};

	Ship->Position = P;
	Ship->Status = ship_status::docked;
	Ship->StationDocked = Station;
}

void StationUndockShip(ship* Ship)
{
	Ship->Position = Ship->StationDocked->Position;
	Ship->StationDocked = {};
	Ship->Status = ship_status::idle;
}

station* StationCreate(game::state* State)
{
	station* Station = &State->Stations[State->StationsCount++];
	Assert(ArrayCount(State->Stations) > State->StationsCount);

	Station->Size = vector2{18.0f, 18.0f};
	Station->Hold.MassLimit = 1000;

	game::RegisterSelectable(selection_type::station, &Station->Position, &Station->Size, (void*)Station, State,
	                         &StationSelected, GameNull
	                        );

	return Station;
}