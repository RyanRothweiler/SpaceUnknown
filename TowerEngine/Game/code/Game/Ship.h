struct ship;

const real64 fuelForcePerGallon = 7.0f;

MetaStruct enum class ship_module_slot_type {
	industrial, structural, science,
	count
};

struct ship_module_slot_definition {
	ship_module_slot_type Type;
	loaded_image* Icon;
	string DisplayName;
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

	i32 Yield;

	int32 CargoAddition;

	union {
		struct {
			real32 ReductionMinutes;
		} Foreman;

		struct {
			r32 RangeAddition;
		} Radar;
	};
};

struct ship_module {
	ship_module_persistent* Persist;

	ship_module_definition Definition;
	stepper Stepper;
}; 

struct ship_definition {
	ship_id ID;
	string DisplayName;

	loaded_image* Icon;

	int64 FuelTankMassLimit;
	real64 FuelRateMassPerSecond;
	int64 Mass;
	r32 RadarRadius;

	int32 HoldMass;

	ship_module_slot_type SlotTypes[64];
	int32 SlotsCount;

	r32 Size;

	union {
		struct {
			r32 IndustrialActivationReductionPerc;
		} Prospector;
	};
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

	b32 CreatingMovement;

	// Use method to get this most updated
	real64 CurrentMassTotal;

	r32 GetRadarRadius() {
		r32 Ret = Definition.RadarRadius + TreeBonusesTotal->ShipRadarRangeAddition;
		
		for (int i = 0; i < ArrayCount(EquippedModules); i++) {
			ship_module* Mod = &EquippedModules[i];
			if (Mod->Persist->Filled) { 
				Ret += Mod->Definition.Radar.RangeAddition;
			}
		}

		return Ret;
	}
};

struct ships_list {
	ship* List[100];
	int32 ListCount;
};

void ShipUpdateMass(ship* Ship);
void ShipRemoveModule(ship_module* Module, state* State);
ship* ShipSetup(ship* Ship, ship_persistent* Persist, state* State);
ship* ShipCreate(state* State, ship_id Type);
