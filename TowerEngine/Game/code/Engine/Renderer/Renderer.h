#pragma once
#ifndef RendererH

enum class correct_gamma { yes, no };

struct v2 {
	real32 X, Y;
};

struct v3 {
	real32 X, Y, Z;

	static v3 From(vector3 In)
	{
		v3 Result;
		Result.X = (real32)In.X;
		Result.Y = (real32)In.Y;
		Result.Z = (real32)In.Z;
		return Result;
	}

	static v3 From(color In)
	{
		v3 Result;
		Result.X = (real32)In.R;
		Result.Y = (real32)In.G;
		Result.Z = (real32)In.B;
		return Result;
	}
};

struct v4 {
	real32 X, Y, Z, W;
};

struct cube_map {
	uint32 GLID;
	bool32 Valid;
};

struct bitmap_pixels {
	unsigned char* Pixels;
};

// NOTE (Ryan) these need to be exactly the same as the glsl types. gl_TYPE
MetaStruct enum class glsl_type {
	none, gl_mat4, gl_vec4, gl_vec3, gl_vec2, gl_sampler2D, gl_bool, gl_samplerCube, gl_float, gl_int,
};

struct shader_uniform {

	glsl_type Type;
	unsigned int Location;
	string Name;

	bool32 Expose;

	// This is kinda a janky special case for entities.
	// The entity image needs to be stored somewhere
	loaded_image* EntityImage;

	union uniform_data {
		v3 Vec3;
		v4 Vec4;
		m4y4 Mat4;

		// This is a GLID
		uint64 ImageID;

		float Float;
		int32 Int;

		m4y4* Mat4Array;
		v3* Vec3Array;
		float* FloatArray;
		int32* IntArray;
	} Data;

	// Used for sending arrays. Maybe a better way to do this. Only used for bones currently
	int32 ArrayCount;
	bool32 IsArray;

	bool32 Equals(shader_uniform* Other)
	{
		//return false;

		if (Type != Other->Type) {
			return false;
		}

		if (Location != Other->Location) {
			return false;
		}

		if (ArrayCount != Other->ArrayCount) {
			return false;
		}

		switch (Type) {
			case glsl_type::gl_vec4:
				if (!IsArray) {
					if (!CloseEnough(Data.Vec4.X, Other->Data.Vec4.X) ||
					        !CloseEnough(Data.Vec4.Y, Other->Data.Vec4.Y) ||
					        !CloseEnough(Data.Vec4.Z, Other->Data.Vec4.Z) ||
					        !CloseEnough(Data.Vec4.W, Other->Data.Vec4.W)
					   ) {
						return false;
					}
				} else {
					return false;

					// vec4 doesn't support array
					//Assert(false);
				}
				break;
			case glsl_type::gl_vec3:
				if (!IsArray) {
					if (!CloseEnough(Data.Vec3.X, Other->Data.Vec3.X) ||
					        !CloseEnough(Data.Vec3.Y, Other->Data.Vec3.Y) ||
					        !CloseEnough(Data.Vec3.Z, Other->Data.Vec3.Z)
					   ) {
						return false;
					}
				} else {
					for (int i = 0; i < ArrayCount; i++) {
						if (!CloseEnough(Data.Vec3Array[i].X, Other->Data.Vec3Array[i].X) ||
						        !CloseEnough(Data.Vec3Array[i].Y, Other->Data.Vec3Array[i].Y) ||
						        !CloseEnough(Data.Vec3Array[i].Z, Other->Data.Vec3Array[i].Z)
						   ) {
							return false;
						}
					}
				}
				break;
			case glsl_type::gl_float:
				if (!IsArray) {
					if (!CloseEnough(Data.Float, Other->Data.Float)) {
						return false;
					}
				} else {
					for (int i = 0; i < ArrayCount; i++) {
						if (Data.FloatArray[i] != Other->Data.FloatArray[i]) {
							return false;
						}
					}
				}
				break;
			case glsl_type::gl_int:
				if (!IsArray) {
					if (Data.Int != Other->Data.Int) {
						return false;
					}
				} else {
					for (int i = 0; i < ArrayCount; i++) {
						if (Data.IntArray[i] != Other->Data.IntArray[i]) {
							return false;
						}
					}
				}
				break;
			case glsl_type::gl_sampler2D:
			case glsl_type::gl_samplerCube:
				// Arrays of images not supported
				Assert(!IsArray);

				if (Data.ImageID != Other->Data.ImageID) {
					return false;
				}
				break;
			case glsl_type::gl_mat4:
				if (!IsArray) {
					for (int x = 0; x < 3; x++) {
						for (int y = 0; y < 3; y++) {
							if (!CloseEnough(Data.Mat4.E[x][y], Other->Data.Mat4.E[x][y])) {
								return false;
							}
						}
					}
				} else {
					for (int a = 0; a < ArrayCount; a++) {
						for (int x = 0; x < 3; x++) {
							for (int y = 0; y < 3; y++) {
								if (!CloseEnough(Data.Mat4Array[a].E[x][y], Other->Data.Mat4Array[a].E[x][y])) {
									return false;
								}
							}
						}
					}
				}
				break;
			default:
				// That layout type isn't supported... yet
				Assert(0);
				break;
		};

		return true;
	}
};

struct uniform_pack {
	shader_uniform* Array[100];
	int32 Count;

	uniform_pack Copy(memory_arena* Mem)
	{
		uniform_pack Ret = {};
		Ret.Count = Count;
		for (int x = 0; x < Count; x++) {
			Ret.Array[x] = (shader_uniform*)ArenaAllocate(Mem, sizeof(shader_uniform));
			*Ret.Array[x] = *Array[x];
		}
		return Ret;
	}

	shader_uniform* GetUniform(char* ID)
	{
		for (int x = 0; x < Count; x++) {
			if (CharArraysEqual(ID, &Array[x]->Name.CharArray[0])) {
				return Array[x];
			}
		}

		//Assert(0);
		return GameNull;
	}

	void SetMat4(char* ID, m4y4 Data)
	{
		shader_uniform* Uni = GetUniform(ID);
		if (Uni != GameNull) {
			Assert(Uni->Type == glsl_type::gl_mat4);
			Uni->Data.Mat4 = Data;
			Uni->IsArray = false;
		}
	}

	void SetMat4Array(char* ID, m4y4* Data, int32 C)
	{
		shader_uniform* Uni = GetUniform(ID);
		if (Uni != GameNull) {
			Assert(Uni->Type == glsl_type::gl_mat4);
			Uni->Data.Mat4Array = Data;
			Uni->ArrayCount = C;
			Uni->IsArray = true;
		}
	}

	void SetFloatArray(char* ID, float* Data, int32 C)
	{
		shader_uniform* Uni = GetUniform(ID);
		if (Uni != GameNull) {
			Assert(Uni->Type == glsl_type::gl_float);
			Uni->Data.FloatArray = Data;
			Uni->ArrayCount = C;
			Uni->IsArray = true;
		}
	}

	void SetVec3Array(char* ID, v3* Data, int32 C)
	{
		shader_uniform* Uni = GetUniform(ID);
		if (Uni != GameNull) {
			Assert(Uni->Type == glsl_type::gl_vec3);
			Uni->Data.Vec3Array = Data;
			Uni->ArrayCount = C;
			Uni->IsArray = true;
		}
	}

	void SetVec3(char* ID, v3 Data)
	{
		shader_uniform* Uni = GetUniform(ID);
		if (Uni != GameNull) {
			Assert(Uni->Type == glsl_type::gl_vec3);
			Uni->Data.Vec3 = Data;
			Uni->IsArray = false;
		}
	}

	void SetVec4(char* ID, v4 Data)
	{
		shader_uniform* Uni = GetUniform(ID);
		if (Uni != GameNull) {
			Assert(Uni->Type == glsl_type::gl_vec4);
			Uni->Data.Vec4 = Data;
			Uni->IsArray = false;
		}
	}

	void SetImage(char* ID, uint64 Data)
	{
		shader_uniform* Uni = GetUniform(ID);
		if (Uni != GameNull) {
			Assert(Uni->Type == glsl_type::gl_sampler2D || Uni->Type == glsl_type::gl_samplerCube);
			Uni->Data.ImageID = Data;
			Uni->IsArray = false;
		}
	}

	void SetFloat(char* ID, real32 Data)
	{
		shader_uniform* Uni = GetUniform(ID);
		if (Uni != GameNull) {
			Assert(Uni->Type == glsl_type::gl_float);
			Uni->Data.Float = Data;
			Uni->IsArray = false;
		}
	}

	void SetInt(char* ID, int32 Data)
	{
		shader_uniform* Uni = GetUniform(ID);
		if (Uni != GameNull) {
			Assert(Uni->Type == glsl_type::gl_int);
			Uni->Data.Int = Data;
			Uni->IsArray = false;
		}
	}

	void SetIntArray(char* ID, int32* Data, int32 C)
	{
		shader_uniform* Uni = GetUniform(ID);
		if (Uni != GameNull) {
			Assert(Uni->Type == glsl_type::gl_int);
			Uni->Data.IntArray = Data;
			Uni->ArrayCount = C;
			Uni->IsArray = true;
		}
	}
};

struct shader_layout {
	uint32 Loc;
	glsl_type Type;
	bool32 In;
	string Name;
};

struct layout_data {
	shader_layout* LayoutInfo;

	union data {
		v4* Vec4;
		v3* Vec3;
		v2* Vec2;
		float* Float;
		m4y4* Mat4;
		int32* Int;
	} Data;
	int32 DataCount;
	int32 DataSize;
	int32 DataStride;

	uint32 BufferID;

	void Allocate(shader_layout* Info, uint32 Count, memory_arena* Memory)
	{
		LayoutInfo = Info;
		DataCount = Count;

		switch (LayoutInfo->Type) {
			case glsl_type::gl_vec4: {

				Data.Vec4 = (v4*)ArenaAllocate(Memory, sizeof(v4) * DataCount);
				DataSize = sizeof(v4);
				DataStride = 4;
			}
			break;

			case glsl_type::gl_vec3: {
				Data.Vec3 = (v3*)ArenaAllocate(Memory, sizeof(v3) * DataCount);
				DataSize = sizeof(v3);
				DataStride = 3;
			}
			break;

			case glsl_type::gl_vec2: {
				Data.Vec2 = (v2*)ArenaAllocate(Memory, sizeof(v2) * DataCount);
				DataSize = sizeof(v2);
				DataStride = 2;
			}
			break;

			case glsl_type::gl_float: {
				Data.Float = (float*)ArenaAllocate(Memory, sizeof(float) * DataCount);
				DataSize = sizeof(float);
				DataStride = 1;
			}
			break;

			case glsl_type::gl_int: {
				Data.Int = (int32*)ArenaAllocate(Memory, sizeof(int32) * DataCount);
				DataSize = sizeof(int32);
				DataStride = 1;
			}
			break;

			default: {
				// That layout type isn't supported... yet
				Assert(0);
			}
			break;
		};

	}
};

struct shader {

	// uniforms

	char* VertSource;
	char* FragSource;

	string VertPath;
	string FragPath;

	uint64 VertWriteTime;

	uint64 FragWriteTime;

	uint32 Program;
	uint32 VertID;
	uint32 FragID;
	bool32 Valid;

	char ErrorInfo[512];
	bool32 FragError;
	bool32 VertError;
	bool32 LinkError;

	uniform_pack Uniforms;

	shader_layout Layouts[20];
	int32 LayoutsCount;

	shader_layout* GetLayout(string Name)
	{
		for (int x = 0; x < LayoutsCount; x++) {
			if (Layouts[x].Name == Name) {
				return &Layouts[x];
			}
		}

		// Could not find that location
		return GameNull;

	}

	int32 GetLayoutLocation(string Name)
	{
		shader_layout* Layout = GetLayout(Name);
		if (Layout != GameNull) { return Layout->Loc; }
		return -1;
	}
};

struct material {
	shader* Shader;
	uniform_pack Uniforms;

	void Create(shader* SH, memory_arena* Mem)
	{
		if (SH != GameNull) {
			Shader = SH;
			Uniforms = SH->Uniforms.Copy(Mem);
		}
	}
};


m4y4 RotationY(real32 Deg)
{
	real32 C = (real32)cos(Deg);
	real32 S = (real32)sin(Deg);

	m4y4 Ret = {
		{
			{C,	0, S, 0},
			{0, 1, 0, 0},
			{ -S, 0, C, 0},
			{0, 0, 0, 1}
		}
	};

	return Ret;
}

m4y4 RotationX(real32 Deg)
{
	real32 C = (real32)cos(Deg);
	real32 S = (real32)sin(Deg);

	m4y4 Ret = {
		{
			{1,	0, 0, 0},
			{0, C, -S, 0},
			{0, S, C, 0},
			{0, 0, 0, 1}
		}
	};

	return Ret;
}

m4y4 RotationZ(real32 Deg)
{
	real32 C = (real32)cos(Deg);
	real32 S = (real32)sin(Deg);

	m4y4 Ret = {
		{
			{C,	-S, 0, 0},
			{S, C, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		}
	};

	return Ret;
}

enum class projection {orthographic, perspective, brokenui};

#include "../camera/camera.h"

// TODO
// NOTE These are cast to int and sent to the shaders, so don't change the order of these!
enum class light_type {
	point, directional
};

/*
https://google.github.io/filament/Filament.md.html#lighting/units
Should we do this for lighting units? For now just guess and use what is easiest
*/

struct light {
	light_type Type;
	color Color;

	transform Transform;

	camera Cam;
};

struct iv3 {
	int32 X, Y, Z;
};

v3 operator+(v3 A, v3 B)
{
	v3 Ret = {};
	Ret.X = A.X + B.X;
	Ret.Y = A.Y + B.Y;
	Ret.Z = A.Z + B.Z;
	return Ret;
};

v3 operator-(v3 A, v3 B)
{
	v3 Ret = {};
	Ret.X = A.X - B.X;
	Ret.Y = A.Y - B.Y;
	Ret.Z = A.Z - B.Z;
	return Ret;
};

v2 operator+(v2 A, v2 B)
{
	v2 Ret = {};
	Ret.X = A.X + B.X;
	Ret.Y = A.Y + B.Y;
	return Ret;
};

v2 operator-(v2 A, v2 B)
{
	v2 Ret = {};
	Ret.X = A.X - B.X;
	Ret.Y = A.Y - B.Y;
	return Ret;
};

struct str {
	char* Data;
	int64 Count;

	string GetString()
	{
		return BuildString(Data, (uint32)Count);
	}
};

struct bone {
	m4y4 BindTrans;

	string Name;
	string ID;
	string SID;

	bool32 Leaf;
	vector3 TipPos;

	bone* Children[20];
	int32 ChildrenCount;
};

struct keyframe {
	real64 Time;
	m4y4 Trans;
};

struct bone_anim {
	bone* Bone;
	string Target;

	keyframe* Keyframes;
	int32 KeyframesCount;
};

// Holds data on how vertecies are transformed by bones
struct skin_controller {
	string URL;

	int32 BonesCount;
	str BoneNameList;

	// v3 because max 3 weights per bone
	v3* BoneWeights;
	v3* BoneIndecies;

	string MeshUrl;

	m4y4* InvBindTrans;
	m4y4* FinalBoneTransforms;

	int32 GetIDForBoneName(string Input)
	{
		int32 i = 0;
		int32 InputI = 0;
		int32 InputLen = StringLength(Input);

		for (int x = 0; x < BoneNameList.Count; ) {

			// move to end of ID or to first mismatch
			while (InputI <= InputLen && Input.CharArray[InputI] == BoneNameList.Data[x] && BoneNameList.Data[x] != ' ' && BoneNameList.Data[x] != '<' && x < BoneNameList.Count) {
				x++;
				InputI++;
			}

			// Check for success
			if ((BoneNameList.Data[x] == ' ' || x == BoneNameList.Count) && InputI == InputLen) {
				return i;
			}

			// Move to next name and try again
			i++;
			InputI = 0;
			while (BoneNameList.Data[x] != ' ') {
				x++;
			}

			// Move over the space
			x++;
		}

		// Could not find the name. That means the bone is not used in this skin controller
		return -1;
	}

	void UpdateBoneTransforms(bone* Bone, m4y4 ParentTrans)
	{
		TIME_BEGIN

		// Cache this or something
		int BoneIndex = GetIDForBoneName(Bone->SID);

		m4y4 Trans = MatrixMultiply(Bone->BindTrans, ParentTrans);

		if (BoneIndex >= 0) {
			m4y4 InvTrans = InvBindTrans[BoneIndex];

			FinalBoneTransforms[BoneIndex] = MatrixMultiply(InvTrans, Trans);
		}

		for (int x = 0; x < Bone->ChildrenCount; x++) {
			UpdateBoneTransforms(Bone->Children[x], Trans);
		}
	}
};

// Holds bone data
struct armature {
	bool32 Valid;

	bone_anim* BoneAnims;
	int32 BoneAnimsCount;

	int32 BonesCount;
	real64 LengthSeconds;

	bone RootBone;
	m4y4 Transform;

	bone* CheckBone(bone* Bone, string* Name)
	{
		if (Bone->SID == *Name) {
			return Bone;
		}

		// Check children
		for (int x = 0; x < Bone->ChildrenCount; x++) {
			bone* Result = CheckBone(Bone->Children[x], Name);
			if (Result != GameNull) {
				return Result;
			}
		}

		return GameNull;
	}

	bone* FindBone(string* Name)
	{
		return CheckBone(&RootBone, Name);
	}

	void UpdateAnim(real64 TimeSeconds, bool32 Loop = true)
	{
		if (Loop) {
			// Keep time within bounds by looping back to 0.
			real64 Base = LengthSeconds * (int)(TimeSeconds / LengthSeconds);
			TimeSeconds = TimeSeconds - Base;
		} else {
			// Clamp
			TimeSeconds = ClampValue(0.0f, LengthSeconds, TimeSeconds);
		}

		for (int b = 0; b < BoneAnimsCount; b++) {
			bone_anim* Bone = &BoneAnims[b];
			if (Bone->KeyframesCount > 0) {

				// Find before, after keyframes using time
				keyframe* Start = {};
				keyframe* End = {};
				for (int x = 1; x < Bone->KeyframesCount; x++) {
					if (Bone->Keyframes[x].Time > TimeSeconds) {
						Start = &Bone->Keyframes[x - 1];
						End = &Bone->Keyframes[x];
						break;
					}
				}

				if (Start != GameNull && End != GameNull) {

					// Find exact time within keyframes
					real64 len = End->Time - Start->Time;
					real64 rel = TimeSeconds - Start->Time;
					real64 NormTime = rel / len;

					// Extract the translation / rotation from matrix
					quat RotationStart = {};
					RotationStart.FromMat(&Start->Trans);

					quat RotationEnd = {};
					RotationEnd.FromMat(&End->Trans);

					vector3 TransStart = Start->Trans.GetTranslation();
					vector3 TransEnd = End->Trans.GetTranslation();

					vector3 ScaleStart = Start->Trans.GetScale();
					vector3 ScaleEnd = End->Trans.GetScale();

					// Lerp the translation / rotation using exact time between the before / after keyframes
					vector3 TransCurr = Vector3Lerp(TransStart, TransEnd, NormTime);
					vector3 ScaleCurr = Vector3Lerp(ScaleStart, ScaleEnd, NormTime);
					quat RotCurr = QuatLerp(RotationStart, RotationEnd, NormTime);

					// Put translation / rotation back into matrix
					m4y4 Trans = Translate(RotCurr.ToMatrix(), TransCurr);
					Trans = Scale(Trans, ScaleStart);

					// Update the bone bind matrix with the lerped matrix
					Bone->Bone->BindTrans = Trans;
				}
			}
		}
	}
};

struct render_command {

	int32 VAO;

	uint32* IndexBuffer;

	vector4 ClipRect;

	layout_data LayoutsData[10];
	uint32 LayoutsDataCount;

	layout_data* GetLayout()
	{
		layout_data* Data = &LayoutsData[LayoutsDataCount++];
		Assert(LayoutsDataCount < ArrayCount(LayoutsData));
		return Data;
	}

	uniform_pack Uniforms;

	int32 BufferCapacity;
	//int32 IndexCapacity;

	shader Shader;

	bool32 Skybox;
	bool32 Wireframe;

	skin_controller* SkinController;

	// This is super wastefull here. We should pass this into the platform renderer in some unified struct.
	loaded_image* DefaultWhiteImage;
};

struct renderer {
	camera* Camera;
	list_head* RenderCommands;

	render_command SkyboxCommand;
};

// TODO rename this to mesh
struct model {
	string URL;

	aabb AABB;
	real32 BoundingRadius;

	v3* Points;
	v2* UVs;

	v3* Normals;
	v3* NormalTans;
	v3* NormalBiTans;

	uint32* Indecies;
	int32 IndeciesCount;

	uint64 FileWriteTime;
	string FileDir;

	// This is only used during mesh loading
	int32* IndeciesFromFile;
};

struct model_polling {
	model* Models[100];
	int32 Count;
};

struct vao {
	uint32 ID;

	uint32 Buffers[10];
	uint32 BuffersCount;
};



namespace render {

	// Common names in shaders. Not necessarily required but lots of rendering paths use these
	char* ShaderVertID = "vertexPos";
	char* ShaderColorID = "color";
	char* ShaderTextureCoordsID = "texCoords";
	char* ShaderNormalID = "normal";
	char* ShaderNormalTanID = "normalTan";
	char* ShaderNormalBiTanID = "normalBiTan";
	char* ShaderBoneWeightsID = "boneWeights";
	char* ShaderBoneIndeciesID = "boneIndecies";

	struct api {
		void (*MakeProgram) (shader* Shader);
		void (*MakeCubeMap) (cube_map* CubeMap, int32 SideWidth, int32 SideHeight, bitmap_pixels* Sides);
		void (*MakeTexture) (loaded_image *Image, uint32 *BitmapPixels, bool32 CorrectGamma);
		void (*GetFramebuffer) (camera* Cam, int32 ColorElementsCount);
		void (*GetFramebufferDepth) (camera* Cam, uint32 Width, uint32 Height);
		void (*GetFramebufferCubeMap)(camera* Cam, bool32 GenerateMipmap, bool32 Trilinear);
		void (*Render) (render::api* API, camera* ActiveCam, camera* ShadowCam, window_info* WindowInfo, renderer* DebugUIRenderer, renderer* UIRenderer, renderer* GameRenderer, shader* GaussianBlurShader);
		vao  (*CreateVAO)();
		void (*VAOBind_v2) (vao* VAO, v2* Verts, uint32 VertsCount, int32 Location);
		void (*VAOBind_v3) (vao* VAO, v3* Verts, uint32 VertsCount, int32 Location);
		int32(*DoPickRender)(camera* Camera, vector2 MousePos, window_info WindowInfo);
		void (*MakeHDRTexture)(loaded_image* Image, float* Data, int32 Width, int32 Height);
		void (*BakeIBL)(renderer* EquiRenderer, renderer* ConvRenderer, renderer* PreFilterRenderer, window_info* WindowInfo);
		void (*RenderCameraToBuffer)(camera * Camera, renderer * Renderer, window_info WindowInfo);
	};


	struct data {
		uint32 IrradianceMap;
		uint32 PrefilterMap;
		uint32 BRDFlut;

		light* Lights[4] = {};
		int32 LightsCount;

		loaded_image WhiteImage;
	};
	data* Data;

	void RegisterLight(light* Light)
	{
		for (int x = 0; x < ArrayCount(Data->Lights); x++) {
			if (Data->Lights[x] == GameNull) {
				Data->Lights[x] = Light;
				Data->LightsCount++;
				return;
			}
		}

		// Hit light maximum
		Assert(0);
	}

	/*
	// Headless mode stuff. Maybe we'll want this?

	// Empty render api for headless mode ------------------------------------------
	void  EMPTY_MakeProgram (shader * Shader, char* VertexSource, char* FragmentSource)
	{
		// Set true so that things don't complain. Maybe this will be confusing?
		Shader->Valid = true;
	}
	void  EMPTY_MakeCubeMap (cube_map * CubeMap, int32 SideWidth, int32 SideHeight, bitmap_pixels * Sides) { }
	void  EMPTY_MakeTexture (loaded_image * Image, uint32 * BitmapPixels, bool32 CorrectGamma) { }
	void  EMPTY_GetFramebuffer (camera * Cam, int32 ColorElementsCount) { }
	void  EMPTY_GetFramebufferDepth (camera * Cam, uint32 Width, uint32 Height) { }
	void  EMPTY_GetFramebufferCubeMap(camera * Cam, bool32 GenerateMipmap, bool32 Trilinear) { }
	void  EMPTY_Render (render::api * API, camera * ActiveCam, camera * ShadowCam, window_info * WindowInfo, renderer * DebugUIRenderer, renderer * UIRenderer, renderer * GameRenderer, shader * GaussianBlurShader) { }
	vao   EMPTY_CreateVAO() { return {}; }
	void  EMPTY_VAOBind_v2 (vao * VAO, v2 * Verts, uint32 VertsCount, int32 Location) { }
	void  EMPTY_VAOBind_v3 (vao * VAO, v3 * Verts, uint32 VertsCount, int32 Location) { }
	int32 EMPTY_DoPickRender(camera * Camera, vector2 MousePos, window_info WindowInfo) { return 0; }
	void  EMPTY_MakeHDRTexture(loaded_image * Image, float * D, int32 Width, int32 Height) { }
	void  EMPTY_BakeIBL(renderer * EquiRenderer, renderer * ConvRenderer, renderer * PreFilterRenderer, window_info * WindowInfo) { }

	api GetEmptyApi()
	{
		api API = {};

		API.MakeProgram = &EMPTY_MakeProgram;
		API.MakeCubeMap = &EMPTY_MakeCubeMap;
		API.MakeTexture = &EMPTY_MakeTexture;
		API.GetFramebuffer = &EMPTY_GetFramebuffer;
		API.GetFramebufferDepth = &EMPTY_GetFramebufferDepth;
		API.GetFramebufferCubeMap = &EMPTY_GetFramebufferCubeMap;
		API.Render = &EMPTY_Render;
		API.CreateVAO = &EMPTY_CreateVAO;
		API.VAOBind_v2 = &EMPTY_VAOBind_v2;
		API.VAOBind_v3 = &EMPTY_VAOBind_v3;
		API.DoPickRender = &EMPTY_DoPickRender;
		API.MakeHDRTexture = &EMPTY_MakeHDRTexture;
		API.BakeIBL = &EMPTY_BakeIBL;

		return API;
	}
	// -----------------------------------------------------------------------------
	*/
};

#endif