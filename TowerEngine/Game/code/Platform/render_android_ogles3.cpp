// Some good examples here
// https://arm-software.github.io/opengl-es-sdk-for-android/shadow_mapping.html

#include <GLES3/gl32.h>
#include <EGL/egl.h>

// Unsupported on ogles, But could be
void SetWireframeFillMode() { }
void SetPolygonFillMode() { }

// Doesn't exist on ogles
void EnableSeamlessCubeMap() { }

#include "../Engine/EngineCore.h"
#include "../Engine/Renderer/ogles3.cpp"