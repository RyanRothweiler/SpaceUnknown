#pragma once

#ifndef GAME_CPP
#define GAME_CPP

#include "Game.h"

namespace game {

	void RegisterStepper(stepper* Stepper, game::state* State)
	{
		Assert(State->SteppersCount < ArrayCount(State->Steppers));
		State->Steppers[State->SteppersCount++] = Stepper;
	}

#include "Ship.cpp"

	void Start(engine_state* EngineState)
	{
		game::state* GameState = &EngineState->GameState;

		ShipSetup(GameState, vector2{0, 0});
	}

	const real32 ZoomMin = 0.0f;
	const real32 ZoomMax = 1.0f;
	const real32 ZoomRealMin = 0.1f;
	const real32 ZoomRealMax = 20.0f;

	const real32 RenderLayerShip = -2;
	const real32 RenderLayerPlanet = -3;
	const real32 KeyboardPanSpeed = 0.75f;
	const real32 MousePanSpeed = 0.03f;

	const real32 MouseZoomSpeed = 0.03f;
	const real32 MouseZoomInvert = -1;

	void Loop(engine_state* EngineState, window_info* Window, game_input* Input)
	{
		game::state* State = &EngineState->GameState;

		State->Zoom = (real32)Lerp(State->Zoom, State->ZoomTarget, 0.5f);
		float Curve = 3.5f;
		EngineState->GameCamera.OrthoZoom = (real32)LerpCurve(ZoomRealMin, ZoomRealMax, Curve, State->Zoom);
		real64 ZoomSpeedAdj = LerpCurve(4.0f, 200.0f, Curve, State->Zoom);


		// zoom window
		{
			static bool Open = true;

			ImGui::SetNextWindowPos(ImVec2(Window->Width * 0.5f, 10.0f));
			ImGui::SetNextWindowSize(ImVec2(500, -1));
			ImGui::Begin("Zoom", &Open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			ImGui::SliderFloat("Zoom", &State->ZoomTarget, ZoomMin, ZoomMax, "%.3f");
			ImGui::End();
		}

		// camera controls
		{
			// Keyboard
			vector2 CamMoveDir = vector2{0, 0};
			if (Input->KeyboardInput['A'].IsDown || Input->KeyboardInput['a'].IsDown) { CamMoveDir.X = -1; }
			if (Input->KeyboardInput['D'].IsDown || Input->KeyboardInput['d'].IsDown) { CamMoveDir.X = 1; }
			if (Input->KeyboardInput['W'].IsDown || Input->KeyboardInput['w'].IsDown) { CamMoveDir.Y = -1; }
			if (Input->KeyboardInput['S'].IsDown || Input->KeyboardInput['s'].IsDown) { CamMoveDir.Y = 1; }
			CamMoveDir = Vector2Normalize(CamMoveDir);
			EngineState->GameCamera.Center.X += CamMoveDir.X * KeyboardPanSpeed * (ZoomSpeedAdj);
			EngineState->GameCamera.Center.Y += CamMoveDir.Y * KeyboardPanSpeed * (ZoomSpeedAdj);

			// Mouse
			static vector2 MouseStart;
			static vector2 CamStart;
			if (Input->MouseLeft.OnDown) {
				MouseStart = Input->MousePos;

				CamStart.X = EngineState->GameCamera.Center.X;
				CamStart.Y = EngineState->GameCamera.Center.Y;
			}
			if (Input->MouseLeft.IsDown) {
				vector2 Offset = MouseStart - Input->MousePos;
				vector2 P = CamStart + (Offset * MousePanSpeed * ZoomSpeedAdj);
				EngineState->GameCamera.Center.X = P.X;
				EngineState->GameCamera.Center.Y = P.Y;
			}

			State->ZoomTarget = (real32)ClampValue(ZoomMin, ZoomMax, State->ZoomTarget + (Input->MouseScrollDelta * MouseZoomSpeed * MouseZoomInvert));
		}

		// Ship
		{
			vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
			vector2 MouseWorldFlat = vector2{MouseWorld.X, MouseWorld.Y};

			RenderCircle(MouseWorldFlat, vector2{1, 1},
			             COLOR_RED, -1, Globals->GameRenderer);

			// Ship window
			if (State->ShipSelected != GameNull) {
				ImGui::Begin("Ship Info");
				ImVec2 window_pos = ImGui::GetWindowPos();

				ImGui::PushID("ActiveRotaiton");

				ImGui::Text("Position");
				ImGui::Columns(2);

				ImGui::Text("x");
				ImGui::Text(string{State->ShipSelected->Position.X} .Array());
				ImGui::NextColumn();
				ImGui::Text("y");
				ImGui::Text(string{State->ShipSelected->Position.Y} .Array());

				ImGui::Columns(1);
				ImGui::PopID();

				ImGui::End();

				vector2 Points[2] = {};
				Points[0] = vector2{window_pos.x, window_pos.y};
				Points[1] = WorldToScreen(vector3{State->ShipSelected->Position.X, State->ShipSelected->Position.Y, 0}, &EngineState->GameCamera);

				render_line Line = {};
				Line.Points = Points;
				Line.PointsCount = ArrayCount(Points);
				RenderLine(Line, 1.5f, color{1, 1, 1, 0.2f}, &EngineState->UIRenderer, false);

				if (Input->MouseLeft.OnDown) {
					float edgeRatio = 0.5;

					State->ShipSelected->JourneyStartPos = State->ShipSelected->Position;
					State->ShipSelected->TargetPos = MouseWorldFlat;
					State->ShipSelected->DistFromSidesToCoast =
					    Vector2Distance(State->ShipSelected->Position, State->ShipSelected->TargetPos) * 0.5f * edgeRatio;
					State->ShipSelected->Moving = true;
				}
			}

			// Ship selecting
			if (
			    State->ShipSelected == GameNull &&
			    Input->MouseLeft.OnDown
			) {

				State->ShipSelected = GameNull;
				for (int i = 0; i < ArrayCount(State->Ships); i++) {
					ship* Ship = &State->Ships[i];
					if (Ship->Using) {

						vector2 TopLeftWorld = Ship->Position - (Ship->Size * 0.5f);
						vector2 BottomRightWorld = Ship->Position + (Ship->Size * 0.5f);

						rect Bounds = {};
						Bounds.TopLeft = WorldToScreen(vector3{TopLeftWorld.X, TopLeftWorld.Y, 0}, &EngineState->GameCamera);
						Bounds.BottomRight = WorldToScreen(vector3{BottomRightWorld.X, BottomRightWorld.Y, 0}, &EngineState->GameCamera);

						if (RectContains(Bounds, Input->MousePos)) {
							State->ShipSelected = Ship;
						}
					}
				}
			}
		}

		// step universe
		{
			for (int i = 0; i < State->SteppersCount; i++) {
				stepper* Stepper = State->Steppers[i];
				Stepper->Step(Stepper->SelfData, EngineState->DeltaTimeMS);
			}
		}

		// Render planets
		RenderCircle(vector2{1200, 200}, vector2{2000, 2000},
		             COLOR_RED, RenderLayerPlanet, Globals->GameRenderer);

		// Render ships
		for (int i = 0; i < ArrayCount(State->Ships); i++) {
			ship* Ship = &State->Ships[i];
			if (Ship->Using) {
				static loaded_image* ShipImage = assets::GetImage("Ship");
				RenderTextureAll(
				    Ship->Position,
				    Ship->Size,
				    COLOR_WHITE,
				    ShipImage->GLID, RenderLayerShip, Globals->GameRenderer);
			}
		}
	}
}

#endif