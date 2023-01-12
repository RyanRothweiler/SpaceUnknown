void ImGuiItemCountList(item_count* Items, int32 Count)
{

	for (int inp = 0; inp < Count; inp++) {
		item_count* IC = &Items[inp];

		ImGui::Image(
		    (ImTextureID)((int64)Globals->AssetsList.ItemDefinitions[(int)IC->ID].Icon->GLID),
		    ImGuiImageSize,
		    ImVec2(0, 0),
		    ImVec2(1, -1),
		    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
		    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
		);

		ImGui::SameLine();
		ImGui::Text("x %i", IC->Count);
		ImGui::SameLine();
	}
}

void StationSelected(selection* Sel, engine_state* EngineState, game_input* Input)
{
	game::state* State = &EngineState->GameState;
	game::editor_state* EditorState = &EngineState->EditorState;

	station* CurrentStation = Sel->GetStation();

	bool Open = true;
	ImGui::Begin("Station Info", &Open);

	ItemDisplayHold(&CurrentStation->Hold, State, Input, true);

	if (ImGui::CollapsingHeader("Services")) {
		if (ImGui::TreeNode("Refinery")) {

			if (ImGui::Button("New Order")) {
				ImGui::OpenPopup("NewOrder");
			}

			// new order modal
			if (ImGui::BeginPopupModal("NewOrder")) {

				ImGui::Text("Select Recipe");
				ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 100));
				ImGui::BeginChild("recipes", ImVec2(0, 300));

				ImGui::Columns(3, "mycolumns"); // 4-ways, with border
				ImGui::Separator();
				ImGui::Text("Inputs"); ImGui::NextColumn();
				ImGui::Text("Outputs"); ImGui::NextColumn();
				ImGui::Text("Duration"); ImGui::NextColumn();
				ImGui::Separator();

				for (int i = 0; i < (int)recipe_id::count; i++) {
					recipe* Recipe = &Globals->AssetsList.RecipeDefinitions[i];

					string dur = string{Recipe->DurationMS};
					static bool sel = false;
					if (ImGui::Selectable(dur.Array(), &sel, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, ImGuiImageSize.x))) {
						//selected = i;
					}
					ImGui::NextColumn();

					ImGuiItemCountList(&Recipe->Inputs[0], Recipe->InputsCount);
					ImGui::NextColumn();

					ImGuiItemCountList(&Recipe->Outputs[0], Recipe->OutputsCount);
					ImGui::NextColumn();

				}

				ImGui::Columns(1);
				ImGui::EndChild();
				ImGui::PopStyleColor();

				ImGui::Separator();
				if (ImGui::Button("Submit", ImVec2(300, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(300, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::TreePop();
		}
	}


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