struct ship;

const real64 fuelForcePerGallon = 7.0f;

enum class ship_module_slot_type {
	industrial, structural, science,
	count
};

struct ship_module_definition {
	ship_module_id ID;
	item_id ItemID;

	step_func ActivationStepMethod;

	ship_module_slot_type SlotType;
	string DisplayName;
	i64 ActivationTimeMS;

	real64 ActivationRange;
	color ActivationRangeDisplayColor;

	int32 CargoAddition;

	union {
		struct {
			real32 ReductionMinutes;
		} Foreman;
	};
};

struct ship_module {
	ship_module_persistent* Persist;

	ship_module_definition Definition;
	stepper Stepper;
}; 

struct ship_definition {
	ship_id ID;

	loaded_image* Icon;

	int64 FuelTankMassLimit;
	real64 FuelRateMassPerSecond;
	int64 Mass;
	r32 RadarRadius;

	int32 HoldMass;

	ship_module_slot_type SlotTypes[64];
	int32 SlotsCount;
};

struct ship {
	ship_persistent* Persist;
	ship_definition Definition;

	// TODO rename this to just modules
	ship_module EquippedModules[16];

	item_hold Hold;
	item_hold FuelTank;

	vector2 Size;

	// Set anywhere outside this 
	r64 IndustrialActivationReductionMinutes;

	stepper Stepper;

	// Use method to get this most updated
	real64 CurrentMassTotal;
};


struct ships_list {
	ship* List[100];
	int32 ListCount;
};

void ShipUpdateMass(ship* Ship);
void ShipRemoveModule(ship_module* Module, state* State);
ship* ShipSetup(ship* Ship, ship_persistent* Persist, state* State);
ship* ShipCreate(state* State, ship_id Type);
