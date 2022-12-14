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

	void Loop(engine_state* EngineState)
	{
		game::state* GameState = &EngineState->GameState;

		// Render ships
		for (int i = 0; i < ArrayCount(GameState->Ships); i++) {
			ship* Ship = &GameState->Ships[i];
			if (Ship->Using) {
				static loaded_image* ShipImage = assets::GetImage("Ship");
				RenderTextureAll(Ship->Pos, vector2{1, 1}, COLOR_WHITE,
				                 ShipImage->GLID, -10, Globals->GameRenderer);
			}
		}
	}
}

#endif