#pragma once
#ifndef RendererCPP
#define RendererCPP

#include "Renderer.h"

char *BackupVertexShader = "layout (location = 0) in vec3 aPos;\n"
                           "uniform mat4 view; \n"
                           "uniform mat4 projection; \n"
                           "uniform mat4 model; \n"
                           "void main()\n"
                           "{\n"
                           "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                           "}\0";

char* BackupFragmentShader = "out mediump vec4 FragColor; \n"
                             "void main() \n"
                             "{ \n"
                             "	FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
                             "}\0";
shader BackupShader;


vector4 RenderScissorRect = {};

void InitRenderCommand(render_command* RendCommand, uint32 BufferSize)
{
	RendCommand->BufferCapacity = BufferSize;
	RendCommand->LayoutsDataCount = 0;
	RendCommand->ClipRect = RenderScissorRect;

	RendCommand->Shader = {};
	RendCommand->VAO = -1;

	RendCommand->DefaultWhiteImage = &render::Data->WhiteImage;

	// Sometimes its nice to get an empty render command and allocate manually
	if (BufferSize > 0) {
		RendCommand->IndexBuffer = (uint32*)ArenaAllocate(GlobalTransMem, sizeof(uint32) * BufferSize);
	}
}

void InitIndexBuffer(render_command* RendCommand)
{
	for (int i = 0; i < RendCommand->BufferCapacity; i++) {
		RendCommand->IndexBuffer[i] = i;
	}
}

void InsertRenderCommand(renderer* Renderer, render_command* RenderCommand)
{
	if (!RenderCommand->Shader.Valid) {
		RenderCommand->Shader = BackupShader;
	}

	// The render command is copied into the list. So we can pass a local scoped RenderCommand here.
	AddLink(Renderer->RenderCommands, (void*)RenderCommand, GlobalTransMem);
}


void RenderRectangleOutline(rect Rect, real64 OutlineSize, color Color, real64 RenderOrder, renderer* UIRenderer)
{
	shader* Shader = &Globals->AssetsList.EngineResources.ScreenDrawShader;
	vector2 Center = RectCenter(Rect);
	vector2 Size = vector2{RectWidth(Rect), RectHeight(Rect)};

	int32 VertsCount = 24;

	render_command RendCommand = {};
	InitRenderCommand(&RendCommand, VertsCount);
	InitIndexBuffer(&RendCommand);
	RendCommand.Shader = *Shader;

	vector2 HalfSize = vector2{Size.X * 0.5f, Size.Y * 0.5f};

	real32 OS = (real32)OutlineSize;

	v3 TopLeft = 			v3{(real32)Center.X - (real32)HalfSize.X, 		(real32)Center.Y - (real32)HalfSize.Y, 			(real32)RenderOrder};
	v3 TopLeftInner = 		v3{(real32)Center.X - (real32)HalfSize.X + OS, 	(real32)Center.Y - (real32)HalfSize.Y + OS, 	(real32)RenderOrder};

	v3 TopRight = 			v3{(real32)Center.X + (real32)HalfSize.X, 		(real32)Center.Y - (real32)HalfSize.Y, 			(real32)RenderOrder};
	v3 TopRightInner = 		v3{(real32)Center.X + (real32)HalfSize.X - OS, 	(real32)Center.Y - (real32)HalfSize.Y + OS, 	(real32)RenderOrder};

	v3 BottomLeft = 		v3{(real32)Center.X - (real32)HalfSize.X, 		(real32)Center.Y + (real32)HalfSize.Y, 			(real32)RenderOrder};
	v3 BottomLeftInner = 	v3{(real32)Center.X - (real32)HalfSize.X + OS, 	(real32)Center.Y + (real32)HalfSize.Y - OS, 	(real32)RenderOrder};

	v3 BottomRight = 		v3{(real32)Center.X + (real32)HalfSize.X, 		(real32)Center.Y + (real32)HalfSize.Y, 			(real32)RenderOrder};
	v3 BottomRightInner = 	v3{(real32)Center.X + (real32)HalfSize.X - OS, 	(real32)Center.Y + (real32)HalfSize.Y - OS, 	(real32)RenderOrder};

	// Vertices
	layout_data* VertexLayout = RendCommand.GetLayout();
	VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), RendCommand.BufferCapacity, GlobalTransMem);
	VertexLayout->Data.Vec3[0] = TopLeft;
	VertexLayout->Data.Vec3[1] = TopRight;
	VertexLayout->Data.Vec3[2] = TopLeftInner;

	VertexLayout->Data.Vec3[3] = TopRight;
	VertexLayout->Data.Vec3[4] = TopRightInner;
	VertexLayout->Data.Vec3[5] = TopLeftInner;

	VertexLayout->Data.Vec3[6] = BottomLeft;
	VertexLayout->Data.Vec3[7] = BottomLeftInner;
	VertexLayout->Data.Vec3[8] = TopLeft;

	VertexLayout->Data.Vec3[9] = TopLeftInner;
	VertexLayout->Data.Vec3[10] = TopLeft;
	VertexLayout->Data.Vec3[11] = BottomLeftInner;

	VertexLayout->Data.Vec3[12] = BottomRight;
	VertexLayout->Data.Vec3[13] = BottomRightInner;
	VertexLayout->Data.Vec3[14] = BottomLeft;

	VertexLayout->Data.Vec3[15] = BottomLeft;
	VertexLayout->Data.Vec3[16] = BottomLeftInner;
	VertexLayout->Data.Vec3[17] = BottomRightInner;

	VertexLayout->Data.Vec3[18] = TopRight;
	VertexLayout->Data.Vec3[19] = TopRightInner;
	VertexLayout->Data.Vec3[20] = BottomRight;

	VertexLayout->Data.Vec3[21] = BottomRight;
	VertexLayout->Data.Vec3[22] = BottomRightInner;
	VertexLayout->Data.Vec3[23] = TopRightInner;

	RendCommand.Uniforms = RendCommand.Shader.Uniforms.Copy(GlobalTransMem);
	RendCommand.Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RendCommand.Uniforms.SetMat4("model", m4y4Identity());
	RendCommand.Uniforms.SetMat4("projection", m4y4Transpose(UIRenderer->Camera->ProjectionMatrix));
	RendCommand.Uniforms.SetMat4("view", m4y4Transpose(UIRenderer->Camera->ViewMatrix));

	InsertRenderCommand(UIRenderer, &RendCommand);
}

void RenderCircle(vector2 Center, vector2 Size, color Color, real64 RenderOrder, renderer* UIRenderer)
{
	static shader* Shader = assets::GetShader("ScreenDrawCircle");

	render_command RendCommand = {};
	InitRenderCommand(&RendCommand, 6);
	InitIndexBuffer(&RendCommand);
	RendCommand.Shader = *Shader;

	vector2 HalfSize = vector2{Size.X * 0.5f, Size.Y * 0.5f};

	v3 TopLeft = 		v3{(real32)Center.X - (real32)HalfSize.X, (real32)Center.Y - (real32)HalfSize.Y, (real32)RenderOrder};
	v3 TopRight = 		v3{(real32)Center.X - (real32)HalfSize.X, (real32)Center.Y + (real32)HalfSize.Y, (real32)RenderOrder};
	v3 BottomRight = 	v3{(real32)Center.X + (real32)HalfSize.X, (real32)Center.Y + (real32)HalfSize.Y, (real32)RenderOrder};
	v3 BottomLeft = 	v3{(real32)Center.X + (real32)HalfSize.X, (real32)Center.Y - (real32)HalfSize.Y, (real32)RenderOrder};

	// Vertices
	layout_data* VertexLayout = RendCommand.GetLayout();

	// Texture coords
	vector2 UVTopRight = vector2{1, 1};
	vector2 UVBottomLeft = vector2{ -1, -1};

	layout_data* TextureLayout = RendCommand.GetLayout();
	TextureLayout->Allocate(Shader->GetLayout(render::ShaderTextureCoordsID), RendCommand.BufferCapacity, GlobalTransMem);
	TextureLayout->LayoutInfo->Loc = RenderApi.GetAttribLocation(Shader, render::ShaderTextureCoordsID);
	TextureLayout->Data.Vec2[0] = v2{(real32)UVBottomLeft.X, (real32)UVBottomLeft.Y};
	TextureLayout->Data.Vec2[1] = v2{(real32)UVTopRight.X, (real32)UVBottomLeft.Y};
	TextureLayout->Data.Vec2[2] = v2{(real32)UVTopRight.X, (real32)UVTopRight.Y};
	TextureLayout->Data.Vec2[3] = v2{(real32)UVBottomLeft.X, (real32)UVBottomLeft.Y};
	TextureLayout->Data.Vec2[4] = v2{(real32)UVTopRight.X, (real32)UVTopRight.Y};
	TextureLayout->Data.Vec2[5] = v2{(real32)UVBottomLeft.X, (real32)UVTopRight.Y};

	VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), RendCommand.BufferCapacity, GlobalTransMem);
	VertexLayout->Data.Vec3[0] = TopRight;
	VertexLayout->Data.Vec3[1] = BottomRight;
	VertexLayout->Data.Vec3[2] = BottomLeft;
	VertexLayout->Data.Vec3[3] = TopRight;
	VertexLayout->Data.Vec3[4] = BottomLeft;
	VertexLayout->Data.Vec3[5] = TopLeft;

	RendCommand.Uniforms = RendCommand.Shader.Uniforms.Copy(GlobalTransMem);
	RendCommand.Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RendCommand.Uniforms.SetMat4("model", m4y4Identity());
	RendCommand.Uniforms.SetMat4("projection", m4y4Transpose(UIRenderer->Camera->ProjectionMatrix));
	RendCommand.Uniforms.SetMat4("view", m4y4Transpose(UIRenderer->Camera->ViewMatrix));

	InsertRenderCommand(UIRenderer, &RendCommand);
}

void RenderCircleOutline(vector2 Center, vector2 Size, color Color, real64 Thickness, real64 RenderOrder, renderer* UIRenderer) {
	RenderCircle(Center, Size, Color, RenderOrder, UIRenderer);
	RenderCircle(Center, vector2{Size.X - Thickness, Size.Y - Thickness}, COLOR_BLACK, RenderOrder + 0.01f, UIRenderer);
}

void RenderRectangle(vector2 Center, vector2 Size, color Color, real64 RenderOrder, renderer* UIRenderer)
{
	shader* Shader = &Globals->AssetsList.EngineResources.ScreenDrawShader;

	render_command RendCommand = {};
	InitRenderCommand(&RendCommand, 6);
	InitIndexBuffer(&RendCommand);
	RendCommand.Shader = *Shader;

	vector2 HalfSize = vector2{Size.X * 0.5f, Size.Y * 0.5f};

	v3 TopLeft = 		v3{(real32)Center.X - (real32)HalfSize.X, (real32)Center.Y - (real32)HalfSize.Y, (real32)RenderOrder};
	v3 TopRight = 		v3{(real32)Center.X - (real32)HalfSize.X, (real32)Center.Y + (real32)HalfSize.Y, (real32)RenderOrder};
	v3 BottomRight = 	v3{(real32)Center.X + (real32)HalfSize.X, (real32)Center.Y + (real32)HalfSize.Y, (real32)RenderOrder};
	v3 BottomLeft = 	v3{(real32)Center.X + (real32)HalfSize.X, (real32)Center.Y - (real32)HalfSize.Y, (real32)RenderOrder};

	// Vertices
	layout_data* VertexLayout = RendCommand.GetLayout();

	VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), RendCommand.BufferCapacity, GlobalTransMem);
	VertexLayout->Data.Vec3[0] = TopRight;
	VertexLayout->Data.Vec3[1] = BottomRight;
	VertexLayout->Data.Vec3[2] = BottomLeft;
	VertexLayout->Data.Vec3[3] = TopRight;
	VertexLayout->Data.Vec3[4] = BottomLeft;
	VertexLayout->Data.Vec3[5] = TopLeft;

	RendCommand.Uniforms = RendCommand.Shader.Uniforms.Copy(GlobalTransMem);
	RendCommand.Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RendCommand.Uniforms.SetMat4("model", m4y4Identity());
	RendCommand.Uniforms.SetMat4("projection", m4y4Transpose(UIRenderer->Camera->ProjectionMatrix));
	RendCommand.Uniforms.SetMat4("view", m4y4Transpose(UIRenderer->Camera->ViewMatrix));

	InsertRenderCommand(UIRenderer, &RendCommand);
}

void RenderRect(rect Rect, color Color, real32 Layer, renderer* Renderer)
{
	RenderRectangle(RectCenter(Rect), vector2{RectWidth(Rect), RectHeight(Rect)}, Color, Layer, Renderer);
}

void RenderTextureCenter(vector2 CenterPosition,
                         vector2 Size, color Color,
                         vector2 UVTopRight, vector2 UVBottomLeft,
                         uint32 GLID, real64 RenderOrder,
                         m4y4 Model,
                         renderer* Renderer)
{
	shader* Shader = &Globals->AssetsList.EngineResources.ScreenDrawTextureShader;

	render_command Command = {};
	InitRenderCommand(&Command, 6);
	InitIndexBuffer(&Command);
	Command.Shader = *Shader;

	vector2 HalfSize = vector2{Size.X * 0.5f, Size.Y * 0.5f};

	/*
	v3 TopLeft = 		v3{(real32)CenterPosition.X - (real32)HalfSize.X, (real32)CenterPosition.Y - (real32)HalfSize.Y, (real32)RenderOrder};
	v3 TopRight = 		v3{(real32)CenterPosition.X - (real32)HalfSize.X, (real32)CenterPosition.Y + (real32)HalfSize.Y, (real32)RenderOrder};
	v3 BottomRight = 	v3{(real32)CenterPosition.X + (real32)HalfSize.X, (real32)CenterPosition.Y + (real32)HalfSize.Y, (real32)RenderOrder};
	v3 BottomLeft = 	v3{(real32)CenterPosition.X + (real32)HalfSize.X, (real32)CenterPosition.Y - (real32)HalfSize.Y, (real32)RenderOrder};
	*/

	v3 TopLeft = 		v3{ -(real32)HalfSize.X, -(real32)HalfSize.Y, (real32)RenderOrder};
	v3 TopRight = 		v3{ -(real32)HalfSize.X, (real32)HalfSize.Y, (real32)RenderOrder};
	v3 BottomRight = 	v3{(real32)HalfSize.X, (real32)HalfSize.Y, (real32)RenderOrder};
	v3 BottomLeft = 	v3{(real32)HalfSize.X, -(real32)HalfSize.Y, (real32)RenderOrder};

	// Vertices
	layout_data* VertexLayout = Command.GetLayout();
	VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), Command.BufferCapacity, GlobalTransMem);
	VertexLayout->LayoutInfo->Loc = RenderApi.GetAttribLocation(Shader, render::ShaderVertID);
	VertexLayout->Data.Vec3[0] = TopRight;
	VertexLayout->Data.Vec3[1] = BottomRight;
	VertexLayout->Data.Vec3[2] = BottomLeft;
	VertexLayout->Data.Vec3[3] = TopRight;
	VertexLayout->Data.Vec3[4] = BottomLeft;
	VertexLayout->Data.Vec3[5] = TopLeft;

	// Texture coords
	layout_data* TextureLayout = Command.GetLayout();
	TextureLayout->Allocate(Shader->GetLayout(render::ShaderTextureCoordsID), Command.BufferCapacity, GlobalTransMem);
	TextureLayout->LayoutInfo->Loc = RenderApi.GetAttribLocation(Shader, render::ShaderTextureCoordsID);
	TextureLayout->Data.Vec2[0] = v2{(real32)UVBottomLeft.X, (real32)UVBottomLeft.Y};
	TextureLayout->Data.Vec2[1] = v2{(real32)UVTopRight.X, (real32)UVBottomLeft.Y};
	TextureLayout->Data.Vec2[2] = v2{(real32)UVTopRight.X, (real32)UVTopRight.Y};
	TextureLayout->Data.Vec2[3] = v2{(real32)UVBottomLeft.X, (real32)UVBottomLeft.Y};
	TextureLayout->Data.Vec2[4] = v2{(real32)UVTopRight.X, (real32)UVTopRight.Y};
	TextureLayout->Data.Vec2[5] = v2{(real32)UVBottomLeft.X, (real32)UVTopRight.Y};

	Model = Translate(Model, vector3{CenterPosition.X, CenterPosition.Y, 0});

	Command.Uniforms = Command.Shader.Uniforms.Copy(GlobalTransMem);
	Command.Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	Command.Uniforms.SetImage("diffuseTex", GLID);
	Command.Uniforms.SetMat4("projection", m4y4Transpose(Renderer->Camera->ProjectionMatrix));
	Command.Uniforms.SetMat4("view", m4y4Transpose(Renderer->Camera->ViewMatrix));
	Command.Uniforms.SetMat4("model", m4y4Transpose(Model));

	InsertRenderCommand(Renderer, &Command);
}


void RenderNineSlice(rect Rect, real64 Left, real32 Right, real32 Top, real32 Bottom, uint32 GLID)
{
	// Example
	/*
	{
		rect R = rect{100, 100, Globals->Input->MousePos.X, Globals->Input->MousePos.Y};
		//RenderRect(R, color{1, 1, 1, 0.5f}, 0, Globals->UIRenderer);
		RenderNineSlice(R,
		                0.1907f, 1.0f - 0.1907f,
		                0.363f, 1.0f - 0.363f,
		                assets::GetImage("PassiveAbilityBar")->GLID);
	}
	*/

	real64 EdgePixels = 50;

	// TopLeft
	rect RectTopLeft = {};
	RectTopLeft.TopLeft = Rect.TopLeft;
	RectTopLeft.BottomRight = Rect.TopLeft + vector2{EdgePixels, EdgePixels};

	RenderTextureCenter(RectCenter(RectTopLeft),
	                    vector2{RectWidth(RectTopLeft), RectHeight(RectTopLeft)},
	                    COLOR_WHITE,
	                    vector2{1.0f - Left, 0}, vector2{1.0f, Top},
	                    GLID, 0, m4y4Identity(), Globals->UIRenderer);

	// Top
	rect RectTop = {};
	RectTop.TopLeft = vector2{RectTopLeft.BottomRight.X, RectTopLeft.TopLeft.Y};
	RectTop.BottomRight = vector2{Rect.BottomRight.X - EdgePixels, Rect.TopLeft.Y + EdgePixels};

	RenderTextureCenter(RectCenter(RectTop),
	                    vector2{RectWidth(RectTop), RectHeight(RectTop)},
	                    COLOR_WHITE,
	                    vector2{Left, 0}, vector2{Right, Top},
	                    GLID, 0, m4y4Identity(), Globals->UIRenderer);

	// TopRight
	rect RectTopRight = {};
	RectTopRight.TopLeft = vector2{RectTop.BottomRight.X, RectTopLeft.TopLeft.Y};
	RectTopRight.BottomRight = vector2{Rect.BottomRight.X, RectTop.BottomRight.Y};

	RenderTextureCenter(RectCenter(RectTopRight),
	                    vector2{RectWidth(RectTopRight), RectHeight(RectTopRight)},
	                    COLOR_WHITE,
	                    vector2{0, 0}, vector2{1.0 - Right, Top},
	                    GLID, 0, m4y4Identity(), Globals->UIRenderer);

	// BottomLeft
	rect RectBottomLeft = {};
	RectBottomLeft.TopLeft = vector2{Rect.TopLeft.X, Rect.BottomRight.Y - EdgePixels};
	RectBottomLeft.BottomRight = vector2{Rect.TopLeft.X + EdgePixels, Rect.BottomRight.Y};

	RenderTextureCenter(RectCenter(RectBottomLeft),
	                    vector2{RectWidth(RectBottomLeft), RectHeight(RectBottomLeft)},
	                    COLOR_WHITE,
	                    vector2{1.0f - Left, Bottom}, vector2{1.0f, 1.0f},
	                    GLID, 0, m4y4Identity(), Globals->UIRenderer);

	// Bottom
	rect RectBottom = {};
	RectBottom.TopLeft = vector2{RectBottomLeft.BottomRight.X, RectBottomLeft.TopLeft.Y};
	RectBottom.BottomRight = vector2{Rect.BottomRight.X - EdgePixels, Rect.BottomRight.Y};

	RenderTextureCenter(RectCenter(RectBottom),
	                    vector2{RectWidth(RectBottom), RectHeight(RectBottom)},
	                    COLOR_WHITE,
	                    vector2{1.0f - Left, Bottom}, vector2{Right, 1.0f},
	                    GLID, 0, m4y4Identity(), Globals->UIRenderer);

	// BottomRight
	rect RectBottomRight = {};
	RectBottomRight.TopLeft = vector2{RectBottom.BottomRight.X, RectBottom.TopLeft.Y};
	RectBottomRight.BottomRight = Rect.BottomRight;

	RenderTextureCenter(RectCenter(RectBottomRight),
	                    vector2{RectWidth(RectBottomRight), RectHeight(RectBottomRight)},
	                    COLOR_WHITE,
	                    vector2{0, Bottom}, vector2{1.0 - Right, 1.0f},
	                    GLID, 0, m4y4Identity(), Globals->UIRenderer);

	// Left
	rect RectLeft = {};
	RectLeft.TopLeft = vector2{Rect.TopLeft.X, RectTopLeft.BottomRight.Y};
	RectLeft.BottomRight = vector2{Rect.TopLeft.X + EdgePixels, RectBottomLeft.TopLeft.Y};

	RenderTextureCenter(RectCenter(RectLeft),
	                    vector2{RectWidth(RectLeft), RectHeight(RectLeft)},
	                    COLOR_WHITE,
	                    vector2{Left, Bottom}, vector2{0, Top},
	                    GLID, 0, m4y4Identity(), Globals->UIRenderer);

	// Right
	rect RectRight = {};
	RectRight.TopLeft = vector2{RectTopRight.TopLeft.X, Rect.TopLeft.Y + EdgePixels};
	RectRight.BottomRight = vector2{Rect.BottomRight.X, RectBottomRight.TopLeft.Y};

	RenderTextureCenter(RectCenter(RectRight),
	                    vector2{RectWidth(RectRight), RectHeight(RectRight)},
	                    COLOR_WHITE,
	                    vector2{0, Bottom}, vector2{1.0 - Right, Top},
	                    GLID, 0, m4y4Identity(), Globals->UIRenderer);

	// Center!!
	// todo

	/*
	RenderRect(RectTopLeft, 	color{1, 1, 1, 0.5f}, 0, Globals->UIRenderer);
	RenderRect(RectTop, 		color{1, 1, 1, 0.5f}, 0, Globals->UIRenderer);
	RenderRect(RectTopRight, 	color{1, 1, 1, 0.5f}, 0, Globals->UIRenderer);
	RenderRect(RectBottomLeft, 	color{1, 1, 1, 0.5f}, 0, Globals->UIRenderer);
	RenderRect(RectBottom, 		color{1, 1, 1, 0.5f}, 0, Globals->UIRenderer);
	RenderRect(RectBottomRight, color{1, 1, 1, 0.5f}, 0, Globals->UIRenderer);
	RenderRect(RectRight, 		color{1, 1, 1, 0.5f}, 0, Globals->UIRenderer);
	RenderRect(RectLeft, 		color{1, 1, 1, 0.5f}, 0, Globals->UIRenderer);
	*/

}

void RenderTexture(vector2 Center, vector2 Size, color Color,
                   vector2 UVTopRight, vector2 UVBottomLeft,
                   uint32 GLID, real32 Layer,
                   m4y4 Model,
                   renderer* Renderer)
{
	RenderTextureCenter(Center, Size, Color, UVTopRight, UVBottomLeft, GLID, Layer, Model, Renderer);
}

void RenderTextureAll(vector2 Center, vector2 Size, color Color, uint32 GLID, real32 Layer, m4y4 Model, renderer* Renderer)
{
	RenderTexture(Center, Size, Color, vector2{1, 1}, vector2{0, 0}, GLID, Layer, Model, Renderer);
}

void RenderSprite(vector2 Center, vector2 Size, color Color,
                  vector2 SpriteChoice, sprite_sheet* Sheet, real32 Layer,
                  renderer* Renderer)
{
	vector2 UVTopRight = {};
	vector2 UVBottomLeft = {};

	// Make choice starts at top left
	SpriteChoice.Y = Sheet->RowCount - SpriteChoice.Y - 1;

	// Assumes the edges aren't padded
	vector2 SizeWithoutPadding = vector2{Sheet->Image.Width - ((real64)Sheet->Padding * (real64)(Sheet->ColumnCount - 1)),
	                                     Sheet->Image.Height - ((real64)Sheet->Padding * (real64)(Sheet->RowCount - 1))};
	vector2 SpritePixelSize = vector2{(real64)SizeWithoutPadding.X / (real64)Sheet->ColumnCount,
	                                  (real64)SizeWithoutPadding.Y / (real64)Sheet->RowCount};
	vector2 SpritePercSize = vector2{SpritePixelSize.X / (real64)Sheet->Image.Width,
	                                 SpritePixelSize.Y / (real64)Sheet->Image.Height};

	real64 PercForPadding = (real64)Sheet->Padding / (real64)Sheet->Image.Width;

	vector2 PaddingToAdd = vector2{SpriteChoice.X * PercForPadding,
	                               SpriteChoice.Y * PercForPadding};

	UVBottomLeft.X = (SpritePercSize.X * SpriteChoice.X) + PaddingToAdd.X;
	UVBottomLeft.Y = (SpritePercSize.Y * SpriteChoice.Y) + PaddingToAdd.Y;
	UVTopRight.X = (UVBottomLeft.X + SpritePercSize.X);
	UVTopRight.Y = (UVBottomLeft.Y + SpritePercSize.Y);

	RenderTexture(Center, Size, Color, UVTopRight, UVBottomLeft, Sheet->Image.GLID, Layer, m4y4Identity(), Renderer);
}

// This assumes looping
struct render_line {
	vector2* Points;
	int32 PointsCount;
};
void RenderLine(render_line Line, real32 Thickness, color Color, renderer* UIRenderer, bool32 Loop)
{
	real32 HalfThick = Thickness * 0.5f;

	// Lines
	int C = Line.PointsCount;
	if (Loop) C++;
	for (int i = 1; i < C; i++) {

		// NOTE this could be more efficient if all of these were one draw
		render_command RendCommand = {};
		InitRenderCommand(&RendCommand, 6);
		InitIndexBuffer(&RendCommand);

		shader* Shader = &Globals->AssetsList.EngineResources.ScreenDrawShader;
		RendCommand.Shader = *Shader;

		vector2 Start = Line.Points[i - 1];
		vector2 End = {};
		if (i == Line.PointsCount) {
			End = Line.Points[0];
		} else {
			End = Line.Points[i];
		}

		vector2 LineDir = Vector2Normalize(End - Start);
		vector2 LineDirPerp = Vector2Perp(LineDir);


		layout_data* VertexLayout = RendCommand.GetLayout();
		VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), RendCommand.BufferCapacity, GlobalTransMem);


		// Lines
		VertexLayout->Data.Vec3[0] = v3{(real32)Start.X + (real32)(LineDirPerp.X * HalfThick),
		                                (real32)Start.Y + (real32)(LineDirPerp.Y * HalfThick),
		                                0};
		VertexLayout->Data.Vec3[1] = v3{(real32)Start.X - (real32)(LineDirPerp.X * HalfThick),
		                                (real32)Start.Y - (real32)(LineDirPerp.Y * HalfThick),
		                                0};
		VertexLayout->Data.Vec3[2] = v3{(real32)End.X + (real32)(LineDirPerp.X * HalfThick),
		                                (real32)End.Y + (real32)(LineDirPerp.Y * HalfThick),
		                                0};

		VertexLayout->Data.Vec3[3] = v3{(real32)End.X + (real32)(LineDirPerp.X * HalfThick),
		                                (real32)End.Y + (real32)(LineDirPerp.Y * HalfThick),
		                                0};
		VertexLayout->Data.Vec3[4] = v3{(real32)End.X - (real32)(LineDirPerp.X * HalfThick),
		                                (real32)End.Y - (real32)(LineDirPerp.Y * HalfThick),
		                                0};
		VertexLayout->Data.Vec3[5] = v3{(real32)Start.X - (real32)(LineDirPerp.X * HalfThick),
		                                (real32)Start.Y - (real32)(LineDirPerp.Y * HalfThick),
		                                0};

		RendCommand.Uniforms = RendCommand.Shader.Uniforms.Copy(GlobalTransMem);
		RendCommand.Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
		RendCommand.Uniforms.SetMat4("model", m4y4Identity());
		RendCommand.Uniforms.SetMat4("projection", m4y4Transpose(UIRenderer->Camera->ProjectionMatrix));
		RendCommand.Uniforms.SetMat4("view", m4y4Transpose(UIRenderer->Camera->ViewMatrix));

		InsertRenderCommand(UIRenderer, &RendCommand);
	}

	if (Loop) {
		// Joints
		//for (int i = 1; i < Line.PointsCount - 1; i++) { // For non looping
		for (int i = 1; i < Line.PointsCount + 1; i++) {

			vector2 PrevPoint = Line.Points[i - 1];
			vector2 CurrPoint = Line.Points[i];
			vector2 NextPoint = Line.Points[i + 1];
			if (i == Line.PointsCount) {
				NextPoint = Line.Points[1];
				CurrPoint = Line.Points[0];
				PrevPoint = Line.Points[Line.PointsCount - 1];
			} else if (i == Line.PointsCount - 1) {
				NextPoint = Line.Points[0];
				CurrPoint = Line.Points[Line.PointsCount - 1];
				PrevPoint = Line.Points[Line.PointsCount - 2];

			}

			vector2 PrevDir = Vector2Normalize(PrevPoint - CurrPoint);
			vector2 NextDir = Vector2Normalize(CurrPoint - NextPoint);

			vector2 PrevPerp = Vector2Perp(PrevDir);
			vector2 NextPerp = Vector2Perp(NextDir);

			vector2 PrevLineStartUpper = vector2{PrevPoint.X - (PrevPerp.X * HalfThick),
			                                     PrevPoint.Y - (PrevPerp.Y * HalfThick)};
			vector2 PrevLineEndUpper = vector2{CurrPoint.X - (PrevPerp.X * HalfThick),
			                                   CurrPoint.Y - (PrevPerp.Y * HalfThick)};

			vector2 NextLineStartUpper = vector2{NextPoint.X - (NextPerp.X * HalfThick),
			                                     NextPoint.Y - (NextPerp.Y * HalfThick)};
			vector2 NextLineEndUpper = vector2{CurrPoint.X - (NextPerp.X * HalfThick),
			                                   CurrPoint.Y - (NextPerp.Y * HalfThick)};

			vector2 PrevLineStartLower = vector2{PrevPoint.X + (PrevPerp.X * HalfThick),
			                                     PrevPoint.Y + (PrevPerp.Y * HalfThick)};
			vector2 PrevLineEndLower = vector2{CurrPoint.X + (PrevPerp.X * HalfThick),
			                                   CurrPoint.Y + (PrevPerp.Y * HalfThick)};

			vector2 NextLineStartLower = vector2{NextPoint.X + (NextPerp.X * HalfThick),
			                                     NextPoint.Y + (NextPerp.Y * HalfThick)};
			vector2 NextLineEndLower = vector2{CurrPoint.X + (NextPerp.X * HalfThick),
			                                   CurrPoint.Y + (NextPerp.Y * HalfThick)};

			intersection_point IntersectionUpper = GetIntersection(PrevLineStartUpper, PrevLineEndUpper, NextLineStartUpper, NextLineEndUpper);
			intersection_point IntersectionLower = GetIntersection(PrevLineStartLower, PrevLineEndLower, NextLineStartLower, NextLineEndLower);
			if (IntersectionLower.Valid && IntersectionUpper.Valid) {

				// Point joint
				render_command RendCommand = {};
				InitRenderCommand(&RendCommand, 12);
				InitIndexBuffer(&RendCommand);

				shader* Shader = &Globals->AssetsList.EngineResources.ScreenDrawShader;

				RendCommand.Shader = *Shader;

				layout_data* VertexLayout = RendCommand.GetLayout();
				VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), RendCommand.BufferCapacity, GlobalTransMem);

				VertexLayout->Data.Vec3[0] = v3{(real32)CurrPoint.X, (real32)CurrPoint.Y, 0};
				VertexLayout->Data.Vec3[1] = v3{(real32)IntersectionUpper.Point.X, (real32)IntersectionUpper.Point.Y, 0};
				VertexLayout->Data.Vec3[2] = v3{(real32)PrevLineEndUpper.X, (real32)PrevLineEndUpper.Y, 0};

				VertexLayout->Data.Vec3[3] = v3{(real32)CurrPoint.X, (real32)CurrPoint.Y, 0};
				VertexLayout->Data.Vec3[4] = v3{(real32)IntersectionUpper.Point.X, (real32)IntersectionUpper.Point.Y, 0};
				VertexLayout->Data.Vec3[5] = v3{(real32)NextLineEndUpper.X, (real32)NextLineEndUpper.Y, 0};

				VertexLayout->Data.Vec3[6] = v3{(real32)CurrPoint.X, (real32)CurrPoint.Y, 0};
				VertexLayout->Data.Vec3[7] = v3{(real32)IntersectionLower.Point.X, (real32)IntersectionLower.Point.Y, 0};
				VertexLayout->Data.Vec3[8] = v3{(real32)PrevLineEndLower.X, (real32)PrevLineEndLower.Y, 0};

				VertexLayout->Data.Vec3[9] = v3{(real32)CurrPoint.X, (real32)CurrPoint.Y, 0};
				VertexLayout->Data.Vec3[10] = v3{(real32)IntersectionLower.Point.X, (real32)IntersectionLower.Point.Y, 0};
				VertexLayout->Data.Vec3[11] = v3{(real32)NextLineEndLower.X, (real32)NextLineEndLower.Y, 0};

				RendCommand.Uniforms = RendCommand.Shader.Uniforms.Copy(GlobalTransMem);
				RendCommand.Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
				//RendCommand.Uniforms.SetVec4("color", v4{0.0f, 0.0f, 1.0f, 1.0f});
				RendCommand.Uniforms.SetMat4("model", m4y4Identity());
				RendCommand.Uniforms.SetMat4("projection", m4y4Transpose(UIRenderer->Camera->ProjectionMatrix));
				RendCommand.Uniforms.SetMat4("view", m4y4Transpose(UIRenderer->Camera->ViewMatrix));
				InsertRenderCommand(UIRenderer, &RendCommand);
			}

			// Miter joint
			/*
			render_command RendCommand = {};
			InitRenderCommand(&RendCommand, 6);
			InitIndexBuffer(&RendCommand);

			shader* Shader = assets::GetShader("ScreenDraw");;
			RendCommand.Shader = *Shader;

			layout_data* VertexLayout = RendCommand.GetLayout();
			AllocateLayout(VertexLayout, Shader->GetLayout(render::ShaderVertID), RendCommand.BufferCapacity, GlobalTransMem);

			VertexLayout->Data.Vec3[0] = v3{(real32)CurrPoint.X, (real32)CurrPoint.Y, 0};
			VertexLayout->Data.Vec3[1] = v3{(real32)CurrPoint.X - (real32)(PrevPerp.X * HalfThick),
			                                (real32)CurrPoint.Y - (real32)(PrevPerp.Y * HalfThick),
			                                0};
			VertexLayout->Data.Vec3[2] = v3{(real32)CurrPoint.X - (real32)(NextPerp.X * HalfThick),
			                                (real32)CurrPoint.Y - (real32)(NextPerp.Y * HalfThick),
			                                0};

			VertexLayout->Data.Vec3[3] = v3{(real32)CurrPoint.X, (real32)CurrPoint.Y, 0};
			VertexLayout->Data.Vec3[4] = v3{(real32)CurrPoint.X + (real32)(PrevPerp.X * HalfThick),
			                                (real32)CurrPoint.Y + (real32)(PrevPerp.Y * HalfThick),
			                                0};
			VertexLayout->Data.Vec3[5] = v3{(real32)CurrPoint.X + (real32)(NextPerp.X * HalfThick),
			                                (real32)CurrPoint.Y + (real32)(NextPerp.Y * HalfThick),
			                                0};


			RendCommand.Uniforms = RendCommand.Shader.Uniforms.Copy(GlobalTransMem);
			RendCommand.Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
			RendCommand.Uniforms.SetMat4("model", m4y4Identity());
			RendCommand.Uniforms.SetMat4("projection", m4y4Transpose(UIRenderer->Camera->ProjectionMatrix));
			RendCommand.Uniforms.SetMat4("view", m4y4Transpose(UIRenderer->Camera->ViewMatrix));
			InsertRenderCommand(UIRenderer, &RendCommand);
			*/
		}
	}
}

// Returns if mouse is over. Builds the line with vertices.
bool32 RenderScreenLineMesh(vector2 Start, vector2 End, real32 Thickness, color Color, renderer* UIRenderer)
{
	render_command RendCommand = {};
	InitRenderCommand(&RendCommand, 6);
	InitIndexBuffer(&RendCommand);

	shader* Shader = &Globals->AssetsList.EngineResources.ScreenDrawShader;
	RendCommand.Shader = *Shader;

	vector2 LineDir = Vector2Normalize(End - Start);
	vector2 LineDirPerp = Vector2Perp(LineDir);
	real32 HalfThick = Thickness * 0.5f;


	layout_data* VertexLayout = RendCommand.GetLayout();
	VertexLayout->Allocate(Shader->GetLayout(render::ShaderVertID), RendCommand.BufferCapacity, GlobalTransMem);

	VertexLayout->Data.Vec3[0] = v3{(real32)Start.X + (real32)(LineDirPerp.X * HalfThick),
	                                (real32)Start.Y + (real32)(LineDirPerp.Y * HalfThick),
	                                0};
	VertexLayout->Data.Vec3[1] = v3{(real32)Start.X - (real32)(LineDirPerp.X * HalfThick),
	                                (real32)Start.Y - (real32)(LineDirPerp.Y * HalfThick),
	                                0};
	VertexLayout->Data.Vec3[2] = v3{(real32)End.X + (real32)(LineDirPerp.X * HalfThick),
	                                (real32)End.Y + (real32)(LineDirPerp.Y * HalfThick),
	                                0};

	VertexLayout->Data.Vec3[3] = v3{(real32)End.X + (real32)(LineDirPerp.X * HalfThick),
	                                (real32)End.Y + (real32)(LineDirPerp.Y * HalfThick),
	                                0};
	VertexLayout->Data.Vec3[4] = v3{(real32)End.X - (real32)(LineDirPerp.X * HalfThick),
	                                (real32)End.Y - (real32)(LineDirPerp.Y * HalfThick),
	                                0};
	VertexLayout->Data.Vec3[5] = v3{(real32)Start.X - (real32)(LineDirPerp.X * HalfThick),
	                                (real32)Start.Y - (real32)(LineDirPerp.Y * HalfThick),
	                                0};

	bool32 Over = false;
	if (
	    TriContainsPoint(Globals->Input->MousePos - vector2{0, 40},
	                     vector2{VertexLayout->Data.Vec3[0].X, VertexLayout->Data.Vec3[0].Y},
	                     vector2{VertexLayout->Data.Vec3[1].X, VertexLayout->Data.Vec3[1].Y},
	                     vector2{VertexLayout->Data.Vec3[2].X, VertexLayout->Data.Vec3[2].Y}
	                    )
	    ||
	    TriContainsPoint(Globals->Input->MousePos - vector2{0, 40},
	                     vector2{VertexLayout->Data.Vec3[3].X, VertexLayout->Data.Vec3[3].Y},
	                     vector2{VertexLayout->Data.Vec3[4].X, VertexLayout->Data.Vec3[4].Y},
	                     vector2{VertexLayout->Data.Vec3[5].X, VertexLayout->Data.Vec3[5].Y}
	                    )
	) {
		//Color = COLOR_WHITE;
		Over = true;
	}

	RendCommand.Uniforms = RendCommand.Shader.Uniforms.Copy(GlobalTransMem);
	RendCommand.Uniforms.SetVec4("color", v4{Color.R, Color.G, Color.B, Color.A});
	RendCommand.Uniforms.SetMat4("model", m4y4Identity());
	RendCommand.Uniforms.SetMat4("view", m4y4Transpose(UIRenderer->Camera->ViewMatrix));
	RendCommand.Uniforms.SetMat4("projection", m4y4Transpose(UIRenderer->Camera->ProjectionMatrix));

	InsertRenderCommand(UIRenderer, &RendCommand);

	return Over;
}

bool IsBigEndian()
{
	union {
		uint32_t i;
		char c[4];
	} bint = {0x01020304};

	return bint.c[0] == 1;
}

uint32 BigEndianToLittle32(uint32 Input)
{
	uint8* Parts = (uint8*)&Input;
	uint32 Value = Parts[3] | Parts[2] << 8 | Parts[1] << 16 | Parts[0] << 24;
	return Value;
}

uint16 BigEndianToLittle16(uint16 Input)
{
	uint8* Parts = (uint8*)&Input;
	uint16 Value = Parts[1] | Parts[0] << 8;
	return Value;
}

void RenderSkybox(model* Mesh, material* Material, renderer* Renderer, vao* VAO, m4y4 Transform)
{
	if (Material->Shader == GameNull) return;

	Renderer->SkyboxCommand = {};
	render_command* Command = &Renderer->SkyboxCommand;

	InitRenderCommand(Command, 0);
	Command->Skybox = true;
	Command->VAO = VAO->ID;
	Command->Shader = *Material->Shader;

	Command->BufferCapacity = Mesh->IndeciesCount;
	Command->IndexBuffer = Mesh->Indecies;

	layout_data* VertexLayout = Command->GetLayout();
	VertexLayout->Allocate(Material->Shader->GetLayout(render::ShaderVertID), Command->BufferCapacity, GlobalTransMem);
	for (int i = 0; i < Command->BufferCapacity; i++) {
		VertexLayout->Data.Vec3[i] = Mesh->Points[i];
	}

	Command->Uniforms = Material->Uniforms.Copy(GlobalTransMem);
	Command->Uniforms.SetMat4("model", m4y4Transpose(Transform));
}

color IDtoCol(uint32 ID)
{
	color Ret = {};
	Ret.R = (real32)((ID & 0x000000FF) >>  0);
	Ret.G = (real32)((ID & 0x0000FF00) >>  8);
	Ret.B = (real32)((ID & 0x00FF0000) >> 16);
	return Ret;
}

void RenderObj(model* Mesh, material* Material, m4y4 Transform, renderer* Renderer, vao* VAO, int32 PickID, skin_controller* SkinCon = GameNull, bool32 Wireframe = false)
{
	//TIME_BEGIN

	render_command Command = {};
	InitRenderCommand(&Command, 0);
	Command.Shader = *Material->Shader;
	Command.VAO = VAO->ID;
	Command.Wireframe = Wireframe;

	Command.SkinController = SkinCon;
	Command.BufferCapacity = Mesh->IndeciesCount;
	Command.IndexBuffer = Mesh->Indecies;

	Command.Uniforms = Material->Uniforms.Copy(GlobalTransMem);
	Command.Uniforms.SetMat4("model", m4y4Transpose(Transform));

	// Update any image ids with the loaded image
	for (int i = 0; i < Command.Uniforms.Count; i++) {
		shader_uniform* Uni = Command.Uniforms.Array[i];
		if (Uni->EntityImage != GameNull && Uni->EntityImage->Valid) {
			Uni->Data.ImageID = Uni->EntityImage->GLID;
		}
	}

	// Set picking id
	{
		color Col = IDtoCol(PickID);
		v3 V = v3{Col.R / 255.0f, Col.G / 255.0f, Col.B / 255.0f};
		Command.Uniforms.SetVec3("PickingColor", V);
	}

	if (SkinCon->BoneWeights != GameNull) {
		Command.Uniforms.SetMat4Array("boneTransforms", SkinCon->FinalBoneTransforms, SkinCon->BonesCount);
	}

	Command.Uniforms.SetInt("lightsCount", render::Data->LightsCount);

	v3* LightPositions = (v3*)ArenaAllocate(GlobalTransMem, sizeof(v3) * render::Data->LightsCount);
	v3* LightColors = (v3*)ArenaAllocate(GlobalTransMem, sizeof(v3) * render::Data->LightsCount);
	v3* LightDirections = (v3*)ArenaAllocate(GlobalTransMem, sizeof(v3) * render::Data->LightsCount);
	int* LightTypes = (int*)ArenaAllocate(GlobalTransMem, sizeof(int) * render::Data->LightsCount);

	for (int x = 0; x < render::Data->LightsCount; x++) {

		render::Data->Lights[x]->Transform.Update(m4y4Identity());

		quat Q = {};
		Q.FromEuler(render::Data->Lights[x]->Transform.LocalRot);
		vector3 Forward = Apply4y4(Q.ToMatrix(), {0, 0, -1});

		vector3 Pos = render::Data->Lights[x]->Transform.LocalPos;
		color Col = render::Data->Lights[x]->Color;
		vector3 Dir = Forward;

		LightDirections[x] = v3{(real32)Dir.X, (real32)Dir.Y, (real32)Dir.Z};
		LightPositions[x] = v3{(real32)Pos.X, (real32)Pos.Y, (real32)Pos.Z};
		LightColors[x] = v3{ Col.R, Col.G, Col.B};
		LightTypes[x] = (int)render::Data->Lights[x]->Type;
	}

	Command.Uniforms.SetVec3Array("lightDirections", LightDirections, render::Data->LightsCount);
	Command.Uniforms.SetVec3Array("lightPositions", LightPositions, render::Data->LightsCount);
	Command.Uniforms.SetVec3Array("lightColors", LightColors, render::Data->LightsCount);
	Command.Uniforms.SetIntArray("lightType", LightTypes, render::Data->LightsCount);
	//Command.Uniforms.SetImage("shadowMap", LightCam->TextureColorbuffers[0]);

	// Set pbr ibl images
	Command.Uniforms.SetImage("irradianceMap", render::Data->IrradianceMap);
	Command.Uniforms.SetImage("prefilterMap", render::Data->PrefilterMap);
	Command.Uniforms.SetImage("brdfLUT", render::Data->BRDFlut);

	InsertRenderCommand(Renderer, &Command);
}

void RenderEntityMat(entity* Entity, material* Material, renderer* Renderer, int32 PickID, bool32 Wireframe)
{
	if (Entity->Model != GameNull) {

		Entity->SkinController.UpdateBoneTransforms(&Entity->Armature->RootBone, m4y4Identity());

		RenderObj(Entity->Model,
		          Material,
		          Entity->Transform.FinalWorld,
		          Renderer,
		          &Entity->VAO,
		          PickID,
		          &Entity->SkinController,
		          Wireframe);
	}

	for (int e = 0; e < Entity->ChildrenCount; e++) {
		RenderEntityMat(Entity->Children[e], Material, Renderer, PickID, Wireframe);
	}
}

void RenderEntity(entity* Entity, renderer* Renderer, int32 PickID)
{
	/*
	TODO Frustum Culling
	https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling

		- Generate sphere volumes for everything
		- Ability to visualize the sphere volumes

		- Build Frustum
		- Cull using frustum and sphere volumes

	*/

	//start here!!!



	if (true) {
		RenderEntityMat(Entity, Entity->Material, Renderer, PickID, Entity->Wireframe);
	}
}

// Only for debug purposes
void RenderLight(light* Light, model* DisplayMesh, renderer* Renderer, memory_arena* TempMem)
{
	//RenderObj(DisplayMesh, Renderer, TempMem);
}

void RenderBone(bone* Bone, m4y4 ParentTrans, engine_state* GameState, game_memory* Memory)
{
	vector3 End = {};

	m4y4 Trans = MatrixMultiply(Bone->BindTrans, ParentTrans);
	vector3 Start = vector3{Trans.E[0][3], Trans.E[1][3], Trans.E[2][3]};

	if (!Bone->Leaf) {
		bone* NextBone = Bone->Children[0];
		m4y4 NextTrans = MatrixMultiply(NextBone->BindTrans, Trans);
		End = vector3{NextTrans.E[0][3], NextTrans.E[1][3], NextTrans.E[2][3]};
	} else {
		vector3 TipWorldPos = Start + Bone->TipPos;

		// Does this actually work or is it even usefull at all??
		// This should renders the tip of the bone, using the blender profile specific information

		m4y4 T = m4y4Identity();
		T.E[0][3] = (real32)(TipWorldPos.X);
		T.E[1][3] = (real32)(TipWorldPos.Y);
		T.E[2][3] = (real32)(TipWorldPos.Z);

		T = MatrixMultiply(Bone->BindTrans, T);

		End = vector3{T.E[0][3], T.E[1][3], T.E[2][3]};
		return;
	}

	//RenderCubeLine(Start, End, COLOR_RED, GameState);

	camera LightCam = {};
	//RenderObj(&GameState->Assets->UnitSphere, &GameState->Assets->LightShader, End, vector3{0.05f, 0.05f, 0.05f}, &GameState->GameRenderer, &Memory->TransientMemory, {}, GameState->Assets->AsphaltAlbedo.GLID, GameState->Assets->AsphaltNormal.GLID, COLOR_RED, m4y4Identity(), &LightCam, false);

	for (int x = 0; x < Bone->ChildrenCount; x++) {
		RenderBone(Bone->Children[x], Trans, GameState, Memory);
	}
}

void RenderArmature(armature* Armature, vector3 Pos, engine_state* GameState, game_memory* Memory)
{
	camera LightCam = {};
	Pos = Apply4y4(Armature->Transform, Pos);
	//RenderObj(&GameState->Assets->UnitSphere, &GameState->Assets->LightShader, Pos, vector3{0.05f, 0.05f, 0.05f}, &GameState->GameRenderer, &Memory->TransientMemory, {}, GameState->Assets->AsphaltAlbedo.GLID, GameState->Assets->AsphaltNormal.GLID, COLOR_RED, m4y4Identity(), &LightCam, false);
	RenderBone(&Armature->RootBone, m4y4Identity(), GameState, Memory);
}


// This builds the view bounding sphere from the frustum.
// Might use in the future for future optimization for quicker sphere checks for early out
#if 0
// Culling!!
{
	State->GameCamera.Far = 100;

	real32 Middle = ((State->GameCamera.Far) + State->GameCamera.Near) * 0.5f;
	State->GameCamera.ViewSphereCenter = State->GameCamera.Center + (State->GameCamera.Forward * -1 * (State->GameCamera.Near + Middle));


	// frustum corner testing
	{
		State->GameCamera.ViewSphereRadius = 0;

		vector2 Cube[4] = {};
		Cube[0] = vector2{0, 0};
		Cube[1] = vector2{(real64)WindowInfo->Width,  (real64)WindowInfo->Height};
		Cube[2] = vector2{0,  (real64)WindowInfo->Height};
		Cube[3] = vector2{(real64)WindowInfo->Width, 0};

		for (int i = 0; i < ArrayCount(Cube); i++) {

			vector3 Corner = ScreenToWorld(Cube[i],
			                               State->GameCamera.Center + (State->GameCamera.Forward * State->GameCamera.Far * -1),
			                               State->GameCamera.Forward,
			                               &State->GameCamera
			                              );


			/*
			//m4y4 Mat = MatrixMultiply(State->GameCamera.ViewMatrixInv, State->GameCamera.ProjectionMatrixInv);
			Cube[i] = Apply4y4_4(Mat, Cube[i]);

			Cube[i].X = Cube[i].X / Cube[i].W;
			Cube[i].Y = Cube[i].Y / Cube[i].W;
			Cube[i].Z = Cube[i].Z / Cube[i].W;
			*/

			GizmoLine(
			    //vector3{Cube[i].X, Cube[i].Y, Cube[i].Z} + State->GameCamera.Center,
			    Corner,
			    State->GameCamera.Center,
			    0.05f
			);

			real32 Dist = (real32)Vector3Distance(State->GameCamera.ViewSphereCenter,
			                                      //vector3{Cube[i].X, Cube[i].Y, Cube[i].Z} + State->GameCamera.Center);
			                                      Corner);

			if (Dist > State->GameCamera.ViewSphereRadius) {
				State->GameCamera.ViewSphereRadius = Dist;
			}

		}
	}

	GizmoSphere(State->GameCamera.ViewSphereCenter, State->GameCamera.ViewSphereRadius);
	//GizmoSphere(State->GameCamera.ViewSphereCenter, 10);
}
#endif

#endif
