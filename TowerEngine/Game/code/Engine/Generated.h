#ifndef GENERATED 
#define GENERATED 
 
meta_member vector2_META[] { 
{meta_member_type::real64, "real64", "X", (uint64)&((vector2 *)0)->X, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::real64, "real64", "Y", (uint64)&((vector2 *)0)->Y, 0, sizeof(real64),{},{},{},{},{}},
}; 
 
void M_ALLOC__vector2(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 1; 
	SafeVoid->DataSize = sizeof(vector2); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(vector2)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(vector2)); 
};

vector2* M_GET__vector2(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 1) { 
		return (vector2*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_vector2 (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &vector2_META[0], ArrayCount(vector2_META), AccData);
}

void SaveDataFillShim_vector2 (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &vector2_META[0], ArrayCount(vector2_META), AccData);
}

void JsonFillStructShim_vector2 (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &vector2_META[0], ArrayCount(vector2_META), DataDest);
}

string glsl_type_NAME[] { 
"none",
"gl_mat4",
"gl_vec4",
"gl_vec3",
"gl_vec2",
"gl_sampler2D",
"gl_bool",
"gl_samplerCube",
"gl_float",
"gl_int",
}; 
string persistent_pointer_type_NAME[] { 
"none",
"station",
"ship",
"asteroid",
"salvage",
}; 
meta_member persistent_pointer_META[] { 
{meta_member_type::enumeration, "persistent_pointer_type", "Type", (uint64)&((persistent_pointer *)0)->Type, 0, sizeof(persistent_pointer_type),{},{},{},{},{}},
{meta_member_type::uint32, "uint32", "GUID", (uint64)&((persistent_pointer *)0)->GUID, 0, sizeof(uint32),{},{},{},{},{}},
}; 
 
void M_ALLOC__persistent_pointer(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 2; 
	SafeVoid->DataSize = sizeof(persistent_pointer); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(persistent_pointer)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(persistent_pointer)); 
};

persistent_pointer* M_GET__persistent_pointer(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 2) { 
		return (persistent_pointer*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_persistent_pointer (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &persistent_pointer_META[0], ArrayCount(persistent_pointer_META), AccData);
}

void SaveDataFillShim_persistent_pointer (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &persistent_pointer_META[0], ArrayCount(persistent_pointer_META), AccData);
}

void JsonFillStructShim_persistent_pointer (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &persistent_pointer_META[0], ArrayCount(persistent_pointer_META), DataDest);
}

string world_target_type_NAME[] { 
"none",
"asteroid",
"salvage",
}; 
meta_member world_target_persistent_META[] { 
{meta_member_type::enumeration, "world_target_type", "Type", (uint64)&((world_target_persistent *)0)->Type, 0, sizeof(world_target_type),{},{},{},{},{}},
{meta_member_type::custom, "persistent_pointer", "Asteroid", (uint64)&((world_target_persistent *)0)->Asteroid, 0, sizeof(persistent_pointer),&StructMetaFill_persistent_pointer, &JsonFillStructShim_persistent_pointer, &SaveDataFillShim_persistent_pointer, &persistent_pointer_META[0], ArrayCount(persistent_pointer_META) },
{meta_member_type::custom, "persistent_pointer", "Salvage", (uint64)&((world_target_persistent *)0)->Salvage, 0, sizeof(persistent_pointer),&StructMetaFill_persistent_pointer, &JsonFillStructShim_persistent_pointer, &SaveDataFillShim_persistent_pointer, &persistent_pointer_META[0], ArrayCount(persistent_pointer_META) },
}; 
 
void M_ALLOC__world_target_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 3; 
	SafeVoid->DataSize = sizeof(world_target_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(world_target_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(world_target_persistent)); 
};

world_target_persistent* M_GET__world_target_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 3) { 
		return (world_target_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_world_target_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &world_target_persistent_META[0], ArrayCount(world_target_persistent_META), AccData);
}

void SaveDataFillShim_world_target_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &world_target_persistent_META[0], ArrayCount(world_target_persistent_META), AccData);
}

void JsonFillStructShim_world_target_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &world_target_persistent_META[0], ArrayCount(world_target_persistent_META), DataDest);
}

meta_member journey_movement_META[] { 
{meta_member_type::custom, "vector2", "EndPosition", (uint64)&((journey_movement *)0)->EndPosition, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::real64, "real64", "EdgeRatio", (uint64)&((journey_movement *)0)->EdgeRatio, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::real64, "real64", "FullDistance", (uint64)&((journey_movement *)0)->FullDistance, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::custom, "vector2", "DirToEnd", (uint64)&((journey_movement *)0)->DirToEnd, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::custom, "vector2", "StartPosition", (uint64)&((journey_movement *)0)->StartPosition, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::real64, "real64", "DistFromSidesToCoast", (uint64)&((journey_movement *)0)->DistFromSidesToCoast, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::real64, "real64", "CachedTime", (uint64)&((journey_movement *)0)->CachedTime, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::real64, "real64", "CachedFuelToUse", (uint64)&((journey_movement *)0)->CachedFuelToUse, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::real64, "real64", "CachedFuelForce", (uint64)&((journey_movement *)0)->CachedFuelForce, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::custom, "vector2", "CachedDirToTargetForce", (uint64)&((journey_movement *)0)->CachedDirToTargetForce, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
}; 
 
void M_ALLOC__journey_movement(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 4; 
	SafeVoid->DataSize = sizeof(journey_movement); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(journey_movement)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(journey_movement)); 
};

journey_movement* M_GET__journey_movement(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 4) { 
		return (journey_movement*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_journey_movement (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &journey_movement_META[0], ArrayCount(journey_movement_META), AccData);
}

void SaveDataFillShim_journey_movement (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &journey_movement_META[0], ArrayCount(journey_movement_META), AccData);
}

void JsonFillStructShim_journey_movement (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &journey_movement_META[0], ArrayCount(journey_movement_META), DataDest);
}

meta_member journey_dock_undock_META[] { 
{meta_member_type::custom, "persistent_pointer", "Station", (uint64)&((journey_dock_undock *)0)->Station, 0, sizeof(persistent_pointer),&StructMetaFill_persistent_pointer, &JsonFillStructShim_persistent_pointer, &SaveDataFillShim_persistent_pointer, &persistent_pointer_META[0], ArrayCount(persistent_pointer_META) },
{meta_member_type::real64, "real64", "TimeAccum", (uint64)&((journey_dock_undock *)0)->TimeAccum, 0, sizeof(real64),{},{},{},{},{}},
}; 
 
void M_ALLOC__journey_dock_undock(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 5; 
	SafeVoid->DataSize = sizeof(journey_dock_undock); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(journey_dock_undock)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(journey_dock_undock)); 
};

journey_dock_undock* M_GET__journey_dock_undock(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 5) { 
		return (journey_dock_undock*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_journey_dock_undock (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &journey_dock_undock_META[0], ArrayCount(journey_dock_undock_META), AccData);
}

void SaveDataFillShim_journey_dock_undock (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &journey_dock_undock_META[0], ArrayCount(journey_dock_undock_META), AccData);
}

void JsonFillStructShim_journey_dock_undock (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &journey_dock_undock_META[0], ArrayCount(journey_dock_undock_META), DataDest);
}

string journey_step_type_NAME[] { 
"movement",
"dock_undock",
}; 
meta_member journey_step_META[] { 
{meta_member_type::enumeration, "journey_step_type", "Type", (uint64)&((journey_step *)0)->Type, 0, sizeof(journey_step_type),{},{},{},{},{}},
{meta_member_type::custom, "journey_movement", "Movement", (uint64)&((journey_step *)0)->Movement, 0, sizeof(journey_movement),&StructMetaFill_journey_movement, &JsonFillStructShim_journey_movement, &SaveDataFillShim_journey_movement, &journey_movement_META[0], ArrayCount(journey_movement_META) },
{meta_member_type::custom, "journey_dock_undock", "DockUndock", (uint64)&((journey_step *)0)->DockUndock, 0, sizeof(journey_dock_undock),&StructMetaFill_journey_dock_undock, &JsonFillStructShim_journey_dock_undock, &SaveDataFillShim_journey_dock_undock, &journey_dock_undock_META[0], ArrayCount(journey_dock_undock_META) },
}; 
 
void M_ALLOC__journey_step(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 6; 
	SafeVoid->DataSize = sizeof(journey_step); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(journey_step)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(journey_step)); 
};

journey_step* M_GET__journey_step(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 6) { 
		return (journey_step*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_journey_step (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &journey_step_META[0], ArrayCount(journey_step_META), AccData);
}

void SaveDataFillShim_journey_step (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &journey_step_META[0], ArrayCount(journey_step_META), AccData);
}

void JsonFillStructShim_journey_step (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &journey_step_META[0], ArrayCount(journey_step_META), DataDest);
}

meta_member ship_journey_META[] { 
{meta_member_type::bool32, "bool32", "InProgress", (uint64)&((ship_journey *)0)->InProgress, 0, sizeof(bool32),{},{},{},{},{}},
{meta_member_type::custom, "journey_step", "Steps", (uint64)&((ship_journey *)0)->Steps, 10, sizeof(journey_step),&StructMetaFill_journey_step, &JsonFillStructShim_journey_step, &SaveDataFillShim_journey_step, &journey_step_META[0], ArrayCount(journey_step_META) },
{meta_member_type::int32, "int32", "CurrentStep", (uint64)&((ship_journey *)0)->CurrentStep, 0, sizeof(int32),{},{},{},{},{}},
{meta_member_type::int32, "int32", "StepsCount", (uint64)&((ship_journey *)0)->StepsCount, 0, sizeof(int32),{},{},{},{},{}},
{meta_member_type::bool32, "bool32", "Repeat", (uint64)&((ship_journey *)0)->Repeat, 0, sizeof(bool32),{},{},{},{},{}},
}; 
 
void M_ALLOC__ship_journey(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 7; 
	SafeVoid->DataSize = sizeof(ship_journey); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(ship_journey)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(ship_journey)); 
};

ship_journey* M_GET__ship_journey(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 7) { 
		return (ship_journey*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_ship_journey (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &ship_journey_META[0], ArrayCount(ship_journey_META), AccData);
}

void SaveDataFillShim_ship_journey (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &ship_journey_META[0], ArrayCount(ship_journey_META), AccData);
}

void JsonFillStructShim_ship_journey (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &ship_journey_META[0], ArrayCount(ship_journey_META), DataDest);
}

string ship_module_id_NAME[] { 
"none",
"asteroid_miner",
"salvager_i",
"count",
}; 
string item_id_NAME[] { 
"venigen",
"pyrexium",
"stl",
"sm_asteroid_miner",
"sm_salvager_i",
"count",
}; 
meta_member item_instance_persistent_META[] { 
{meta_member_type::enumeration, "item_id", "ID", (uint64)&((item_instance_persistent *)0)->ID, 0, sizeof(item_id),{},{},{},{},{}},
{meta_member_type::real64, "real64", "Count", (uint64)&((item_instance_persistent *)0)->Count, 0, sizeof(real64),{},{},{},{},{}},
}; 
 
void M_ALLOC__item_instance_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 8; 
	SafeVoid->DataSize = sizeof(item_instance_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(item_instance_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(item_instance_persistent)); 
};

item_instance_persistent* M_GET__item_instance_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 8) { 
		return (item_instance_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_item_instance_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &item_instance_persistent_META[0], ArrayCount(item_instance_persistent_META), AccData);
}

void SaveDataFillShim_item_instance_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &item_instance_persistent_META[0], ArrayCount(item_instance_persistent_META), AccData);
}

void JsonFillStructShim_item_instance_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &item_instance_persistent_META[0], ArrayCount(item_instance_persistent_META), DataDest);
}

meta_member item_hold_persistent_META[] { 
{meta_member_type::int64, "int64", "GUID", (uint64)&((item_hold_persistent *)0)->GUID, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::custom, "item_instance_persistent", "Items", (uint64)&((item_hold_persistent *)0)->Items, 64, sizeof(item_instance_persistent),&StructMetaFill_item_instance_persistent, &JsonFillStructShim_item_instance_persistent, &SaveDataFillShim_item_instance_persistent, &item_instance_persistent_META[0], ArrayCount(item_instance_persistent_META) },
}; 
 
void M_ALLOC__item_hold_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 9; 
	SafeVoid->DataSize = sizeof(item_hold_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(item_hold_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(item_hold_persistent)); 
};

item_hold_persistent* M_GET__item_hold_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 9) { 
		return (item_hold_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_item_hold_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &item_hold_persistent_META[0], ArrayCount(item_hold_persistent_META), AccData);
}

void SaveDataFillShim_item_hold_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &item_hold_persistent_META[0], ArrayCount(item_hold_persistent_META), AccData);
}

void JsonFillStructShim_item_hold_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &item_hold_persistent_META[0], ArrayCount(item_hold_persistent_META), DataDest);
}

string recipe_id_NAME[] { 
"venigen_stl",
"ship_advent",
"sm_salvager_i",
"count",
"none",
}; 
meta_member color_persistent_META[] { 
{meta_member_type::real32, "real32", "R", (uint64)&((color_persistent *)0)->R, 0, sizeof(real32),{},{},{},{},{}},
{meta_member_type::real32, "real32", "G", (uint64)&((color_persistent *)0)->G, 0, sizeof(real32),{},{},{},{},{}},
{meta_member_type::real32, "real32", "B", (uint64)&((color_persistent *)0)->B, 0, sizeof(real32),{},{},{},{},{}},
{meta_member_type::real32, "real32", "A", (uint64)&((color_persistent *)0)->A, 0, sizeof(real32),{},{},{},{},{}},
}; 
 
void M_ALLOC__color_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 10; 
	SafeVoid->DataSize = sizeof(color_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(color_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(color_persistent)); 
};

color_persistent* M_GET__color_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 10) { 
		return (color_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_color_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &color_persistent_META[0], ArrayCount(color_persistent_META), AccData);
}

void SaveDataFillShim_color_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &color_persistent_META[0], ArrayCount(color_persistent_META), AccData);
}

void JsonFillStructShim_color_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &color_persistent_META[0], ArrayCount(color_persistent_META), DataDest);
}

meta_member world_object_META[] { 
{meta_member_type::custom, "vector2", "Position", (uint64)&((world_object *)0)->Position, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::custom, "vector2", "Size", (uint64)&((world_object *)0)->Size, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::real64, "real64", "Rotation", (uint64)&((world_object *)0)->Rotation, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::real64, "real64", "RotationRate", (uint64)&((world_object *)0)->RotationRate, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::custom, "color_persistent", "Color", (uint64)&((world_object *)0)->Color, 0, sizeof(color_persistent),&StructMetaFill_color_persistent, &JsonFillStructShim_color_persistent, &SaveDataFillShim_color_persistent, &color_persistent_META[0], ArrayCount(color_persistent_META) },
}; 
 
void M_ALLOC__world_object(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 11; 
	SafeVoid->DataSize = sizeof(world_object); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(world_object)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(world_object)); 
};

world_object* M_GET__world_object(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 11) { 
		return (world_object*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_world_object (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &world_object_META[0], ArrayCount(world_object_META), AccData);
}

void SaveDataFillShim_world_object (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &world_object_META[0], ArrayCount(world_object_META), AccData);
}

void JsonFillStructShim_world_object (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &world_object_META[0], ArrayCount(world_object_META), DataDest);
}

string ship_id_NAME[] { 
"advent",
"count",
}; 
string ship_status_NAME[] { 
"idle",
"moving",
"docking",
"undocking",
"docked",
}; 
meta_member ship_module_persistent_META[] { 
{meta_member_type::bool32, "bool32", "Filled", (uint64)&((ship_module_persistent *)0)->Filled, 0, sizeof(bool32),{},{},{},{},{}},
{meta_member_type::enumeration, "ship_module_id", "Type", (uint64)&((ship_module_persistent *)0)->Type, 0, sizeof(ship_module_id),{},{},{},{},{}},
{meta_member_type::custom, "world_target_persistent", "Target", (uint64)&((ship_module_persistent *)0)->Target, 0, sizeof(world_target_persistent),&StructMetaFill_world_target_persistent, &JsonFillStructShim_world_target_persistent, &SaveDataFillShim_world_target_persistent, &world_target_persistent_META[0], ArrayCount(world_target_persistent_META) },
{meta_member_type::custom, "persistent_pointer", "Owner", (uint64)&((ship_module_persistent *)0)->Owner, 0, sizeof(persistent_pointer),&StructMetaFill_persistent_pointer, &JsonFillStructShim_persistent_pointer, &SaveDataFillShim_persistent_pointer, &persistent_pointer_META[0], ArrayCount(persistent_pointer_META) },
{meta_member_type::real64, "real64", "ActivationTimerMS", (uint64)&((ship_module_persistent *)0)->ActivationTimerMS, 0, sizeof(real64),{},{},{},{},{}},
}; 
 
void M_ALLOC__ship_module_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 12; 
	SafeVoid->DataSize = sizeof(ship_module_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(ship_module_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(ship_module_persistent)); 
};

ship_module_persistent* M_GET__ship_module_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 12) { 
		return (ship_module_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_ship_module_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &ship_module_persistent_META[0], ArrayCount(ship_module_persistent_META), AccData);
}

void SaveDataFillShim_ship_module_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &ship_module_persistent_META[0], ArrayCount(ship_module_persistent_META), AccData);
}

void JsonFillStructShim_ship_module_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &ship_module_persistent_META[0], ArrayCount(ship_module_persistent_META), DataDest);
}

meta_member salvage_persistent_META[] { 
{meta_member_type::uint32, "uint32", "GUID", (uint64)&((salvage_persistent *)0)->GUID, 0, sizeof(uint32),{},{},{},{},{}},
{meta_member_type::int32, "int32", "KnowledgeAmount", (uint64)&((salvage_persistent *)0)->KnowledgeAmount, 0, sizeof(int32),{},{},{},{},{}},
{meta_member_type::custom, "world_object", "WorldObject", (uint64)&((salvage_persistent *)0)->WorldObject, 0, sizeof(world_object),&StructMetaFill_world_object, &JsonFillStructShim_world_object, &SaveDataFillShim_world_object, &world_object_META[0], ArrayCount(world_object_META) },
{meta_member_type::int64, "int64", "SpriteIndex", (uint64)&((salvage_persistent *)0)->SpriteIndex, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::custom, "vector2", "SpawnCenter", (uint64)&((salvage_persistent *)0)->SpawnCenter, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::real32, "real32", "SpawnRadius", (uint64)&((salvage_persistent *)0)->SpawnRadius, 0, sizeof(real32),{},{},{},{},{}},
}; 
 
void M_ALLOC__salvage_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 13; 
	SafeVoid->DataSize = sizeof(salvage_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(salvage_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(salvage_persistent)); 
};

salvage_persistent* M_GET__salvage_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 13) { 
		return (salvage_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_salvage_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &salvage_persistent_META[0], ArrayCount(salvage_persistent_META), AccData);
}

void SaveDataFillShim_salvage_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &salvage_persistent_META[0], ArrayCount(salvage_persistent_META), AccData);
}

void JsonFillStructShim_salvage_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &salvage_persistent_META[0], ArrayCount(salvage_persistent_META), DataDest);
}

meta_member asteroid_persistent_META[] { 
{meta_member_type::uint32, "uint32", "GUID", (uint64)&((asteroid_persistent *)0)->GUID, 0, sizeof(uint32),{},{},{},{},{}},
{meta_member_type::int32, "int32", "OreCount", (uint64)&((asteroid_persistent *)0)->OreCount, 0, sizeof(int32),{},{},{},{},{}},
{meta_member_type::enumeration, "item_id", "OreItem", (uint64)&((asteroid_persistent *)0)->OreItem, 0, sizeof(item_id),{},{},{},{},{}},
{meta_member_type::bool32, "bool32", "Using", (uint64)&((asteroid_persistent *)0)->Using, 0, sizeof(bool32),{},{},{},{},{}},
{meta_member_type::custom, "world_object", "WorldObject", (uint64)&((asteroid_persistent *)0)->WorldObject, 0, sizeof(world_object),&StructMetaFill_world_object, &JsonFillStructShim_world_object, &SaveDataFillShim_world_object, &world_object_META[0], ArrayCount(world_object_META) },
{meta_member_type::int64, "int64", "SpriteIndex", (uint64)&((asteroid_persistent *)0)->SpriteIndex, 0, sizeof(int64),{},{},{},{},{}},
}; 
 
void M_ALLOC__asteroid_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 14; 
	SafeVoid->DataSize = sizeof(asteroid_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(asteroid_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(asteroid_persistent)); 
};

asteroid_persistent* M_GET__asteroid_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 14) { 
		return (asteroid_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_asteroid_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &asteroid_persistent_META[0], ArrayCount(asteroid_persistent_META), AccData);
}

void SaveDataFillShim_asteroid_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &asteroid_persistent_META[0], ArrayCount(asteroid_persistent_META), AccData);
}

void JsonFillStructShim_asteroid_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &asteroid_persistent_META[0], ArrayCount(asteroid_persistent_META), DataDest);
}

meta_member asteroid_cluster_persistent_META[] { 
{meta_member_type::enumeration, "item_id", "OreItem", (uint64)&((asteroid_cluster_persistent *)0)->OreItem, 0, sizeof(item_id),{},{},{},{},{}},
{meta_member_type::custom, "vector2", "Center", (uint64)&((asteroid_cluster_persistent *)0)->Center, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::real64, "real64", "Radius", (uint64)&((asteroid_cluster_persistent *)0)->Radius, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::custom, "asteroid_persistent", "Asteroids", (uint64)&((asteroid_cluster_persistent *)0)->Asteroids, 32, sizeof(asteroid_persistent),&StructMetaFill_asteroid_persistent, &JsonFillStructShim_asteroid_persistent, &SaveDataFillShim_asteroid_persistent, &asteroid_persistent_META[0], ArrayCount(asteroid_persistent_META) },
}; 
 
void M_ALLOC__asteroid_cluster_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 15; 
	SafeVoid->DataSize = sizeof(asteroid_cluster_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(asteroid_cluster_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(asteroid_cluster_persistent)); 
};

asteroid_cluster_persistent* M_GET__asteroid_cluster_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 15) { 
		return (asteroid_cluster_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_asteroid_cluster_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &asteroid_cluster_persistent_META[0], ArrayCount(asteroid_cluster_persistent_META), AccData);
}

void SaveDataFillShim_asteroid_cluster_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &asteroid_cluster_persistent_META[0], ArrayCount(asteroid_cluster_persistent_META), AccData);
}

void JsonFillStructShim_asteroid_cluster_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &asteroid_cluster_persistent_META[0], ArrayCount(asteroid_cluster_persistent_META), DataDest);
}

meta_member ship_persistent_META[] { 
{meta_member_type::int32, "int32", "GUID", (uint64)&((ship_persistent *)0)->GUID, 0, sizeof(int32),{},{},{},{},{}},
{meta_member_type::enumeration, "ship_status", "Status", (uint64)&((ship_persistent *)0)->Status, 0, sizeof(ship_status),{},{},{},{},{}},
{meta_member_type::enumeration, "ship_id", "Type", (uint64)&((ship_persistent *)0)->Type, 0, sizeof(ship_id),{},{},{},{},{}},
{meta_member_type::custom, "persistent_pointer", "StationDocked", (uint64)&((ship_persistent *)0)->StationDocked, 0, sizeof(persistent_pointer),&StructMetaFill_persistent_pointer, &JsonFillStructShim_persistent_pointer, &SaveDataFillShim_persistent_pointer, &persistent_pointer_META[0], ArrayCount(persistent_pointer_META) },
{meta_member_type::custom, "ship_journey", "CurrentJourney", (uint64)&((ship_persistent *)0)->CurrentJourney, 0, sizeof(ship_journey),&StructMetaFill_ship_journey, &JsonFillStructShim_ship_journey, &SaveDataFillShim_ship_journey, &ship_journey_META[0], ArrayCount(ship_journey_META) },
{meta_member_type::custom, "vector2", "Position", (uint64)&((ship_persistent *)0)->Position, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::real64, "real64", "Rotation", (uint64)&((ship_persistent *)0)->Rotation, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::custom, "vector2", "Velocity", (uint64)&((ship_persistent *)0)->Velocity, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::custom, "item_hold_persistent", "ItemHold", (uint64)&((ship_persistent *)0)->ItemHold, 0, sizeof(item_hold_persistent),&StructMetaFill_item_hold_persistent, &JsonFillStructShim_item_hold_persistent, &SaveDataFillShim_item_hold_persistent, &item_hold_persistent_META[0], ArrayCount(item_hold_persistent_META) },
{meta_member_type::custom, "item_hold_persistent", "FuelHold", (uint64)&((ship_persistent *)0)->FuelHold, 0, sizeof(item_hold_persistent),&StructMetaFill_item_hold_persistent, &JsonFillStructShim_item_hold_persistent, &SaveDataFillShim_item_hold_persistent, &item_hold_persistent_META[0], ArrayCount(item_hold_persistent_META) },
{meta_member_type::custom, "ship_module_persistent", "Modules", (uint64)&((ship_persistent *)0)->Modules, 16, sizeof(ship_module_persistent),&StructMetaFill_ship_module_persistent, &JsonFillStructShim_ship_module_persistent, &SaveDataFillShim_ship_module_persistent, &ship_module_persistent_META[0], ArrayCount(ship_module_persistent_META) },
}; 
 
void M_ALLOC__ship_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 16; 
	SafeVoid->DataSize = sizeof(ship_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(ship_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(ship_persistent)); 
};

ship_persistent* M_GET__ship_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 16) { 
		return (ship_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_ship_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &ship_persistent_META[0], ArrayCount(ship_persistent_META), AccData);
}

void SaveDataFillShim_ship_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &ship_persistent_META[0], ArrayCount(ship_persistent_META), AccData);
}

void JsonFillStructShim_ship_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &ship_persistent_META[0], ArrayCount(ship_persistent_META), DataDest);
}

meta_member converter_persistent_META[] { 
{meta_member_type::enumeration, "recipe_id", "RecipeID", (uint64)&((converter_persistent *)0)->RecipeID, 0, sizeof(recipe_id),{},{},{},{},{}},
{meta_member_type::real64, "real64", "OrderTime", (uint64)&((converter_persistent *)0)->OrderTime, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::int32, "int32", "RunsCount", (uint64)&((converter_persistent *)0)->RunsCount, 0, sizeof(int32),{},{},{},{},{}},
}; 
 
void M_ALLOC__converter_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 17; 
	SafeVoid->DataSize = sizeof(converter_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(converter_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(converter_persistent)); 
};

converter_persistent* M_GET__converter_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 17) { 
		return (converter_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_converter_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &converter_persistent_META[0], ArrayCount(converter_persistent_META), AccData);
}

void SaveDataFillShim_converter_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &converter_persistent_META[0], ArrayCount(converter_persistent_META), AccData);
}

void JsonFillStructShim_converter_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &converter_persistent_META[0], ArrayCount(converter_persistent_META), DataDest);
}

meta_member station_persistent_META[] { 
{meta_member_type::int32, "int32", "GUID", (uint64)&((station_persistent *)0)->GUID, 0, sizeof(int32),{},{},{},{},{}},
{meta_member_type::custom, "vector2", "Position", (uint64)&((station_persistent *)0)->Position, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::int32, "int32", "DockedCount", (uint64)&((station_persistent *)0)->DockedCount, 0, sizeof(int32),{},{},{},{},{}},
{meta_member_type::custom, "item_hold_persistent", "ItemHold", (uint64)&((station_persistent *)0)->ItemHold, 0, sizeof(item_hold_persistent),&StructMetaFill_item_hold_persistent, &JsonFillStructShim_item_hold_persistent, &SaveDataFillShim_item_hold_persistent, &item_hold_persistent_META[0], ArrayCount(item_hold_persistent_META) },
{meta_member_type::custom, "converter_persistent", "Converters", (uint64)&((station_persistent *)0)->Converters, 10, sizeof(converter_persistent),&StructMetaFill_converter_persistent, &JsonFillStructShim_converter_persistent, &SaveDataFillShim_converter_persistent, &converter_persistent_META[0], ArrayCount(converter_persistent_META) },
}; 
 
void M_ALLOC__station_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 18; 
	SafeVoid->DataSize = sizeof(station_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(station_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(station_persistent)); 
};

station_persistent* M_GET__station_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 18) { 
		return (station_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_station_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &station_persistent_META[0], ArrayCount(station_persistent_META), AccData);
}

void SaveDataFillShim_station_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &station_persistent_META[0], ArrayCount(station_persistent_META), AccData);
}

void JsonFillStructShim_station_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &station_persistent_META[0], ArrayCount(station_persistent_META), DataDest);
}

meta_member skill_bonuses_META[] { 
{meta_member_type::real32, "real32", "FuelForce", (uint64)&((skill_bonuses *)0)->FuelForce, 0, sizeof(real32),{},{},{},{},{}},
{meta_member_type::int32, "int32", "ShipLimit", (uint64)&((skill_bonuses *)0)->ShipLimit, 0, sizeof(int32),{},{},{},{},{}},
}; 
 
void M_ALLOC__skill_bonuses(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 19; 
	SafeVoid->DataSize = sizeof(skill_bonuses); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(skill_bonuses)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(skill_bonuses)); 
};

skill_bonuses* M_GET__skill_bonuses(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 19) { 
		return (skill_bonuses*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_skill_bonuses (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &skill_bonuses_META[0], ArrayCount(skill_bonuses_META), AccData);
}

void SaveDataFillShim_skill_bonuses (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &skill_bonuses_META[0], ArrayCount(skill_bonuses_META), AccData);
}

void JsonFillStructShim_skill_bonuses (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &skill_bonuses_META[0], ArrayCount(skill_bonuses_META), DataDest);
}

meta_member save_file_META[] { 
{meta_member_type::int64, "int64", "RealTimeSaved", (uint64)&((save_file *)0)->RealTimeSaved, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::real64, "real64", "UniverseTimeMS", (uint64)&((save_file *)0)->UniverseTimeMS, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::int64, "int64", "SkillNodesIDUnlocked", (uint64)&((save_file *)0)->SkillNodesIDUnlocked, 256, sizeof(int64),{},{},{},{},{}},
{meta_member_type::custom, "skill_bonuses", "TreeBonuses", (uint64)&((save_file *)0)->TreeBonuses, 0, sizeof(skill_bonuses),&StructMetaFill_skill_bonuses, &JsonFillStructShim_skill_bonuses, &SaveDataFillShim_skill_bonuses, &skill_bonuses_META[0], ArrayCount(skill_bonuses_META) },
{meta_member_type::int64, "int64", "Knowledge", (uint64)&((save_file *)0)->Knowledge, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::int64, "int64", "ShipsCount", (uint64)&((save_file *)0)->ShipsCount, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::custom, "ship_persistent", "Ships", (uint64)&((save_file *)0)->Ships, 256, sizeof(ship_persistent),&StructMetaFill_ship_persistent, &JsonFillStructShim_ship_persistent, &SaveDataFillShim_ship_persistent, &ship_persistent_META[0], ArrayCount(ship_persistent_META) },
{meta_member_type::int64, "int64", "StationsCount", (uint64)&((save_file *)0)->StationsCount, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::custom, "station_persistent", "Stations", (uint64)&((save_file *)0)->Stations, 256, sizeof(station_persistent),&StructMetaFill_station_persistent, &JsonFillStructShim_station_persistent, &SaveDataFillShim_station_persistent, &station_persistent_META[0], ArrayCount(station_persistent_META) },
{meta_member_type::int64, "int64", "AsteroidClustersCount", (uint64)&((save_file *)0)->AsteroidClustersCount, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::custom, "asteroid_cluster_persistent", "AsteroidClusters", (uint64)&((save_file *)0)->AsteroidClusters, 256, sizeof(asteroid_cluster_persistent),&StructMetaFill_asteroid_cluster_persistent, &JsonFillStructShim_asteroid_cluster_persistent, &SaveDataFillShim_asteroid_cluster_persistent, &asteroid_cluster_persistent_META[0], ArrayCount(asteroid_cluster_persistent_META) },
{meta_member_type::int64, "int64", "SalvagesCount", (uint64)&((save_file *)0)->SalvagesCount, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::custom, "salvage_persistent", "Salvages", (uint64)&((save_file *)0)->Salvages, 256, sizeof(salvage_persistent),&StructMetaFill_salvage_persistent, &JsonFillStructShim_salvage_persistent, &SaveDataFillShim_salvage_persistent, &salvage_persistent_META[0], ArrayCount(salvage_persistent_META) },
}; 
 
void M_ALLOC__save_file(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 20; 
	SafeVoid->DataSize = sizeof(save_file); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(save_file)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(save_file)); 
};

save_file* M_GET__save_file(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 20) { 
		return (save_file*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_save_file (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &save_file_META[0], ArrayCount(save_file_META), AccData);
}

void SaveDataFillShim_save_file (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &save_file_META[0], ArrayCount(save_file_META), AccData);
}

void JsonFillStructShim_save_file (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &save_file_META[0], ArrayCount(save_file_META), DataDest);
}

string station_service_NAME[] { 
"refinery",
"shipyard",
"count",
}; 
string recipe_member_type_NAME[] { 
"item",
"ship",
}; 
meta_member skill_node_persistent_META[] { 
{meta_member_type::int64, "int64", "ID", (uint64)&((skill_node_persistent *)0)->ID, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::custom, "skill_bonuses", "Bonuses", (uint64)&((skill_node_persistent *)0)->Bonuses, 0, sizeof(skill_bonuses),&StructMetaFill_skill_bonuses, &JsonFillStructShim_skill_bonuses, &SaveDataFillShim_skill_bonuses, &skill_bonuses_META[0], ArrayCount(skill_bonuses_META) },
{meta_member_type::int64, "int64", "KnowledgeCost", (uint64)&((skill_node_persistent *)0)->KnowledgeCost, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::custom, "vector2", "Position", (uint64)&((skill_node_persistent *)0)->Position, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::custom, "skill_bonuses", "BonusAdditions", (uint64)&((skill_node_persistent *)0)->BonusAdditions, 0, sizeof(skill_bonuses),&StructMetaFill_skill_bonuses, &JsonFillStructShim_skill_bonuses, &SaveDataFillShim_skill_bonuses, &skill_bonuses_META[0], ArrayCount(skill_bonuses_META) },
{meta_member_type::int64, "int64", "ChildrenIDs", (uint64)&((skill_node_persistent *)0)->ChildrenIDs, 10, sizeof(int64),{},{},{},{},{}},
}; 
 
void M_ALLOC__skill_node_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 21; 
	SafeVoid->DataSize = sizeof(skill_node_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(skill_node_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(skill_node_persistent)); 
};

skill_node_persistent* M_GET__skill_node_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 21) { 
		return (skill_node_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_skill_node_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &skill_node_persistent_META[0], ArrayCount(skill_node_persistent_META), AccData);
}

void SaveDataFillShim_skill_node_persistent (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &skill_node_persistent_META[0], ArrayCount(skill_node_persistent_META), AccData);
}

void JsonFillStructShim_skill_node_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &skill_node_persistent_META[0], ArrayCount(skill_node_persistent_META), DataDest);
}




 #endif 
