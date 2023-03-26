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

struct item_instance {
	item_definition Definition;
	real64 Count;
};

MetaStruct struct item_instance_persistent {
	item_id ID;
	int32 Count;
};

MetaStruct struct item_hold_persistent {
	int64 GUID;
};

struct item_hold {

	item_hold_persistent Persist;

	real64 MassCurrent;
	changed_flag MassChanged;

	item_instance Items[256];
	int64 MassLimit;

	// Add cargo weight
	void UpdateMass()
	{
		MassChanged.MarkChanged();

		MassCurrent = 0;
		for (int i = 0; i < ArrayCount(Items); i++) {
			if (Items[i].Count > 0) {
				MassCurrent += Items[i].Definition.Mass * Items[i].Count;
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
		if (Items[0].Definition.ID == item_id::stl) {
			return Items[0].Count * Items[0].Definition.Mass;
		}
		return 0;
	}

	void ConsumeFuel(real64 Count)
	{
		//Assert(Items[0].Definition.ID == item_id::stl);
		Items[0].Count -= Count;
	}
};