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
meta_member third_META[] { 
{meta_member_type::int32, "int32", "Roger", (uint64)&((third *)0)->Roger, 0, sizeof(int32),{},{},{},{},{}},
{meta_member_type::real64, "real64", "Droid", (uint64)&((third *)0)->Droid, 0, sizeof(real64),{},{},{},{},{}},
}; 
 
void M_ALLOC__third(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 1; 
	SafeVoid->DataSize = sizeof(third); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(third)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(third)); 
};

third* M_GET__third(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 1) { 
		return (third*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_third (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &third_META[0], ArrayCount(third_META), AccData);
}

void SaveDataFillShim_third (save_data::member* Dest, string KeyParent, void* AccData, memory_arena* Memory){
return save_data::AddMembers(Dest, KeyParent, &third_META[0], ArrayCount(third_META), AccData, Memory);
}

void JsonFillStructShim_third (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &third_META[0], ArrayCount(third_META), DataDest);
}

meta_member second_META[] { 
{meta_member_type::int32, "int32", "Boost", (uint64)&((second *)0)->Boost, 0, sizeof(int32),{},{},{},{},{}},
{meta_member_type::real64, "real64", "HPRate", (uint64)&((second *)0)->HPRate, 0, sizeof(real64),{},{},{},{},{}},
}; 
 
void M_ALLOC__second(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 2; 
	SafeVoid->DataSize = sizeof(second); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(second)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(second)); 
};

second* M_GET__second(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 2) { 
		return (second*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_second (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &second_META[0], ArrayCount(second_META), AccData);
}

void SaveDataFillShim_second (save_data::member* Dest, string KeyParent, void* AccData, memory_arena* Memory){
return save_data::AddMembers(Dest, KeyParent, &second_META[0], ArrayCount(second_META), AccData, Memory);
}

void JsonFillStructShim_second (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &second_META[0], ArrayCount(second_META), DataDest);
}

meta_member ryan_test_META[] { 
{meta_member_type::int32, "int32", "Here", (uint64)&((ryan_test *)0)->Here, 0, sizeof(int32),{},{},{},{},{}},
{meta_member_type::custom, "second", "Moar", (uint64)&((ryan_test *)0)->Moar, 0, sizeof(second),&StructMetaFill_second, &JsonFillStructShim_second, &SaveDataFillShim_second, &second_META[0], ArrayCount(second_META) },
}; 
 
void M_ALLOC__ryan_test(s_void* SafeVoid, memory_arena* Memory) { 
	SafeVoid->IsAllocated = true; 
	SafeVoid->Type = 3; 
	SafeVoid->DataSize = sizeof(ryan_test); 
	SafeVoid->Data = ArenaAllocate(Memory, sizeof(ryan_test)); 
	ClearMemory((uint8*)SafeVoid->Data, sizeof(ryan_test)); 
};

ryan_test* M_GET__ryan_test(s_void* SafeVoid) { 
	if (SafeVoid->IsAllocated && SafeVoid->Type == 3) { 
		return (ryan_test*)SafeVoid->Data; 
	} 
	return GameNull; 
};

void StructMetaFill_ryan_test (json::struct_string_return* Dest,  void* AccData){
return json::StructMetaFill(Dest, &ryan_test_META[0], ArrayCount(ryan_test_META), AccData);
}

void SaveDataFillShim_ryan_test (save_data::member* Dest, string KeyParent, void* AccData, memory_arena* Memory){
return save_data::AddMembers(Dest, KeyParent, &ryan_test_META[0], ArrayCount(ryan_test_META), AccData, Memory);
}

void JsonFillStructShim_ryan_test (json::json_data* JsonData, string KeyParent, void* DataDest){
return json::FillStruct(JsonData, KeyParent, &ryan_test_META[0], ArrayCount(ryan_test_META), DataDest);
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
