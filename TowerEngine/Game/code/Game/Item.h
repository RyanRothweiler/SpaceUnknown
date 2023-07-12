enum class item_hold_type {
	ship_cargo, station_cargo, fuel_tank
};

struct item_hold {

	item_hold_persistent* Persist;

	real64 MassCurrent;
	changed_flag MassChanged;

	item_hold_type Type;

	// Always use GetMassLimit()
	int64 MassLimitBase;
	int64 GetMassLimit() {
		if (Type == item_hold_type::ship_cargo) {
			return MassLimitBase + TreeBonusesTotal->CargoSize;
		} else {
			return MassLimitBase;
		}
	}

	void Setup(int64 ML, item_hold_type TY, item_hold_persistent* Per)
	{
		Persist = Per;
		Type = TY;

		MassLimitBase = ML;
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
