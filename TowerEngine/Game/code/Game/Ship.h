//const real64 fuelForcePerGallon = 10000.0f;
const real64 fuelForcePerGallon = 0.01f;

struct ship_definition {
	real64 FuelRateGallonsPerSecond;
};

struct ship {
	bool32 Using;
	ship_definition Definition;

	vector2 Position;
	vector2 Velocity;
	vector2 Size;

	real64 DistFromSidesToCoast;
	vector2 JourneyStartPos;
	vector2 TargetPos;
	bool32 Moving;

	real64 FuelGallons;

	stepper Stepper;
};
