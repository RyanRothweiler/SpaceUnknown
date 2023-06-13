struct ship;

const real64 fuelForcePerGallon = 8.0f;

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
	real64 ActivationTimeMS;
	real64 ActivationRange;
};

enum class world_target_type {
	none, asteroid, salvage
};

struct world_target {
	world_target_type Type;

	union {
		asteroid* Asteroid;
		salvage* Salvage;
	};

	void Set(asteroid* Input)
	{
		Asteroid = Input;
		Type = world_target_type::asteroid;
	}

	void Set(salvage* Input)
	{
		Salvage = Input;
		Type = world_target_type::salvage;
	}

	void Clear()
	{
		Type = world_target_type::none;
	}

	bool32 HasTarget()
	{
		return Type != world_target_type::none;
	}

	asteroid* GetAsteroid()
	{
		Assert(Type == world_target_type::asteroid);
		return Asteroid;
	}

	salvage* GetSalvage()
	{
		Assert(Type == world_target_type::salvage);
		return Salvage;
	}

	vector2 GetTargetPosition()
	{
		switch (Type) {
			case world_target_type::asteroid: return Asteroid->WorldObject.Position; break;
			case world_target_type::salvage: return Salvage->WorldObject.Position; break;
				INVALID_DEFAULT
		}

		return {};
	}
};

struct ship_module {
	bool32 Filled;

	ship_module_definition Definition;
	stepper Stepper;
	real64 ActivationTimerMS;
	ship* Owner;

	world_target Target;
}; 

struct ship_definition {
	ship_id ID;

	loaded_image* Icon;

	int64 FuelTankMassLimit;
	real64 FuelRateMassPerSecond;
	int64 Mass;

	int32 HoldMass;

	ship_module_slot_type SlotTypes[64];
	int32 SlotsCount;
};

struct ship {
	ship_persistent* Persist;

	ship_definition Definition;

	ship_module EquippedModules[64];

	item_hold Hold;
	item_hold FuelTank;

	vector2 Velocity;
	vector2 Size;

	bool32 IsMoving;
	ship_journey CurrentJourney;

	stepper Stepper;

	// Use method to get this most updated
	real64 CurrentMassTotal;
};


void ShipUpdateMass(ship* Ship);
void ShipRemoveModule(ship_module* Module, state* State);
ship* ShipSetup(vector2 Pos, ship_id ID, state * State);
