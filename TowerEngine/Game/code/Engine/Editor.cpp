namespace editor {

	vector2 ScreenNearestOnAxis(vector3 ObjCenter, vector3 Axis, vector3 PerpAxis, vector2 MousePos, window_info * WindowInfo, camera * Cam)
	{
		// Object line
		vector2 A = WorldToScreen(ObjCenter, Cam);
		vector2 B = WorldToScreen(ObjCenter + Axis, Cam);

		vector2 LineDir = Vector2Normalize(A - B);
		LineDir = Vector2Perp(LineDir);

		// Mouse line
		vector3 MouseWorld = ScreenToWorld(MousePos, ObjCenter, vector3{0, 0, 1}, Cam);

		vector2 C = WorldToScreen(MouseWorld, Cam);
		vector2 D = WorldToScreen(MouseWorld + PerpAxis, Cam);

		// Find intersection between mouse line and axis line
		intersection_point P = GetIntersection(A, B, C, D);
		return P.Point;
	}

	void RenderAABB(entity * Entity, game_state * GameState, color Col = COLOR_RED)
	{
		real64 Thickness = 0.01f;
		GizmoPushColor(Col);
		aabb Bounds = Entity->GetScaledAABB();

		// right left
		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMin, Bounds.ZMax},
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMin, Bounds.ZMax},
		    Thickness);
		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMax, Bounds.ZMax},
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMax, Bounds.ZMax},
		    Thickness);

		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMin, Bounds.ZMin},
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMin, Bounds.ZMin},
		    Thickness);
		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMax, Bounds.ZMin},
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMax, Bounds.ZMin},
		    Thickness);

		// front back
		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMin, Bounds.ZMax},
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMax, Bounds.ZMax},
		    Thickness);
		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMin, Bounds.ZMax},
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMax, Bounds.ZMax},
		    Thickness);

		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMin, Bounds.ZMin},
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMax, Bounds.ZMin},
		    Thickness);
		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMin, Bounds.ZMin},
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMax, Bounds.ZMin},
		    Thickness);

		// Vertical edges
		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMin, Bounds.ZMin},
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMin, Bounds.ZMax},
		    Thickness);
		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMax, Bounds.ZMin},
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMax, Bounds.ZMax},
		    Thickness);

		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMin, Bounds.ZMin},
		    Entity->Transform.LocalPos + vector3{Bounds.XMax, Bounds.YMin, Bounds.ZMax},
		    Thickness);

		GizmoLine(
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMax, Bounds.ZMin},
		    Entity->Transform.LocalPos + vector3{Bounds.XMin, Bounds.YMax, Bounds.ZMax},
		    Thickness);

		GizmoPopColor();
	}

	void SwitchToEditorCam(game_state* GameState)
	{
		if (GameState->StateSerializing.ActiveCam != &GameState->StateSerializing.EditorCamera) {

			GameState->StateSerializing.EditorCamera.Center = GameState->StateSerializing.ActiveCam->Center;
			GameState->StateSerializing.EditorCamera.EulerRotation = GameState->StateSerializing.ActiveCam->EulerRotation;

			GameState->StateSerializing.ActiveCam = &GameState->StateSerializing.EditorCamera;
		}
	}

	void RenderCube(color Color, vector3 Pos, vector3 Size, game_state* GameState, uint32 ID)
	{
		Globals->UnitCube->Transform.LocalPos = Pos;
		Globals->UnitCube->Transform.LocalScale = Size;
		Globals->UnitCube->Transform.LocalRot = {};
		Globals->UnitCube->Transform.Update(m4y4Identity());

		Globals->UnitCube->ID = ID;

		Globals->UnitCube->Material->Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
		RenderEntity(Globals->UnitCube, &GameState->GameRenderer, ID);
	}

	void UseEditorCam(game_state* GameState)
	{
		editor::data* Data = &Globals->EditorData;
		state_to_serialize* State = &GameState->StateSerializing;

		Data->EditorMode = true;
		State->ActiveCam = &State->EditorCamera;
	}

	void TransformEditor(transform* Transform)
	{
		enum class moving_state {
			none,
			translation_choice, tx, ty, tz, txy, txz, tyz,
			rotation_choice, rx, ry, rz,
		};
		enum class moving_type {keyboard, mouse };

		static vector3 TranslationAxis;
		static vector3 TranslationPerpAxis;

		// Second axis is for plane movement, two axis movement
		static vector3 TranslationPerpAxisTwo;
		static vector3 TranslationAxisTwo;

		static vector3 TranslationPlaneNormal;

		enum class transform_relation { local, global };
		static transform_relation Relation;

		static vector3 RotationAxis;
		static vector2 RotationDirPrev;

		static moving_state MovingState;
		static moving_type MovingType;

		static vector3 PosStart;
		static vector3 PosStartTwo;
		static vector3 EntityStart;

		editor::data* EditorData = &Globals->EditorData;

		transform* TransformSelected = Transform;

		// ImGui
		{
			/*
			ImGui::Columns(3);
			if (ImGui::Button("Game Cam")) {
				State->ActiveCam = &State->GameCamera;
			}
			ImGui::NextColumn();
			if (ImGui::Button("Editor Cam")) {
				State->ActiveCam = &State->EditorCamera;
			}
			ImGui::NextColumn();
			if (ImGui::Button("Light Cam")) {
				State->ActiveCam = &State->Light.Cam;
			}
			ImGui::Columns(1);
			*/

			const char* items[] = { "Global"};
			static int item = 0;
			//ImGui::Combo("Relation", &item, items, IM_ARRAYSIZE(items));
			Relation = (transform_relation)item;

			if (TransformSelected != GameNull) {
				ImGui::Separator();

				ImGui::Text("Local Position");

				ImGui::Columns(3);

				float X = (float)TransformSelected->LocalPos.X;
				ImGui::DragFloat("X", &X, 0.001f);
				TransformSelected->LocalPos.X = (double)X;

				ImGui::NextColumn();

				float Y = (float)TransformSelected->LocalPos.Y;
				ImGui::DragFloat("Y", &Y, 0.001f);
				TransformSelected->LocalPos.Y = (double)Y;

				ImGui::NextColumn();

				float Z = (float)TransformSelected->LocalPos.Z;
				ImGui::DragFloat("Z", &Z, 0.001f);
				TransformSelected->LocalPos.Z = (double)Z;

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::PushID("RotationControls");

				ImGui::Text("Local Rotation");
				ImGui::Columns(3);

				float RX = (float)TransformSelected->LocalRot.X;
				ImGui::DragFloat("X", &RX, 0.001f);
				TransformSelected->LocalRot.X = (double)RX;

				ImGui::NextColumn();

				float RY = (float)TransformSelected->LocalRot.Y;
				ImGui::DragFloat("Y", &RY, 0.001f);
				TransformSelected->LocalRot.Y = (double)RY;

				ImGui::NextColumn();

				float RZ = (float)TransformSelected->LocalRot.Z;
				ImGui::DragFloat("Z", &RZ, 0.001f);
				TransformSelected->LocalRot.Z = (double)RZ;

				ImGui::Columns(1);

				ImGui::PopID();
			}
		}

		ImGui::Separator();


		bool32 XAxisClicked = false;
		bool32 YAxisClicked = false;
		bool32 ZAxisClicked = false;
		bool32 XYAxisClicked = false;
		bool32 ZXAxisClicked = false;
		bool32 ZYAxisClicked = false;

		if (TransformSelected != GameNull) {

			vector3 XAxis = vector3{1, 0, 0};
			vector3 YAxis = vector3{0, 1, 0};
			vector3 ZAxis = vector3{0, 0, 1};

			if (Relation == transform_relation::local) {
				quat Rot = {};
				Rot.FromEuler(TransformSelected->LocalRot);
				m4y4 RotMat = Rot.ToMatrix();

				XAxis = Apply4y4(RotMat, XAxis);
				YAxis = Apply4y4(RotMat, YAxis);
				ZAxis = Apply4y4(RotMat, ZAxis);
			}

			// Render origin axis
			{
				real32 Length = 0.5f;
				vector2 XYEnd = WorldToScreen(TransformSelected->LocalPos + (XAxis + YAxis) * Length, Globals->ActiveCam);
				vector2 XYStart = WorldToScreen(TransformSelected->LocalPos + (XAxis + YAxis) * Length * 2, Globals->ActiveCam);

				vector2 XZEnd = WorldToScreen(TransformSelected->LocalPos + (XAxis + ZAxis) * Length, Globals->ActiveCam);
				vector2 XZStart = WorldToScreen(TransformSelected->LocalPos + (XAxis + ZAxis) * Length * 2, Globals->ActiveCam);

				vector2 YZStart = WorldToScreen(TransformSelected->LocalPos + (YAxis + ZAxis) * Length, Globals->ActiveCam);
				vector2 YZEnd = WorldToScreen(TransformSelected->LocalPos + (YAxis + ZAxis) * Length * 2, Globals->ActiveCam);


				real32 AxisStart = 0.5f;
				real32 AxisEnd = 2.5f;
				vector2 XStart = WorldToScreen(TransformSelected->LocalPos + (XAxis * AxisStart), Globals->ActiveCam);
				vector2 YStart = WorldToScreen(TransformSelected->LocalPos + (YAxis * AxisStart), Globals->ActiveCam);
				vector2 ZStart = WorldToScreen(TransformSelected->LocalPos + (ZAxis * AxisStart), Globals->ActiveCam);
				vector2 XEnd = WorldToScreen(TransformSelected->LocalPos + (XAxis * AxisEnd), Globals->ActiveCam);
				vector2 YEnd = WorldToScreen(TransformSelected->LocalPos + (YAxis * AxisEnd), Globals->ActiveCam);
				vector2 ZEnd = WorldToScreen(TransformSelected->LocalPos + (ZAxis * AxisEnd), Globals->ActiveCam);

				vector2 Center = WorldToScreen(TransformSelected->LocalPos, Globals->ActiveCam);

				//RenderCubeLine(EntitySelected->Transform.LocalPos, EntitySelected->Transform.LocalPos + (XAxis * 2), 0.1f, COLOR_RED, GameState, IDStart);

				real32 Dist = (real32)Vector3Distance(Globals->ActiveCam->Center, TransformSelected->LocalPos) * 0.2f;
				real32 ArrowSize = 0.08f * Dist;
				real32 PlaneSize = 0.1f * Dist;


				uint32 IDStart = 500;
				uint32 XTranslateArrowID = 501;
				uint32 YTranslateArrowID = 502;
				uint32 ZTranslateArrowID = 503;
				uint32 XYTranslatePlaneID = 504;
				uint32 YZTranslatePlaneID = 505;
				uint32 ZXTranslatePlaneID = 506;

				GizmoArrow(TransformSelected->LocalPos, XAxis, ArrowSize, XTranslateArrowID, COLOR_RED);
				GizmoArrow(TransformSelected->LocalPos, YAxis, ArrowSize, YTranslateArrowID, COLOR_GREEN);
				GizmoArrow(TransformSelected->LocalPos, ZAxis, ArrowSize, ZTranslateArrowID, COLOR_BLUE);

				real32 PlaneAlpha = 0.06f;
				GizmoPushColor(color{1.0f, 1.0f, 1.0f, PlaneAlpha});
				GizmoPlane(TransformSelected->LocalPos + ((XAxis + YAxis) * (0.5f * Dist)), vector3{}, PlaneSize, XYTranslatePlaneID);
				GizmoPlane(TransformSelected->LocalPos + ((ZAxis + YAxis) * (0.5f * Dist)), vector3{0, PI / 2.0f, 0}, PlaneSize, YZTranslatePlaneID);
				GizmoPlane(TransformSelected->LocalPos + ((ZAxis + XAxis) * (0.5f * Dist)), vector3{PI / 2.0f, 0, 0}, PlaneSize, ZXTranslatePlaneID);
				GizmoPopColor();

				if (Globals->Input->MouseLeft.OnDown) {
					if (Globals->IDSelected == XTranslateArrowID) {
						XAxisClicked = true;
						MovingType = moving_type::mouse;
					}
					if (Globals->IDSelected == YTranslateArrowID) {
						YAxisClicked = true;
						MovingType = moving_type::mouse;
					}
					if (Globals->IDSelected == ZTranslateArrowID) {
						ZAxisClicked = true;
						MovingType = moving_type::mouse;
					}
					if (Globals->IDSelected == ZXTranslatePlaneID) {
						ZXAxisClicked = true;
						MovingType = moving_type::mouse;
					}
					if (Globals->IDSelected == XYTranslatePlaneID) {
						XYAxisClicked = true;
						MovingType = moving_type::mouse;
					}
					if (Globals->IDSelected == YZTranslatePlaneID) {
						ZYAxisClicked = true;
						MovingType = moving_type::mouse;
					}
				}
			}


			if (MovingState != moving_state::none) {

				bool open = true;
				ImGui::SetNextWindowPos(ImVec2((float)Globals->Input->MousePos.X, (float)Globals->Input->MousePos.Y));
				ImGui::Begin("Notify", &open, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);

				// Translation
				{
					if (MovingState == moving_state::translation_choice || MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz) {
						ImGui::Text("Moving");
					}

					if (MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz ||
					        MovingState == moving_state::txy || MovingState == moving_state::tyz || MovingState == moving_state::txz) {

						if (MovingState == moving_state::tx) {
							ImGui::Text("X");
						} else if (MovingState == moving_state::ty)  {
							ImGui::Text("Y");
						} else if (MovingState == moving_state::tz)  {
							ImGui::Text("Z");
						} else if (MovingState == moving_state::txz)  {
							ImGui::Text("XZ");
						} else if (MovingState == moving_state::tyz)  {
							ImGui::Text("YZ");
						} else if (MovingState == moving_state::txy)  {
							ImGui::Text("XY");
						}

						if (MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz) {

							vector2 Pos = ScreenNearestOnAxis(EntityStart, TranslationAxis, TranslationPerpAxis, Globals->Input->MousePos, Globals->Window, Globals->ActiveCam);
							vector3 PosNew = ScreenToWorld(Pos, EntityStart, TranslationPlaneNormal, Globals->ActiveCam);

							vector3 Delta = PosNew - PosStart;
							TransformSelected->LocalPos = EntityStart + Delta;

						} else if (MovingState == moving_state::txy || MovingState == moving_state::tyz || MovingState == moving_state::txz) {

							vector2 PosOne = ScreenNearestOnAxis(EntityStart, TranslationAxis, TranslationPerpAxis, Globals->Input->MousePos, Globals->Window, Globals->ActiveCam);
							vector3 PosOneNew = ScreenToWorld(PosOne, EntityStart, TranslationPlaneNormal, Globals->ActiveCam);

							vector2 PosTwo = ScreenNearestOnAxis(EntityStart, TranslationAxisTwo, TranslationPerpAxisTwo, Globals->Input->MousePos, Globals->Window, Globals->ActiveCam);
							vector3 PosTwoNew = ScreenToWorld(PosTwo, EntityStart, TranslationPlaneNormal, Globals->ActiveCam);

							vector3 DeltaOne = PosOneNew - PosStart;
							vector3 DeltaTwo = PosTwoNew - PosStartTwo;
							TransformSelected->LocalPos = EntityStart + DeltaOne + DeltaTwo;
						}
					}
				}

				// Rotation
				{
					if (MovingState == moving_state::rotation_choice || MovingState == moving_state::rx || MovingState == moving_state::ry || MovingState == moving_state::rz) {
						ImGui::Text("Rotating");
					}

					if (MovingState == moving_state::rx || MovingState == moving_state::ry || MovingState == moving_state::rz) {
						if (MovingState == moving_state::rx) {
							ImGui::Text("X");
						} else if (MovingState == moving_state::ry)  {
							ImGui::Text("Y");
						} else if (MovingState == moving_state::rz)  {
							ImGui::Text("Z");
						}


						vector2 ObjScreen = WorldToScreen(TransformSelected->LocalPos, Globals->ActiveCam);
						vector2 DirCurr = Vector2Normalize(ObjScreen - Globals->Input->MousePos) * -1.0f;

						real64 Rot = Vector2AngleBetween(RotationDirPrev, DirCurr);

						vector2 DirPerp = vector2{ -DirCurr.Y, DirCurr.X};

						vector2 CurrPoint = ObjScreen + (DirCurr * 100.0f);
						vector2 PrevPoint = ObjScreen + (RotationDirPrev * 100.0f);

						real64 DistToCurr = Vector2Distance(CurrPoint, CurrPoint + (DirPerp * 50.0f));
						real64 DistToPrev = Vector2Distance(PrevPoint, CurrPoint + (DirPerp * 50.0f));

						if (DistToCurr < DistToPrev) { Rot = -Rot; }

						if (RotationDirPrev.X != 0 && RotationDirPrev.Y != 0) {
							vector3 VecRot = vector3{Rot * RotationAxis.X, Rot * RotationAxis.Y, Rot * RotationAxis.Z};
							quat QuatRot = {};
							QuatRot.FromEuler(VecRot);
							m4y4 MatRot = QuatRot.ToMatrix();


							quat QuatRotCurrent = {};
							QuatRotCurrent.FromEuler(TransformSelected->LocalRot);
							m4y4 MatRotCurrent = QuatRotCurrent.ToMatrix();


							m4y4 Rotated = MatrixMultiply(MatRotCurrent, MatRot);
							quat QuatRotated = {};
							QuatRotated.FromMat(&Rotated);

							TransformSelected->LocalRot = QuatRotated.ToEuler();


							//EntitySelected->Transform.LocalRot = EntitySelected->Transform.LocalRot + vector3{Rot * RotationAxis.X, Rot * RotationAxis.Y, Rot * RotationAxis.Z};

							ImGui::Text(string{RotationAxis.X} .CharArray);
							ImGui::Text(string{RotationAxis.Y} .CharArray);
							ImGui::Text(string{RotationAxis.Z} .CharArray);
						}

						RenderScreenLineMesh(ObjScreen, CurrPoint, 2, COLOR_TEAL, Globals->DebugUIRenderer);

						RotationDirPrev = DirCurr;
					}
				}

				ImGui::End();
			}

			// Select transform type
			if (Globals->Input->KeyboardInput['G'].OnDown) {
				MovingState = moving_state::translation_choice;
				MovingType = moving_type::keyboard;
			}

			if (Globals->Input->KeyboardInput['R'].OnDown) {
				RotationDirPrev = {};
				MovingState = moving_state::rotation_choice;
			}


			// Translation
			if ((MovingState == moving_state::translation_choice &&
			        (Globals->Input->KeyboardInput['X'].OnDown || Globals->Input->KeyboardInput['Y'].OnDown || Globals->Input->KeyboardInput['Z'].OnDown))
			        || XAxisClicked || YAxisClicked || ZAxisClicked || XYAxisClicked || ZXAxisClicked || ZYAxisClicked
			   ) {

				if (Globals->Input->KeyboardInput['X'].OnDown || XAxisClicked) {
					MovingState = moving_state::tx;
					TranslationPlaneNormal = vector3{0, 0, -1};

					TranslationAxis = XAxis;
					TranslationPerpAxis = YAxis;

					TranslationAxisTwo = {};

				} else if (Globals->Input->KeyboardInput['Y'].OnDown || YAxisClicked) {
					MovingState = moving_state::ty;
					TranslationPlaneNormal = vector3{0, 0, -1};

					TranslationAxis = YAxis;
					TranslationPerpAxis = XAxis;

					TranslationAxisTwo = {};

				} else if (Globals->Input->KeyboardInput['Z'].OnDown || ZAxisClicked) {
					MovingState = moving_state::tz;
					TranslationPlaneNormal = vector3{0, -1, 0};

					TranslationAxis = ZAxis;
					TranslationPerpAxis = XAxis;

					TranslationAxisTwo = {};
				} else if (ZXAxisClicked) {
					MovingState = moving_state::txz;
					TranslationPlaneNormal = vector3{0, -1, 0};

					TranslationAxis = ZAxis;
					TranslationPerpAxis = XAxis;

					TranslationAxisTwo = XAxis;
					TranslationPerpAxisTwo = ZAxis;

				} else if (ZYAxisClicked) {
					MovingState = moving_state::tyz;
					TranslationPlaneNormal = vector3{ -1, 0, 0};

					TranslationAxis = ZAxis;
					TranslationPerpAxis = YAxis;

					TranslationAxisTwo = YAxis;
					TranslationPerpAxisTwo = ZAxis;

				} else if (XYAxisClicked) {
					MovingState = moving_state::txy;
					TranslationPlaneNormal = vector3{0, 0, -1};

					TranslationAxis = XAxis;
					TranslationPerpAxis = YAxis;

					TranslationAxisTwo = YAxis;
					TranslationPerpAxisTwo = XAxis;
				}



				// NOTE copied from above
				if (MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz) {

					vector2 Pos = ScreenNearestOnAxis(TransformSelected->LocalPos, TranslationAxis, TranslationPerpAxis, Globals->Input->MousePos, Globals->Window, Globals->ActiveCam);
					PosStart = ScreenToWorld(Pos, TransformSelected->LocalPos, TranslationPlaneNormal, Globals->ActiveCam);
					EntityStart = TransformSelected->LocalPos;

				} else if (MovingState == moving_state::txy || MovingState == moving_state::tyz || MovingState == moving_state::txz) {

					vector2 Pos = {};

					Pos = ScreenNearestOnAxis(TransformSelected->LocalPos, TranslationAxis, TranslationPerpAxis, Globals->Input->MousePos, Globals->Window, Globals->ActiveCam);
					PosStart = ScreenToWorld(Pos, TransformSelected->LocalPos, TranslationPlaneNormal, Globals->ActiveCam);

					Pos = ScreenNearestOnAxis(TransformSelected->LocalPos, TranslationAxisTwo, TranslationPerpAxisTwo, Globals->Input->MousePos, Globals->Window, Globals->ActiveCam);
					PosStartTwo = ScreenToWorld(Pos, TransformSelected->LocalPos, TranslationPlaneNormal, Globals->ActiveCam);

					EntityStart = TransformSelected->LocalPos;
				}

			}

			// Rotation
			if (MovingState == moving_state::rotation_choice &&
			        (Globals->Input->KeyboardInput['X'].OnDown || Globals->Input->KeyboardInput['Y'].OnDown || Globals->Input->KeyboardInput['Z'].OnDown)) {

				if (Globals->Input->KeyboardInput['X'].OnDown) {
					MovingState = moving_state::rx;
					RotationAxis = XAxis;
				} else if (Globals->Input->KeyboardInput['Y'].OnDown) {
					MovingState = moving_state::ry;
					RotationAxis = YAxis;
				} else if (Globals->Input->KeyboardInput['Z'].OnDown) {
					MovingState = moving_state::rz;
					RotationAxis = ZAxis;
				}
			}

			if (Globals->Input->KeyboardInput[KEY_ESC].OnDown) {
				MovingState = moving_state::none;
				Globals->EditorData.TransformSelected = GameNull;
				Globals->EditorData.EntitySelected = GameNull;
			}

			// Releasing
			{
				if (MovingType == moving_type::keyboard && Globals->Input->MouseLeft.OnDown &&
				        (
				            MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz ||
				            MovingState == moving_state::rx || MovingState == moving_state::ry || MovingState == moving_state::rz
				        )
				   ) {
					MovingState = moving_state::none;
				}
				if (MovingType == moving_type::mouse && Globals->Input->MouseLeft.OnUp &&
				        (
				            MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz ||
				            MovingState == moving_state::txy || MovingState == moving_state::tyz || MovingState == moving_state::txz ||
				            MovingState == moving_state::rx || MovingState == moving_state::ry || MovingState == moving_state::rz
				        )
				   ) {
					MovingState = moving_state::none;
				}
			}
		} else {
			MovingState = moving_state::none;
		}

	}

	void Update(game_state* GameState, window_info* WindowInfo)
	{
		uint32 ConnectionsIDStart = 1000;
		uint32 MapIDStart = 2000;
		uint32 LightsIDStart = 3000;

		editor::data* Data = &Globals->EditorData;
		state_to_serialize* State = &GameState->StateSerializing;

		asset_material* MaterialEditing = Data->MaterialEditing;

		if (Data->DebugLights) {
			real64 Size = 0.1f;

			static entity* PointObj = assets::AllocateEntity("Engine_LightPoint");
			static entity* DirectionalObj = assets::AllocateEntity("Engine_LightDirectional");

			for (int x = 0; x < render::Data->LightsCount; x++) {

				light* Light = render::Data->Lights[x];

				uint32 ID = LightsIDStart + x;
				//real32 Dist = (real32)Vector3Distance(GameState->StateSerializing.ActiveCam->Center, Light->Transform.LocalPos) * 0.2f;

				entity* Obj = {};

				if (Light->Type == light_type::point) 		{ Obj = PointObj; }
				if (Light->Type == light_type::directional) { Obj = DirectionalObj; }

				Obj->Transform.LocalPos = Light->Transform.LocalPos;
				Obj->Transform.LocalScale = vector3{Size, Size, Size};
				Obj->Transform.LocalRot = Light->Transform.LocalRot;
				Obj->Transform.Update(m4y4Identity());

				Obj->Material->Uniforms.SetVec4("color", v4{1.0f, 1.0f, 1.0f, 1.0f});
				Obj->ID = ID;

				RenderEntity(Obj, &GameState->GameRenderer, ID);

				if (Globals->Input->MouseLeft.OnDown && Globals->IDSelected == ID) {
					Data->TransformSelected = &Light->Transform;
					Data->EntitySelected = GameNull;
				}
			}
		}

		// Top bar
		if (ImGui::BeginMainMenuBar()) {
			// fps
			{
				string str = "FPS " + string{GameState->PrevFrameFPS};
				ImGui::Text(str.CharArray);
				ImGui::Separator();
			}

			if (ImGui::BeginMenu("Windows")) {
				if (ImGui::MenuItem("Entity Library"))  { Data->EntityLibraryOpen = !Data->EntityLibraryOpen;  		}
				if (ImGui::MenuItem("Materials"))  		{ Data->MaterialEditorOpen = !Data->MaterialEditorOpen; 	}
				if (ImGui::MenuItem("Transform"))  		{ Data->TransformWindowOpen = !Data->TransformWindowOpen; 	}
				if (ImGui::MenuItem("Profiler"))  		{ Data->ProfilerWindowOpen = !Data->ProfilerWindowOpen; 	}

				ImGui::EndMenu();
			}

			// Debug window
			if (ImGui::BeginMenu("Debug")) {
				ImGui::Checkbox("Debug Lights", &Data->DebugLights);
				ImGui::Checkbox("Radius Collisions", &Data->DisplayBoundingRadius);
				ImGui::Checkbox("Cube Collisions", &Data->DisplayBoundingCube);
				ImGui::Checkbox("Render Camera Objects", &Data->RenderCameraObjects);

				if (ImGui::BeginMenu("Map")) {
					ImGui::Checkbox("Display Connections", &Data->DisplayConnections);
					ImGui::Checkbox("Display Path", &Data->DebugPath);
					ImGui::Checkbox("Room Graph", &Data->DebugRoomGraph);
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (Data->GodMode) {
				// god mode editor stuff here
			}

			// Editor / GameMode
			{
				ImGui::Separator();
				ImGui::Checkbox("Show Gizmos", &Data->ShowGizmos);
				ImGui::Checkbox("God Mode", &Data->GodMode);

				ImGui::Separator();

				if (Data->EditorMode) {
					ImGui::Text("EDITOR");
				} else {
					ImGui::Text("GAME");
				}
				ImGui::Separator();

				/*
				int ModeSelect = 0;
				if (Globals->EditorData.EditorMode) { ModeSelect = 1; }
				int PrevSelect = ModeSelect;

				ImGui::RadioButton("Game Mode", &ModeSelect, 0);
				ImGui::RadioButton("Editor Mode", &ModeSelect, 1);
				if (PrevSelect != ModeSelect) {
					if (ModeSelect == 0) {
						Data->EditorMode = false;
						State->ActiveCam = &State->GameCamera;
					} else if (ModeSelect == 1) {
						UseEditorCam(GameState);
					}
				}
				*/

			}

			ImGui::EndMainMenuBar();
		}

		static float time = 0;
		time += 0.05f;

		// Entity selector
		if (Globals->EditorData.EntityLibraryOpen) {
			ImGui::Begin("Entities Library", &Globals->EditorData.EntityLibraryOpen);

			ImGui::Columns(2);
			for (int x = 0; x < Globals->AssetsList.EntitiesCount; x++) {
				ImGui::PushID(Globals->AssetsList.Entities[x].ID.CharArray);

				ImGui::Text(Globals->AssetsList.Entities[x].ID.CharArray);
				ImGui::NextColumn();

				if (ImGui::Button("preview")) {
					UseEditorCam(GameState);
					State->EditorCamera.EulerRotation = { -5.08f, -0.348f, 0};
					State->EditorCamera.Center = vector3{2.19f, -3.985f, 2.962f} + vector3{1000, 1000, 1000};

					SceneClear(&Data->EntityPreviewScene);

					Data->EntityPreviewing = assets::AllocateEntity(Globals->AssetsList.Entities[x].ID);
					SceneAddTo(Data->EntityPreviewing, &Data->EntityPreviewScene);
				}

				if (Data->EntityPreviewing != GameNull) {
					Data->EntityPreviewing->Transform.LocalPos = vector3{1000, 1000, 1000};
					//Data->EntityPreviewing->Transform.LocalRot.Z += 0.0005f;
					Data->EntityPreviewing->Armature->UpdateAnim(time);
				}

				ImGui::PopID();
				ImGui::NextColumn();
			}
			ImGui::Columns(1);

			SceneUpdate(&Data->EntityPreviewScene, &GameState->GameRenderer);
			ImGui::End();
		}

		if (Globals->EditorData.RenderCameraObjects) {
			if (&State->GameCamera != State->ActiveCam) {
				GizmoCamera(&State->GameCamera);
				GizmoArrow(State->GameCamera.Center, State->GameCamera.Forward * -1, 0.1f);

				vector3 Right = Vector3Normalize(Vector3Cross(vector3{0, 0, 1}, State->GameCamera.Forward));
				GizmoArrow(State->GameCamera.Center, Right * -1, 0.1f, 0, COLOR_GREEN);
			}
		}

		// Transform editor
		if (Globals->EditorData.TransformWindowOpen) {

			enum class moving_state {
				none,
				translation_choice, tx, ty, tz, txy, txz, tyz,
				rotation_choice, rx, ry, rz,
			};
			enum class moving_type {keyboard, mouse };

			static vector3 TranslationAxis;
			static vector3 TranslationPerpAxis;

			// Second axis is for plane movement, two axis movement
			static vector3 TranslationPerpAxisTwo;
			static vector3 TranslationAxisTwo;

			static vector3 TranslationPlaneNormal;

			enum class transform_relation { local, global };
			static transform_relation Relation;

			static vector3 RotationAxis;
			static vector2 RotationDirPrev;

			static moving_state MovingState;
			static moving_type MovingType;

			static vector3 PosStart;
			static vector3 PosStartTwo;
			static vector3 EntityStart;

			editor::data* EditorData = &Globals->EditorData;

			entity* NewSelection = CheckSceneSelection(&State->Scene, &GameState->GameRenderer, State->ActiveCam, WindowInfo);
			if (NewSelection != GameNull) {
				EditorData->EntitySelected = NewSelection;
				EditorData->TransformSelected = &EditorData->EntitySelected->Transform;
			}

			transform* TransformSelected = EditorData->TransformSelected;
			entity* EntitySelected = EditorData->EntitySelected;

			// ImGui
			{
				ImGui::Begin("Transform Editor", &Globals->EditorData.TransformWindowOpen);

				// cam rotation
				{
					ImGui::PushID("ActiveRotaiton");

					ImGui::Text("Active Rotation");
					ImGui::Columns(3);

					float X = (float)State->ActiveCam->EulerRotation.X;
					ImGui::DragFloat("X", &X, 0.001f);
					State->ActiveCam->EulerRotation.X = (double)X;

					ImGui::NextColumn();

					float Y = (float)State->ActiveCam->EulerRotation.Y;
					ImGui::DragFloat("Y", &Y, 0.001f);
					State->ActiveCam->EulerRotation.Y = (double)Y;

					ImGui::NextColumn();

					float Z = (float)State->ActiveCam->EulerRotation.Z;
					ImGui::DragFloat("Z", &Z, 0.001f);
					State->ActiveCam->EulerRotation.Z = (double)Z;

					ImGui::Columns(1);
					ImGui::PopID();
				}

				// cam pos
				{
					ImGui::PushID("ActivePos");

					ImGui::Text("Active Position");
					ImGui::Columns(3);

					float X = (float)State->ActiveCam->Center.X;
					ImGui::DragFloat("X", &X, 0.001f);
					State->ActiveCam->Center.X = (double)X;

					ImGui::NextColumn();

					float Y = (float)State->ActiveCam->Center.Y;
					ImGui::DragFloat("Y", &Y, 0.001f);
					State->ActiveCam->Center.Y = (double)Y;

					ImGui::NextColumn();

					float Z = (float)State->ActiveCam->Center.Z;
					ImGui::DragFloat("Z", &Z, 0.001f);
					State->ActiveCam->Center.Z = (double)Z;

					ImGui::Columns(1);
					ImGui::PopID();
				}

				ImGui::Separator();

				/*
				ImGui::Columns(3);
				if (ImGui::Button("Game Cam")) {
					State->ActiveCam = &State->GameCamera;
				}
				ImGui::NextColumn();
				if (ImGui::Button("Editor Cam")) {
					State->ActiveCam = &State->EditorCamera;
				}
				ImGui::NextColumn();
				if (ImGui::Button("Light Cam")) {
					State->ActiveCam = &State->Light.Cam;
				}
				ImGui::Columns(1);
				*/

				const char* items[] = { "Global"};
				static int item = 0;
				//ImGui::Combo("Relation", &item, items, IM_ARRAYSIZE(items));
				Relation = (transform_relation)item;

				if (TransformSelected != GameNull) {
					ImGui::Separator();

					ImGui::Text("Local Position");

					ImGui::Columns(3);

					float X = (float)TransformSelected->LocalPos.X;
					ImGui::DragFloat("X", &X, 0.001f);
					TransformSelected->LocalPos.X = (double)X;

					ImGui::NextColumn();

					float Y = (float)TransformSelected->LocalPos.Y;
					ImGui::DragFloat("Y", &Y, 0.001f);
					TransformSelected->LocalPos.Y = (double)Y;

					ImGui::NextColumn();

					float Z = (float)TransformSelected->LocalPos.Z;
					ImGui::DragFloat("Z", &Z, 0.001f);
					TransformSelected->LocalPos.Z = (double)Z;

					ImGui::Columns(1);
					ImGui::Separator();
					ImGui::PushID("RotationControls");

					ImGui::Text("Local Rotation");
					ImGui::Columns(3);

					float RX = (float)TransformSelected->LocalRot.X;
					ImGui::DragFloat("X", &RX, 0.001f);
					TransformSelected->LocalRot.X = (double)RX;

					ImGui::NextColumn();

					float RY = (float)TransformSelected->LocalRot.Y;
					ImGui::DragFloat("Y", &RY, 0.001f);
					TransformSelected->LocalRot.Y = (double)RY;

					ImGui::NextColumn();

					float RZ = (float)TransformSelected->LocalRot.Z;
					ImGui::DragFloat("Z", &RZ, 0.001f);
					TransformSelected->LocalRot.Z = (double)RZ;

					ImGui::Columns(1);

					ImGui::PopID();
				}

				if (EntitySelected != GameNull) {

					RenderAABB(EntitySelected, GameState);

					ImGui::Separator();
					ImGui::Text(&EntitySelected->MaterialID.CharArray[0]);
					ImGui::SameLine();
					if (ImGui::Button("Edit")) {
						Globals->EditorData.MaterialEditorOpen = true;

						list_link* CurrentLink = Globals->AssetsList.Materials->TopLink;
						for (uint32 i = 0; i < Globals->AssetsList.Materials->LinkCount; i++) {
							asset_material* Mat = (asset_material*)CurrentLink->Data;
							CurrentLink = CurrentLink->NextLink;

							if (Mat->Name == EntitySelected->MaterialID) {
								Globals->EditorData.MaterialEditing = Mat;
								break;
							}
						}
					}
				}

				ImGui::End();
			}


			bool32 XAxisClicked = false;
			bool32 YAxisClicked = false;
			bool32 ZAxisClicked = false;
			bool32 XYAxisClicked = false;
			bool32 ZXAxisClicked = false;
			bool32 ZYAxisClicked = false;

			if (TransformSelected != GameNull) {

				vector3 XAxis = vector3{1, 0, 0};
				vector3 YAxis = vector3{0, 1, 0};
				vector3 ZAxis = vector3{0, 0, 1};

				if (Relation == transform_relation::local) {
					quat Rot = {};
					Rot.FromEuler(TransformSelected->LocalRot);
					m4y4 RotMat = Rot.ToMatrix();

					XAxis = Apply4y4(RotMat, XAxis);
					YAxis = Apply4y4(RotMat, YAxis);
					ZAxis = Apply4y4(RotMat, ZAxis);
				}

				// Render origin axis
				{
					real32 Length = 0.5f;
					vector2 XYEnd = WorldToScreen(TransformSelected->LocalPos + (XAxis + YAxis) * Length, State->ActiveCam);
					vector2 XYStart = WorldToScreen(TransformSelected->LocalPos + (XAxis + YAxis) * Length * 2, State->ActiveCam);

					vector2 XZEnd = WorldToScreen(TransformSelected->LocalPos + (XAxis + ZAxis) * Length, State->ActiveCam);
					vector2 XZStart = WorldToScreen(TransformSelected->LocalPos + (XAxis + ZAxis) * Length * 2, State->ActiveCam);

					vector2 YZStart = WorldToScreen(TransformSelected->LocalPos + (YAxis + ZAxis) * Length, State->ActiveCam);
					vector2 YZEnd = WorldToScreen(TransformSelected->LocalPos + (YAxis + ZAxis) * Length * 2, State->ActiveCam);


					real32 AxisStart = 0.5f;
					real32 AxisEnd = 2.5f;
					vector2 XStart = WorldToScreen(TransformSelected->LocalPos + (XAxis * AxisStart), State->ActiveCam);
					vector2 YStart = WorldToScreen(TransformSelected->LocalPos + (YAxis * AxisStart), State->ActiveCam);
					vector2 ZStart = WorldToScreen(TransformSelected->LocalPos + (ZAxis * AxisStart), State->ActiveCam);
					vector2 XEnd = WorldToScreen(TransformSelected->LocalPos + (XAxis * AxisEnd), State->ActiveCam);
					vector2 YEnd = WorldToScreen(TransformSelected->LocalPos + (YAxis * AxisEnd), State->ActiveCam);
					vector2 ZEnd = WorldToScreen(TransformSelected->LocalPos + (ZAxis * AxisEnd), State->ActiveCam);

					vector2 Center = WorldToScreen(TransformSelected->LocalPos, State->ActiveCam);

					//RenderCubeLine(EntitySelected->Transform.LocalPos, EntitySelected->Transform.LocalPos + (XAxis * 2), 0.1f, COLOR_RED, GameState, IDStart);

					real32 Dist = (real32)Vector3Distance(State->ActiveCam->Center, TransformSelected->LocalPos) * 0.2f;
					real32 ArrowSize = 0.08f * Dist;
					real32 PlaneSize = 0.1f * Dist;


					uint32 IDStart = 500;
					uint32 XTranslateArrowID = 501;
					uint32 YTranslateArrowID = 502;
					uint32 ZTranslateArrowID = 503;
					uint32 XYTranslatePlaneID = 504;
					uint32 YZTranslatePlaneID = 505;
					uint32 ZXTranslatePlaneID = 506;

					GizmoArrow(TransformSelected->LocalPos, XAxis, ArrowSize, XTranslateArrowID, COLOR_RED);
					GizmoArrow(TransformSelected->LocalPos, YAxis, ArrowSize, YTranslateArrowID, COLOR_GREEN);
					GizmoArrow(TransformSelected->LocalPos, ZAxis, ArrowSize, ZTranslateArrowID, COLOR_BLUE);

					real32 PlaneAlpha = 0.06f;
					GizmoPushColor(color{1.0f, 1.0f, 1.0f, PlaneAlpha});
					GizmoPlane(TransformSelected->LocalPos + ((XAxis + YAxis) * (0.5f * Dist)), vector3{}, PlaneSize, XYTranslatePlaneID);
					GizmoPlane(TransformSelected->LocalPos + ((ZAxis + YAxis) * (0.5f * Dist)), vector3{0, PI / 2.0f, 0}, PlaneSize, YZTranslatePlaneID);
					GizmoPlane(TransformSelected->LocalPos + ((ZAxis + XAxis) * (0.5f * Dist)), vector3{PI / 2.0f, 0, 0}, PlaneSize, ZXTranslatePlaneID);
					GizmoPopColor();

					if (Globals->Input->MouseLeft.OnDown) {
						if (Globals->IDSelected == XTranslateArrowID) {
							XAxisClicked = true;
							MovingType = moving_type::mouse;
						}
						if (Globals->IDSelected == YTranslateArrowID) {
							YAxisClicked = true;
							MovingType = moving_type::mouse;
						}
						if (Globals->IDSelected == ZTranslateArrowID) {
							ZAxisClicked = true;
							MovingType = moving_type::mouse;
						}
						if (Globals->IDSelected == ZXTranslatePlaneID) {
							ZXAxisClicked = true;
							MovingType = moving_type::mouse;
						}
						if (Globals->IDSelected == XYTranslatePlaneID) {
							XYAxisClicked = true;
							MovingType = moving_type::mouse;
						}
						if (Globals->IDSelected == YZTranslatePlaneID) {
							ZYAxisClicked = true;
							MovingType = moving_type::mouse;
						}
					}
				}


				if (MovingState != moving_state::none) {

					bool open = true;
					ImGui::SetNextWindowPos(ImVec2((float)Globals->Input->MousePos.X, (float)Globals->Input->MousePos.Y));
					ImGui::Begin("Notify", &open, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);

					// Translation
					{
						if (MovingState == moving_state::translation_choice || MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz) {
							ImGui::Text("Moving");
						}

						if (MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz ||
						        MovingState == moving_state::txy || MovingState == moving_state::tyz || MovingState == moving_state::txz) {

							if (MovingState == moving_state::tx) {
								ImGui::Text("X");
							} else if (MovingState == moving_state::ty)  {
								ImGui::Text("Y");
							} else if (MovingState == moving_state::tz)  {
								ImGui::Text("Z");
							} else if (MovingState == moving_state::txz)  {
								ImGui::Text("XZ");
							} else if (MovingState == moving_state::tyz)  {
								ImGui::Text("YZ");
							} else if (MovingState == moving_state::txy)  {
								ImGui::Text("XY");
							}

							if (MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz) {

								vector2 Pos = ScreenNearestOnAxis(EntityStart, TranslationAxis, TranslationPerpAxis, Globals->Input->MousePos, WindowInfo, State->ActiveCam);
								vector3 PosNew = ScreenToWorld(Pos, EntityStart, TranslationPlaneNormal, State->ActiveCam);

								vector3 Delta = PosNew - PosStart;
								TransformSelected->LocalPos = EntityStart + Delta;

							} else if (MovingState == moving_state::txy || MovingState == moving_state::tyz || MovingState == moving_state::txz) {

								vector2 PosOne = ScreenNearestOnAxis(EntityStart, TranslationAxis, TranslationPerpAxis, Globals->Input->MousePos, WindowInfo, State->ActiveCam);
								vector3 PosOneNew = ScreenToWorld(PosOne, EntityStart, TranslationPlaneNormal, State->ActiveCam);

								vector2 PosTwo = ScreenNearestOnAxis(EntityStart, TranslationAxisTwo, TranslationPerpAxisTwo, Globals->Input->MousePos, WindowInfo, State->ActiveCam);
								vector3 PosTwoNew = ScreenToWorld(PosTwo, EntityStart, TranslationPlaneNormal, State->ActiveCam);

								vector3 DeltaOne = PosOneNew - PosStart;
								vector3 DeltaTwo = PosTwoNew - PosStartTwo;
								TransformSelected->LocalPos = EntityStart + DeltaOne + DeltaTwo;
							}
						}
					}

					// Rotation
					{
						if (MovingState == moving_state::rotation_choice || MovingState == moving_state::rx || MovingState == moving_state::ry || MovingState == moving_state::rz) {
							ImGui::Text("Rotating");
						}

						if (MovingState == moving_state::rx || MovingState == moving_state::ry || MovingState == moving_state::rz) {
							if (MovingState == moving_state::rx) {
								ImGui::Text("X");
							} else if (MovingState == moving_state::ry)  {
								ImGui::Text("Y");
							} else if (MovingState == moving_state::rz)  {
								ImGui::Text("Z");
							}


							vector2 ObjScreen = WorldToScreen(TransformSelected->LocalPos, State->ActiveCam);
							vector2 DirCurr = Vector2Normalize(ObjScreen - Globals->Input->MousePos) * -1.0f;

							real64 Rot = Vector2AngleBetween(RotationDirPrev, DirCurr);

							vector2 DirPerp = vector2{ -DirCurr.Y, DirCurr.X};

							vector2 CurrPoint = ObjScreen + (DirCurr * 100.0f);
							vector2 PrevPoint = ObjScreen + (RotationDirPrev * 100.0f);

							real64 DistToCurr = Vector2Distance(CurrPoint, CurrPoint + (DirPerp * 50.0f));
							real64 DistToPrev = Vector2Distance(PrevPoint, CurrPoint + (DirPerp * 50.0f));

							if (DistToCurr < DistToPrev) { Rot = -Rot; }

							if (RotationDirPrev.X != 0 && RotationDirPrev.Y != 0) {
								vector3 VecRot = vector3{Rot * RotationAxis.X, Rot * RotationAxis.Y, Rot * RotationAxis.Z};
								quat QuatRot = {};
								QuatRot.FromEuler(VecRot);
								m4y4 MatRot = QuatRot.ToMatrix();


								quat QuatRotCurrent = {};
								QuatRotCurrent.FromEuler(TransformSelected->LocalRot);
								m4y4 MatRotCurrent = QuatRotCurrent.ToMatrix();


								m4y4 Rotated = MatrixMultiply(MatRotCurrent, MatRot);
								quat QuatRotated = {};
								QuatRotated.FromMat(&Rotated);

								TransformSelected->LocalRot = QuatRotated.ToEuler();


								//EntitySelected->Transform.LocalRot = EntitySelected->Transform.LocalRot + vector3{Rot * RotationAxis.X, Rot * RotationAxis.Y, Rot * RotationAxis.Z};

								ImGui::Text(string{RotationAxis.X} .CharArray);
								ImGui::Text(string{RotationAxis.Y} .CharArray);
								ImGui::Text(string{RotationAxis.Z} .CharArray);
							}

							RenderScreenLineMesh(ObjScreen, CurrPoint, 2, COLOR_TEAL, &GameState->DebugUIRenderer);

							RotationDirPrev = DirCurr;
						}
					}

					ImGui::End();
				}

				// Select transform type
				if (Globals->Input->KeyboardInput['G'].OnDown) {
					MovingState = moving_state::translation_choice;
					MovingType = moving_type::keyboard;
				}

				if (Globals->Input->KeyboardInput['R'].OnDown) {
					RotationDirPrev = {};
					MovingState = moving_state::rotation_choice;
				}


				// Translation
				if ((MovingState == moving_state::translation_choice &&
				        (Globals->Input->KeyboardInput['X'].OnDown || Globals->Input->KeyboardInput['Y'].OnDown || Globals->Input->KeyboardInput['Z'].OnDown))
				        || XAxisClicked || YAxisClicked || ZAxisClicked || XYAxisClicked || ZXAxisClicked || ZYAxisClicked
				   ) {

					if (Globals->Input->KeyboardInput['X'].OnDown || XAxisClicked) {
						MovingState = moving_state::tx;
						TranslationPlaneNormal = vector3{0, 0, -1};

						TranslationAxis = XAxis;
						TranslationPerpAxis = YAxis;

						TranslationAxisTwo = {};

					} else if (Globals->Input->KeyboardInput['Y'].OnDown || YAxisClicked) {
						MovingState = moving_state::ty;
						TranslationPlaneNormal = vector3{0, 0, -1};

						TranslationAxis = YAxis;
						TranslationPerpAxis = XAxis;

						TranslationAxisTwo = {};

					} else if (Globals->Input->KeyboardInput['Z'].OnDown || ZAxisClicked) {
						MovingState = moving_state::tz;
						TranslationPlaneNormal = vector3{0, -1, 0};

						TranslationAxis = ZAxis;
						TranslationPerpAxis = XAxis;

						TranslationAxisTwo = {};
					} else if (ZXAxisClicked) {
						MovingState = moving_state::txz;
						TranslationPlaneNormal = vector3{0, -1, 0};

						TranslationAxis = ZAxis;
						TranslationPerpAxis = XAxis;

						TranslationAxisTwo = XAxis;
						TranslationPerpAxisTwo = ZAxis;

					} else if (ZYAxisClicked) {
						MovingState = moving_state::tyz;
						TranslationPlaneNormal = vector3{ -1, 0, 0};

						TranslationAxis = ZAxis;
						TranslationPerpAxis = YAxis;

						TranslationAxisTwo = YAxis;
						TranslationPerpAxisTwo = ZAxis;

					} else if (XYAxisClicked) {
						MovingState = moving_state::txy;
						TranslationPlaneNormal = vector3{0, 0, -1};

						TranslationAxis = XAxis;
						TranslationPerpAxis = YAxis;

						TranslationAxisTwo = YAxis;
						TranslationPerpAxisTwo = XAxis;
					}



					// NOTE copied from above
					if (MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz) {

						vector2 Pos = ScreenNearestOnAxis(TransformSelected->LocalPos, TranslationAxis, TranslationPerpAxis, Globals->Input->MousePos, WindowInfo, State->ActiveCam);
						PosStart = ScreenToWorld(Pos, TransformSelected->LocalPos, TranslationPlaneNormal, State->ActiveCam);
						EntityStart = TransformSelected->LocalPos;

					} else if (MovingState == moving_state::txy || MovingState == moving_state::tyz || MovingState == moving_state::txz) {

						vector2 Pos = {};

						Pos = ScreenNearestOnAxis(TransformSelected->LocalPos, TranslationAxis, TranslationPerpAxis, Globals->Input->MousePos, WindowInfo, State->ActiveCam);
						PosStart = ScreenToWorld(Pos, TransformSelected->LocalPos, TranslationPlaneNormal, State->ActiveCam);

						Pos = ScreenNearestOnAxis(TransformSelected->LocalPos, TranslationAxisTwo, TranslationPerpAxisTwo, Globals->Input->MousePos, WindowInfo, State->ActiveCam);
						PosStartTwo = ScreenToWorld(Pos, TransformSelected->LocalPos, TranslationPlaneNormal, State->ActiveCam);

						EntityStart = TransformSelected->LocalPos;
					}

				}

				// Rotation
				if (MovingState == moving_state::rotation_choice &&
				        (Globals->Input->KeyboardInput['X'].OnDown || Globals->Input->KeyboardInput['Y'].OnDown || Globals->Input->KeyboardInput['Z'].OnDown)) {

					if (Globals->Input->KeyboardInput['X'].OnDown) {
						MovingState = moving_state::rx;
						RotationAxis = XAxis;
					} else if (Globals->Input->KeyboardInput['Y'].OnDown) {
						MovingState = moving_state::ry;
						RotationAxis = YAxis;
					} else if (Globals->Input->KeyboardInput['Z'].OnDown) {
						MovingState = moving_state::rz;
						RotationAxis = ZAxis;
					}
				}

				if (Globals->Input->KeyboardInput[KEY_ESC].OnDown) {
					MovingState = moving_state::none;
					Globals->EditorData.TransformSelected = GameNull;
					Globals->EditorData.EntitySelected = GameNull;
				}

				// Releasing
				{
					if (MovingType == moving_type::keyboard && Globals->Input->MouseLeft.OnDown &&
					        (
					            MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz ||
					            MovingState == moving_state::rx || MovingState == moving_state::ry || MovingState == moving_state::rz
					        )
					   ) {
						MovingState = moving_state::none;
					}
					if (MovingType == moving_type::mouse && Globals->Input->MouseLeft.OnUp &&
					        (
					            MovingState == moving_state::tx || MovingState == moving_state::ty || MovingState == moving_state::tz ||
					            MovingState == moving_state::txy || MovingState == moving_state::tyz || MovingState == moving_state::txz ||
					            MovingState == moving_state::rx || MovingState == moving_state::ry || MovingState == moving_state::rz
					        )
					   ) {
						MovingState = moving_state::none;
					}
				}
			} else {
				MovingState = moving_state::none;
			}
		}

		// Material Editor
		if (Globals->EditorData.MaterialEditorOpen) {

			ImGui::Begin("Materials", &Globals->EditorData.MaterialEditorOpen, ImGuiWindowFlags_MenuBar);
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::Button("+ New Material")) {
						asset_material* NewMat = (asset_material*)ArenaAllocate(GlobalPermMem, sizeof(material));
						NewMat->Name = "NewMaterial_";
						NewMat->Version = MATERIAL_VERSION;

						Globals->EditorData.MaterialEditing = NewMat;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			// List box selection
			ImGui::BeginChild("materialselect", ImVec2(0, 100), true);
			list_link* CurrentLink = Globals->AssetsList.Materials->TopLink;
			for (uint32 i = 0; i < Globals->AssetsList.Materials->LinkCount; i++) {
				asset_material* Mat = (asset_material*)CurrentLink->Data;
				CurrentLink = CurrentLink->NextLink;

				if (ImGui::Button(&Mat->Name.CharArray[0])) {
					Globals->EditorData.MaterialEditing = Mat;
				}
			}
			ImGui::EndChild();

			ImGui::Separator();

			if (MaterialEditing != GameNull) {

				ImGui::InputText("Name", MaterialEditing->Name.CharArray, 64);

				ImGui::Text(MaterialEditing->ShaderID.CharArray);
				ImGui::SameLine();
				ImGui::SameLine();
				if (ImGui::Button("Select Shader")) {
					ImGui::OpenPopup("ShaderSelect");
				}

				if (ImGui::BeginPopupModal("ShaderSelect", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

					ImGui::BeginChild("imgdisp", ImVec2(400, 400));
					for (int i = 0; i < Globals->AssetsList.ShadersCount; i++) {
						if (ImGui::Button(Globals->AssetsList.Shaders[i].ID.CharArray)) {
							MaterialEditing->ShaderID = Globals->AssetsList.Shaders[i].ID;
							MaterialEditing->Material.Create(&Globals->AssetsList.Shaders[i].Shader, GlobalPermMem);
							ImGui::CloseCurrentPopup();
						}
					}
					ImGui::EndChild();

					if (ImGui::Button("Cancel", ImVec2(-1, 0))) { ImGui::CloseCurrentPopup(); }
					ImGui::EndPopup();
				}


				if (MaterialEditing->Material.Shader != GameNull) {

					if (ImGui::Button("Open Vert", ImVec2(ImGui::GetWindowWidth() * 0.45f, 0))) {
						PlatformApi.OpenFileExternal(MaterialEditing->Material.Shader->VertPath.CharArray);
					}
					ImGui::SameLine();
					if (ImGui::Button("Open Frag", ImVec2(-1, 0))) {
						PlatformApi.OpenFileExternal(MaterialEditing->Material.Shader->FragPath.CharArray);
					}

					ImGui::Dummy(ImVec2(0, 20));
					ImGui::Separator();

					material* Mat = &MaterialEditing->Material;
					for (int x = 0; x < Mat->Uniforms.Count; x++) {
						ImGui::PushID(x);

						shader_uniform* Uni = Mat->Uniforms.Array[x];
						if (Uni->Expose) {
							switch (Uni->Type) {
								case glsl_type::gl_sampler2D: {
									if (ImGui::ImageButton((ImTextureID)Uni->Data.ImageID, ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f))) {
										ImGui::OpenPopup("ImageSelect");
									}

									if (ImGui::BeginPopupModal("ImageSelect", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

										ImGui::BeginChild("imgdisp", ImVec2(1200, 600));
										ImGui::Columns(2);
										for (int i = 0; i < Globals->AssetsList.ImagesCount; i++) {
											ImGui::Text(Globals->AssetsList.Images[i].FilePath.CharArray);
											if (ImGui::ImageButton((ImTextureID)(uint64)Globals->AssetsList.Images[i].LoadedImage.GLID, ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f))) {
												Uni->Data.ImageID = Globals->AssetsList.Images[i].LoadedImage.GLID;
												ImGui::CloseCurrentPopup();
											}
											if (i % 1 == 0) { ImGui::NextColumn(); }
										}
										ImGui::Columns(1);
										ImGui::EndChild();

										if (ImGui::Button("Cancel", ImVec2(-1, 0))) { ImGui::CloseCurrentPopup(); }
										ImGui::EndPopup();
									}
									ImGui::SameLine();
									ImGui::Text(Uni->Name.CharArray);


								} break;

								case glsl_type::gl_vec3: {

									// Use vec4 but the alpha is ignored
									ImVec4 color = ImVec4(Uni->Data.Vec3.X, Uni->Data.Vec3.Y, Uni->Data.Vec3.Z, 0.0f);
									ImGui::ColorEdit3(Uni->Name.CharArray, (float*)&color);

									Uni->Data.Vec3.X = color.x;
									Uni->Data.Vec3.Y = color.y;
									Uni->Data.Vec3.Z = color.z;

								} break;

								case glsl_type::gl_int: {
									ImGui::DragInt(Uni->Name.CharArray, &Uni->Data.Int, 1);
								} break;

								case glsl_type::gl_float: {
									ImGui::DragFloat(Uni->Name.CharArray, &Uni->Data.Float, 0.005f);
								} break;

								default: {

								} break;
							}
						}

						ImGui::PopID();
					}
				}

				ImGui::Separator();
				ImGui::Dummy(ImVec2(0, 20));

				if (ImGui::Button("Save", ImVec2(-1, 0))) {
					assets::SaveMaterial(MaterialEditing);
				}
			}
			ImGui::End();
		}
	}
}
