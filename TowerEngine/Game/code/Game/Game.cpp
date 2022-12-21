#pragma once

#ifndef GAME_CPP
#define GAME_CPP

#include "Game.h"

#include <chrono>
#include <ctime>

namespace game {

	string ChronoToString(std::chrono::seconds SecondsTotal)
	{
		int64 Hours = std::chrono::duration_cast<std::chrono::hours>(SecondsTotal).count();
		int64 Minutes = std::chrono::duration_cast<std::chrono::minutes>(SecondsTotal).count() - (Hours * 24);
		int64 Seconds = (int64)(SecondsTotal.count() - (Minutes * 60.0f));

		return string{Hours} + string{"h "} + string{Minutes} + string{"m "} + string{Seconds} + string{"s "};
	}

	void RegisterStepper(stepper* Stepper,
	                     step_func Method,
	                     void* SelfData,
	                     game::state* State)
	{
		Assert(State->SteppersCount < ArrayCount(State->Steppers));

		Stepper->Step = Method;
		Stepper->SelfData = SelfData;

		State->Steppers[State->SteppersCount++] = Stepper;
	}

	void TimeStep(void* SelfData, real64 Time)
	{
		game::universe_time* UT = (game::universe_time*)SelfData;
		UT->TimeMS += Time;
	}


#include "Ship.cpp"

	void Start(engine_state* EngineState)
	{
		game::state* State = &EngineState->GameState;

		RegisterStepper(&State->UniverseTime.Stepper, &TimeStep, (void*)(&State->UniverseTime), State);

		ShipSetup(State, vector2{0, 0});
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
		game::editor_state* EditorState = &EngineState->EditorState;

		State->Zoom = (real32)Lerp(State->Zoom, State->ZoomTarget, 0.5f);
		float Curve = 3.5f;
		EngineState->GameCamera.OrthoZoom = (real32)LerpCurve(ZoomRealMin, ZoomRealMax, Curve, State->Zoom);
		real64 ZoomSpeedAdj = LerpCurve(4.0f, 200.0f, Curve, State->Zoom);

		// Editor
		{
			if (Input->FunctionKeys[1].OnDown) {
				EditorState->EditorMode = !EditorState->EditorMode;
			}
		}

		// Main window
		{
			static bool Open = true;
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(Window->Width * 0.15f, -1));
			ImGui::Begin("Main", &Open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

			if (EditorState->EditorMode) {
				ImGui::Text("EDITOR MODE");
			}

			ImGui::Text("Time");

			std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> Time = {};
			Time += std::chrono::milliseconds((int)State->UniverseTime.TimeMS);
			std::chrono::seconds Diff = std::chrono::duration_cast<std::chrono::seconds>(Time.time_since_epoch());
			string Disp = ChronoToString(Diff);
			ImGui::Text(Disp.Array());

			ImGui::Separator();

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
			ship* CurrentShip = State->ShipSelected;

			vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
			vector2 MouseWorldFlat = vector2{MouseWorld.X, MouseWorld.Y};

			RenderCircle(MouseWorldFlat, vector2{1, 1},
			             COLOR_RED, -1, Globals->GameRenderer);

			// Ship window
			if (CurrentShip != GameNull) {
				ImGui::Begin("Ship Info");
				ImVec2 window_pos = ImGui::GetWindowPos();

				//if (ImGui::CollapsingHeader("Info")) {
				// Posititon
				/*
				{
					ImGui::PushID("position");
					ImGui::Text("Position");
					ImGui::Columns(2);

					ImGui::Text("x");
					ImGui::SameLine();
					ImGui::Text(string{CurrentShip->Position.X} .Array());

					ImGui::NextColumn();

					ImGui::Text("y");
					ImGui::SameLine();
					ImGui::Text(string{CurrentShip->Position.Y} .Array());

					ImGui::Columns(1);
					ImGui::PopID();
				}
				*/

				// Velocity
				{
					string V = Humanize((int64)(Vector2Length(CurrentShip->Velocity) * UnitToMeters * 1000.0f));
					ImGui::Text("Velocity (kph)");
					ImGui::SameLine();
					ImGui::Text(V.Array());
				}
				//}

				ImGui::Separator();

				if (
				    !CurrentShip->IsMoving &&
				    CurrentShip->CurrentJourney.EndPosition.X != 0 &&
				    CurrentShip->CurrentJourney.EndPosition.Y != 0
				) {
					if (ImGui::Button("Execute Movement", ImVec2(-1.0f, 0.0f))) {
						ShipMove(CurrentShip, CurrentShip->CurrentJourney);
					}
				}

				ImGui::End();

				// Render line
				vector2 Points[2] = {};
				Points[0] = vector2{window_pos.x, window_pos.y};
				Points[1] = WorldToScreen(vector3{CurrentShip->Position.X, CurrentShip->Position.Y, 0}, &EngineState->GameCamera);

				render_line Line = {};
				Line.Points = Points;
				Line.PointsCount = ArrayCount(Points);
				RenderLine(Line, 1.5f, color{1, 1, 1, 0.2f}, &EngineState->UIRenderer, false);

				if (Input->MouseLeft.OnDown && !CurrentShip->IsMoving) {
					float edgeRatio = 1.0;

					CurrentShip->CurrentJourney.StartPosition = CurrentShip->Position;
					CurrentShip->CurrentJourney.EndPosition = MouseWorldFlat;
					CurrentShip->CurrentJourney.DistFromSidesToCoast =
					    Vector2Distance(CurrentShip->Position, CurrentShip->CurrentJourney.EndPosition) * 0.5f * edgeRatio;
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