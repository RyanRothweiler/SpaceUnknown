// This will also free the scene entities
void SceneClear(scene * Scene)
{
	for (int x = 0; x < ArrayCount(Scene->Entities); x++) {
		if (Scene->Entities[x] != GameNull) {
			assets::FreeEntity(Scene->Entities[x]);
			Scene->Entities[x] = GameNull;
		}
	}
}

void SceneAddTo(entity * Entity, scene * Scene)
{
	for (int x = 0; x < ArrayCount(Scene->Entities); x++) {
		if (Scene->Entities[x] == GameNull) {
			Scene->Entities[x] = Entity;
			return;
		}
	}

	// No room in scene!
	Assert(0);
}

void SceneRemoveFrom(entity * Entity, scene * Scene)
{
	for (int x = 0; x < ArrayCount(Scene->Entities); x++) {
		if (Scene->Entities[x] == Entity) {
			Scene->Entities[x] = GameNull;
			return;
		}
	}
}

void SceneUpdate(scene* Scene, renderer* GameRenderer)
{
	TIME_BEGIN

	for (uint32 i = 0; i < ArrayCount(Scene->Entities); i++) {

		entity* Entity = Scene->Entities[i];
		if (Entity != GameNull) {

			bool32 Visible = false;

			// This can be useful for debugging;
			//camera* Cam = GameRenderer->Camera;
			camera* Cam = Globals->ActiveCam;

			// frustum culling
			/*
			This one used the most
			https://bruop.github.io/frustum_culling/

			https://www.flipcode.com/archives/Frustum_Culling.shtml
			https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
			https://bruop.github.io/frustum_culling/
			*/

			{
				vector3 Max = vector3{Entity->AABBLocal.XMax, Entity->AABBLocal.YMax, Entity->AABBLocal.ZMax} + Entity->Transform.LocalPos;
				vector3 Min = vector3{Entity->AABBLocal.XMin, Entity->AABBLocal.YMin, Entity->AABBLocal.ZMin} + Entity->Transform.LocalPos;

				vector4 Corners[8] = {
					vector4{Min.X, Min.Y, Min.Z, 1.0f},
					vector4{Max.X, Min.Y, Min.Z, 1.0f},
					vector4{Min.X, Max.Y, Min.Z, 1.0f},
					vector4{Max.X, Max.Y, Min.Z, 1.0f},

					vector4{Min.X, Min.Y, Max.Z, 1.0f},
					vector4{Max.X, Min.Y, Max.Z, 1.0f},
					vector4{Min.X, Max.Y, Max.Z, 1.0f},
					vector4{Max.X, Max.Y, Max.Z, 1.0f},
				};

				m4y4 Mat = MatrixMultiply(Cam->ViewMatrix, Cam->ProjectionMatrix);

				for (int j = 0; j < ArrayCount(Corners); j++) {

					vector4 Point = Apply4y4_4(Mat, Corners[j]);

					// This works because math. Internet says so.
					if (
					    (-Point.W <= Point.X && Point.X <= Point.W) &&
					    (-Point.W <= Point.Y && Point.Y <= Point.W) &&
					    (0 <= Point.Z && Point.Z <= Point.W)
					) {
						Visible = true;
					}
				}
			}

			if (Visible) {

				if (Globals->EditorData.DisplayBoundingRadius) {
					GizmoSphere(Entity->Transform.LocalPos, Entity->BoundingRadius, 0, COLOR_RED);
				}
				if (Globals->EditorData.DisplayBoundingCube) {
					color Col = COLOR_RED;
					if (Visible) {
						Col = COLOR_GREEN;
					}

					vector3 Center = AABBCenter(&Entity->AABBLocal) + Entity->Transform.LocalPos;
					GizmoBox(Center,
					         vector3{AABBSizeX(&Entity->AABBLocal), AABBSizeY(&Entity->AABBLocal), AABBSizeZ(&Entity->AABBLocal)} * 0.5f,
					         0, Col
					        );
				}

				Entity->Transform.Update(m4y4Identity());
				RenderEntity(Entity, GameRenderer, Entity->ID);
			}
		}
	}
}
