const real64 fuelForcePerGallon = 8.0f;

struct ship_definition {
	real64 FuelRateGallonsPerSecond;
};

struct ship {
	bool32 Using;
	ship_definition Definition;

	vector2 Position;
	vector2 Velocity;
	vector2 Size;
	real64 Rotation;

	bool32 IsMoving;
	ship_journey CurrentJourney;

	real64 FuelGallons;

	stepper Stepper;
};
