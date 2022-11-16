#pragma once
#ifndef RectH
#define RectH

struct rect {
	vector2 TopLeft;
	vector2 BottomRight;

	/*
	vector2 TopLeft() 		{ return TopLeft; }
	vector2 BottomRight() 	{ return BottomRight; }
	vector2 TopRight() 		{ return vector2{BottomRight.X, TopLeft.Y}; }
	vector2 BottomLeft() 	{ return vector2{TopLeft.X, BottomRight.Y}; }
	*/
};

#endif