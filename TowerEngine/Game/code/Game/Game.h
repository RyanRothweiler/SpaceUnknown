#pragma once

#ifndef GAME_H
#define GAME_H

#include <ctime>
#include <chrono>

namespace game {

	const real64 UnitToMeters = 10.0f;

	struct editor_state {
		bool32 EditorMode;
		bool Paused;
	};

	struct ship_journey {
		vector2 EndPosition;
		vector2 StartPosition;
		real64 DistFromSidesToCoast;
		float EdgeRatio;
		vector2 DirToEnd;
	};

	struct state;
	typedef void(*step_func)(void* SelfData, real64 time, game::state* State);

	struct stepper {
		void* SelfData;
		step_func Step;
	};

	struct universe_time {
		stepper Stepper;
		real64 TimeMS;
	};

#include "Asteroid.h"
#include "Item.h"
#include "Ship.h"

	struct state {
		universe_time UniverseTime;

		ship Ships[100];
		ship* ShipSelected;

		asteroid_cluster Asteroids[100];
		int32 ClustersCount;

		real32 Zoom = 1.0f;
		real32 ZoomTarget;

		stepper* Steppers[1000];
		int32 SteppersCount;

		bool ShipInfoWindowShowing;
	};
};

#endif