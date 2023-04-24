struct recipe_inputs_missing_return {
	item_count Items[100];
	int32 Count;
};

recipe RecipeGetDefinition(recipe_id ID)
{
	return Globals->AssetsList.RecipeDefinitions[(int)ID];
}

recipe_inputs_missing_return RecipeInputsMissing(recipe* Order, item_hold* Source)
{
	recipe_inputs_missing_return Ret = {};

	for (int i = 0; i < Order->InputsCount; i++) {
		item_count* C = &Order->Inputs[i];

		bool32 Found = false;

		for (int h = 0; h < ArrayCount(Source->Persist->Items) && !Found; h++) {

			// If we have the item at all
			if (Source->Persist->Items[h].ID == C->ItemID) {
				Found = true;

				// If we don't have enough, then add
				if (Source->Persist->Items[h].Count < C->Count) {
					Ret.Items[Ret.Count].ItemID = C->ItemID;
					Ret.Items[Ret.Count].Count = C->Count - Source->Persist->Items[h].Count;
					Ret.Count++;
				}
			}
		}

		if (!Found) {
			Ret.Items[Ret.Count].ItemID = C->ItemID;
			Ret.Items[Ret.Count].Count = C->Count;
			Ret.Count++;
		}
	}

	return Ret;
}