struct converter {
	converter_persistent* Persist;

	stepper Stepper;
	station* Owner;

	bool32 HasOrder()
	{
		return Persist->RunsCount > 0;
	}

	void Setup(station* Stat, converter_persistent* Per) {
		Owner = Stat;
		Persist = Per;
	}
};

struct station {
	station_persistent* Persist;

	vector2 Size;
	real64 Rotation;

	converter Converters[10];
	int32 ConvertersCount;

	item_hold Hold;
};


void StationDockShip(station * Station, ship * Ship);
