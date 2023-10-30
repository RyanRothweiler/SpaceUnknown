namespace journey {

	journey_step* AddStep(ship_journey* SJ) {
		journey_step* Step = &SJ->Steps[SJ->StepsCount++];
		Assert(SJ->StepsCount < ArrayCount(SJ->Steps));

		return Step;
	}

	void Execute(ship_journey* SJ) {
		SJ->InProgress = true;
		SJ->CurrentStep = -1;
	}

	void Clear(ship_journey* Jour) {
		Jour->StepsCount = 0;
		Jour->InProgress = false;
		Jour->CurrentStep = 0;
	}

	void ImGuiDrawSteps(ship_journey* Jour, ship* CurrentShip, state* State) {

		ImGui::Separator();

		bool32 DockState = (CurrentShip->Persist->Status == ship_status::docked);

		for (int i = 0; i < CurrentShip->Persist->CurrentJourney.StepsCount; i++) {

			//string id = "COMMAND_" + string{i};
			//ImGui::PushID(id.Array());

			journey_step* Step = &CurrentShip->Persist->CurrentJourney.Steps[i];
			switch (Step->Type) {
				case journey_step_type::movement: {
					ImGui::Text("%i. Move", (i+1));
				} break;

				case journey_step_type::dock_undock: {
					if (DockState) {
						ImGui::Text("%i. Undock", (i+1));
					} else {
						ImGui::Text("%i. Dock", (i+1));
					}
					DockState = !DockState;
				} break;


				INVALID_DEFAULT
			}

			//ImGui::PopID();
		}

		ImGui::Separator();
	}
}
