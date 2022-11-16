// https://docs.microsoft.com/en-us/windows/win32/hidpi/high-dpi-desktop-application-development-on-windows#pragma once
// https://www.enlyze.com/blog/writing-win32-apps-like-its-2020/part-3/


#ifndef FontCPP
#define FontCPP

// NOTE We're getting stb_truetype from imgui
// #define STB_TRUETYPE_IMPLEMENTATION
// #include "stb_truetype.h"
#include "EngineCore.h"

// NOTE this is here from removing stbtruetype. I don't want to go through and adjust all the sizes again.

real64 WordSpacing = 2.0f;
real64 ParagraphLineHeightAdj = 0.85f;

real64
GetWordPixelWidth(loaded_font* Font, string Word, real64 DotSize)
{
	real64 Accum = 0.0f;

	for (int Index = 0; Index < StringLength(Word); Index++) {
		char Letter = Word.CharArray[Index];
		glyph_info* GlyphInfo = &Font->Info[(int)Letter];
		Accum += GlyphInfo->XAdvance * DotSize * PlatformApi.ScreenTypePoints();
	}

	return Accum;
}

void FontFillRenderCommand(layout_data* VertexLayout, layout_data* UVLayout, int32 CommandIndexStart,
                           loaded_font* Font, string String, vector2 CurrPos, real64 DotSize, real32 RenderLayer, renderer* Renderer)
{
	//TIME_BEGIN

	bool32 Debug = false;

	if (Debug) {
		rect R = {};
		R.TopLeft = CurrPos;
		R.BottomRight = CurrPos + vector2{400, 1};
		RenderRect(R, color{1, 0, 0, 0.5f}, 0, Globals->DebugUIRenderer);
	}

	int32 StringLen = StringLength(String);
	for (int Index = 0; Index < StringLen; Index++) {

		char Letter = String.CharArray[Index];

		glyph_info* GlyphInfo = &Font->Info[(int)Letter];

		Assert((int)Letter < FontGlyphsTotalCount);
		Assert(GlyphInfo->Valid);

		vector2 UVTopRight = vector2{GlyphInfo->Right / (real64)(Font->Atlas.Width), GlyphInfo->Bottom / (real64)(Font->Atlas.Height)};
		vector2 UVBottomLeft = vector2{GlyphInfo->Left / (real64)(Font->Atlas.Width), GlyphInfo->Top / (real64)(Font->Atlas.Height)};

		vector2 Size = vector2{1, 1} * DotSize * PlatformApi.ScreenTypePoints();

		vector2 PlaneTopLeft = CurrPos + (vector2{GlyphInfo->PlaneLeft, -GlyphInfo->PlaneTop} * DotSize * PlatformApi.ScreenTypePoints());
		vector2 PlaneBottomRight = CurrPos + (vector2{GlyphInfo->PlaneRight, -GlyphInfo->PlaneBottom}  * DotSize * PlatformApi.ScreenTypePoints());

		vector2 HalfSize = vector2{Size.X * 0.5f, Size.Y * 0.5f};

		v3 TopLeft = 		v3{(real32)PlaneTopLeft.X, (real32)PlaneTopLeft.Y, (real32)RenderLayer};
		v3 TopRight = 		v3{(real32)PlaneBottomRight.X, (real32)PlaneTopLeft.Y, (real32)RenderLayer};
		v3 BottomRight = 	v3{(real32)PlaneBottomRight.X, (real32)PlaneBottomRight.Y, (real32)RenderLayer};
		v3 BottomLeft = 	v3{(real32)PlaneTopLeft.X, (real32)PlaneBottomRight.Y, (real32)RenderLayer};

		VertexLayout->Data.Vec3[0 + (Index * 6) + CommandIndexStart] = TopLeft;
		VertexLayout->Data.Vec3[1 + (Index * 6) + CommandIndexStart] = TopRight;
		VertexLayout->Data.Vec3[2 + (Index * 6) + CommandIndexStart] = BottomRight;
		VertexLayout->Data.Vec3[3 + (Index * 6) + CommandIndexStart] = TopLeft;
		VertexLayout->Data.Vec3[4 + (Index * 6) + CommandIndexStart] = BottomRight;
		VertexLayout->Data.Vec3[5 + (Index * 6) + CommandIndexStart] = BottomLeft;

		UVLayout->Data.Vec2[0 + (Index * 6) + CommandIndexStart] = v2{(real32)UVBottomLeft.X, (real32)UVBottomLeft.Y};
		UVLayout->Data.Vec2[1 + (Index * 6) + CommandIndexStart] = v2{(real32)UVTopRight.X, (real32)UVBottomLeft.Y};
		UVLayout->Data.Vec2[2 + (Index * 6) + CommandIndexStart] = v2{(real32)UVTopRight.X, (real32)UVTopRight.Y};
		UVLayout->Data.Vec2[3 + (Index * 6) + CommandIndexStart] = v2{(real32)UVBottomLeft.X, (real32)UVBottomLeft.Y};
		UVLayout->Data.Vec2[4 + (Index * 6) + CommandIndexStart] = v2{(real32)UVTopRight.X, (real32)UVTopRight.Y};
		UVLayout->Data.Vec2[5 + (Index * 6) + CommandIndexStart] = v2{(real32)UVBottomLeft.X, (real32)UVTopRight.Y};

		if (Debug) {
			rect R = {};
			R.TopLeft = vector2{TopLeft.X, TopLeft.Y};
			R.BottomRight = vector2{BottomRight.X, BottomRight.Y};
			RenderRect(R, color{1, 0, 0, 0.5f}, 0, Globals->DebugUIRenderer);
		}


		// Check for kerning pair
		/*
		real64 KerningPair = 0;
		if (Font->KerningPairs.TableSize > 0 && Index + 1 < StringLength(String)) {
			int32 First = (int)Letter;
			int32 Second = (int)String.CharArray[Index + 1];
			string Concat = string{First} + string{Second};
			uint32 Hash = hash::CalcString(Concat.CharArray, Font->KerningPairs.TableSize);

			Assert(Hash < Font->KerningPairs.TableSize);

			list_head List = Font->KerningPairs.Table[Hash];
			list_link* CurrentLink = List.TopLink;
			for (uint32 i = 0; i < List.LinkCount; i++) {
				kerning_pair* Pair = (kerning_pair*)CurrentLink->Data;

				CurrentLink = CurrentLink->NextLink;

				if (Pair->FirstGlyph == First && Pair->SecondGlyph == Second) {
					KerningPair = Pair->Amount;
					break;
				}
			}
		}
		*/

		CurrPos.X = CurrPos.X + (GlyphInfo->XAdvance * DotSize * PlatformApi.ScreenTypePoints());
	}

	//TIME_END
}

void FontRenderString(loaded_font* Font, string String, vector2 Baseline, real64 DotSize, color Color, real32 RenderLayer, renderer* Renderer)
{
	//TIME_BEGIN

	if (Font == GameNull) { Font = &Globals->AssetsList.EngineResources.DefaultFont; }

	bool32 Debug = false;

	int32 StringLen = StringLength(String);

	render_command Command = {};
	InitRenderCommand(&Command, 6 * StringLen);
	InitIndexBuffer(&Command);

	shader* Shader = &Globals->AssetsList.EngineResources.FontSDFShader;
	Command.Shader = *Shader;

	layout_data* VertexLayout = Command.GetLayout();
	VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), Command.BufferCapacity, GlobalTransMem);

	layout_data* TextureLayout = Command.GetLayout();
	TextureLayout->Allocate(Shader->GetLayout(render::ShaderTextureCoordsID), Command.BufferCapacity, GlobalTransMem);

	FontFillRenderCommand(VertexLayout, TextureLayout, 0, Font, String, Baseline, DotSize, RenderLayer, Renderer);

	Command.Uniforms = Command.Shader.Uniforms.Copy(GlobalTransMem);
	Command.Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	Command.Uniforms.SetImage("diffuseTex", Font->Atlas.GLID);
	Command.Uniforms.SetMat4("projection", m4y4Transpose(Renderer->Camera->ProjectionMatrix));
	Command.Uniforms.SetMat4("view", m4y4Transpose(Renderer->Camera->ViewMatrix));

	InsertRenderCommand(Renderer, &Command);

	//TIME_END
}

void FontRenderString(font_style* Style, string String, vector2 Baseline, color Color, real32 RenderLayer, renderer* Renderer)
{
	FontRenderString(Style->Font, String, Baseline, Style->SizePoints, Color, RenderLayer, Renderer);
}

enum string_align {
	StringAlign_Middle,
	StringAlign_Left,
	StringAlign_Right,
};

void
FontRenderStringAlign(loaded_font * Font, string Word, vector2 BaseLine, real64 ScaleModifier, color Color, string_align Alignment, real64 Layer, renderer * Renderer)
{
	if (Font == GameNull) { Font = &Globals->AssetsList.EngineResources.DefaultFont; }

	real64 WordLength = GetWordPixelWidth(Font, Word, ScaleModifier);

	if (Alignment == StringAlign_Right) {
		BaseLine.X -= WordLength;
	} else if (Alignment == StringAlign_Middle) {
		BaseLine.X -= WordLength * 0.5f;
	}

	FontRenderString(Font, Word, BaseLine, ScaleModifier, Color, (real32)Layer, Renderer);
}

enum font_align {
	FONTALIGN_TOP_LEFT,
	FONTALIGN_BOTTOM_LEFT,

	// TODO implement these options
	FONTALIGN_BOTTOM_RIGHT,
	FONTALIGN_TOP_RIGHT,
};

void
FontRenderParagraph(loaded_font * Font, string Paragraph, rect Rect, real64 DotSize, color Color, font_align Alignment, real32 Layer, renderer * Renderer, memory_arena * Memory)
{
	if (Font == GameNull) { Font = &Globals->AssetsList.EngineResources.DefaultFont; }

	int32 StringLen = StringLength(Paragraph);

	render_command Command = {};
	InitRenderCommand(&Command, 6 * StringLen);
	InitIndexBuffer(&Command);

	shader* Shader = &Globals->AssetsList.EngineResources.FontSDFShader;
	Command.Shader = *Shader;

	layout_data* VertexLayout = Command.GetLayout();
	VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), Command.BufferCapacity, GlobalTransMem);

	layout_data* TextureLayout = Command.GetLayout();
	TextureLayout->Allocate(Shader->GetLayout(render::ShaderTextureCoordsID), Command.BufferCapacity, GlobalTransMem);

	real64 LineHeight = Font->LineHeight * DotSize * PlatformApi.ScreenTypePoints();

	// View the rect
	// RenderRect(Rect, COLOR_RED, (real32)MaxRenderOrder, &GameState->Renderer, &Memory->TransientMemory, &GameState->UICamera);

	vector2 PosAt = {};
	int32 VertIndex = 0;

	// This will handle first line indentation. But why does the first line need to be pushed in? Even at 0 the first line is too far to the left.
	real32 FirstLineAdj = 5.0f;

	switch (Alignment) {
		case (FONTALIGN_TOP_LEFT): {
			PosAt = vector2{Rect.TopLeft.X + FirstLineAdj, Rect.TopLeft.Y};
			break;
		}

		case (FONTALIGN_BOTTOM_LEFT): {
			PosAt = vector2{Rect.TopLeft.X + FirstLineAdj, Rect.BottomRight.Y};
			break;
		}

		case (FONTALIGN_BOTTOM_RIGHT): {
			// TODO this
			Assert(0);
			break;
		}

		case (FONTALIGN_TOP_RIGHT): {
			// TODO this
			Assert(0);
			break;
		}
	}

	uint32 WordLength = CharArrayLength(Paragraph.CharArray);
	split_info SplitStrings = SplitString(Paragraph, ' ', true, true, GlobalTransMem);
	string *StringsArray = (string *)SplitStrings.Strings;

	PosAt.Y += LineHeight;

	for (uint32 WordIndex = 0; WordIndex < SplitStrings.StringsCount; WordIndex++) {
		string NextString = StringsArray[WordIndex];

		real64 WordWidth = GetWordPixelWidth(Font, NextString, DotSize);
		if (WordWidth + PosAt.X > Rect.BottomRight.X) {
			PosAt = vector2{Rect.TopLeft.X, PosAt.Y + LineHeight};
		}

		// Check for line break
		if (NextString.CharArray[0] == '\n' || NextString.CharArray[1] == '\n') {
			PosAt = vector2{Rect.TopLeft.X, PosAt.Y + LineHeight};
			// continue;
		} else {
			FontFillRenderCommand(VertexLayout, TextureLayout, VertIndex, Font, NextString, PosAt, DotSize, Layer, Renderer);
			VertIndex += StringLength(NextString) * 6;
			PosAt.X += WordWidth + WordSpacing;
		}
	}

	Command.Uniforms = Command.Shader.Uniforms.Copy(GlobalTransMem);
	Command.Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	Command.Uniforms.SetImage("diffuseTex", Font->Atlas.GLID);
	Command.Uniforms.SetMat4("projection", m4y4Transpose(Renderer->Camera->ProjectionMatrix));
	Command.Uniforms.SetMat4("view", m4y4Transpose(Renderer->Camera->ViewMatrix));

	InsertRenderCommand(Renderer, &Command);
}

void FontRenderParagraph(font_style* Style, string Paragraph, rect Rect, color Color, font_align Alignment, real32 Layer, renderer * Renderer, memory_arena * Memory)
{
	FontRenderParagraph(Style->Font, Paragraph, Rect, Style->SizePoints, Color, Alignment, Layer, Renderer, Memory);
}


#endif