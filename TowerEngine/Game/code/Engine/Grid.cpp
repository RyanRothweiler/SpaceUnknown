#pragma once

#ifndef GridCPP
#define GridCPP

static real32 GridSize = 50;
static real32 GridSizeHalf = 25;

static real32 IsoGridWidth = 100;
static real32 IsoGridHeight = 50;

vector2 WorldPosToSnappedPos(vector2 WorldPosition)
{
	WorldPosition.X = (real64)Round(WorldPosition.X / GridSize) * GridSize;
	WorldPosition.Y = (real64)Round(WorldPosition.Y / GridSize) * GridSize;
	return WorldPosition;
}

vector2 WorldPosToGridPos(vector2 WorldPosition)
{
	WorldPosition.X = (real64)Round(WorldPosition.X / GridSize);
	WorldPosition.Y = (real64)Round(WorldPosition.Y / GridSize);
	return WorldPosition;
}

vector2 SnapPosition(vector2 WorldPosition, real32 GS)
{
	WorldPosition.X = (real64)Round(WorldPosition.X / GS) * GS;
	WorldPosition.Y = (real64)Round(WorldPosition.Y / GS) * GS;
	return WorldPosition;
}

vector2 GridPositionToWorldPosition(vector2 GridPosition)
{
	GridPosition.X = GridPosition.X * GridSize;
	GridPosition.Y = GridPosition.Y * GridSize;
	return GridPosition;
}

// http://clintbellanger.net/articles/isometric_math/

vector2 GridPosToWorldPosIso(vector2 GridPos)
{
	return vector2{(GridPos.X - GridPos.Y) * (IsoGridWidth * 0.5f),
	               (GridPos.X + GridPos.Y) * (IsoGridHeight * 0.5f)};
}

vector2 WorldPosToGridPosIso(vector2 WorldPos)
{
	vector2 Ret =  vector2{(WorldPos.X / (IsoGridWidth * 0.5f)) + (WorldPos.Y / (IsoGridHeight * 0.5f)),
	                       (WorldPos.Y / (IsoGridHeight * 0.5f)) - ((WorldPos.X / (IsoGridWidth * 0.5f)))
	                      } * 0.5f;
	Ret.X = (int32)Ret.X;
	Ret.Y = (int32)Ret.Y;
	return Ret;
}

#endif