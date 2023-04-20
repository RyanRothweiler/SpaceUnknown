
struct item_hold {

	item_hold_persistent Persist;

	real64 MassCurrent;
	changed_flag MassChanged;

	int64 MassLimit;

	// Add cargo weight
	void UpdateMass()
	{
		MassChanged.MarkChanged();

		MassCurrent = 0;
		for (int i = 0; i < ArrayCount(Persist.Items); i++) {
			if (Persist.Items[i].Count > 0) {
				MassCurrent += Persist.Items[i].Def->Mass * Persist.Items[i].Count;
			}
		}
	}

	void Setup(int64 ML)
	{
		MassLimit = ML;
		Persist.GUID = StringHash(PlatformApi.GetGUID());
		MassChanged.RegisterConsumer();
	}

	real64 GetFuel()
	{
		// Assume the only item here is the stl fuel
		if (Persist.Items[0].ID == item_id::stl) {
			return Persist.Items[0].Count * Persist.Items[0].Def->Mass;
		}
		return 0;
	}

	void ConsumeFuel(real64 Count)
	{
		//Assert(Items[0].Definition.ID == item_id::stl);
		Persist.Items[0].Count -= Count;
	}
};