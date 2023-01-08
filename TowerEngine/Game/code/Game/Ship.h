struct ship;

const real64 fuelForcePerGallon = 8.0f;

MetaStruct enum class ship_status {
	idle, moving, docking, undocking, docked
};

enum class ship_module_slot_type {
	industrial, structural,
	count
};

MetaStruct enum class ship_module_id {
	asteroid_miner,
	count
};

struct ship_module_definition {
	ship_module_id ID;
	ship_module_slot_type SlotType;
	string DisplayName;
	real64 ActivationTimeMS;
	real64 ActivationRange;
};

struct ship_module {
	bool32 Filled;

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

	ship_module_slot_type SlotTypes[64];
	int32 SlotsCount;
};

struct ship {
	ship_status Status;

	station* StationDocked;

	int64 CurrentMassTotal;

	bool32 Using;
	ship_definition Definition;

	ship_module EquippedModules[64];

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
