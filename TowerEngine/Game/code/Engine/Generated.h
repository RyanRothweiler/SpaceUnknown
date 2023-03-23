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

void SaveDataFillShim_vector2 (save_data::member* Dest, string KeyParent, void* AccData, memory_arena* Memory){
return save_data::AddMembers(Dest, KeyParent, &vector2_META[0], ArrayCount(vector2_META), AccData, Memory);
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
meta_member save_file_META[] { 
{meta_member_type::int64, "int64", "RealTimeSaved", (uint64)&((save_file *)0)->RealTimeSaved, 0, sizeof(int64),{},{},{},{},{}},
{meta_member_type::real64, "real64", "UniverseTimeMS", (uint64)&((save_file *)0)->UniverseTimeMS, 0, sizeof(real64),{},{},{},{},{}},
}; 
 
void M_ALLOC__save_file(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 2; 
	SafeVoid->DataSize = sizeof(save_file); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(save_file)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(save_file)); 
};

save_file* M_GET__save_file(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 2) { 
		return (save_file*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_save_file (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &save_file_META[0], ArrayCount(save_file_META), AccData);
}

void SaveDataFillShim_save_file (save_data::member* Dest, string KeyParent, void* AccData, memory_arena* Memory){
return save_data::AddMembers(Dest, KeyParent, &save_file_META[0], ArrayCount(save_file_META), AccData, Memory);
}

void JsonFillStructShim_save_file (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &save_file_META[0], ArrayCount(save_file_META), DataDest);
}

string item_id_NAME[] { 
"venigen",
"pyrexium",
"stl",
"sm_asteroid_miner",
"sm_salvager_i",
"count",
}; 
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
	SafeVoid->Type = 3; 
	SafeVoid->DataSize = sizeof(skill_bonuses); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(skill_bonuses)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(skill_bonuses)); 
};

skill_bonuses* M_GET__skill_bonuses(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 3) { 
		return (skill_bonuses*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_skill_bonuses (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &skill_bonuses_META[0], ArrayCount(skill_bonuses_META), AccData);
}

void SaveDataFillShim_skill_bonuses (save_data::member* Dest, string KeyParent, void* AccData, memory_arena* Memory){
return save_data::AddMembers(Dest, KeyParent, &skill_bonuses_META[0], ArrayCount(skill_bonuses_META), AccData, Memory);
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
	SafeVoid->Type = 4; 
	SafeVoid->DataSize = sizeof(skill_node_persistent); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(skill_node_persistent)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(skill_node_persistent)); 
};

skill_node_persistent* M_GET__skill_node_persistent(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 4) { 
		return (skill_node_persistent*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_skill_node_persistent (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &skill_node_persistent_META[0], ArrayCount(skill_node_persistent_META), AccData);
}

void SaveDataFillShim_skill_node_persistent (save_data::member* Dest, string KeyParent, void* AccData, memory_arena* Memory){
return save_data::AddMembers(Dest, KeyParent, &skill_node_persistent_META[0], ArrayCount(skill_node_persistent_META), AccData, Memory);
}

void JsonFillStructShim_skill_node_persistent (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &skill_node_persistent_META[0], ArrayCount(skill_node_persistent_META), DataDest);
}

meta_member skill_node_player_META[] { 
{meta_member_type::int64, "int64", "ID", (uint64)&((skill_node_player *)0)->ID, 0, sizeof(int64),{},{},{},{},{}},
}; 
 
void M_ALLOC__skill_node_player(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 5; 
	SafeVoid->DataSize = sizeof(skill_node_player); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(skill_node_player)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(skill_node_player)); 
};

skill_node_player* M_GET__skill_node_player(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 5) { 
		return (skill_node_player*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_skill_node_player (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &skill_node_player_META[0], ArrayCount(skill_node_player_META), AccData);
}

void SaveDataFillShim_skill_node_player (save_data::member* Dest, string KeyParent, void* AccData, memory_arena* Memory){
return save_data::AddMembers(Dest, KeyParent, &skill_node_player_META[0], ArrayCount(skill_node_player_META), AccData, Memory);
}

void JsonFillStructShim_skill_node_player (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &skill_node_player_META[0], ArrayCount(skill_node_player_META), DataDest);
}




 #endif 
