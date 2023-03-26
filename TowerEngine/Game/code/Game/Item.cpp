// Add item to a stack without exceeding the cargo mass limit
real64 ItemStackGive(item_hold* Hold, item_instance_persistent* Inst, item_definition* Def, real64 Count)
{
	real64 NewMass = Hold->MassCurrent + (Def->Mass * Count);
	if (NewMass <= Hold->MassLimit) {
		Inst->Count += Count;
		return Count;
	} else {
		real64 MassAvail = Hold->MassLimit - Hold->MassCurrent;
		int32 CountCanGive = (int32)(MassAvail / Def->Mass);
		Inst->Count += CountCanGive;
		return CountCanGive;
	}
}

// returns amount given
real64 ItemGive(item_hold* Hold, item_id ItemID, real64 Count)
{
	item_definition* Def = &Globals->AssetsList.ItemDefinitions[(int)ItemID];
	real64 AmountGiven = 0;

	if (Def->Stackable) {
		// Add to existing stack
		for (int i = 0; i < ArrayCount(Hold->Persist.Items); i++) {
			if (Hold->Persist.Items[i].Count > 0 && Hold->Persist.Items[i].Def->ID == ItemID) {
				AmountGiven = ItemStackGive(Hold, &Hold->Persist.Items[i], Def, Count);
				goto end;
			}
		}

		// Make new stack
		for (int i = 0; i < ArrayCount(Hold->Persist.Items); i++) {
			if (Hold->Persist.Items[i].Count <= 0) {

				Hold->Persist.Items[i].Count = 0;
				Hold->Persist.Items[i].Def = Def;
				AmountGiven = ItemStackGive(Hold, &Hold->Persist.Items[i], Def, Count);
				goto end;
			}
		}

		ConsoleLog("Hold is full");
		goto end;
	}

	// Not stackable, so make new stacks
	{
		// Verify we have space
		if (Hold->MassCurrent + Def->Mass > Hold->MassLimit) { return 0; }

		// Give
		for (int c = 0; c < Count; c++) {
			for (int i = 0; i < ArrayCount(Hold->Persist.Items); i++) {
				if (Hold->Persist.Items[i].Count <= 0) {
					Hold->Persist.Items[i].Count = 1;
					Hold->Persist.Items[i].Def = Def;
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

void ItemTransfer(item_instance_persistent* Inst, item_hold* Source, item_hold* Dest, real64 Count)
{
	real64 CountMoving = ItemGive(Dest, Inst->Def->ID, Count);
	Inst->Count -= CountMoving;

	Source->UpdateMass();
	Dest->UpdateMass();
}

enum class item_hold_filter {
	any, stl,
};

void ItemDisplayHold(string Title, item_hold* Hold, state* State, game_input* Input, bool32 CanTransfer, item_hold_filter AllowedItems)
{
	int64 CargoWeight = (int64)Hold->MassCurrent;
	string CargoTitle = Title + " (" + string{CargoWeight} + "/" + string{(int64)Hold->MassLimit} + ")(t)###" + string{Hold->Persist.GUID};
	if (ImGui::CollapsingHeader(CargoTitle.Array())) {

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 100));
		string ChildID = string{"itemchild"} + string{Hold->Persist.GUID};
		ImGui::BeginChild(ChildID.Array(), ImVec2(0, 300), true, ImGuiWindowFlags_None);

		for (int i = 0; i < ArrayCount(Hold->Persist.Items); i++) {

			ImGui::PushID(i);

			item_instance_persistent* Item = &Hold->Persist.Items[i];
			int64 ptr = (int64)Item;
			if (Item->Count > 0) {

				ImGui::Image(
				    (ImTextureID)((int64)Item->Def->Icon->GLID),
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
					    (ImTextureID)((int64)Item->Def->Icon->GLID),
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

				ImGui::Text(Item->Def->DisplayName.Array());
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

				bool32 Allowed = true;
				if (AllowedItems == item_hold_filter::stl) {
					Allowed = (State->ItemDragging->Def->ID == item_id::stl);
				}

				if (Allowed) {
					item_instance_persistent* Inst = State->ItemDragging;
					item_hold* SourceHold = State->HoldItemDraggingFrom;

					ItemTransfer(Inst, SourceHold, Hold, Inst->Count);
				} else {
					// TODO dipslay error maybe
				}
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ImguiShipModuleUnequippingDraggingID)) {
				ship_module* Inst = State->ModuleUnequipping;

				ItemGive(Hold, Inst->Definition.ItemID, 1);
				ShipRemoveModule(Inst, State);
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::PopStyleColor();

	}
}