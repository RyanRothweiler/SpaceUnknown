#pragma once

#ifndef FontH
#define FontH

struct glyph_info {
	bool32 Valid;

	real64 XAdvance;

	// For UVs. This is position of glyph within the atlas
	real64 Left, Right, Top, Bottom;
	// Offsets of plane from baseline when drawing glyph
	real64 PlaneLeft, PlaneRight, PlaneTop, PlaneBottom;
};

static int FontGlyphsTotalCount = 128;

struct kerning_pair {
	int32 FirstGlyph;
	int32 SecondGlyph;
	int32 Amount;
};

struct loaded_font {
	real64 LineHeight;
	glyph_info* Info;
	loaded_image Atlas;

	hash::table KerningPairs;
};

struct font_style {
	loaded_font* Font;
	real64 SizePoints;
};

#endif