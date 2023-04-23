
struct item_hold {

	item_hold_persistent* Persist;

	real64 MassCurrent;
	changed_flag MassChanged;

	int64 MassLimit;

	void Setup(int64 ML, item_hold_persistent* Per)
	{
		Persist = Per;

		MassLimit = ML;
		Persist->GUID = StringHash(PlatformApi.GetGUID());
		MassChanged.RegisterConsumer();
	}

	void ConsumeFuel(real64 Count)
	{
		// TODO this should check that the first item is actually fuel
		//Assert(Items[0].Definition.ID == item_id::stl);
		Persist->Items[0].Count -= Count;
	}
};