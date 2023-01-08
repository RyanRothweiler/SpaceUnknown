#pragma once

#ifndef GAME_H
#define GAME_H

#include <ctime>
#include <chrono>

namespace game {

	struct state;
	struct ship;
	struct journey_step;
	struct station;

	const real64 UnitToMeters = 10.0f;

	struct editor_state {
		bool32 EditorMode;
		bool Paused;
	};

	typedef void(*journey_step_start_func)(ship* Ship, journey_step* JourneyStep, game::state* State);
	// returns true if finished
	typedef bool(*journey_step_func)(ship* Ship, journey_step* JourneyStep, real64 Time, game::state* State);

	struct journey_movement {
		// settings
		vector2 EndPosition;
		float EdgeRatio;

		// precalc and accumulations for efficiency
		vector2 StartPosition;
		real64 DistFromSidesToCoast;
		real64 FullDistance;
		vector2 DirToEnd;
		real64 CurrDistToEnd;
		real64 CurrVelocityMag;
	};

	struct journey_dock_undock {
		station* Station;
		real64 TimeAccum;
	};

	MetaStruct enum class journey_step_type { movement, dock_undock };
	struct journey_step {
		journey_step_type Type;

		union {
			journey_movement Movement;
			journey_dock_undock DockUndock;
		};

		journey_step_start_func Start;
		journey_step_func Step;
	};

	struct ship_journey {
		bool32 InProgress;

		journey_step Steps[100];
		int32 CurrentStep;
		int32 StepsCount;

		journey_step* AddStep()
		{
			journey_step* Step = &Steps[StepsCount++];
			Assert(StepsCount < ArrayCount(Steps));

			return Step;
		}
	};

	typedef void(*step_func)(void* SelfData, real64 time, game::state* State);

	struct stepper {
		void* SelfData;
		step_func Step;
	};

	struct universe_time {
		stepper Stepper;
		real64 TimeMS;
	};

#define ImGuiImageSize ImVec2(40, 40)

	char* ImguiItemDraggingID = "ITEM_INSTANCE";
	char* ImguiShipModuleUnequippingDraggingID = "SHIP_MODULE_UNEQUIP";

#include "Asteroid.h"
#include "Item.h"
#include "Ship.h"
#include "Station.h"

	// This also dictates the priority
	enum class selection_type { none, ship, station };

	struct selection;

	typedef void(*selection_update_func)(selection* Sel, engine_state* EngineState, game_input* Input);
	typedef void(*selection_on_func)(selection* Sel, engine_state* EngineState, game_input* Input);
	struct selectable {

		bool32 Selected;
		void* Data;
		vector2* Center;
		vector2* Size;

		vector2 InfoWindowPos;

		selection_update_func SelectionUpdate;
		selection_on_func OnSelection;
		selection_type Type;

		ship* GetShip()
		{
			Assert(Type == selection_type::ship);
			return (ship*)Data;
		}

		station* GetStation()
		{
			Assert(Type == selection_type::station);
			return (station*)Data;
		}
	};

	struct selection {

		selectable* Current;

		bool32 None() { return Current == GameNull; }
		bool32 IsShip() { return Current != GameNull && Current->Type == selection_type::ship; }
		bool32 IsStation() { return Current != GameNull && Current->Type == selection_type::station; }

		ship* GetShip() { return Current->GetShip(); }
		station* GetStation() { return Current->GetStation(); }

		void Clear()
		{
			Current->Selected = false;
			Current = {};
		}
	};

	struct state {
		universe_time UniverseTime;
		selection Selections[100];
		selectable* Hovering;

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

		item_instance* ItemDragging;
		ship_module* ModuleUnequipping;
	};
};

#endif