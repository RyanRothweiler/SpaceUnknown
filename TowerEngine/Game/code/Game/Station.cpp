void StationSelected(engine_state* EngineState, game_input* Input)
{
	game::state* State = &EngineState->GameState;
	game::editor_state* EditorState = &EngineState->EditorState;

	station* CurrentStation = State->Selection.GetStation();

	bool Open = true;
	ImGui::Begin("Station Info", &Open);

	ImVec2 window_pos = ImGui::GetWindowPos();
	State->Selection.Current->InfoWindowPos = vector2{window_pos.x, window_pos.y};

	ImGui::End();

	if (!Open) { State->Selection.Clear(); }
}


station* StationCreate(game::state* State)
{
	station* Station = &State->Stations[State->StationsCount++];
	Assert(ArrayCount(State->Stations) > State->StationsCount);

	Station->Size = vector2{18.0f, 18.0f};

	game::RegisterSelectable(selection_type::station, &Station->Position, &Station->Size, (void*)Station, State,
	                         &StationSelected, GameNull
	                        );

	return Station;
}