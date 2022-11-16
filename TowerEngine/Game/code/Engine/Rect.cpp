#pragma once
#ifndef RectCPP
#define RectCPP

#include "Rect.h"

bool32
RectContains(rect Rect, vector2 Pos)
{
	if (Rect.TopLeft.X < Pos.X &&
	        Rect.TopLeft.Y < Pos.Y &&
	        Rect.BottomRight.X > Pos.X &&
	        Rect.BottomRight.Y > Pos.Y) {
		return (true);
	}

	return (false);
}

real64
RectWidth(rect Rect)
{
	return (Abs(Rect.TopLeft.X - Rect.BottomRight.X));
}

real64
RectHeight(rect Rect)
{
	return (Abs(Rect.TopLeft.Y - Rect.BottomRight.Y));
}

rect
RectOutline(rect Rect, real64 OutlineSize)
{
	rect OutlineRect = {};
	OutlineRect.TopLeft = Rect.TopLeft + vector2{ -OutlineSize, -OutlineSize};
	OutlineRect.BottomRight = Rect.BottomRight + vector2{OutlineSize, OutlineSize};
	return (OutlineRect);
}

vector2
RectCenter(rect Rect)
{
	vector2 Center = {};
	Center.X = (Rect.BottomRight.X + Rect.TopLeft.X) * 0.5f;
	Center.Y = (Rect.BottomRight.Y + Rect.TopLeft.Y) * 0.5f;
	return (Center);
}

vector2
RectLeftEdgeMiddle(rect Rect)
{
	vector2 Center = {};
	Center.X = Rect.TopLeft.X;
	Center.Y = (Rect.BottomRight.Y + Rect.TopLeft.Y) * 0.5f;
	return (Center);
}

void
RectSetHeight(rect* Rect, real64 Height)
{
	real64 Width = RectWidth(*Rect);
	Rect->BottomRight = Rect->TopLeft + vector2{Width, Height};
}

rect
RectCenterSize(vector2 Center, vector2 Size)
{
	rect Rect = {};
	Rect.TopLeft.X = Center.X - (Size.X * 0.5f);
	Rect.TopLeft.Y = Center.Y - (Size.Y * 0.5f);

	Rect.BottomRight.X = Center.X + (Size.X * 0.5f);
	Rect.BottomRight.Y = Center.Y + (Size.Y * 0.5f);

	return Rect;
}

// Keep the same width and height and modify to manuall set the center
rect
RectSetCenter(rect Rect, vector2 Center)
{
	real64 W = RectWidth(Rect) * 0.5f;
	real64 H = RectHeight(Rect) * 0.5f;

	Rect.TopLeft.X = Center.X - W;
	Rect.TopLeft.Y = Center.Y - H;
	Rect.BottomRight.X = Center.X + W;
	Rect.BottomRight.Y = Center.Y + H;

	return Rect;
}

rect
RectLerp(rect A, rect B, real64 T)
{
	rect Ret = {};
	Ret.TopLeft.X = Lerp(A.TopLeft.X, B.TopLeft.X, T);
	Ret.TopLeft.Y = Lerp(A.TopLeft.Y, B.TopLeft.Y, T);
	Ret.BottomRight.X = Lerp(A.BottomRight.X, B.BottomRight.X, T);
	Ret.BottomRight.Y = Lerp(A.BottomRight.Y, B.BottomRight.Y, T);
	return Ret;
}


rect RectUnderline(rect A, real32 LineSize)
{
	rect Ret = {};
	Ret.TopLeft = vector2{A.TopLeft.X, A.BottomRight.Y + LineSize};
	Ret.BottomRight = A.BottomRight;
	return Ret;
}
#endif