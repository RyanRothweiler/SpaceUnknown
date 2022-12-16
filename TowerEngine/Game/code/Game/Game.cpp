#pragma once

#ifndef GAME_CPP
#define GAME_CPP

#include "Game.h"

namespace game {

	game::ship* SetupShip(game::state* State, vector2 Pos)
	{
		for (int i = 0; i < ArrayCount(State->Ships); i++) {
			ship* Ship = &State->Ships[i];
			if (!Ship->Using) {
				Ship->Using = true;
				Ship->Pos = Pos;
				return Ship;
			}
		}

		return GameNull;
	}

	void Start(engine_state* EngineState)
	{
		game::state* GameState = &EngineState->GameState;

		SetupShip(GameState, vector2{0, 0});
	}

	const real32 ZoomMin = 0.0f;
	const real32 ZoomMax = 1.0f;
	const real32 ZoomRealMin = 1.0f;
	const real32 ZoomRealMax = 1000.0f;

	const real32 RenderLayerShip = -1;
	const real32 RenderLayerPlanet = -2;
	const real32 KeyboardPanSpeed = 0.75f;
	const real32 MousePanSpeed = 0.015f;

	const real32 MouseZoomSpeed = 0.03f;
	const real32 MouseZoomInvert = -1;

	void Loop(engine_state* EngineState, window_info* Window, game_input* Input)
	{
		game::state* State = &EngineState->GameState;

		State->Zoom = (real32)Lerp(State->Zoom, State->ZoomTarget, 0.5f);
		real64 ZoomReal = LerpCurve(ZoomRealMin, ZoomRealMax, 4.0f, State->Zoom);;
		real64 ZoomCo = 1.0f / ZoomReal;

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
			if (Input->KeyboardInput['A'].IsDown) { CamMoveDir.X = 1; }
			if (Input->KeyboardInput['D'].IsDown) { CamMoveDir.X = -1; }
			if (Input->KeyboardInput['W'].IsDown) { CamMoveDir.Y = 1; }
			if (Input->KeyboardInput['S'].IsDown) { CamMoveDir.Y = -1; }
			CamMoveDir = Vector2Normalize(CamMoveDir);
			State->CamPos.X += CamMoveDir.X * KeyboardPanSpeed * (ZoomReal);
			State->CamPos.Y += CamMoveDir.Y * KeyboardPanSpeed * (ZoomReal);

			// Mouse
			static vector2 MouseStart;
			static vector2 CamStart;
			if (Input->MouseLeft.OnDown) {
				MouseStart = Input->MousePos;
				CamStart = State->CamPos;
			}
			if (Input->MouseLeft.IsDown) {
				vector2 Offset = Input->MousePos - MouseStart;
				State->CamPos = CamStart + (Offset * MousePanSpeed * ZoomReal);
			}

			State->ZoomTarget = (real32)ClampValue(ZoomMin, ZoomMax, State->ZoomTarget + (Input->MouseScrollDelta * MouseZoomSpeed * MouseZoomInvert));
		}

		RenderCircle(
		    (State->CamPos + vector2{200, 200}) * ZoomCo,
		    vector2{500, 500} * ZoomCo,
		    COLOR_RED, RenderLayerPlanet, Globals->GameRenderer);

		// Render ships
		for (int i = 0; i < ArrayCount(State->Ships); i++) {
			ship* Ship = &State->Ships[i];
			if (Ship->Using) {
				static loaded_image* ShipImage = assets::GetImage("Ship");
				RenderTextureAll(
				    (State->CamPos + Ship->Pos) * ZoomCo,
				    vector2{1, 1} * ZoomCo,
				    COLOR_WHITE,
				    ShipImage->GLID, RenderLayerShip, Globals->GameRenderer);
			}
		}
	}
}

#endif