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

	// -----------------------------------------------------------------------------
#define ImGuiImageSize ImVec2(40, 40)

	char* ImguiItemDraggingID = "ITEM_INSTANCE";
	char* ImguiShipModuleUnequippingDraggingID = "SHIP_MODULE_UNEQUIP";

	const real64 UnitToMeters = 10.0f;
	// -----------------------------------------------------------------------------

	enum class scene { universe, skill_tree };

	typedef void(*journey_step_start_func)(ship* Ship, journey_step* JourneyStep, game::state* State);
	// returns true if finished
	typedef bool(*journey_step_func)(ship* Ship, journey_step* JourneyStep, real64 Time, game::state* State);

	struct journey_movement {
		// settings
		vector2 EndPosition;
		float EdgeRatio;

		real64 FullDistance;
		vector2 DirToEnd;
		vector2 StartPosition;
		real64 DistFromSidesToCoast;

		// Caching for optimzation
		real64 CachedTime;
		real64 CachedFuelToUse;
		real64 CachedFuelForce;
		vector2 CachedDirToTargetForce;
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

		bool Repeat;

		journey_step* AddStep()
		{
			journey_step* Step = &Steps[StepsCount++];
			Assert(StepsCount < ArrayCount(Steps));

			return Step;
		}

		void Execute()
		{
			InProgress = true;
			CurrentStep = -1;
		}
	};
	typedef void(*step_func)(void* SelfData, real64 time, game::state* State);

	struct stepper {
		void* SelfData;
		step_func Step;

		real64 WakeupTime;
	};

	struct stepper_ptr {
		stepper* Stp;
	};

	struct universe_time {
		stepper Stepper;
		real64 TimeMS;
	};

	MetaStruct enum class ship_module_id {
		none,
		asteroid_miner,
		count
	};

#include "Asteroid.h"
#include "Salvage.h"
#include "Item.h"
#include "Recipe.h"
#include "Ship.h"
#include "Station.h"
#include "SkillTree.h"

	skill_bonuses* TreeBonusesTotal = {};

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

	struct editor_state {
		bool32 EditorMode;

		bool32 ItemWindowOpen;
		item_hold DebugHold;
		item_instance DebugItemDragging;

		bool32 SkillNodeWindowOpen;
		skill_node* NodeSelected;

		bool Paused;
	};

	struct state {
		scene Scene;

		skill_node SkillNodes[100];
		int32 SkillNodesCount;

		skill_bonuses TreeBonusesTotal;
		int64 Knowledge;

		universe_time UniverseTime;
		selection Selections[100];
		selectable* Hovering;
		skill_node* NodeHovering = {};

		ship Ships[100];

		asteroid_cluster Asteroids[100];
		int32 ClustersCount;

		salvage Salvages[100];
		int32 SalvagesCount;

		station Stations[100];
		int32 StationsCount;

		real32 Zoom = 1.0f;
		real32 ZoomTarget;

		stepper* Steppers[1000];
		int32 SteppersCount;

		list_head* SleepingSteppers;

		selectable Selectables[1024];
		int32 SelectablesCount;

		bool32 ForwardSimulating;

		item_instance* ItemDragging;
		item_hold* HoldItemDraggingFrom;
		ship_module* ModuleUnequipping;
	};
};

#endif