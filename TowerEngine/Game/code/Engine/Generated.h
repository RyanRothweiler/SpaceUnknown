#ifndef GENERATED 
#define GENERATED 
 
char* AssetsFolderStructure[] { 
"Asteroids/",
"EngineResources/",
"Icons/",
"Salvage/",
"Shaders/",
"SkillTreeNodes/",
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
meta_member third_META[] { 
{meta_member_type::int32, "int32", "Roger", (uint64)&((third *)0)->Roger, 0,{},{},{}},
{meta_member_type::real64, "real64", "Droid", (uint64)&((third *)0)->Droid, 0,{},{},{}},
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

void StructMetaFill_third (struct_string_return* Dest,  void* AccData){
return StructMetaFill(Dest, &third_META[0], ArrayCount(third_META), AccData);
}

meta_member second_META[] { 
{meta_member_type::int32, "int32", "Boost", (uint64)&((second *)0)->Boost, 0,{},{},{}},
{meta_member_type::custom, "third", "Robot", (uint64)&((second *)0)->Robot, 0,&StructMetaFill_third, &third_META[0], ArrayCount(third_META) },
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

void StructMetaFill_second (struct_string_return* Dest,  void* AccData){
return StructMetaFill(Dest, &second_META[0], ArrayCount(second_META), AccData);
}

meta_member ryan_test_META[] { 
{meta_member_type::int32, "int32", "FieldHere", (uint64)&((ryan_test *)0)->FieldHere, 0,{},{},{}},
{meta_member_type::uint32, "uint32", "FieldHereAnother", (uint64)&((ryan_test *)0)->FieldHereAnother, 0,{},{},{}},
{meta_member_type::real64, "real64", "Moar", (uint64)&((ryan_test *)0)->Moar, 0,{},{},{}},
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

void StructMetaFill_ryan_test (struct_string_return* Dest,  void* AccData){
return StructMetaFill(Dest, &ryan_test_META[0], ArrayCount(ryan_test_META), AccData);
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
