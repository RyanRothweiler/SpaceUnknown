#pragma once

#ifndef GAME_H
#define GAME_H

namespace game {

	const real64 UnitToMeters = 10.0f;

	struct ship_journey {
		vector2 EndPosition;
		vector2 StartPosition;
		real64 DistFromSidesToCoast;
	};

	struct stepper {
		void* SelfData;
		void(*Step)(void* SelfData, real64 time);
	};

#include "Ship.h"

	struct state {
		ship Ships[100];
		ship* ShipSelected;

		real32 Zoom = 1.0f;
		real32 ZoomTarget;

		stepper* Steppers[1000];
		int32 SteppersCount;
	};
};

#endif