
void ItemHoldUpdateMass(item_hold* Hold)
{
	// Add cargo weight
	Hold->MassChanged.MarkChanged();

	Hold->MassCurrent = 0;
	for (int i = 0; i < ArrayCount(Hold->Persist->Items); i++) {
		if (Hold->Persist->Items[i].Count > 0) {
			item_definition* Def = GetItemDefinition(Hold->Persist->Items[i].ID);
			Hold->MassCurrent += Def->Mass * Hold->Persist->Items[i].Count;
		}
	}
}

real64 ItemHoldGetFuel(item_hold* Hold)
{
	// Assume the only item here is the stl fuel
	if (Hold->Persist->Items[0].ID == item_id::stl) {
		item_definition* Def = GetItemDefinition(Hold->Persist->Items[0].ID);
		return Hold->Persist->Items[0].Count * Def->Mass;
	}
	return 0;
}

// Add item to a stack without exceeding the cargo mass limit
real64 ItemStackGive(item_hold* Hold, item_instance_persistent* Inst, item_definition* Def, real64 Count)
{
	real64 NewMass = Hold->MassCurrent + (Def->Mass * Count);
	if (NewMass <= Hold->GetMassLimit()) {
		Inst->Count += Count;
		return Count;
	} else {
		real64 MassAvail = Hold->GetMassLimit() - Hold->MassCurrent;
		int32 CountCanGive = (int32)(MassAvail / Def->Mass);
		Inst->Count += CountCanGive;
		return CountCanGive;
	}
}

void ItemHoldConsumeItem(item_hold* Hold, int ItemIndex, real64 Count) { 
	Assert(ItemIndex >= 0);
	Assert(ArrayCount(Hold->Persist->Items) > ItemIndex);

	Hold->Persist->Items[ItemIndex].Count -= Count;

	// Cannot go below 0
	if (Hold->Persist->Items[ItemIndex].Count < 0) {
		Hold->Persist->Items[ItemIndex].Count = 0;
	}

	ItemHoldUpdateMass(Hold);
}

// returns amount given
real64 ItemGive(item_hold* Hold, item_id ItemID, real64 Count)
{
	item_definition* Def = &Globals->AssetsList.ItemDefinitions[(int)ItemID];
	real64 AmountGiven = 0;

	if (Def->Stackable) {

		// Add to existing stack
		for (int i = 0; i < ArrayCount(Hold->Persist->Items); i++) {
			if (Hold->Persist->Items[i].Count > 0 && Hold->Persist->Items[i].ID == ItemID) {
				AmountGiven = ItemStackGive(Hold, &Hold->Persist->Items[i], Def, Count);
				goto end;
			}
		}

		// Make new stack
		for (int i = 0; i < ArrayCount(Hold->Persist->Items); i++) {
			if (Hold->Persist->Items[i].Count <= 0) {

				Hold->Persist->Items[i].Count = 0;
				Hold->Persist->Items[i].ID = ItemID;

				AmountGiven = ItemStackGive(Hold, &Hold->Persist->Items[i], Def, Count);
				goto end;
			}
		}

		ConsoleLog("Hold is full");
		goto end;
	}

	// Not stackable, so make new stacks
	{
		// Verify we have space
		if (Hold->MassCurrent + Def->Mass > Hold->GetMassLimit()) { return 0; }

		// Give
		for (int c = 0; c < Count; c++) {
			for (int i = 0; i < ArrayCount(Hold->Persist->Items); i++) {
				if (Hold->Persist->Items[i].Count <= 0) {

					Hold->Persist->Items[i].ID = ItemID;
					Hold->Persist->Items[i].Count = 1;

					AmountGiven = 1;
					goto end;
				}
			}
		}
	}
end:

	ItemHoldUpdateMass(Hold);
	Save();
	return AmountGiven;
}

void ItemTransfer(item_instance_persistent* Inst, item_hold* Source, item_hold* Dest, real64 Count)
{
	real64 CountMoving = ItemGive(Dest, Inst->ID, Count);
	Inst->Count -= CountMoving;

	ItemHoldUpdateMass(Source);
	ItemHoldUpdateMass(Dest);
}

enum class item_hold_filter {
	any, stl,
};

void ItemDisplayHold(string Title, item_hold* Hold, state* State, game_input* Input, bool32 CanTransfer, item_hold_filter AllowedItems)
{
	int64 CargoWeight = (int64)Hold->MassCurrent;
	string CargoTitle = Title + " (" + string{CargoWeight} + "/" + string{(int64)Hold->GetMassLimit()} + ")(t)###" + string{Hold->Persist->GUID};
	if (ImGui::CollapsingHeader(CargoTitle.Array())) {

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 100));
		string ChildID = string{"itemchild"} + string{Hold->Persist->GUID};
		ImGui::BeginChild(ChildID.Array(), ImVec2(0, 300), true, ImGuiWindowFlags_None);

		static int ItemDestroying = -1;
		bool OpenDestroyPopup = false;

		for (int i = 0; i < ArrayCount(Hold->Persist->Items); i++) {

			ImGui::PushID(i);

			item_instance_persistent* Item = &Hold->Persist->Items[i];
			item_definition* Def = GetItemDefinition(Item->ID);

			int64 ptr = (int64)Item;
			if (Item->Count > 0) {

				if (ImGuiItemIcon(Item->ID, true)) {
					ImGui::CloseCurrentPopup();
					ItemDestroying = i;
					OpenDestroyPopup = true;
				}

				if (CanTransfer && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					State->ItemDragging = Item;
					State->HoldItemDraggingFrom = Hold;

					int D = 0;
					ImGui::SetDragDropPayload(ImguiItemDraggingID, &D, sizeof(D));

					ImGui::Image(
					    (ImTextureID)((int64)Def->Icon->GLID),
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

				ImGui::Text(Def->DisplayName.Array());
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
					Allowed = (State->ItemDragging->ID == item_id::stl);
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

		// destroy popup
		static char* DestroyAcceptWindow = "Destory";
		if (OpenDestroyPopup) { ImGui::OpenPopup(DestroyAcceptWindow); }
		if (ImGui::BeginPopupModal(DestroyAcceptWindow)) {
			ImGui::Text("Really destroy this item?");

			real32 HW = ImGui::GetWindowContentRegionWidth();
			if (ImGui::Button("Yes", ImVec2(HW * 0.5f, 0.0f))) {
				ItemHoldConsumeItem(Hold, ItemDestroying, Hold->Persist->Items[ItemDestroying].Count);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(HW * 0.5f, 0.0f))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopStyleColor();

	}
}
