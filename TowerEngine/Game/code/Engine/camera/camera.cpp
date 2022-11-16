
void InitCamera(camera* Cam, shader* DisplayShader, vector2 Resolution, projection Projection, real64 FOV, real32 Near, real32 Far)
{
	Cam->Resolution = Resolution;
	Cam->Projection = Projection;
	RenderApi.GetFramebuffer(Cam, 0);
	//Cam->DisplayShader = assets::GetShader(DisplayShaderID);
	Cam->DisplayShader = DisplayShader;

	Cam->Near = Near;
	Cam->Far = Far;
}

vector2 WorldToScreen(vector3 WorldPos, camera* Cam)
{
	m4y4 Mat = MatrixMultiply(Cam->ViewMatrix, Cam->ProjectionMatrix);
	vector4 Point = Apply4y4_4(Mat, vector4{WorldPos.X, WorldPos.Y, WorldPos.Z, 1.0f});

	vector3 NDC = {
		Point.X / Point.W,
		Point.Y / Point.W,
		Point.Z / Point.W,
	};

	vector2 Screen = {
		(NDC.X * Globals->Window->Width * 0.5f) + (Globals->Window->Width * 0.5f),
		Globals->Window->Height - ((NDC.Y * Globals->Window->Height * 0.5f) + (Globals->Window->Height * 0.5f)),
	};

	return Screen;
}

real32 PlaneIntersectGetDist(vector3 RayOrigin, vector3 RayDir, vector3 PlaneCenter, vector3 PlaneNormal)
{
	real32 Denom = Vector3Dot(PlaneNormal, RayDir);
	if (Denom > 1e-6) {
		vector3 P = PlaneCenter - RayOrigin;
		real32 T = Vector3Dot(P, PlaneNormal) / Denom;
		return T;
	}

	return -1;
}

vector3 ScreenToWorld(vector2 Pos, vector3 PlanePos, vector3 PlaneNormal, camera * Cam)
{
	vector3 ClipCoords = {};
	ClipCoords.X = ((2.0f * Pos.X) / Globals->Window->Width) - 1.0f;
	ClipCoords.Y = 1.0f - ((2.0f * Pos.Y) / Globals->Window->Height);

	vector3 eyespace = Apply4y4(Cam->ProjectionMatrixInv, ClipCoords);
	vector3 worldspace = Apply4y4(Cam->ViewMatrixInv, eyespace);

	vector3 Dir = Vector3Normalize(worldspace - Cam->Center);
	real32 Length = PlaneIntersectGetDist(Cam->Center, Dir, PlanePos, PlaneNormal);

	// Check the opposite side of the plane
	if (Length < 1) {
		Length = PlaneIntersectGetDist(Cam->Center, Dir * -1, PlanePos, PlaneNormal) * -1;
	}

	// If still not intersection, then error
	if (Length < 1)  {
		ConsoleLog("Could not find intersecton.");
	}

	worldspace = Cam->Center + (Dir * Length);

	return (worldspace);
}