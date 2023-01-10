
// Add item to a stack without exceeding the cargo mass limit
int32 ItemStackGive(item_hold* Hold, item_instance* Inst, item_definition Def, int32 Count)
{
	int64 NewMass = Hold->MassCurrent + (Def.Mass * Count);
	if (NewMass <= Hold->MassLimit) {
		Inst->Count += Count;
		return Count;
	} else {
		int64 MassAvail = Hold->MassLimit - Hold->MassCurrent;
		int32 CountCanGive = (int32)(MassAvail / Def.Mass);
		Inst->Count += CountCanGive;
		return CountCanGive;
	}
}

// returns amount given
int32 ItemGive(item_hold* Hold, item_id ItemID, int32 Count)
{
	item_definition Def = Globals->AssetsList.ItemDefinitions[(int)ItemID];
	int AmountGiven = 0;

	if (Def.Stackable) {
		// Add to existing stack
		for (int i = 0; i < ArrayCount(Hold->Items); i++) {
			if (Hold->Items[i].Count > 0 && Hold->Items[i].Definition.ID == ItemID) {
				AmountGiven = ItemStackGive(Hold, &Hold->Items[i], Def, Count);
				goto end;
			}
		}

		// Make new stack
		for (int i = 0; i < ArrayCount(Hold->Items); i++) {
			if (Hold->Items[i].Count <= 0) {

				Hold->Items[i].Count = 0;
				Hold->Items[i].Definition = Def;
				AmountGiven = ItemStackGive(Hold, &Hold->Items[i], Def, Count);
				goto end;
			}
		}

		ConsoleLog("Hold is full");
		goto end;
	}

	// Not stackable, so make new stacks
	{
		// Verify we have space
		if (Hold->MassCurrent + Def.Mass > Hold->MassLimit) { return 0; }

		// Give
		for (int c = 0; c < Count; c++) {
			for (int i = 0; i < ArrayCount(Hold->Items); i++) {
				if (Hold->Items[i].Count <= 0) {
					Hold->Items[i].Count = 1;
					Hold->Items[i].Definition = Def;
					AmountGiven = 1;
					goto end;
				}
			}
		}
	}
end:

	Hold->UpdateMass();
	return AmountGiven;
}

void ItemTransfer(item_instance* Inst, item_hold* Dest, int32 Count)
{
	int CountMoving = ItemGive(Dest, Inst->Definition.ID, Count);
	Inst->Count -= CountMoving;
}

void ItemDisplayHold(item_hold* Hold, ship* SelfShip, station* SelfStation, game::state* State, game_input* Input)
{
	int64 CargoWeight = (int64)Hold->MassCurrent;
	string CargoTitle = "Cargo (" + string{CargoWeight} + "/" + string{(int64)Hold->MassLimit} + ")(t)###CARGO";
	if (ImGui::CollapsingHeader(CargoTitle.Array())) {

		ImGui::Text("Drag items here or onto target");
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ImguiItemDraggingID)) {
				item_instance* Inst = State->ItemDragging;

				if (SelfShip != GameNull) {
					ItemTransfer(Inst, &SelfShip->Hold, Inst->Count);
				} else if (SelfStation != GameNull) {
					ItemTransfer(Inst, &SelfStation->Hold, Inst->Count);
				}
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ImguiShipModuleUnequippingDraggingID)) {
				ship_module* Inst = State->ModuleUnequipping;
				ship* Ship = Inst->Owner;

				ItemGive(&Ship->Hold, item_id::sm_asteroid_miner, 1);
				ShipRemoveModule(Inst, State);
			}

			ImGui::EndDragDropTarget();
		}

		for (int i = 0; i < ArrayCount(Hold->Items); i++) {

			ImGui::PushID(i);

			item_instance* Item = &Hold->Items[i];
			int64 ptr = (int64)Item;
			if (Item->Count > 0) {

				ImGui::Image(
				    (ImTextureID)((int64)Item->Definition.Icon->GLID),
				    ImGuiImageSize,
				    ImVec2(0, 0),
				    ImVec2(1, -1),
				    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
				    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
				);

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					State->ItemDragging = Item;

					int D = 0;
					ImGui::SetDragDropPayload(ImguiItemDraggingID, &D, sizeof(D));

					ImGui::Image(
					    (ImTextureID)((int64)Item->Definition.Icon->GLID),
					    ImVec2(40, 40),
					    ImVec2(0, 0),
					    ImVec2(1, -1),
					    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
					    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
					);

					ImGui::EndDragDropSource();
				}

				ImGui::SameLine();
				ImGui::Text(Item->Definition.DisplayName.Array());
				ImGui::SameLine();
				ImGui::Text("x");
				ImGui::SameLine();
				ImGui::Text(string{Item->Count} .Array());
			}

			ImGui::PopID();
		}
	}

	if (Input->MouseLeft.OnUp) {
		if (State->ItemDragging != GameNull && State->Hovering != GameNull) {
			if (State->Hovering->Type == selection_type::ship) {
				ItemTransfer(State->ItemDragging, &State->Hovering->GetShip()->Hold, State->ItemDragging->Count);
			} else if (State->Hovering->Type == selection_type::station) {
				ItemTransfer(State->ItemDragging, &State->Hovering->GetStation()->Hold, State->ItemDragging->Count);
			}

		}

		State->ItemDragging = {};
		State->ModuleUnequipping = {};
	}
}