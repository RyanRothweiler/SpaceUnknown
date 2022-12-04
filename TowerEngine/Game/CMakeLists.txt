cmake_minimum_required(VERSION 3.10)

project(SpaceUnknown)

add_compile_definitions(PLATFORM_WINDOWS)

set(WARNING_FLAGS /WX /W4 -wd4201 -wd4100 -wd4127 -wd4189 -wd4505 -wd4065 -wd4700 -wd4068 -wd4702 -wd4307 -wd4464)

add_executable(${PROJECT_NAME} WIN32 code/Platform/platform_win32.cpp)
target_link_libraries(${PROJECT_NAME} gdi32.lib opengl32.lib user32.lib Rpcrt4.lib shcore.lib Comdlg32.lib Shell32.lib)
target_compile_options(${PROJECT_NAME} PRIVATE ${WARNING_FLAGS})

add_library(render_win32_ogles3 SHARED code/Platform/render_win32_ogles3.cpp)
target_link_libraries(render_win32_ogles3 opengl32.lib)
target_compile_options(render_win32_ogles3 PRIVATE ${WARNING_FLAGS})

add_library(EngineCore SHARED code/Engine/EngineCore.cpp)
target_compile_options(EngineCore PRIVATE ${WARNING_FLAGS})
