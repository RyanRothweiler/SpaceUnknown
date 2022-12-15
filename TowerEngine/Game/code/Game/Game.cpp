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
		GameState->Zoom = 1.0f;

		SetupShip(GameState, vector2{0, 0});
	}

	void Loop(engine_state* EngineState, window_info* Window, game_input* Input)
	{
		game::state* State = &EngineState->GameState;

		real32 RenderLayer = -10;
		real32 ZoomCo = 1.0f / State->Zoom;

		// zoom window
		{
			static bool Open = true;

			ImGui::SetNextWindowPos(ImVec2(Window->Width * 0.5f, 10.0f));
			ImGui::SetNextWindowSize(ImVec2(500, -1));
			ImGui::Begin("Zoom", &Open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			ImGui::SliderFloat("Zoom", &State->Zoom, 1.0f, 200.0f, "%.3f", 2.0f);
			ImGui::End();
		}

		// camera controls
		{
			real32 KeyboardSpeed = 0.75f;
			real32 MouseSpeed = 0.015f;

			// Keyboard
			vector2 CamMoveDir = vector2{0, 0};
			if (Input->KeyboardInput['A'].IsDown) { CamMoveDir.X = 1; }
			if (Input->KeyboardInput['D'].IsDown) { CamMoveDir.X = -1; }
			if (Input->KeyboardInput['W'].IsDown) { CamMoveDir.Y = 1; }
			if (Input->KeyboardInput['S'].IsDown) { CamMoveDir.Y = -1; }
			CamMoveDir = Vector2Normalize(CamMoveDir);
			State->CamPos.X += CamMoveDir.X * KeyboardSpeed * (State->Zoom);
			State->CamPos.Y += CamMoveDir.Y * KeyboardSpeed * (State->Zoom);

			// Mouse
			static vector2 MouseStart;
			static vector2 CamStart;
			if (Input->MouseLeft.OnDown) {
				MouseStart = Input->MousePos;
				CamStart = State->CamPos;
			}
			if (Input->MouseLeft.IsDown) {
				vector2 Offset = Input->MousePos - MouseStart;
				State->CamPos = CamStart + (Offset * MouseSpeed * State->Zoom);
			}
		}

		RenderCircle(
		    (State->CamPos + vector2{200, 200}) * ZoomCo,
		    vector2{500, 500} * ZoomCo,
		    COLOR_RED, -11, Globals->GameRenderer);

		// Render ships
		for (int i = 0; i < ArrayCount(State->Ships); i++) {
			ship* Ship = &State->Ships[i];
			if (Ship->Using) {
				static loaded_image* ShipImage = assets::GetImage("Ship");
				RenderTextureAll(
				    (State->CamPos + Ship->Pos) * ZoomCo,
				    vector2{1, 1} * ZoomCo,
				    COLOR_WHITE,
				    ShipImage->GLID, RenderLayer, Globals->GameRenderer);
			}
		}
	}
}

#endif