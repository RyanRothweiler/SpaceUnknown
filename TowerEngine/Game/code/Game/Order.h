MetaStruct enum class recipe_id {
	venigen_stl,
	count
};

struct item_count {
	item_id ID;
	int32 Count;
};

struct recipe {
	item_count Inputs[100];
	int32 InputsCount;

	item_count Outputs[100];
	int32 OutputsCount;

	real64 DurationMS;

	void RegisterInput(item_id ID, int32 Count)
	{
		Assert(InputsCount < ArrayCount(Inputs));
		Inputs[InputsCount].ID = ID;
		Inputs[InputsCount].Count = Count;
		InputsCount++;
	}

	void RegisterOutput(item_id ID, int32 Count)
	{
		Assert(OutputsCount < ArrayCount(Outputs));
		Outputs[OutputsCount].ID = ID;
		Outputs[OutputsCount].Count = Count;
		OutputsCount++;
	}
};