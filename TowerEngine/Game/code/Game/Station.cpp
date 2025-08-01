
void ConverterUpdate(void* SelfData, real64 Time, state* State)
{
	converter* Converter = (converter*)SelfData;
	if (Converter->Persist->RunsCount > 0) {

		recipe Recipe = RecipeGetDefinition(Converter->Persist->RecipeID);

		if (Converter->Persist->IsRunning) {
			Converter->Persist->OrderTime += Time;
			if (Converter->Persist->OrderTime >= Recipe.DurationMS) {

				Converter->Persist->RunsCount--;
				Converter->Persist->OrderTime = 0;
				Converter->Persist->IsRunning = false;

				// Give outputs
				for (int i = 0; i < Recipe.OutputsCount; i++) {
					item_count* C = &Recipe.Outputs[i];

					switch (C->Type) {
						case (recipe_member_type::item): {
							ItemGive(&Converter->Owner->Hold, C->ItemID, C->Count);
						} break;

						case (recipe_member_type::ship): {
							ship* Ship = ShipCreate(State, C->ShipID);
							ShipSetup(Ship, Ship->Persist, State);
							StationDockShip(Converter->Owner, Ship);
							Save();
						} break;

						INVALID_DEFAULT;
					}
				}
			}
		} else {

			recipe_inputs_missing_return InputsMissing = RecipeInputsMissing(&Recipe, &Converter->Owner->Hold, State);
			if (InputsMissing.Meets()) {
				Converter->Persist->IsRunning = true;

				// Consume items. We can assume they exist in sufficient amounts
				item_hold* Source = &Converter->Owner->Hold;

				for (int i = 0; i < Recipe.InputsCount; i++) {
					item_count* C = &Recipe.Inputs[i];

					for (int h = 0; h < ArrayCount(Source->Persist->Items); h++) {
						if (Source->Persist->Items[h].ID == C->ItemID) {
							ItemHoldConsumeItem(Source, h, C->Count);
						}
					}
				}

			}
		}
	}
}

void ConverterAddOrder(converter* Converter, recipe_id ID)
{
	Converter->Persist->IsRunning = false;

	Converter->Persist->RunsCount = 1;
	Converter->Persist->OrderTime = 0.0f;
	Converter->Persist->RecipeID = ID;
}

void ImGuiItemCountList(item_count* Items, int32 Count)
{
	for (int inp = 0; inp < Count; inp++) {
		item_count* IC = &Items[inp];

		int64 IconGLID = 0;
		switch (IC->Type) {
			case (recipe_member_type::item): {
				ImGuiItemIcon(IC->ItemID, false);
				break;
			}

			case (recipe_member_type::ship): {

				IconGLID = Globals->AssetsList.ShipDefinitions[(int)IC->ShipID].Icon->GLID;
				ImGui::Image(
					(ImTextureID)(IconGLID),
					ImGuiImageSize,
					ImVec2(0, 0),
					ImVec2(1, -1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
					ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
				);

				if (ImGui::BeginPopupContextItem("item context menu")) {
					if (ImGui::MenuItem("Info")) { 
						InfoWindow::Show(IC->ShipID);
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
				
				break;
			}


			INVALID_DEFAULT;
		}


		ImGui::SameLine();
		ImGui::Text("x %i", (int)IC->Count);
		if (inp != Count - 1) { ImGui::SameLine(); }
	}
}

void StationProductionService(station* Station, int32 ConverterIndex, station_service Service, state* State)
{
	static recipe_id RecipeIDSelected = recipe_id::none;

	recipe_list* RecipeList = &Globals->AssetsList.RecipesByService[(int)Service];
	converter* Converter = &Station->Converters[ConverterIndex];

	if (Converter->HasOrder()) {
		recipe Recipe = RecipeGetDefinition(Converter->Persist->RecipeID);

		ImGui::Columns(2, "mycolumns"); // 4-ways, with border

		ImGui::Separator();
		ImGui::Text("Inputs"); ImGui::NextColumn();
		ImGui::Text("Outputs"); ImGui::NextColumn();
		ImGui::Separator();

		ImGui::PushID("inputs");
		ImGuiItemCountList(&Recipe.Inputs[0], Recipe.InputsCount);
		ImGui::PopID();
		ImGui::NextColumn();

		ImGui::PushID("outputs");
		ImGuiItemCountList(&Recipe.Outputs[0], Recipe.OutputsCount);
		ImGui::PopID();
		ImGui::NextColumn();

		ImGui::Columns(1);

		if (Converter->Persist->IsRunning) {
			// Progress
			r64 MSRemaining = Recipe.DurationMS - Converter->Persist->OrderTime;
			r64 TimeLeftMinutes = MillisecondsToMinutes(MSRemaining);
			ImGui::Text("%.2f (m) remaining", TimeLeftMinutes);

			float Progress = (float)(Converter->Persist->OrderTime / Recipe.DurationMS);
			float PD = Progress * 100.0f;
			ImGui::ProgressBar(Progress, ImVec2(-1.0f, 1.0f));
		} else {
			recipe_inputs_missing_return InputsMissing = RecipeInputsMissing(&Recipe, &Station->Hold, State);

			for (int i = 0; i < InputsMissing.Count; i++) {
				item_definition Def = Globals->AssetsList.ItemDefinitions[(int)InputsMissing.Items[i].ItemID];
				string Output = "Item Missing - " + Def.DisplayName + " x" + InputsMissing.Items[i].Count;
				ImGui::TextColored(ImVec4(1, 0, 0, 1), Output.Array());
			}
			if (InputsMissing.ShipLimit) { 
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Currently at ship limit. Cannot create more ships.");
			}
		}

		ImGui::SliderInt("Runs Remaining", &Converter->Persist->RunsCount, 1, 100);

		if (ImGui::Button("Cancel Order", ImVec2(-1, 0))) {
			Converter->Persist->RunsCount = {};
			Converter->Persist->RecipeID = {};
			// TODO return consumed items
		}

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

		ImGui::Columns(3, "mycolumns");
		ImGui::Separator();
		ImGui::Text("Duration"); ImGui::NextColumn();
		ImGui::Text("Inputs"); ImGui::NextColumn();
		ImGui::Text("Outputs"); ImGui::NextColumn();
		ImGui::Separator();

		for (int i = 0; i < RecipeList->Count; i++) {
			recipe_id ID = RecipeList->IDs[i];

			// Can't use recipes that we don't have access to 
			if (!TreeBonusesTotal->RecipeUnlocked[(int)ID]) { continue; }

			ImGui::PushID(recipe_id_NAME[(int)ID].Array());

			recipe* Recipe = &Globals->AssetsList.RecipeDefinitions[(int)ID];

			string dur = Humanize((int64)MillisecondsToMinutes(Recipe->DurationMS)) + "(m)";
			static bool sel = false;
			if (ImGui::Selectable(dur.Array(), &sel, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, ImGuiImageSize.x))) {
				ConverterAddOrder(Converter, ID);
				ImGui::CloseCurrentPopup();
				sel = false;
			}
			ImGui::NextColumn();

			ImGui::PushID("input");
			ImGuiItemCountList(&Recipe->Inputs[0], Recipe->InputsCount);
			ImGui::PopID();
			ImGui::NextColumn();

			ImGui::PushID("outputs");
			ImGuiItemCountList(&Recipe->Outputs[0], Recipe->OutputsCount);
			ImGui::PopID();
			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);
		ImGui::EndChild();
		ImGui::PopStyleColor();

		real32 HW = ImGui::GetWindowWidth();

		if (ImGui::Button("Cancel", ImVec2(HW, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void StationSelected(selection * Sel, engine_state * EngineState, game_input * Input, i32 SelectionIndex)
{
	state* State = &EngineState->GameState;
	editor_state* EditorState = &EngineState->EditorState;

	station* CurrentStation = Sel->GetStation();

	bool Open = true;
	ImGuiSelectionWindowSetup(SelectionIndex);
	ImGui::Begin("Station Info", &Open);

	ItemDisplayHold("Cargo", &CurrentStation->Hold, State, Input, true, item_hold_filter::any);

	if (ImGui::CollapsingHeader("Services")) {
		if (ImGui::TreeNode("Ship Yard")) {
			StationProductionService(CurrentStation, 0, station_service::shipyard, State);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Refinery")) {
			StationProductionService(CurrentStation, 1, station_service::refinery, State);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Manufacturing")) {
			StationProductionService(CurrentStation, 2, station_service::manufacturing, State);
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
	Ship->Persist->DockIndex = Station->Persist->DockedCount;
	int DockIndex = Station->Persist->DockedCount;
	Station->Persist->DockedCount++;

	real64 DockRel = (real64)DockIndex / (real64)StationDockCount;
	real64 DockRadians = DockRel * (2 * PI);

	real64 DockRadius = Station->Size.X * 0.5f * 0.9f;
	vector2 P = Station->Persist->Position + vector2 {
		DockRadius * sin(DockRadians),
		DockRadius * cos(DockRadians)
	};

	Ship->Persist->Position = P;
	Ship->Persist->Status = ship_status::docked;
	per::SetStation(&Ship->Persist->StationDocked, Station);
}

void StationUndockShip(ship * Ship, state* State)
{
	station* StationDocked = per::GetStation(&Ship->Persist->StationDocked, State);
	per::Clear(&Ship->Persist->StationDocked);
	Ship->Persist->Status = ship_status::idle;
}

// Create a new station
station* StationCreate(state * State)
{
	station* Station = &State->Stations[State->PersistentData.StationsCount];
	Station->Persist = &State->PersistentData.Stations[State->PersistentData.StationsCount];

	State->PersistentData.StationsCount++;
	Assert(ArrayCount(State->PersistentData.Stations) > State->PersistentData.StationsCount);
	Assert(ArrayCount(State->Stations) > State->PersistentData.StationsCount);

	Station->Persist->GUID = PlatformApi.GetGUID();

	return Station;
}

// Setup a station data. Not creating a new one
void StationSetup(station* Station, station_persistent* Persist, state* State) 
{
	Station->Persist = Persist;
	Station->Size = vector2{18.0f, 18.0f};

	Station->Hold.Setup(1000, item_hold_type::station_cargo, &Station->Persist->ItemHold);
	ItemHoldUpdateMass(&Station->Hold);

	selectable* Sel = RegisterSelectable(selection_type::station, &Station->Persist->Position, &Station->Size, (void*)Station, State);
	Sel->SelectionUpdate = &StationSelected;

	RegisterStepper(&Station->Converters[0].Stepper, &ConverterUpdate, (void*)(&Station->Converters[0]), State);
	Station->Converters[0].Setup(Station, &Station->Persist->Converters[0]);

	RegisterStepper(&Station->Converters[1].Stepper, &ConverterUpdate, (void*)(&Station->Converters[1]), State);
	Station->Converters[1].Setup(Station, &Station->Persist->Converters[1]);

	RegisterStepper(&Station->Converters[2].Stepper, &ConverterUpdate, (void*)(&Station->Converters[2]), State);
	Station->Converters[2].Setup(Station, &Station->Persist->Converters[2]);

	per::AddSource(Station->Persist->GUID, Station, State);
}
