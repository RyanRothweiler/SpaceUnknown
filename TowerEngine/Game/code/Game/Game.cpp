#pragma once

#ifndef GAME_CPP
#define GAME_CPP

#include "Game.h"

namespace game {

	void SetupShip(game::state* State, vector2 Pos)
	{
		for (int i = 0; i < ArrayCount(State->Ships); i++) {
			ship* Ship = &State->Ships[i];
			if (!Ship->Using) {
				Ship->Using = true;
				Ship->Pos = Pos;
			}
		}
	}

	void Start(engine_state* EngineState)
	{
		game::state* GameState = &EngineState->GameState;

		SetupShip(GameState, vector2{});
	}

	void Loop(engine_state* EngineState, window_info* Window, game_input* Input)
	{
		game::state* State = &EngineState->GameState;

		// zoom window
		{
			static bool Open = true;

			ImGui::SetNextWindowPos(ImVec2(Window->Width * 0.5f, 10.0f));
			//ImGui::SetNextWindowSize(ImVec2(demo_window_size_x, demo_window_size_y));
			ImGui::Begin("Zoom", &Open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
			ImGui::SliderFloat("Zoom", &State->Zoom, 0.0f, 100.0f, "%.3f", 2.0f);
			ImGui::End();
		}

		// camera controls
		{
			real32 KeyboardSpeed = 0.75f;
			real32 MouseSpeed = 0.015f;

			// Keyboard
			vector2 CamMoveDir = vector2{0, 0};
			if (Input->KeyboardInput['A'].IsDown) { CamMoveDir.X = -1; }
			if (Input->KeyboardInput['D'].IsDown) { CamMoveDir.X =  1; }
			if (Input->KeyboardInput['W'].IsDown) { CamMoveDir.Y = -1; }
			if (Input->KeyboardInput['S'].IsDown) { CamMoveDir.Y =  1; }
			CamMoveDir = Vector2Normalize(CamMoveDir);
			EngineState->GameCamera.Center.X += CamMoveDir.X * KeyboardSpeed;
			EngineState->GameCamera.Center.Y += CamMoveDir.Y * KeyboardSpeed;

			// Mouse
			static vector2 MouseStart;
			static vector3 CamStart;
			if (Input->MouseLeft.OnDown) {
				MouseStart = Input->MousePos;
				CamStart = EngineState->GameCamera.Center;
			}
			if (Input->MouseLeft.IsDown) {
				vector2 Offset = MouseStart - Input->MousePos;
				EngineState->GameCamera.Center = CamStart + vector3{Offset.X * MouseSpeed, Offset.Y * MouseSpeed, 0.0f};
			}
		}

		// Render ships
		for (int i = 0; i < ArrayCount(State->Ships); i++) {
			ship* Ship = &State->Ships[i];
			if (Ship->Using) {
				static loaded_image* ShipImage = assets::GetImage("Ship");
				RenderTextureAll(Ship->Pos, vector2{1, 1}, COLOR_WHITE,
				                 ShipImage->GLID, -10, Globals->GameRenderer);
			}
		}
	}
}

#endif