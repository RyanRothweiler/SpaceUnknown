// Some good examples here
// https://arm-software.github.io/opengl-es-sdk-for-android/shadow_mapping.html

#include <GLES3/gl32.h>
#include <EGL/egl.h>

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

// Unsupported on ogles, But could be
void SetWireframeFillMode() { }
void SetPolygonFillMode() { }

// Doesn't exist on ogles
void EnableSeamlessCubeMap() { }

//#include "../Engine/EngineCore.h"
#include "T:/Game/code/Engine/Renderer/ogles3.cpp"