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
	item_definition Def = GetItemDefinition(ItemID);
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

void ItemDisplayHold(item_hold* Hold, ship* Ship)
{
	int64 CargoWeight = (int64)Hold->MassCurrent;
	string CargoTitle = "Cargo (" + string{CargoWeight} + "/" + string{(int64)Hold->MassLimit} + ")(t)###CARGO";
	if (ImGui::CollapsingHeader(CargoTitle.Array())) {
		for (int i = 0; i < ArrayCount(Hold->Items); i++) {
			item_instance* Item = &Hold->Items[i];
			if (Item->Count > 0) {
				ImGui::Text(Item->Definition.DisplayName.Array());
				ImGui::SameLine();
				ImGui::Text("x");
				ImGui::SameLine();
				ImGui::Text(string{Item->Count} .Array());

				if (Ship != GameNull && Ship->Status == ship_status::docked) {
					ImGui::SameLine();
					if (ImGui::Button(">> To Station >>")) {
						ItemTransfer(Item, &Ship->StationDocked->Hold, Item->Count);
					}
				}
			}
		}
	}
}