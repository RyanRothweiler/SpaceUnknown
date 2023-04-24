struct converter {
	converter_persistent Persist;

	stepper Stepper;

	bool32 IsRunning;

	station* Owner;

	bool32 HasOrder()
	{
		return Persist.RunsCount > 0;
	}
};

struct station {
	station_persistent* Persist;

	vector2 Position;
	vector2 Size;
	real64 Rotation;

	converter Converters[10];
	int32 ConvertersCount;

	item_hold Hold;

	int32 DockedCount;
};