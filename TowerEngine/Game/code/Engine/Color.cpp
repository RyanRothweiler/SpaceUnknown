#pragma once

#ifndef ColorCPP
#define ColorCPP

struct color {
	real32 R;
	real32 G;
	real32 B;
	real32 A;

	bool32 Empty()
	{
		if (R != 0 || G != 0 || B != 0 || A != 0) {
			return false;
		}
		return true;
	}
};

color operator * (color Input, real32 M)
{
	return color{Input.R * M, Input.G * M, Input.B * M, Input.A * M};
}

color LerpColor(color A, color B, real64 T)
{
	color Output = {};
	Output.R = (real32)Lerp(A.R, B.R, T);
	Output.G = (real32)Lerp(A.G, B.G, T);
	Output.B = (real32)Lerp(A.B, B.B, T);
	Output.A = (real32)Lerp(A.A, B.A, T);
	return Output;
}

// http://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients
color HeatMapColor(real32 Input)
{
	const int NumColors = 4;
	color Colors[NumColors] = {
		{0, 0, 1, 1},
		{0, 1, 0, 1},
		{1, 1, 0, 1},
		{1, 0, 0, 1}
	};

	int idx1;        // |-- Our desired color will be between these two indexes in "color".
	int idx2;        // |
	float fractBetween = 0;  // Fraction between "idx1" and "idx2" where our value is.

	if (Input <= 0)      {
		// accounts for an input <=0
		idx1 = idx2 = 0;
	} else if (Input >= 1) {
		// accounts for an input >=0
		idx1 = idx2 = NumColors - 1;
	} else {
		// Will multiply value by 3.
		Input = Input * (NumColors - 1);

		// Our desired color will be after this index.
		idx1  = (int)floor(Input);

		// ... and before this index (inclusive).
		idx2  = idx1 + 1;

		// Distance between the two indexes (0-1).
		fractBetween = Input - (float)idx1;
	}

	color Output = {};
	Output.R 	= (Colors[idx2].R - Colors[idx1].R) * fractBetween + Colors[idx1].R;
	Output.G 	= (Colors[idx2].G - Colors[idx1].G) * fractBetween + Colors[idx1].G;
	Output.B  	= (Colors[idx2].B - Colors[idx1].B) * fractBetween + Colors[idx1].B;
	Output.A 	= 1.0f;

	return Output;
}

const color COLOR_WHITE = color{1.0f, 1.0f, 1.0f, 1.0f};
const color COLOR_BLACK = color{0.0f, 0.0f, 0.0f, 1.0f};
const color COLOR_GREY = color{0.5f, 0.5f, 0.5f, 1.0f};
const color COLOR_RED = color{1.0f, 0, 0, 1.0f};
const color COLOR_ORANGE = color{1.0f, 0.5f, 0, 1.0f};
const color COLOR_BLUE = color{0, 0, 1.0f, 1.0f};
const color COLOR_GREEN = color{0, 1.0f, 0, 1.0f};
const color COLOR_PINK = color{1.0, 1.0f, 0.6f, 1.0f};
const color COLOR_YELLOW = color{1.0f, 1.0f, 0.0, 1.0f};
const color COLOR_LIGHT_BLUE = color{0.0, 1.0f, 1.0f, 1.0f};
const color COLOR_PURPLE = color{0.5f, 0.0f, 0.5f, 1.0f};
const color COLOR_TEAL = color{0.0f, 0.5f, 0.5f, 1.0f};

const color EditorColor_Blue = color{0.04f, 0.46f, 0.88f, 1.0f};
const color EditorColor_LightBlue = color{0.33f, 0.69f, 0.72f, 1.0f};
const color EditorColor_Pink = color{0.91f, 0.17f, 0.5f, 1.0f};
const color EditorColor_Purple = color{0.55f, 0.3f, 0.9f, 1.0f};
const color EditorColor_MainDark = color{0.07f, 0.07f, 0.10f, 1.0f};
const color EditorColor_SecondaryDark = color{0.22f, 0.22f, 0.29f, 1.0f};


// These go from darkest to lightest. One = dark.
const color GameColor_BlueOne = color{0.11f, 0.27f, 0.45f, 1.0f};
const color GameColor_BlueTwo = color{0.18f, 0.43f, 0.60f, 1.0f};
const color GameColor_BlueThree = color{0.25f, 0.62f, 0.86f, 1.0f};

const color GameColor_BlackOne = color{0.11f, 0.12f, 0.13f, 1.0f};
const color GameColor_BlackTwo = color{0.18f, 0.20f, 0.22f, 1.0f};
const color GameColor_BlackThree = color{0.27f, 0.29f, 0.30f, 1.0f};

// -----------------------------------------------------------------------------
const color Icon_GreenDark = color{65.0f / 255.0f, 117.0f / 255.0f, 5.0f / 255.0f, 1.0f};
const color Icon_GreenMid = color{126.0f / 255.0f, 211.0f / 255.0f, 33.0f / 255.0f, 1.0f};
const color Icon_GreenLight = color{184.0f / 255.0f, 233.0f / 255.0f, 134.0f / 255.0f, 1.0f};

const color Icon_BlueDark = color{	74.0f / 255.0f, 		144.0f / 255.0f, 		226.0f / 255.0f, 	1.0f};
const color Icon_BlueMid = color{	72.0f / 255.0f, 		186.0f / 255.0f, 		255.0f / 255.0f, 	1.0f};
const color Icon_BlueLight = color{	80.0f / 255.0f, 		277.0f / 255.0f, 		194.0f / 255.0f, 	1.0f};

const color Icon_BrownDark = color{		139.0f / 255.0f, 		87.0f / 255.0f, 		42.0f / 255.0f, 	1.0f};
const color Icon_BrownMid = color{		181.0f / 255.0f, 		114.0f / 255.0f, 		4.0f / 255.0f, 	1.0f};
const color Icon_BrownLight = color{	245.0f / 255.0f, 		166.0f / 255.0f, 		35.0f / 255.0f, 	1.0f};

// -----------------------------------------------------------------------------
const color Color_XPBarBackground = color{0, 0, 0, 0.5f};
const color Color_XPBarForeground = color{125.0f / 255.0f, 163.0f / 255.0f, 723.0f / 255.0f, 1.0f};
// -----------------------------------------------------------------------------


#endif