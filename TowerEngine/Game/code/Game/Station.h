struct converter {
	recipe Order;
	stepper Stepper;
	real64 CurrentOrderTime;
	int32 RunsCount;

	bool32 HasOrder()
	{
		return RunsCount > 0;
	}
};

struct station {
	vector2 Position;
	vector2 Size;
	real64 Rotation;

	converter Converters[10];
	int32 ConvertersCount;

	item_hold Hold;

	int32 DockedCount;
};