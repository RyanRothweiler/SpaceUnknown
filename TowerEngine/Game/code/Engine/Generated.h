#ifndef GENERATED 
#define GENERATED 
 
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
	SafeVoid->Type = 1; 
	SafeVoid->DataSize = sizeof(save_file); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(save_file)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(save_file)); 
};

save_file* M_GET__save_file(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 1) { 
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



 #endif 
