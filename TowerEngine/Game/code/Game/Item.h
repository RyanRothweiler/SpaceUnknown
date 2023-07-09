struct item_hold {

	item_hold_persistent* Persist;

	real64 MassCurrent;
	changed_flag MassChanged;

	int64 MassLimit;
	bool32 Ship;

	int64 GetMassLimit() {
		if (Ship) {
			return MassLimit + TreeBonusesTotal->CargoSize;
		} else {
			return MassLimit;
		}
	}

	void Setup(int64 ML, b32 IsShip, item_hold_persistent* Per)
	{
		Persist = Per;
		Ship = IsShip;

		MassLimit = ML;
		Persist->GUID = PlatformApi.GetGUID();
		MassChanged.RegisterConsumer();
	}

	void ConsumeFuel(real64 Count)
	{
		// TODO this should check that the first item is actually fuel
		//Assert(Items[0].Definition.ID == item_id::stl);
		Persist->Items[0].Count -= Count;
	}
};
