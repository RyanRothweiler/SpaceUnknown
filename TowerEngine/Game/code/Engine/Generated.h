#ifndef GENERATED 
#define GENERATED 
 
enum class meta_member_type { 
uint32, 
uint16, 
uint8, 
int32, 
int16, 
int8, 
int64, 
real32, 
real64, 
}; 
 
 struct meta_member { meta_member_type Type;
 string Name;
 uint64 Offset;
 bool32 ArrayLength;
 };

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
