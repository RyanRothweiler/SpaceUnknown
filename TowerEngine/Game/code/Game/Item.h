// sm is short for ship_module. That is checked so its required to mark ship modules. The name matters
MetaStruct enum class item_id {
	venigen,
	stl,
	sm_asteroid_miner,
	count,
};

struct item_definition {
	item_id ID;
	string DisplayName;
	bool32 Stackable;
	int64 Mass;

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

struct item_hold {

	string GUID;

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
		GUID = PlatformApi.GetGUID();
		MassChanged.RegisterConsumer();
	}
};