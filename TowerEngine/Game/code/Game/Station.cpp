void ConverterUpdate(void* SelfData, real64 Time, game::state* State)
{
	converter* Converter = (converter*)SelfData;
	if (Converter->RunsCount > 0) {

		if (Converter->IsRunning) {
			Converter->CurrentOrderTime += Time;
			if (Converter->CurrentOrderTime >= Converter->Order.DurationMS) {
				Converter->RunsCount--;

				if (Converter->RunsCount <= 0) {
					Converter->IsRunning = false;
				}

				// Give items
				for (int i = 0; i < Converter->Order.OutputsCount; i++) {
					item_count* C = &Converter->Order.Outputs[i];
					ItemGive(&Converter->Owner->Hold, C->ID, C->Count);
				}
			}
		} else {
			recipe_inputs_missing_return InputsMissing = RecipeInputsMissing(&Converter->Order, &Converter->Owner->Hold);
			if (InputsMissing.Count == 0) {
				Converter->IsRunning = true;

				// Consume items. We can assume they exist in sufficient amounts
				item_hold* Source = &Converter->Owner->Hold;
				recipe* Order = &Converter->Order;

				for (int i = 0; i < Order->InputsCount; i++) {
					item_count* C = &Order->Inputs[i];

					for (int h = 0; h < ArrayCount(Source->Items); h++) {
						if (Source->Items[h].Definition.ID == C->ID) {
							Source->Items[h].Count -= C->Count;
						}
					}
				}

			}
		}
	}
}

void ConverterAddOrder(converter * Converter, recipe Order)
{
	Converter->IsRunning = false;
	Converter->RunsCount = 1;
	Converter->CurrentOrderTime = 0.0f;
	Converter->Order = Order;
}

void ImGuiItemCountList(item_count * Items, int32 Count)
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
		ImGui::Text("x %i", (int)IC->Count);
		if (inp != Count - 1) { ImGui::SameLine(); }
	}
}

void StationProductionService(station* Station, int32 ConverterIndex, station_service Service)
{
	static recipe_id RecipeIDSelected = recipe_id::none;

	recipe_list* RecipeList = &Globals->AssetsList.RecipesByService[(int)Service];
	converter* Converter = &Station->Converters[ConverterIndex];

	if (Converter->HasOrder()) {
		recipe* Recipe = &Converter->Order;

		ImGui::Columns(2, "mycolumns"); // 4-ways, with border

		ImGui::Separator();
		ImGui::Text("Inputs"); ImGui::NextColumn();
		ImGui::Text("Outputs"); ImGui::NextColumn();
		ImGui::Separator();

		ImGuiItemCountList(&Recipe->Inputs[0], Recipe->InputsCount);
		ImGui::NextColumn();

		ImGuiItemCountList(&Recipe->Outputs[0], Recipe->OutputsCount);
		ImGui::NextColumn();

		ImGui::Columns(1);

		recipe_inputs_missing_return InputsMissing = RecipeInputsMissing(Recipe, &Station->Hold);

		if (Converter->IsRunning) {
			// Progress
			float Progress = (float)(Converter->CurrentOrderTime / Converter->Order.DurationMS);
			float PD = Progress * 100.0f;
			string ProgStr = Real64ToString(PD, 2);
			string ProgDisp = ProgStr + "%";
			ImGui::ProgressBar(Progress, ImVec2(-1.0f, 0.0f), ProgDisp.Array());
		} else {
			for (int i = 0; i < InputsMissing.Count; i++) {
				item_definition Def = Globals->AssetsList.ItemDefinitions[(int)InputsMissing.Items[i].ID];
				string Output = "Item Missing - " + Def.DisplayName + " x" + InputsMissing.Items[i].Count;
				ImGui::TextColored(ImVec4(1, 0, 0, 1), Output.Array());
			}
		}

		/*
		if (ImGui::Button("Cancel Order", ImVec2(-1, 0))) {
			Converter->RunsCount = 0;
			Station->Converters[0] = {};

			// TODO return consumed items
		}
		*/
	} else {
		if (ImGui::Button("New Order")) {
			ImGui::OpenPopup("NewOrder");
			RecipeIDSelected = recipe_id::none;
		}
	}

	// new order modal
	if (ImGui::BeginPopupModal("NewOrder")) {

		ImGui::Text("Select Recipe");
		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 100));
		ImGui::BeginChild("recipes", ImVec2(500, 300));

		ImGui::Columns(3, "mycolumns"); // 4-ways, with border
		ImGui::Separator();
		ImGui::Text("Duration"); ImGui::NextColumn();
		ImGui::Text("Inputs"); ImGui::NextColumn();
		ImGui::Text("Outputs"); ImGui::NextColumn();
		ImGui::Separator();


		for (int i = 0; i < RecipeList->Count; i++) {
			recipe_id ID = RecipeList->IDs[i];
			recipe* Recipe = &Globals->AssetsList.RecipeDefinitions[(int)ID];

			string dur = Humanize((int64)MillisecondsToMinutes(Recipe->DurationMS)) + "(m)";
			static bool sel = false;
			if (ImGui::Selectable(dur.Array(), &sel, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, ImGuiImageSize.x))) {
				RecipeIDSelected = ID;
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

		real32 HW = ImGui::GetWindowWidth();
		if (RecipeIDSelected != recipe_id::none) { HW = HW * 0.5f; }

		ImGui::Separator();
		if (RecipeIDSelected != recipe_id::none) {
			if (ImGui::Button("Submit", ImVec2(HW, 0))) {
				ConverterAddOrder(&Station->Converters[0], Globals->AssetsList.RecipeDefinitions[(int)RecipeIDSelected]);

				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
		}
		if (ImGui::Button("Cancel", ImVec2(HW, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void StationSelected(selection * Sel, engine_state * EngineState, game_input * Input)
{
	game::state* State = &EngineState->GameState;
	game::editor_state* EditorState = &EngineState->EditorState;

	station* CurrentStation = Sel->GetStation();

	bool Open = true;
	ImGui::Begin("Station Info", &Open);

	ItemDisplayHold("Cargo", &CurrentStation->Hold, State, Input, true, item_hold_filter::any);

	if (ImGui::CollapsingHeader("Services")) {
		if (ImGui::TreeNode("Ship Yard")) {
			StationProductionService(CurrentStation, 0, station_service::shipyard);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Refinery")) {
			StationProductionService(CurrentStation, 1, station_service::refinery);
			ImGui::TreePop();
		}
	}


	ImVec2 window_pos = ImGui::GetWindowPos();
	Sel->Current->InfoWindowPos = vector2{window_pos.x, window_pos.y};
	ImGui::End();

	if (!Open) { Sel->Clear(); }
}

void StationDockShip(station * Station, ship * Ship)
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

void StationUndockShip(ship * Ship)
{
	Ship->Position = Ship->StationDocked->Position;
	Ship->StationDocked = {};
	Ship->Status = ship_status::idle;
}

station* StationCreate(game::state * State)
{
	station* Station = &State->Stations[State->StationsCount++];
	Assert(ArrayCount(State->Stations) > State->StationsCount);

	Station->Size = vector2{18.0f, 18.0f};

	Station->Hold.Setup(1000);

	selectable* Sel = RegisterSelectable(selection_type::station, &Station->Position, &Station->Size, (void*)Station, State);
	Sel->SelectionUpdate = &StationSelected;

	game::RegisterStepper(&Station->Converters[0].Stepper, &ConverterUpdate, (void*)(&Station->Converters[0]), State);
	Station->Converters[0].Owner = Station;

	return Station;
}