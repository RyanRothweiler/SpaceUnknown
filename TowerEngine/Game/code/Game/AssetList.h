/* NOTE

	Don't change the order of these! The initialization relies on the order.
	Anything new added here,  ID should always come first.

*/

struct asset_image {
	string ID;

	string FilePath;
	gl_blend_type BlendType;
	correct_gamma GammaCorrect;

	loaded_image LoadedImage;
};

struct asset_shader {
	string ID;

	string VertFilePath;
	string FragFilePath;

	shader Shader;
};

#define MATERIAL_DIR "Materials/"
#define MATERIAL_VERSION 1
struct asset_material_uniform {

	glsl_type Type;
	string Name;

	union uniform_data {
		v3 Vec3;
		m4y4 Mat4;
		string ImageName;
		m4y4* Mat4Array;
		float Float;
	} Data;
};

struct asset_entity {
	string ID;
	string ModelPath;
	string MaterialName;

	entity Entity;
};

struct asset_material {
	// Relevant file info
	int32 Version;
	string Name;
	string ShaderID;

	// This is only for saving / loading the material file
	asset_material_uniform Uniforms[100];
	int32 UniformsCount;

	// State. Is saved but won't be valid until initialized, obviously
	material Material;
};
// -----------------------------------------------------------------------------

struct assets_list {

	// Engine resources
	struct engine_resources {
		shader ImGuiShader;
		shader ScreenDrawShader;
		shader ScreenDrawTextureShader;
		shader FontSDFShader;

		loaded_font DefaultFont;
		font_style DefaultFontStyle;
	} EngineResources;

	string AssetRootDir;

	// Common lists

	asset_image* Images;
	int32 ImagesCount;

	asset_shader* Shaders;
	int32 ShadersCount;

	list_head* Materials;

	asset_entity* Entities;
	int32 EntitiesCount;
	loaded_image EntityTextures[512];
	int32 EntityTexturesCount;

	fixed_allocator::memory LoadedEntities;

	// Special case individual assets
	cube_map SpaceCubeMap;
	loaded_image HDRImage;
	loaded_image BlueprintIcon;

	loaded_image* ImagesToLoad[1024];
	int32 ImagesToLoadCount;
	entity* EntitiesToUpload[1024];
	int32 EntitiesToUploadCount;
};