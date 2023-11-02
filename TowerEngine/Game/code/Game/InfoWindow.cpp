info_window InfoWindow::Windows[10] = {};

void InfoWindow::Show(item_id Item) {
	for (int i = 0; i < ArrayCount(Windows); i++) {
		if (!Windows[i].Showing) {
			Windows[i].Showing = true;
			Windows[i].Item = Item;
			return;
		}
	}

	// no windows available, using first window
	Windows[0].Showing = true;
	Windows[0].Item = Item;
}

void InfoWindow::ImGuiRender() {
	for (int i = 0; i < ArrayCount(Windows); i++) {
		info_window* IW = &Windows[i];
		if (IW->Showing) {
			item_definition* Def = GetItemDefinition(IW->Item); 

			ImGui::PushID(i);
			string ID = "Info###" + string{i};
			ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
			ImGui::Begin(ID.Array(), &IW->Showing);

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

				Locals.ShowMemberIfValid("Activation Range", ShipModuleDef.ActivationRange, "");
				Locals.ShowMemberIfValid("Activation Time", MillisecondsToMinutes(ShipModuleDef.ActivationTimeMS), "(min)");
				Locals.ShowMemberIfValid("Cargo Increase", ShipModuleDef.CargoAddition, "(t)");
				Locals.ShowMemberIfValid("Foreman Activation Time Reduction", ShipModuleDef.Foreman.ReductionMinutes, "(min)");

				ImGui::Separator();
			}

			ImGui::Columns(1, "mycolumns");

			ImGui::End();
			ImGui::PopID();
		}
	}
}
