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

void ItemTransfer(item_instance* Inst, item_hold* Source, item_hold* Dest, int32 Count)
{
	int CountMoving = ItemGive(Dest, Inst->Definition.ID, Count);
	Inst->Count -= CountMoving;

	Source->UpdateMass();
	Dest->UpdateMass();
}

void ItemDisplayHold(item_hold* Hold, game::state* State, game_input* Input, bool32 CanTransfer)
{
	int64 CargoWeight = (int64)Hold->MassCurrent;
	string CargoTitle = "Cargo (" + string{CargoWeight} + "/" + string{(int64)Hold->MassLimit} + ")(t)###CARGO";
	if (ImGui::CollapsingHeader(CargoTitle.Array())) {


		/*
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ImguiItemDraggingID)) {

				item_instance* Inst = State->ItemDragging;
				item_hold* SourceHold = State->HoldItemDraggingFrom;

				ItemTransfer(Inst, SourceHold, Hold, Inst->Count);
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ImguiShipModuleUnequippingDraggingID)) {
				ship_module* Inst = State->ModuleUnequipping;

				ItemGive(Hold, item_id::sm_asteroid_miner, 1);
				ShipRemoveModule(Inst, State);
			}

			ImGui::EndDragDropTarget();
		}
		*/

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 100));
		ImGui::BeginChild("testing", ImVec2(0, 300), true, ImGuiWindowFlags_None);

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

				if (CanTransfer && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					State->ItemDragging = Item;
					State->HoldItemDraggingFrom = Hold;

					int D = 0;
					ImGui::SetDragDropPayload(ImguiItemDraggingID, &D, sizeof(D));

					ImGui::Image(
					    (ImTextureID)((int64)Item->Definition.Icon->GLID),
					    ImGuiImageSize,
					    ImVec2(0, 0),
					    ImVec2(1, -1),
					    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
					    ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
					);

					ImGui::EndDragDropSource();
				}


				ImGui::SameLine();

				ImGui::BeginGroup();

				ImGui::Text(Item->Definition.DisplayName.Array());
				ImGui::Text("x");
				ImGui::SameLine();
				ImGui::Text(string{Item->Count} .Array());

				ImGui::EndGroup();
			}

			ImGui::PopID();
		}

		ImGui::EndChild();

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ImguiItemDraggingID)) {

				item_instance* Inst = State->ItemDragging;
				item_hold* SourceHold = State->HoldItemDraggingFrom;

				ItemTransfer(Inst, SourceHold, Hold, Inst->Count);
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ImguiShipModuleUnequippingDraggingID)) {
				ship_module* Inst = State->ModuleUnequipping;

				ItemGive(Hold, item_id::sm_asteroid_miner, 1);
				ShipRemoveModule(Inst, State);
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::PopStyleColor();

	}
}