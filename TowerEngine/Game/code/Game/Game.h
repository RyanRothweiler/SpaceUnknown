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
		// settings
		vector2 EndPosition;
		vector2 StartPosition;
		float EdgeRatio;

		// precalc and accumulations for efficiency
		real64 DistFromSidesToCoast;
		real64 FullDistance;
		vector2 DirToEnd;
		real64 CurrDistToEnd;
		real64 CurrVelocityMag;
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
#include "Station.h"

	struct selection {
		enum class type { none, ship, station };
		type Type;

		union data {
			ship* Ship;
			station* Station;
		} Data;

		bool32 None() { return Type == type::none; }
		bool32 IsShip() { return Type == type::ship; }
		bool32 IsStation() { return Type == type::station; }

		void Set(ship* Ship)
		{
			Data.Ship = Ship;
			Type = type::ship;
		}

		void Set(station* Station)
		{
			Data.Station = Station;
			Type = type::station;
		}

		void Clear()
		{
			Data = {};
			Type = type::none;
		}
	};

	struct state {
		universe_time UniverseTime;
		selection Selection;

		ship Ships[100];

		asteroid_cluster Asteroids[100];
		int32 ClustersCount;

		station Stations[100];
		int32 StationsCount;

		real32 Zoom = 1.0f;
		real32 ZoomTarget;

		stepper* Steppers[1000];
		int32 SteppersCount;

		bool ShipInfoWindowShowing;
	};
};

#endif