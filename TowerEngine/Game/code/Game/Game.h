#pragma once

#ifndef GAME_H
#define GAME_H

#include <ctime>
#include <chrono>

#include "BuildConfig.h"

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

MetaStruct enum class persistent_pointer_type {
	none, station, ship, asteroid, salvage
};

MetaStruct struct persistent_pointer {
 	persistent_pointer_type Type;
	uint32 GUID;
	void* Data;
};

MetaStruct enum class world_target_type {
	none, asteroid, salvage
};

struct asteroid;
struct salvage;

MetaStruct struct world_target_persistent {
	world_target_type Type;

	persistent_pointer Asteroid;
	persistent_pointer Salvage;
};

enum class game_scene { universe, skill_tree };

typedef void(*journey_step_start_func)(ship* Ship, journey_step* JourneyStep, state* State);
// returns true if finished
typedef bool(*journey_step_func)(ship* Ship, journey_step* JourneyStep, real64 Time, state* State);

MetaStruct struct journey_movement {
	// settings
	vector2 EndPosition;
	real64 EdgeRatio;

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

MetaStruct struct journey_dock_undock {
	persistent_pointer Station;
	real64 TimeAccum;
};

MetaStruct enum class journey_step_type { movement, dock_undock };
MetaStruct struct journey_step {
	journey_step_type Type;

	// should be unioned but meta parser can't handle that yet
	journey_movement Movement;
	journey_dock_undock DockUndock;
	// -----
};

MetaStruct struct ship_journey_estimate {
	real64 DurationMS;
	real64 FuelUsage;
};

MetaStruct struct ship_journey {
	bool32 InProgress;
	real64 UniverseTimeEndMS;

	journey_step Steps[10];
	int32 CurrentStep;
	int32 StepsCount;

	ship_journey_estimate Estimate; 

	bool32 Repeat;
};
 
typedef void(*step_func)(void* SelfData, real64 time, state* State);

struct stepper {
	void* SelfData;
	step_func Step;

	real64 WakeupTime;
};

struct stepper_ptr {
	stepper* Stp;
};


MetaStruct enum class ship_module_id {
	none,
	asteroid_miner			= 1,
	salvager_i				= 2,
	cargo_expansion_i		= 3,
	foreman_i				= 4,
};

// ITEM ------------------------------------------------------------------------

// sm is short for ship_module. That is checked so its required to mark ship modules. The name matters
MetaStruct enum class item_id {
	venigen 				= 0,
	pyrexium 				= 1,
	stl 					= 2,
	sm_asteroid_miner 		= 3,
	sm_salvager_i 			= 4,
	cargo_expansion_i		= 5,
	foreman_i				= 6,
};

struct item_definition {
	item_id ID;
	string DisplayName;
	string DisplayDescription;
	bool32 Stackable;
	real64 Mass;

	loaded_image* Icon;

	ship_module_id ShipModuleID;

	bool32 IsModule()
	{
		return ShipModuleID != ship_module_id::none;
	}
};

MetaStruct struct item_instance_persistent {
	item_id ID;
	real64 Count;
};

MetaStruct struct item_hold_persistent {

	// NOTE this is here only for UI.
	int64 GUID;

	item_instance_persistent Items[64];
};

MetaStruct enum class recipe_id {
	venigen_stl			= 0,
	ship_advent			= 1,
	sm_salvager_i		= 2,
	cargo_expansion_i	= 3,
	asteroid_miner_i	= 4,
	// increment count define

	count,
	none,
};
#define recipe_id_count 5
static_assert(recipe_id_count == (int)recipe_id::count, "recipe_id define must be incremented");

// -----------------------------------------------------------------------------

// Save Data persistence--------------------------------------------------------

MetaStruct struct color_persistent {
	real32 R;
	real32 G;
	real32 B;
	real32 A;
};

color ColorPersistToColor(color_persistent Col) { 
	color Ret = {};
	Ret.R = Col.R;
	Ret.G = Col.G;
	Ret.B = Col.B;
	Ret.A = Col.A;
	return Ret;
}

MetaStruct struct world_object {
	vector2 Position;
	vector2 Size;
	real64 Rotation;
	real64 RotationRate;
	color_persistent Color;

	loaded_image* Image;
};

// TODO rename this to ship_type
MetaStruct enum class ship_id {
	advent, 
};

MetaStruct enum class ship_status {
	idle, moving, docking, undocking, docked
};

MetaStruct struct ship_module_persistent {
	bool32 Filled;
	ship_module_id Type;

	world_target_persistent Target;
	persistent_pointer Owner;

	real64 ActivationTimerMS;
};

MetaStruct struct salvage_persistent {
	uint32 GUID;
	int32 KnowledgeAmount;

	world_object WorldObject;
	int64 SpriteIndex;

	vector2 SpawnCenter;
	real32 SpawnRadius;
};

MetaStruct struct asteroid_persistent {
	uint32 GUID;

	int32 OreCount;
	item_id OreItem;
	bool32 Using;

	world_object WorldObject;
	int64 SpriteIndex;
};

MetaStruct struct asteroid_cluster_persistent {
	item_id OreItem;
	vector2 Center;
	real64 Radius;

	asteroid_persistent Asteroids[32];
};

MetaStruct struct ship_persistent {
	int32 GUID;

	ship_status Status;
	ship_id Type;

	persistent_pointer StationDocked;

	ship_journey CurrentJourney;

	vector2 Position;
	real64 Rotation;
	vector2 Velocity;

	item_hold_persistent ItemHold;
	item_hold_persistent FuelHold;

	ship_module_persistent Modules[16];
};

MetaStruct struct converter_persistent {
	recipe_id RecipeID;
	real64 OrderTime;
	int32 RunsCount;
};

MetaStruct struct station_persistent {
	int32 GUID;

	vector2 Position;
	int32 DockedCount;

	item_hold_persistent ItemHold;
	converter_persistent Converters[10];
};

MetaStruct enum class skill_node_icon {
	ship_limit, 
	fuel_force,
	cargo_size,
	industrial_activation_time,
};


/*
	When adding new fields
	- Add to operators in SkillBonusesAdd
	- Add to displays SkillTreeImguiDisplayBonuses
	- Add to editor display
	- Implement effect
*/
MetaStruct struct skill_bonuses {

	real32 FuelForce;
	int32 ShipLimit;
	int32 CargoSize;
	int32 IndustrialActivationTimeMinutes;

	bool32 RecipeUnlocked[recipe_id_count];
};

// NOTE no pointers
MetaStruct struct save_file {
	int64 RealTimeSaved;
	real64 UniverseTimeMS;
	int64 SkillNodesIDUnlocked[256];

	skill_bonuses TreeBonuses;
	int64 Knowledge;

	int64 ShipsCount;
	ship_persistent Ships[256];

	int64 StationsCount;
	station_persistent Stations[256];

	int64 AsteroidClustersCount;
	asteroid_cluster_persistent AsteroidClusters[256];

	int64 SalvagesCount;
	salvage_persistent Salvages[256];
};
// -----------------------------------------------------------------------------

skill_bonuses* TreeBonusesTotal = {};

#include "WorldObject.h"
#include "Salvage.h"
#include "Item.h"
#include "Asteroid.h"
#include "Ship.h"
#include "Recipe.h"
#include "Station.h"
#include "SkillTree.h"
#include "InfoWindow.h"


// This also dictates the priority
enum class selection_type { none, ship, station, asteroid, salvage };

struct selection;
struct selectable;

typedef void(*selection_update_func)(selection* Sel, engine_state* EngineState, game_input* Input);
typedef void(*selection_on_func)(selection* Sel, engine_state* EngineState, game_input* Input);
typedef void(*selection_on_hover_func)(selectable* Sel, engine_state* EngineState, game_input* Input);

struct selectable {

	bool32 Selected;
	void* Data;
	vector2* Center;
	vector2* Size;

	vector2 InfoWindowPos;

	selection_update_func 		SelectionUpdate;
	selection_on_func 			OnSelection;
	selection_on_hover_func 	OnHover;
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

	asteroid* GetAsteroid()
	{
		Assert(Type == selection_type::asteroid);
		return (asteroid*)Data;
	}

	salvage* GetSalvage()
	{
		Assert(Type == selection_type::salvage);
		return (salvage*)Data;
	}
};

struct selection {

	selectable* Current;

	bool32 None() { return Current == GameNull; }
	bool32 IsShip() { return Current != GameNull && Current->Type == selection_type::ship; }
	bool32 IsStation() { return Current != GameNull && Current->Type == selection_type::station; }

	ship* GetShip() { return Current->GetShip(); }
	station* GetStation() { return Current->GetStation(); }
	asteroid* GetAsteroid() { return Current->GetAsteroid(); }
	salvage* GetSalvage() { return Current->GetSalvage(); }

	void Clear()
	{
		Current->Selected = false;
		Current = {};
	}
};

struct editor_state {
	bool32 EditorMode;

	bool32 ItemWindowOpen;
	bool32 ShipWindowOpen;

	item_hold_persistent DebugHoldPersist;
	item_hold DebugHold;
	item_instance_persistent DebugItemDragging;

	bool32 SkillNodeWindowOpen;
	skill_node* NodeSelected;

	bool Paused;
	bool HideFog;
};

struct state {

	info_window InfoWindows[10];

	b32 LoadedFromFile;
	save_file PersistentData;

	hash::table PersistentPointerSources;

	game_scene Scene;

	skill_node SkillNodes[100];
	int32 SkillNodesCount;

	stepper UniverseTimeStepper;

	selection Selections[100];
	selectable* Hovering;
	skill_node* NodeHovering = {};

	ship Ships[100];
	station Stations[100];
	asteroid_cluster AsteroidClusters[256];
	salvage Salvages[100];

	world_object* WorldObjects[1024];
	int32 WorldObjectsCount;

	real32 Zoom = 1.0f;
	real32 ZoomTarget;

	stepper* Steppers[1000];
	int32 SteppersCount;

	list_head* SleepingSteppers;

	selectable Selectables[1024];
	int32 SelectablesCount;

	bool32 ForwardSimulating;
	real64 ForwardSimulatingTimeRemaining;

	item_instance_persistent* ItemDragging;
	item_hold* HoldItemDraggingFrom;
	ship_module* ModuleUnequipping;

	// Could do better here by just having two cameras
	vector3 UniverseCamPos;
	real32 UniverseOrthoZoom;
	vector3 SkillTreeCamPos;
	real32 SkillTreeOrthoZoom;
};

#endif
