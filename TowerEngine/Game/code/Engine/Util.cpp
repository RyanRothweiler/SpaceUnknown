#pragma once
#ifndef UtilCPP
#define UtilCPP

struct circle {
	vector2 Center;
	real64 Radius;
};


bool32
CircleContains(vector2 PointChecking, vector2 CircleCenter, real64 CircleRadius)
{
	real64 CheckingDist = Vector2Distance(CircleCenter, PointChecking);
	if (CheckingDist < CircleRadius) {
		return (true);
	}
	return (false);
}

#endif