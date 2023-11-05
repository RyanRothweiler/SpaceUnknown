enum class item_hold_type {
	ship_cargo, station_cargo, fuel_tank
};

struct item_hold {

	item_hold_persistent* Persist;

	real64 MassCurrent;
	changed_flag MassChanged;

	item_hold_type Type;

	// Always use GetMassLimit()
	
	// base value 
	int64 MassLimitBase;

	// Modification from any outside influence (such as ship modules)
	int64 MassLimitMod;

	int64 GetMassLimit() {
		if (Type == item_hold_type::ship_cargo) {
			return MassLimitBase + TreeBonusesTotal->CargoSize + MassLimitMod;
		} else {
			return MassLimitBase + MassLimitMod;
		}
	}

	void Setup(int64 ML, item_hold_type TY, item_hold_persistent* Per)
	{
		Persist = Per;
		Type = TY;

		MassLimitBase = ML;
		MassChanged.RegisterConsumer();

		// Maybe this can be moved to only happen once on initialize? This if does the same thing but maybe kinda janky
		if (Persist->GUID == 0) {
			Persist->GUID = PlatformApi.GetGUID();
		}
	}

	void ConsumeFuel(real64 Count) {
		Assert(Persist->Items[0].ID == item_id::stl);
		Persist->Items[0].Count -= Count;
		MassChanged.MarkChanged();
	}

	r64 GetFuelLevel() {
		if (Persist->Items[0].ID == item_id::stl) {
			return Persist->Items[0].Count;
		}
		return 0;
	}

	void SetFuelLevel(r64 Count) { 
		Assert(Persist->Items[0].Count == 0);

		Persist->Items[0].ID = item_id::stl;
		Persist->Items[0].Count = Count;
		MassChanged.MarkChanged();
	}
};
