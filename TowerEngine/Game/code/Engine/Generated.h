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
string journey_step_type_NAME[] { 
"movement",
"dock_undock",
}; 
string ship_module_id_NAME[] { 
"none",
"asteroid_miner",
"salvager_i",
"count",
}; 
meta_member ship_persistent_META[] { 
{meta_member_type::custom, "vector2", "Position", (uint64)&((ship_persistent *)0)->Position, 0, sizeof(vector2),&StructMetaFill_vector2, &JsonFillStructShim_vector2, &SaveDataFillShim_vector2, &vector2_META[0], ArrayCount(vector2_META) },
{meta_member_type::real64, "real64", "Rotation", (uint64)&((ship_persistent *)0)->Rotation, 0, sizeof(real64),{},{},{},{},{}},
}; 
 
void M_ALLOC__ship_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 2; 
	SafeVoid->DataSize = sizeof(ship_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(ship_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(ship_persistent)); 
};

ship_persistent* M_GET__ship_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 2) { 
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

meta_member save_file_META[] { 
{meta_member_type::int64, "int64", "RealTimeSaved", (uint64)&((save_file *)0)->RealTimeSaved, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::real64, "real64", "UniverseTimeMS", (uint64)&((save_file *)0)->UniverseTimeMS, 0, sizeof(real64),{},{},{},{},{}},
{meta_member_type::int64, "int64", "SkillNodesIDUnlocked", (uint64)&((save_file *)0)->SkillNodesIDUnlocked, 256, sizeof(int64),{},{},{},{},{}},
{meta_member_type::int64, "int64", "ShipsCount", (uint64)&((save_file *)0)->ShipsCount, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::custom, "ship_persistent", "Ships", (uint64)&((save_file *)0)->Ships, 256, sizeof(ship_persistent),&StructMetaFill_ship_persistent, &JsonFillStructShim_ship_persistent, &SaveDataFillShim_ship_persistent, &ship_persistent_META[0], ArrayCount(ship_persistent_META) },
}; 
 
void M_ALLOC__save_file(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 3; 
	SafeVoid->DataSize = sizeof(save_file); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(save_file)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(save_file)); 
};

save_file* M_GET__save_file(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 3) { 
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

string ryan_enum_NAME[] { 
"foo",
"bar",
"count",
}; 
meta_member ryan_type_META[] { 
{meta_member_type::int64, "int64", "x", (uint64)&((ryan_type *)0)->x, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::enumeration, "ryan_enum", "nm", (uint64)&((ryan_type *)0)->nm, 0, sizeof(ryan_enum),{},{},{},{},{}},
}; 
 
void M_ALLOC__ryan_type(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 4; 
	SafeVoid->DataSize = sizeof(ryan_type); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(ryan_type)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(ryan_type)); 
};

ryan_type* M_GET__ryan_type(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 4) { 
		return (ryan_type*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_ryan_type (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &ryan_type_META[0], ArrayCount(ryan_type_META), AccData);
}

void SaveDataFillShim_ryan_type (save_data::member* Dest, string KeyParent, void* AccData){
return save_data::AddMembers(Dest, KeyParent, &ryan_type_META[0], ArrayCount(ryan_type_META), AccData);
}

void JsonFillStructShim_ryan_type (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &ryan_type_META[0], ArrayCount(ryan_type_META), DataDest);
}

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
{meta_member_type::int32, "int32", "Count", (uint64)&((item_instance_persistent *)0)->Count, 0, sizeof(int32),{},{},{},{},{}},
}; 
 
void M_ALLOC__item_instance_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 5; 
	SafeVoid->DataSize = sizeof(item_instance_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(item_instance_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(item_instance_persistent)); 
};

item_instance_persistent* M_GET__item_instance_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 5) { 
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
}; 
 
void M_ALLOC__item_hold_persistent(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 6; 
	SafeVoid->DataSize = sizeof(item_hold_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(item_hold_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(item_hold_persistent)); 
};

item_hold_persistent* M_GET__item_hold_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 6) { 
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

string station_service_NAME[] { 
"refinery",
"shipyard",
"count",
}; 
string recipe_id_NAME[] { 
"venigen_stl",
"ship_advent",
"count",
"none",
}; 
string recipe_member_type_NAME[] { 
"item",
"ship",
}; 
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
meta_member skill_bonuses_META[] { 
{meta_member_type::real32, "real32", "FuelForceAddition", (uint64)&((skill_bonuses *)0)->FuelForceAddition, 0, sizeof(real32),{},{},{},{},{}},
}; 
 
void M_ALLOC__skill_bonuses(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 7; 
	SafeVoid->DataSize = sizeof(skill_bonuses); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(skill_bonuses)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(skill_bonuses)); 
};

skill_bonuses* M_GET__skill_bonuses(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 7) { 
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
	SafeVoid->Type = 8; 
	SafeVoid->DataSize = sizeof(skill_node_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(skill_node_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(skill_node_persistent)); 
};

skill_node_persistent* M_GET__skill_node_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 8) { 
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
