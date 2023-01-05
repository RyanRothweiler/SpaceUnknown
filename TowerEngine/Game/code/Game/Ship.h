const real64 fuelForcePerGallon = 8.0f;

MetaStruct enum class ship_status {
	idle, moving, docking, undocking, docked
};

struct ship_module_definition {
	string DisplayName;
	real64 ActivationTimeMS;
	real64 ActivationRange;
};

struct ship;

struct ship_module {
	ship_module_definition Definition;
	stepper Stepper;
	real64 ActivationTimerMS;
	ship* Owner;

	asteroid* Target;
};

struct ship_definition {
	real64 FuelTankGallons;
	real64 FuelRateGallonsPerSecond;
	int64 Mass;
};

struct ship {
	ship_status Status;

	station* StationDocked;

	int64 CurrentMassTotal;

	bool32 Using;
	ship_definition Definition;

	ship_module Modules[50];
	int32 ModulesCount;

	item_hold Hold;

	vector2 Position;
	vector2 Velocity;
	vector2 Size;
	real64 Rotation;

	bool32 IsMoving;
	ship_journey CurrentJourney;

	real64 FuelGallons;

	stepper Stepper;
};
