MetaStruct enum class station_service {
	refinery, shipyard,
	count
};

MetaStruct enum class recipe_id {
	venigen_stl,
	

	count,
	none,
};

struct item_count {
	item_id ID;
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
		Inputs[InputsCount].ID = ItemID;
		Inputs[InputsCount].Count = Count;
		InputsCount++;
	}

	void RegisterOutput(item_id ItemID, real64 Count)
	{
		Assert(OutputsCount < ArrayCount(Outputs));
		Outputs[OutputsCount].ID = ItemID;
		Outputs[OutputsCount].Count = Count;
		OutputsCount++;
	}
};

struct recipe_list {
	recipe_id IDs[100];
	int32 Count;
};