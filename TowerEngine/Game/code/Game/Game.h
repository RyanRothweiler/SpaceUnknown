#pragma once

#ifndef GAME_H
#define GAME_H

namespace game {

	struct ship {
		bool32 Using;
		vector2 Pos;
		vector2 Size;

		vector2 TargetPos;
	};

	struct state {
		ship Ships[100];
		ship* ShipSelected;

		real32 Zoom = 1.0f;
		real32 ZoomTarget;
	};
};

#endif