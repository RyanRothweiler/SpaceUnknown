
namespace journey_methods {

	journey_step_start_func Start(journey_step_type Type) {
		switch (Type) {
			case (journey_step_type::dock_undock): return &ShipDockUndockStart; break;
			case (journey_step_type::movement): return &ShipMovementStart; break;
		}
		return GameNull;
	}

	journey_step_func Step(journey_step_type Type) {
		switch (Type) {
			case (journey_step_type::dock_undock): return &ShipDockUndockStep; break;
			case (journey_step_type::movement): return &ShipMovementStep; break;
		}
		return GameNull;
	}

};
