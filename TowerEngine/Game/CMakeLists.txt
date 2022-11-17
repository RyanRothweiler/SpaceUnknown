cmake_minimum_required(VERSION 3.10)

project(SpaceUnknown)

add_executable(${PROJECT_NAME} WIN32 code/Platform/platform_win32.cpp)
target_link_libraries(${PROJECT_NAME} gdi32.lib opengl32.lib user32.lib Rpcrt4.lib shcore.lib Comdlg32.lib Shell32.lib)

add_library(render_win32_ogles3 SHARED code/Platform/render_win32_ogles3.cpp)
target_link_libraries(render_win32_ogles3 opengl32.lib)

add_library(EngineCore SHARED code/Engine/EngineCore.cpp)