MetaStruct enum class item_id {
	venigen,
};

struct item_definition {
	item_id ID;
	string DisplayName;
	bool32 Stackable;
	int64 Mass;
};

MetaStruct enum class item_transfer_target { ship, station };

struct item_instance {
	stepper TransferStepper;
	item_transfer_target TransferTarget;
	ship* ToShipTransfer;
	station* ToStationTransfer;

	item_definition Definition;
	int32 Count;
};