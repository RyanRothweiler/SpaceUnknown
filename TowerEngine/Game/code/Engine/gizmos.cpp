#pragma once
#ifndef GIZMOS_CPP
#define GIZMOS_CPP

#include "gizmos.h"

void GizmoPushColor(color Color)
{
	Globals->GizmoColorStackIndex++;
	Globals->GizmoColorStack[Globals->GizmoColorStackIndex] = Color;
	Assert(Globals->GizmoColorStackIndex < ArrayCount(Globals->GizmoColorStack));
}

void GizmoPopColor()
{
	// Always leave the 0 index color
	if (Globals->GizmoColorStackIndex > 0) {
		Globals->GizmoColorStackIndex--;
	}
}

color GizmoGetColor(color Override)
{


	if (Override.Empty()) {
		return Globals->GizmoColorStack[Globals->GizmoColorStackIndex];
	}
	return Override;
}

/*
void GizmosRenderAxis(vector3 CenterWorld, state_to_serialize* State, renderer* UIRenderer)
{
	vector2 X = WorldToScreen(CenterWorld + vector3{1, 0, 0}, State->ActiveCam);
	vector2 Y = WorldToScreen(CenterWorld + vector3{0, 1, 0}, State->ActiveCam);
	vector2 Z = WorldToScreen(CenterWorld + vector3{0, 0, 1}, State->ActiveCam);

	vector2 Center = WorldToScreen(CenterWorld, State->ActiveCam);

	RenderScreenLineMesh(Center, X, 2, COLOR_RED, UIRenderer);
	RenderScreenLineMesh(Center, Y, 2, COLOR_GREEN, UIRenderer);
	RenderScreenLineMesh(Center, Z, 2, COLOR_BLUE, UIRenderer);
}
*/

void GizmoPlane(vector3 Center, vector3 Rotation, real64 Size,
                int32 ID = 0, color Color = {})
{
	if (!Globals->EditorData.ShowGizmos) return;

	static entity* Arrow = assets::AllocateEntity("Engine_Plane");
	Arrow->Transform.LocalPos = Center;
	Arrow->Transform.LocalScale = vector3{Size, Size, 0.1f};
	Arrow->Transform.LocalRot = Rotation;
	Arrow->Transform.Update(m4y4Identity());

	Arrow->ID = ID;

	Color = GizmoGetColor(Color);

	Arrow->Material->Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RenderEntity(Arrow, Globals->GameRenderer, ID);
}

void GizmoArrow(vector3 Center, vector3 Dir, real64 Size,
                int32 ID = 0, color Color = {})
{
	if (!Globals->EditorData.ShowGizmos) return;

	static entity* Arrow = assets::AllocateEntity("Engine_Arrow");
	Arrow->Transform.LocalPos = Center;
	Arrow->Transform.LocalScale = vector3{Size, Size, Size};
	Arrow->Transform.LocalRot = LookAt(Dir).ToEuler();
	Arrow->Transform.Update(m4y4Identity());

	Arrow->ID = ID;

	Color = GizmoGetColor(Color);

	Arrow->Material->Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RenderEntity(Arrow, Globals->GameRenderer, ID);
}

void GizmoLine(vector3 Start, vector3 End, real64 Thickness,
               int32 ID = 0, color Color = {})
{
	if (!Globals->EditorData.ShowGizmos) return;

	vector3 Dir = Vector3Normalize(Start - End);
	vector3 Rot = LookAt(Dir).ToEuler();
	vector3 Center = (Start + End) * 0.5f;

	real64 Len = Vector3Distance(Start, End);
	vector3 Size = vector3{Thickness, Thickness, Len} * 0.5f;

	static entity* Cube = assets::AllocateEntity("UnitCube");
	Cube->Transform.LocalPos = Center;
	Cube->Transform.LocalScale = Size;
	Cube->Transform.LocalRot = Rot;
	Cube->Transform.Update(m4y4Identity());
	Cube->Wireframe = true;

	Cube->ID = ID;

	Color = GizmoGetColor(Color);

	Cube->Material->Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RenderEntity(Cube, Globals->GameRenderer, ID);
}

void GizmoLine(line Line, real64 Thickness,
               int32 ID = 0, color Color = {})
{
	if (!Globals->EditorData.ShowGizmos) return;

	GizmoLine(vector3{Line.Start.X, Line.Start.Y, 0},
	          vector3{Line.End.X, Line.End.Y, 0},
	          Thickness, ID, Color
	         );
}

void GizmoBox(vector3 Center, vector3 Size,
              int32 ID = 0, color Color = {})
{
	if (!Globals->EditorData.ShowGizmos) return;

	static entity* Cube = assets::AllocateEntity("UnitCube");
	Cube->Transform.LocalPos = Center;
	Cube->Transform.LocalScale = Size;
	Cube->Transform.LocalRot = {};
	Cube->Transform.Update(m4y4Identity());
	Cube->Wireframe = true;

	Cube->ID = ID;

	Color = GizmoGetColor(Color);

	Cube->Material->Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RenderEntity(Cube, Globals->GameRenderer, ID);
}

void GizmoSphere(vector3 Center, real32 Radius,
                 int32 ID = 0, color Color = {})
{
	if (!Globals->EditorData.ShowGizmos) return;

	static entity* Arrow = assets::AllocateEntity("Engine_Sphere");
	Arrow->Transform.LocalPos = Center;
	Arrow->Transform.LocalScale = vector3{Radius, Radius, Radius};
	Arrow->Transform.Update(m4y4Identity());
	Arrow->Wireframe = true;

	Arrow->ID = ID;

	Color = GizmoGetColor(Color);

	Arrow->Material->Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RenderEntity(Arrow, Globals->GameRenderer, ID);

}

// Render the camera as an object in the scene to debug its transforms
void GizmoCamera(camera* Camera,
                 int32 ID = 0, color Color = {})
{
	if (!Globals->EditorData.ShowGizmos) return;

	static entity* Arrow = assets::AllocateEntity("Engine_Camera");
	Arrow->Wireframe = true;

	Arrow->Transform.LocalPos = Camera->Center;
	Arrow->Transform.LocalScale = vector3{1, 1, 1};
	Arrow->Transform.Update(m4y4Identity());

	vector3 Rot = LookAt(Camera->Forward).ToEuler();
	Arrow->Transform.LocalRot = Rot;

	Arrow->ID = 0;

	Color = GizmoGetColor(Color);

	Arrow->Material->Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RenderEntity(Arrow, Globals->GameRenderer, ID);
}

void GizmoCircle(vector3 Center, real32 Radius,
                 int32 ID = 0, color Color = {})
{
	if (!Globals->EditorData.ShowGizmos) return;

	static entity* Arrow = assets::AllocateEntity("Engine_Circle");
	Arrow->Transform.LocalPos = Center;
	Arrow->Transform.LocalScale = vector3{Radius, Radius, Radius};
	Arrow->Transform.Update(m4y4Identity());
	Arrow->Wireframe = true;

	Arrow->ID = ID;

	Color = GizmoGetColor(Color);

	Arrow->Material->Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RenderEntity(Arrow, Globals->GameRenderer, ID);
}

void GizmoScreenCircle(vector2 Center, real32 Radius,
                       int32 ID = 0, color Color = {})
{
	if (!Globals->EditorData.ShowGizmos) return;

	static int GLID = assets::GetImage("Gizmo_Circle")->GLID;

	RenderTextureCenter(Center,
	                    vector2{Radius, Radius},
	                    GizmoGetColor(Color),
	                    vector2{}, vector2{1, 1},
	                    GLID,
	                    0,
	                    m4y4Identity(),
	                    Globals->DebugUIRenderer);
}

void GizmoAABB(aabb* WorldAABB,
               int32 ID = 0, color Color = {})
{
	if (!Globals->EditorData.ShowGizmos) return;

	GizmoBox(

	    AABBCenter(WorldAABB),
	vector3{
		AABBSizeX(WorldAABB) * 0.5f,
		AABBSizeY(WorldAABB) * 0.5f,
		AABBSizeZ(WorldAABB) * 0.5f,
	},
	ID, Color

	);
}

#endif