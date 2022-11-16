
struct plane {
	vector3 Normal;

	// Distance from the origin to the nearest point on the plane
	real32 Distance;
};

struct camera {
	vector3 Center;

	vector2 Resolution;
	real64 FOV;

	vector3 EulerRotation;
	vector3 Forward;

	real32 Near, Far;

	m4y4 ProjectionMatrix;
	m4y4 ProjectionMatrixInv;

	m4y4 ViewMatrix;
	m4y4 ViewMatrixInv;

	uint32 Framebuffer;
	uint32 TextureColorbuffers[3];
	shader* DisplayShader;

	projection Projection;

	//plane NearPlane, FarPlane, LeftPlane, RightPlane, TopPlane, BottomPlane;

	void UpdateMatricies(vector3 Up = vector3{0, 0, 1})
	{
		// Projection matrix
		if (Projection == projection::perspective) {

			real32 focalLength = 0.95f;
			real32 aspect = (real32)Resolution.X / (real32)Resolution.Y;

			real32 a = 1.0f;
			real32 b = aspect;
			real32 c = focalLength;

			real32 d = (Near + Far) / (Near - Far);
			real32 e = (2 * Far * Near) / (Near - Far);

			if (FOV > 0) {
				real64 S = 1 / tan( (FOV / 2) * (PI / 180) );

				ProjectionMatrix = {
					{
						{(real32)S,	0, 		0, 	0},
						{0, 	(real32)S, 	0, 	0},
						{0, 	0, 		d, 	e},
						{0, 	0, 		-1, 0}
					}
				};

			} else {
				ProjectionMatrix = {
					{
						{a * c,		0, 		0, 	0},
						{0, 	b * c, 	0, 	0},
						{0, 	0, 		d, 	e},
						{0, 	0, 		-1, 0}
					}
				};
			}

			ProjectionMatrixInv = {
				{
					{1 / (a * c),	0, 				0, 			0},
					{0, 			1 / (b * c), 	0, 			0},
					{0, 			0, 				0, 			-1},
					{0, 			0, 				1 / e, 		d / e}
				}
			};

			// Update the planes
			{

				/*
				vector3 FrontByFar = Forward * farClip;

				NearPlane.Distance = Center + (Forward * nearClip);
				NearPlane.Normal = Forward;
				*/

				/*
				FarPlane.Center = Center + (Forward * farClip);
				FarPlane.Normal = Forward * -1.0f;

				RightPlane.Center = Center;
				//RightPlane.Normal = Vector3Cross(Up, );

				LeftPlane.Center = Center;
				TopPlane.Center = Center;
				BottomPlane.Center = Center;
				*/
			}

		} else if (Projection == projection::brokenui) {

			real32 Left = 0;
			real32 Right = (real32)Resolution.X;
			real32 Bottom = (real32)Resolution.Y;
			real32 Top = 0;
			//real32 Near = -10.0f;
			//real32 Far = 10.0f;


			real32 tx = -((Right + Left) / (Right - Left));
			real32 ty = -((Top + Bottom) / (Top - Bottom));
			real32 tz = -((Far + Near) / (Far - Near));

			ProjectionMatrix = {
				{
					{2 / (Right - Left), 	0, 						0, 						tx},
					{0, 					2 / ( Top - Bottom), 	0, 						ty},
					{0, 					0, 						-2 / (Far - Near), 		tz},
					{0, 					0, 						0, 						1},
				}
			};


			ViewMatrix = {
				{
					{1,		0,		0, 		0},
					{0, 	1, 		0, 		0},
					{0, 	0, 		1, 		0},
					{0, 	0, 		0, 		1},
				}
			};

			return;

		} else if (Projection == projection::orthographic) {

			// TODO we don't need this anymore? Remove and test. We should just be able to use resolution
			real32 Width = (real32)Resolution.X * 0.01f;
			real32 Height = (real32)Resolution.Y * 0.01f;

			real32 Left = Width * -0.5f;
			real32 Right = Width * 0.5f;
			real32 Bottom = Height * -0.5f;
			real32 Top = Height * 0.5f;
			//real32 Near = 0.01f;
			//real32 Far = 50.0f;

			real32 tx = -((Right + Left) / (Right - Left));
			real32 ty = -((Top + Bottom) / (Top - Bottom));
			real32 tz = -((Far + Near) / (Far - Near));

			ProjectionMatrix = {
				{
					{2 / (Right - Left), 	0, 						0, 						tx},
					{0, 					2 / ( Top - Bottom), 	0, 						ty},
					{0, 					0, 						-2 / (Far - Near), 		tz},
					{0, 					0, 						0, 						1},
				}
			};

		} else {
			Assert(0);
		}

		// Rotate camera forward
		m4y4 yRot = RotationY((real32)EulerRotation.Y);
		Forward = Apply4y4(yRot, vector3{1, 0, 0});
		m4y4 zRot = RotationZ(-(real32)EulerRotation.X);
		Forward = Apply4y4(zRot, Forward);
		//m4y4 xRot = RotationX(-(real32)EulerRotation.Z);
		//Forward = Apply4y4(zRot, Forward);

		// View matrix
		{
			vector3 Target = (Center * -1) + Forward;

			vector3 zaxis = Vector3Normalize((Center * -1) - Target);
			vector3 xaxis = Vector3Normalize(Vector3Cross(Up, zaxis));
			vector3 yaxis = Vector3Cross(zaxis, xaxis);

			zaxis = zaxis * -1;


			// Forward

			ViewMatrix = {
				{
					{(real32)xaxis.X, (real32)xaxis.Y, (real32)xaxis.Z, 0},
					{(real32)yaxis.X, (real32)yaxis.Y, (real32)yaxis.Z, 0},
					{(real32)zaxis.X, (real32)zaxis.Y, (real32)zaxis.Z, 0},
					{0, 0, 0, 1}
				}
			};
			vector3 Pos = Apply4y4(ViewMatrix, (Center * -1));
			ViewMatrix.E[0][3] = (real32)Pos.X;
			ViewMatrix.E[1][3] = (real32)Pos.Y;
			ViewMatrix.E[2][3] = (real32)Pos.Z;


			// Inverse

			vector3 ix = xaxis / Square(Vector3Length(xaxis));
			vector3 iy = yaxis / Square(Vector3Length(yaxis));
			vector3 iz = zaxis / Square(Vector3Length(zaxis));
			vector3 ip = {
				(Pos.X * ix.X) + (Pos.Y * iy.X) + (Pos.Z * iz.X),
				(Pos.X * ix.Y) + (Pos.Y * iy.Y) + (Pos.Z * iz.Y),
				(Pos.X * ix.Z) + (Pos.Y * iy.Z) + (Pos.Z * iz.Z)
			};

			ViewMatrixInv = {
				{
					{(real32)ix.X, (real32)iy.X, (real32)iz.X, 0},
					{(real32)ix.Y, (real32)iy.Y, (real32)iz.Y, 0},
					{(real32)ix.Z, (real32)iy.Z, (real32)iz.Z, 0},
					{0, 0, 0, 1}
				}
			};
			ViewMatrixInv.E[0][3] = (real32)ip.X * -1;
			ViewMatrixInv.E[1][3] = (real32)ip.Y * -1;
			ViewMatrixInv.E[2][3] = (real32)ip.Z * -1;
		}
	}
};