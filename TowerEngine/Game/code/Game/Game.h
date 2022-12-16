#pragma once

#ifndef GAME_H
#define GAME_H

namespace game {

	struct ship {
		bool32 Using;
		vector2 Pos;
	};

	struct state {
		ship Ships[100];

		real32 Zoom = 1.0f;
		real32 ZoomTarget;
		vector2 CamPos;
	};
};

#endif