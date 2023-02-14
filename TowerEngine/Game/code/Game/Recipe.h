MetaStruct enum class station_service {
	refinery, shipyard,
	count
};

MetaStruct enum class recipe_id {
	venigen_stl,

	ship_advent,

	count,
	none,
};

MetaStruct enum class recipe_member_type {
	item, ship
};

struct item_count {
	recipe_member_type Type;

	item_id ItemID;
	ship_id ShipID;

	real64 Count;
};

struct recipe {
	station_service ServiceRequired;
	recipe_id ID;

	item_count Inputs[100];
	int32 InputsCount;
	item_count Outputs[100];
	int32 OutputsCount;

	real64 DurationMS;

	void RegisterInput(item_id ItemID, real64 Count)
	{
		Assert(InputsCount < ArrayCount(Inputs));
		Inputs[InputsCount].ItemID = ItemID;
		Inputs[InputsCount].Count = Count;
		Inputs[InputsCount].Type = recipe_member_type::item;
		InputsCount++;
	}

	void RegisterOutput(item_id InputID, real64 Count)
	{
		Assert(OutputsCount < ArrayCount(Outputs));
		Outputs[OutputsCount].ItemID = InputID;
		Outputs[OutputsCount].Count = Count;
		Outputs[OutputsCount].Type = recipe_member_type::item;
		OutputsCount++;
	}

	void RegisterOutput(ship_id InputID, real64 Count)
	{
		Assert(OutputsCount < ArrayCount(Outputs));
		Outputs[OutputsCount].ShipID = InputID;
		Outputs[OutputsCount].Count = Count;
		Outputs[OutputsCount].Type = recipe_member_type::ship;
		OutputsCount++;
	}
};

struct recipe_list {
	recipe_id IDs[100];
	int32 Count;
};