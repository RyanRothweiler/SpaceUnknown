info_window InfoWindow::Windows[10] = {};

info_window* InfoWindow::OpenWindow() { 
	// Init to first window as backup deafult
	info_window* Ret = &Windows[0];

	for (int i = 0; i < ArrayCount(Windows); i++) {
		if (!Windows[i].Showing) {
			Ret = &Windows[i];
			break;
		}
	}

	Ret->Showing = true;
	return Ret;
}

void InfoWindow::Show(item_id Item) {
	info_window* Window = OpenWindow();
	Window->ShowingItem = true;
	Window->Item = Item;
}

void InfoWindow::Show(ship_id Ship) {
	info_window* Window = OpenWindow();
	Window->ShowingItem = false;
	Window->Ship = Ship;
}

void InfoWindow::ImGuiRender() {
	struct {
		void ShowMemberIfValid(char* DisplayName, r64 Value, char* Unit) {
			if (Value > 0) {
				ImGui::TextWrapped(DisplayName); ImGui::NextColumn();

				string Val = Humanize((int64)Value);
				ImGui::TextWrapped((Val + " " + string{Unit}).Array()); ImGui::NextColumn();

				ImGui::Separator();
			}
		}
	} Locals;
	

	for (int i = 0; i < ArrayCount(Windows); i++) {
		info_window* IW = &Windows[i];
		if (IW->Showing) {

			ImGui::PushID(i);
			string ID = "Info###" + string{i};
			ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
			ImGui::Begin(ID.Array(), &IW->Showing);

			if (IW->ShowingItem) { 
				item_definition* Def = GetItemDefinition(IW->Item); 

				ImGui::Image(
					(ImTextureID)((int64)Def->Icon->GLID),
					ImGuiImageSize,
					ImVec2(0, 0),
					ImVec2(1, -1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
					ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
				);

				ImGui::SameLine();
				ImGui::Text(Def->DisplayName.Array());

				ImGui::Columns(2, "mycolumns");
				ImGui::Separator();

				ImGui::Text("Description"); ImGui::NextColumn();
				ImGui::TextWrapped(Def->DisplayDescription.Array()); ImGui::NextColumn();

				ImGui::Separator();

				ImGui::Text("Mass"); ImGui::NextColumn();
				ImGui::Text(string{Def->Mass}.Array()); ImGui::NextColumn();

				ImGui::Separator();

				if (Def->ShipModuleID != ship_module_id::none) { 
					ship_module_definition ShipModuleDef = Globals->AssetsList.ShipModuleDefinitions[(int)Def->ShipModuleID];

					Locals.ShowMemberIfValid("Activation Range", ShipModuleDef.ActivationRange, "");
					Locals.ShowMemberIfValid("Activation Time", MillisecondsToMinutes(ShipModuleDef.ActivationTimeMS), "(min)");
					Locals.ShowMemberIfValid("Cargo Increase", ShipModuleDef.CargoAddition, "(t)");
					Locals.ShowMemberIfValid("Yield", ShipModuleDef.Yield, "");

					Locals.ShowMemberIfValid("Foreman Activation Time Reduction", ShipModuleDef.Foreman.ReductionMinutes, "(min)");

					ImGui::Separator();
					ImGui::Columns(1, "mycolumns");
				}
			} else {
				// Ship
				ship_definition* Def = GetShipDefinition(IW->Ship);

				ImGui::Image(
					(ImTextureID)((int64)Def->Icon->GLID),
					ImGuiImageSize,
					ImVec2(0, 0),
					ImVec2(1, -1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
					ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
				);

				ImGui::SameLine();
				ImGui::Text(Def->DisplayName.Array());

				ImGui::Separator();
				ImGui::Text("Module Slots");
				for (int s = 0; s < Def->SlotsCount; s++) {
					ship_module_slot_definition* SlotDef = GetShipModuleSlotDefinition(Def->SlotTypes[s]);

					ImGui::Image(
						(ImTextureID)((int64)SlotDef->Icon->GLID),
						ImGuiImageSize * 0.75f,
						ImVec2(0, 0),
						ImVec2(1, -1),
						ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
						ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
					);
					ImGui::SameLine();
					ImGui::Text(SlotDef->DisplayName.Array());
				}
				ImGui::Separator();

				ImGui::Columns(2, "cols");

				Locals.ShowMemberIfValid("Hull Mass", (r64)Def->Mass, "(t)");
				Locals.ShowMemberIfValid("Radar Radius", (r64)Def->RadarRadius, "");
				Locals.ShowMemberIfValid("Engine Fuel Rate", (r64)Def->FuelRateMassPerSecond, "(t/s)");
				Locals.ShowMemberIfValid("Cargo Hold Mass Limit", (r64)Def->HoldMass, "(t)");
				Locals.ShowMemberIfValid("Fuel Tank Mass Limit", (r64)Def->FuelTankMassLimit, "(t)");

				ImGui::Columns(1, "cols");

			}

			ImGui::End();
			ImGui::PopID();
		}
	}
}
