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

	enum class selection_type { none, ship, station };

	struct selection;

	typedef void(*selection_update_func)(engine_state* EngineState, game_input* Input);
	typedef void(*selection_on_func)(engine_state* EngineState, game_input* Input);
	struct selectable {

		void* Data;
		vector2* Center;
		vector2* Size;

		selection_update_func SelectionUpdate;
		selection_on_func OnSelection;
		selection_type Type;
	};

	struct selection {

		selectable* Selectable;

		bool32 None() { return Selectable == GameNull; }
		bool32 IsShip() { return Selectable != GameNull && Selectable->Type == selection_type::ship; }
		bool32 IsStation() { return Selectable != GameNull && Selectable->Type == selection_type::station; }

		ship* GetShip()
		{
			Assert(Selectable->Type == selection_type::ship);
			return (ship*)Selectable->Data;
		}

		station* GetStation()
		{
			Assert(Selectable->Type == selection_type::station);
			return (station*)Selectable->Data;
		}

		void Clear()
		{
			Selectable = {};
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

		selectable Selectables[1024];
		int32 SelectablesCount;
	};
};

#endif