MetaStruct enum class item_id {
	venigen,
};

struct item_definition {
	item_id ID;
	string DisplayName;
	bool32 Stackable;
	int64 Mass;
};

struct item_instance {
	item_definition Definition;
	int32 Count;
};

struct item_hold {
	int64 MassCurrent;

	item_instance Items[256];
	int64 MassLimit;

	// Add cargo weightt
	void UpdateMass()
	{
		MassCurrent = 0;
		for (int i = 0; i < ArrayCount(Items); i++) {
			if (Items[i].Count > 0) {
				MassCurrent += Items[i].Definition.Mass * Items[i].Count;
			}
		}
	}
};