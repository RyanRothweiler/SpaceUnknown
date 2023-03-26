// sm is short for ship_module. That is checked so its required to mark ship modules. The name matters
MetaStruct enum class item_id {
	venigen 				= 0,
	pyrexium 				= 1,
	stl 					= 2,
	sm_asteroid_miner 		= 3,
	sm_salvager_i 			= 4,
	count 					= 5,
};

struct item_definition {
	item_id ID;
	string DisplayName;
	bool32 Stackable;
	real64 Mass;

	loaded_image* Icon;

	ship_module_id ShipModuleID;

	bool32 IsModule()
	{
		return ShipModuleID != ship_module_id::none;
	}
};

MetaStruct struct item_instance_persistent {
	item_id ID;
	real64 Count;

	item_definition* Def;
};

MetaStruct struct item_hold_persistent {
	int64 GUID;
	item_instance_persistent Items[256];
};

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