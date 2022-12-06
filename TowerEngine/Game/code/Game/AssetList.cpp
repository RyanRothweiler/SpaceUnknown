#define STB_IMAGE_IMPLEMENTATION
#include "T:\Game\code\Engine\stb_image.h"
#include "T:\Game\code\Engine\ModelLoading\Dae.cpp"

namespace assets {

	int32 KnownLoadingSteps;
	int32 CurrentLoadingStep;
	char* CurrentLoadingStepDisplay = "loading";

	entity* GetEntity(string ID)
	{
		for (int x = 0; x < Globals->AssetsList.EntitiesCount; x++) {
			asset_entity* Entity = &Globals->AssetsList.Entities[x];
			if (Entity->ID == ID) {
				return &Entity->Entity;
			}
		}

		ConsoleLog("Could not find entity.");
		ConsoleLog(ID);
		return {};
	}

	entity* CopyEntity(entity* Source, int32 ID)
	{
		entity* Copy = (entity*)fixed_allocator::Alloc(&Globals->AssetsList.LoadedEntities);

		MemoryCopy((char*)Copy, (char*)Source, sizeof(entity));
		Copy->Name = Source->ID;
		Copy->ID = ID;

		return Copy;
	}

	// This returns a copy of the entity
	entity* AllocateEntity(string ID)
	{
		entity* Source = GetEntity(ID);
		if (Source != GameNull) {
			entity* Copy = CopyEntity(Source, 0);

			// Copy material. Maybe this isn't always what we want. This probably also causes leaks.
			Copy->Material = (material*)ArenaAllocate(GlobalPermMem, sizeof(material));
			Copy->Material->Shader = Source->Material->Shader;
			Copy->Material->Uniforms = Source->Material->Uniforms.Copy(GlobalPermMem);

			return Copy;
		}
		return {};
	}
}

namespace assets {

	void UploadMesh(vao* VAO, entity* Entity, shader* Shader)
	{
		model* Mesh = Entity->Model;

		int vertLoc = Shader->GetLayoutLocation(render::ShaderVertID);
		if (vertLoc >= 0) {
			RenderApi.VAOBind_v3(VAO, Mesh->Points, Mesh->IndeciesCount, vertLoc);
		}

		int texCoordsLoc = Shader->GetLayoutLocation(render::ShaderTextureCoordsID);
		if (texCoordsLoc >= 0) {
			RenderApi.VAOBind_v2(VAO, Mesh->UVs, Mesh->IndeciesCount, texCoordsLoc);
		}

		int normalLoc = Shader->GetLayoutLocation(render::ShaderNormalID);
		if (normalLoc >= 0) {
			RenderApi.VAOBind_v3(VAO, Mesh->Normals, Mesh->IndeciesCount, normalLoc);
		}

		int normalTanLoc = Shader->GetLayoutLocation(render::ShaderNormalTanID);
		if (normalTanLoc >= 0) {
			RenderApi.VAOBind_v3(VAO, Mesh->NormalTans, Mesh->IndeciesCount, normalTanLoc);
		}

		int normalBiTanLoc = Shader->GetLayoutLocation(render::ShaderNormalBiTanID);
		if (normalBiTanLoc >= 0) {
			RenderApi.VAOBind_v3(VAO, Mesh->NormalBiTans, Mesh->IndeciesCount, normalBiTanLoc);
		}

		int boneWeightsLoc = Shader->GetLayoutLocation(render::ShaderBoneWeightsID);
		if (boneWeightsLoc >= 0) {
			RenderApi.VAOBind_v3(VAO, Entity->SkinController.BoneWeights, Mesh->IndeciesCount, boneWeightsLoc);
		}

		int boneIndeciesLoc = Shader->GetLayoutLocation(render::ShaderBoneIndeciesID);
		if (boneIndeciesLoc >= 0) {
			RenderApi.VAOBind_v3(VAO, Entity->SkinController.BoneIndecies, Mesh->IndeciesCount, boneIndeciesLoc);
		}
	}

	void UploadEntityMat(entity* Entity, material* Material)
	{
		if (Entity->Model != GameNull) {
			Entity->VAO = RenderApi.CreateVAO();
			UploadMesh(&Entity->VAO, Entity, Material->Shader);
		}

		for (int e = 0; e < Entity->ChildrenCount; e++) {
			UploadEntityMat(Entity->Children[e], Material);
		}
	}

	void UploadEntity(entity* Entity)
	{
		UploadEntityMat(Entity, Entity->Material);
	}

	void SetupEntityUpload(entity* Entity, assets_list* Assets)
	{
		Assets->EntitiesToUpload[Assets->EntitiesToUploadCount++] = Entity;
		Assert(Assets->EntitiesToUploadCount < ArrayCount(Assets->EntitiesToUpload));
	}

	unsigned char* STBI_Load(string FilePath, int* Width, int* Height, memory_arena* TransMemory)
	{
		read_file_result FileResult = PlatformApi.ReadFile(FilePath.CharArray, TransMemory);
		unsigned char* Data = (unsigned char*)FileResult.Contents;

		// TODO test this. We flip vertically but I found this online, might work
		//stbi_set_flip_vertically_on_load(true);

		int n;
		unsigned char *data = stbi_load_from_memory(Data, FileResult.ContentsSize, Width, Height, &n, 4);
		if (data != NULL) {

			// Mirror vertically
			uint32 YHalf = (uint32)(*Height * 0.5f);
			uint32 RowSize = 4 * *Width;
			void* DummyData = ArenaAllocate(TransMemory, RowSize);
			for (uint32 Row = 0; Row < YHalf; Row++) {
				unsigned char* TopRow = data + (Row * RowSize);
				unsigned char* BottomRow = (data + (RowSize * *Height)) - ((Row + 1) * RowSize);

				memcpy(DummyData, (void*)TopRow, RowSize);
				memcpy((void*)TopRow, (void*)BottomRow, RowSize);
				memcpy((void*)BottomRow, DummyData, RowSize);
			}

			return data;
		} else {
			return GameNull;
			ConsoleLog("Could not find file " + FilePath);
		}
	}

	// Order is right, left, top, bottom, front, back
	cube_map GLLoadCubeMap(char* Files[6], memory_arena* TransMem)
	{
		cube_map Result = {};

		stbi_set_flip_vertically_on_load(true);

		int X, Y;
		bitmap_pixels Pixels[6] = {};
		for (int x = 0; x < 6; x++) {
			Pixels[x].Pixels = STBI_Load(Files[x], &X, &Y, TransMem);
			if (Pixels[x].Pixels == GameNull) {
				Result.Valid = false;
				return Result;
			}
		}

		stbi_set_flip_vertically_on_load(false);

		RenderApi.MakeCubeMap(&Result, X, Y, &Pixels[0]);

		// Should we free here?s
		// stbi_image_free(Data);

		Result.Valid = true;
		return Result;
	}

	loaded_image GLLoadHDRTexture(char* FilePath)
	{
		loaded_image Ret = {};

		stbi_set_flip_vertically_on_load(true);

		int width, height, nrComponents;
		float *Data = stbi_loadf(FilePath, &width, &height, &nrComponents, 0);

		if (Data) {
			RenderApi.MakeHDRTexture(&Ret, Data, width, height);
			Ret.Valid = true;
		} else {
			Ret.Valid = false;
		}

		stbi_set_flip_vertically_on_load(false);
		stbi_image_free(Data);

		return Ret;
	}

	void SetupImage(loaded_image* Image, string FilePath, gl_blend_type BlendType, correct_gamma CorrectGamma, assets_list* AssetsList)
	{
		Image->FilePath = FilePath;
		Image->BlendType = BlendType;
		Image->CorrectGamma = (CorrectGamma == correct_gamma::yes);

		AssetsList->ImagesToLoad[AssetsList->ImagesToLoadCount++] = Image;
		Assert(AssetsList->ImagesToLoadCount < ArrayCount(AssetsList->ImagesToLoad));
	}

	void GLLoadPNG_Pointer(loaded_image* Image, memory_arena* TransMem)
	{
		unsigned char* Data = STBI_Load(Image->FilePath, &Image->Width, &Image->Height, TransMem);

		if (Data != GameNull) {
			RenderApi.MakeTexture(Image, (uint32*)Data, Image->CorrectGamma);
			Image->Valid = true;
		} else {
			string Error = "Could not find image " + Image->FilePath;
			ConsoleLog(Error);
			Image->Valid = false;
		}

		stbi_image_free(Data);
	}

	loaded_image GLLoadPNG(string FilePath, gl_blend_type BlendType, correct_gamma CorrectGamma, memory_arena* TransMem)
	{
		loaded_image Result = {};
		Result.FilePath = FilePath;
		Result.BlendType = BlendType;
		Result.CorrectGamma = (CorrectGamma == correct_gamma::yes);

		GLLoadPNG_Pointer(&Result, TransMem);

		return Result;
	}

	int32 ExtractInt(char* CurrPos)
	{
		char* CharsCountStart = CurrPos;
		uint32 Count = 0;

		int32 Neg = 1;
		if (*CurrPos == '-') {
			Neg = -1;
			CurrPos++;
			CharsCountStart = CurrPos;
		}

		while (CharIsNumber(*CurrPos)) { CurrPos++; Count++; } CurrPos++;
		string CharsCountStr = BuildString(CharsCountStart, Count);

		int32 Num = StringToInt32(CharsCountStr) * Neg;
		return Num;
	}

	// https://github.com/Chlumsky/msdf-atlas-gen
	// https://github.com/Chlumsky/msdfgen
	void LoadFontMSDF(loaded_font* Font, string Path, memory_arena* TransMem, assets_list* AssetsList)
	{
		string AtlasPath = Path + ".png";
		string JsonPath = Path + ".json";
		string MetricsPath = Path + ".csv";

		// atlas
		SetupImage(&Font->Atlas, AtlasPath, gl_blend_type::linear, correct_gamma::no, AssetsList);

		// line height is only set in json
		// This doesn't work because my json loader isn't robust enough
		//json::json_data json = json::LoadFile(JsonPath, GlobalTransMem);
		//Font.LineHeight = json::GetInt32Safe("metrics.lineHeight", &json, 0);

		// Get glyph info from csv
		csv::csv CSV = csv::Load(MetricsPath, TransMem);
		Font->Info = (glyph_info*)ArenaAllocate(GlobalPermMem, sizeof(glyph_info) * FontGlyphsTotalCount); // assume ascii so 128 total
		for (int i = 0; i < CSV.LinesCount; i++) {
			Assert(i < FontGlyphsTotalCount);

			int64 Unicode = 		csv::GetInt64(&CSV, i, 0);
			real64 Advance = 		csv::GetReal64(&CSV, i, 1);
			real64 PlaneLeft = 		csv::GetReal64(&CSV, i, 2);
			real64 PlaneBottom = 	csv::GetReal64(&CSV, i, 3);
			real64 PlaneRight = 	csv::GetReal64(&CSV, i, 4);
			real64 PlaneTop = 		csv::GetReal64(&CSV, i, 5);
			real64 Left = 			csv::GetReal64(&CSV, i, 6);
			real64 Bottom = 		csv::GetReal64(&CSV, i, 7);
			real64 Right = 			csv::GetReal64(&CSV, i, 8);
			real64 Top = 			csv::GetReal64(&CSV, i, 9);

			glyph_info* Info = &Font->Info[Unicode];
			Info->Left = Left;
			Info->Right = Right;
			Info->Top = Top;
			Info->Bottom = Bottom;
			Info->PlaneLeft = PlaneLeft;
			Info->PlaneRight = PlaneRight;
			Info->PlaneTop = PlaneTop;
			Info->PlaneBottom = PlaneBottom;
			Info->XAdvance = Advance;

			Info->Valid = true;
		}
	}

	loaded_image* GetImage(string ID)
	{
		for (int x = 0; x < Globals->AssetsList.ImagesCount; x++) {
			asset_image* Image = &Globals->AssetsList.Images[x];
			if (Image->ID == ID) {
				return &Image->LoadedImage;
			}
		}

		ConsoleLog("Could not find image.");
		ConsoleLog(ID);
		return {};
	}

	shader* GetShader(string ID)
	{
		for (int x = 0; x < Globals->AssetsList.ShadersCount; x++) {
			asset_shader* Shader = &Globals->AssetsList.Shaders[x];
			if (Shader->ID == ID) {
				return &Shader->Shader;
			}
		}
		ConsoleLog("Could not find shader.");
		ConsoleLog(ID);
		return {};
	}

	material* GetMaterial(string ID)
	{
		list_link* CurrentLink = Globals->AssetsList.Materials->TopLink;
		for (uint32 i = 0; i < Globals->AssetsList.Materials->LinkCount; i++) {

			asset_material* Material = (asset_material*)CurrentLink->Data;
			if (Material->Name == ID) {
				return &Material->Material;
			}

			CurrentLink = CurrentLink->NextLink;
		}

		ConsoleLog("Could not find material.");
		ConsoleLog(ID);
		return {};
	}

	void FreeEntity(entity* Entity)
	{
		fixed_allocator::Free(&Globals->AssetsList.LoadedEntities, (void*)Entity);
	}

	void LoadAssets(assets_list* Assets, string AssetRootDir, memory_arena* TransMemory)
	{
		Assets->AssetRootDir = AssetRootDir;

		// Setup lists to load first, so that we know how much loading tasks will be done for the loading displays
		KnownLoadingSteps = 0;

		asset_image Images[] = {
			{"Gizmo_Circle", 		"EngineResources/Circle.png", 		gl_blend_type::linear, correct_gamma::no},
			{"Lock", 				"padlock.png", 						gl_blend_type::linear, correct_gamma::no},
			{"CheckMark", 			"check-mark.png", 					gl_blend_type::linear, correct_gamma::no},
		};
		KnownLoadingSteps += ArrayCount(Images);

		asset_shader Shaders[] = {
			{"ScreenDrawTexture",	"Shaders/ScreenDrawTexture.vs",		"Shaders/ScreenDrawTexture.fs"},
			//{"ScreenDraw",			"Shaders/ScreenDraw.vs",			"Shaders/ScreenDraw.fs"},

			//{"FontSDF",				"Shaders/FontSDF.vs",				"Shaders/FontSDF.fs"},
			//{"PBR", 				"Shaders/PBR.vs", 					"Shaders/PBR.fs"},

			/*
			{"PBR_Skeletal", 		"Shaders/PBR_Skeletal.vs", 			"Shaders/PBR.fs"},
			{"BasicSkeletal", 		"Shaders/BasicSkeletal.vs", 		"Shaders/BasicSkeletal.fs"},
			{"GaussianBlur", 		"Shaders/GaussianBlur.vs", 			"Shaders/GaussianBlur.fs"},
			{"DepthDisplay", 		"Shaders/DepthDisplay.vs", 			"Shaders/DepthDisplay.fs"},
			{"BasicTex", 			"Shaders/Basic_Tex.vs", 			"Shaders/Basic_Tex.fs"},
			{"ImGui", 				"Shaders/ImGui.vs", 				"Shaders/ImGui.fs"},
			//{"CamBasic", 			"Shaders/CamBasic.vs", 				"Shaders/CamBasic.fs"},
			{"Light", 				"Shaders/Light.vs", 				"Shaders/Light.fs"},
			{"Skybox", 				"Shaders/Skybox.vs", 				"Shaders/Skybox.fs"},
			{"IBLConv", 			"Shaders/IBLConv.vs", 				"Shaders/IBLConv.fs"},
			{"Gizmo", 				"Shaders/Gizmo.vs", 				"Shaders/Gizmo.fs"},
			//{"ScreenDraw", 			"Shaders/ScreenDraw.vs", 			"Shaders/ScreenDraw.fs"},
			{"Basic", 				"Shaders/Basic.vs", 				"Shaders/Basic.fs"},
			{"EquiToCube", 			"Shaders/EquiToCube.vs", 			"Shaders/EquiToCube.fs"},
			{"BRDF", 				"Shaders/BRDF.vs", 					"Shaders/BRDF.fs"},
			{"PreFilter", 			"Shaders/PreFilter.vs", 			"Shaders/PreFilter.fs"},
			*/
		};
		KnownLoadingSteps += ArrayCount(Shaders);

		char* Materials[] = {
			"Gizmo",
			"Basic",
		};
		KnownLoadingSteps += ArrayCount(Materials);

		asset_entity Entities[] = {
			//{"SPIKE", 						"Enemies/Atom/Atom.dae",							"Gizmo"},
			//{"SPIKE", 						"Monsters/SpikeBall/SpikeBall.dae",					"Gizmo"},

			{"UnitCube", 					"UnitCube.dae",										"Gizmo"},
			{"Engine_Arrow", 				"EngineResources/Arrow.dae",						"Gizmo"},
			{"Engine_Circle", 				"EngineResources/Circle.dae",						"Gizmo"},
			{"Engine_Camera", 				"EngineResources/Camera.dae",						"Gizmo"},
			{"Engine_Plane", 				"EngineResources/Plane.dae",						"Gizmo"},
			{"Engine_Sphere", 				"EngineResources/Sphere.dae",						"Gizmo"},
			{"Engine_LightDirectional", 	"EngineResources/Light_Directional.dae",			"Gizmo"},
			{"Engine_LightPoint", 			"EngineResources/Light_Point.dae",					"Gizmo"},
		};
		KnownLoadingSteps += ArrayCount(Entities);

		path_list EntityPaths = {};
		PlatformApi.GetPathsForFileType(".entity", AssetRootDir.Array(), TransMemory, &EntityPaths);
		KnownLoadingSteps += PathListCount(&EntityPaths);

		CurrentLoadingStep = 0;

		// Images
		{
			Assets->Images = (asset_image*)ArenaAllocate(GlobalPermMem, ArrayCount(Images) * sizeof(asset_image));
			Assets->ImagesCount = ArrayCount(Images);
			// Copy and load images. We will need a more dynamic solution for loading the images on demand
			for (int x = 0; x < ArrayCount(Images); x++) {
				Assets->Images[x] = Images[x];
				SetupImage(&Assets->Images[x].LoadedImage, AssetRootDir + Assets->Images[x].FilePath, gl_blend_type::linear, Assets->Images[x].GammaCorrect, Assets);

				CurrentLoadingStepDisplay = Assets->Images[x].FilePath.Array();
				CurrentLoadingStep++;
			}
		}

		// Shaders
		{
			Assets->Shaders = (asset_shader*)ArenaAllocate(GlobalPermMem, ArrayCount(Shaders) * sizeof(asset_shader));
			Assets->ShadersCount = ArrayCount(Shaders);

			for (int x = 0; x < ArrayCount(Shaders); x++) {
				Assets->Shaders[x] = Shaders[x];
				Globals->ShaderLoader.Load(&Assets->Shaders[x].Shader, AssetRootDir + Shaders[x].VertFilePath, AssetRootDir + Shaders[x].FragFilePath, TransMemory);
				Globals->ShaderLoader.SetAutoReloadShader(&Assets->Shaders[x].Shader);

				CurrentLoadingStepDisplay = Shaders[x].VertFilePath.Array();
				CurrentLoadingStep++;
			}
		}


		// Materials
		{
			Assets->Materials = CreateList(GlobalPermMem, sizeof(asset_material));

			for (int x = 0; x < 0; x++) {

				string Dest = AssetRootDir + MATERIAL_DIR + Materials[x] + ".material";
				read_file_result File = PlatformApi.ReadFile(&Dest.CharArray[0], GlobalPermMem);

				if (File.ContentsSize > 0) {
					CurrentLoadingStep++;
					CurrentLoadingStepDisplay = Dest.Array();

					int32 Version = *(int32*)(File.Contents);

					if (Version == 1) {

						asset_material* Mat = (asset_material*)File.Contents;
						Mat->Version = MATERIAL_VERSION;

						// Link shader and reconcile the file and shader uniforms. Maybe the shader changed.
						Mat->Material.Create(GetShader(Mat->ShaderID), GlobalPermMem);

						// Update the values in the material using the file uniforms
						// The order of the uniforms might have changed
						for (int y = 0; y < Mat->UniformsCount; y++) {
							asset_material_uniform* FileAssetUni = &Mat->Uniforms[y];

							// Find the uniform from the shader
							for (int z = 0; z < Mat->Material.Uniforms.Count; z++) {
								if (Mat->Material.Uniforms.Array[z]->Expose) {

									if (Mat->Material.Uniforms.Array[z]->Name == FileAssetUni->Name) {

										switch (FileAssetUni->Type) {

											case glsl_type::gl_samplerCube: {
												// Find image and save the name
											} break;

											case glsl_type::gl_sampler2D: {
												for (int i = 0; i < Globals->AssetsList.ImagesCount; i++) {
													if (Globals->AssetsList.Images[i].ID == FileAssetUni->Data.ImageName) {
														Mat->Material.Uniforms.Array[z]->Data.ImageID = Globals->AssetsList.Images[i].LoadedImage.GLID;
														break;
													}
												}
											} break;

											case glsl_type::gl_float: {
												Mat->Material.Uniforms.Array[z]->Data.Float = FileAssetUni->Data.Float;
											} break;

											case glsl_type::gl_mat4: {
												Mat->Material.Uniforms.Array[z]->Data.Float = FileAssetUni->Data.Float;
											} break;

											case glsl_type::gl_vec3: {
												Mat->Material.Uniforms.Array[z]->Data.Vec3 = FileAssetUni->Data.Vec3;
											} break;

											default: {
												// Unsupported glsl type
												Assert(0);
											} break;
										}

										break;
									}
								}
							}
						}

						asset_material* MatLink = (asset_material*)AddLink(Globals->AssetsList.Materials, (void*)Mat, GlobalPermMem);
						Globals->ShaderLoader.SetAutoReloadMaterial(&MatLink->Material);

					} else {
						// Invalid material version
						Assert(0);
					}
				} else {
					ConsoleLog("Could not find material file");
					ConsoleLog(Dest);
				}
			}
		}

		// Entities
		{
			Assets->LoadedEntities = fixed_allocator::Create(sizeof(entity), Thousand(10));

			// TODO this count can be gathered during GetPathsForFileType. Just pass as pointer
			int32 EntitiesCount = 0;
			{
				path_list* P = &EntityPaths;
				while (StringLength(P->Path) > 0) {
					EntitiesCount++;
					P = P->Next;
				}
			}

			Assets->Entities = (asset_entity*)ArenaAllocate(GlobalPermMem, (ArrayCount(Entities) + EntitiesCount) * sizeof(asset_entity));
			Assets->EntitiesCount = ArrayCount(Entities);

			for (int x = 0; x < ArrayCount(Entities); x++) {
				CurrentLoadingStep++;
				CurrentLoadingStepDisplay = Entities[x].ID.Array();

				Assets->Entities[x] = Entities[x];

				// Load model
				Dae::Load(&Assets->Entities[x].Entity, AssetRootDir + Entities[x].ModelPath, true, TransMemory);

				// Set material
				Assets->Entities[x].Entity.MaterialID = Entities[x].MaterialName;
				Assets->Entities[x].Entity.Material = assets::GetMaterial(Entities[x].MaterialName);

				// Upload to gpu
				SetupEntityUpload(&Assets->Entities[x].Entity, Assets);
			}

			// Also load entity definition files
			{
				path_list* P = &EntityPaths;
				while (StringLength(P->Path) > 0) {
					CurrentLoadingStep++;

					ConsoleLog("Loading Entity");
					CurrentLoadingStepDisplay = P->Path.Array();
					ConsoleLog(P->Path);

					asset_entity* AE = &Assets->Entities[Assets->EntitiesCount];
					Assets->EntitiesCount++;
					// Don't need an assert here since we know this will always be smaller than the array size

					// Get json data
					json::json_data json = json::LoadFile(P->Path, TransMemory);

					AE->ID = json::GetString("name", &json);
					entity* Entity = &AE->Entity;

					// Load model
					Dae::Load(Entity, AssetRootDir + json::GetString("model", &json), true, TransMemory);

					// Create and set material
					string ShaderID = json::GetString("shader", &json);

					material* Material = (material*)ArenaAllocate(GlobalPermMem, sizeof(material));
					Material->Create(GetShader(ShaderID), GlobalPermMem);

					// Get the uniforms from the enetity def
					for (int y = 0; y < Material->Uniforms.Count; y++) {
						shader_uniform* Uni = Material->Uniforms.Array[y];

						if (Uni->Expose) {
							switch (Uni->Type) {

								case glsl_type::gl_float: {
									string Key = "uniforms." + Uni->Name + ".data";
									string Data = json::GetData(Key, &json);
									if (Data != EmptyString) {
										Uni->Data.Float = (float)StringToReal64(Data);
									} else {
										ConsoleLog("Missing uniform float.");
										ConsoleLog(Key);
									}
								} break;

								case glsl_type::gl_sampler2D: {

									string ImageKey = "uniforms." + Uni->Name + ".data";
									string GammaKey = "uniforms." + Uni->Name + ".gamma_correct";

									// Default is not to gamma correct
									correct_gamma GC = correct_gamma::no;
									string Data = json::GetData(GammaKey, &json);
									if (Data != EmptyString) {
										if (json::GetBool(GammaKey, &json)) {
											GC = correct_gamma::yes;
										}
									}

									string ImagePath = json::GetData(ImageKey, &json);
									if (ImagePath != EmptyString) {

										SetupImage(&Assets->EntityTextures[Assets->EntityTexturesCount], AssetRootDir + ImagePath, gl_blend_type::linear, GC, Assets);
										Uni->EntityImage = &Assets->EntityTextures[Assets->EntityTexturesCount];

										Assets->EntityTexturesCount++;
										Assert(Assets->EntityTexturesCount < ArrayCount(Assets->EntityTextures));

									} else {
										ConsoleLog("Missing uniform image.");
										ConsoleLog(ImageKey);
									}
								} break;

								default: {
									// Unsupported glsl type
									// Not sure we would evet want to support more??
								} break;
							}
						}
					}

					Globals->ShaderLoader.SetAutoReloadMaterial(Material);

					Entity->MaterialID = "what to put here?? idk";
					Entity->Material = Material;

					SetupEntityUpload(Entity, Assets);

					P = P->Next;
				}
			}
		}

		// Fonts
		{
			LoadFontMSDF(&Globals->RajMedium, AssetRootDir + "Fonts/Rajdhani/Rajdhani_Medium", TransMemory, Assets);
			Globals->RajMedium.LineHeight = 1.276f;

			LoadFontMSDF(&Globals->RajBold, AssetRootDir + "Fonts/Rajdhani/Rajdhani_Bold", TransMemory, Assets);
			Globals->RajBold.LineHeight = 1.276f;

			LoadFontMSDF(&Globals->RajSemiBold, AssetRootDir + "Fonts/Rajdhani/Rajdhani_SemiBold", TransMemory, Assets);
			Globals->RajSemiBold.LineHeight = 1.276f;

			LoadFontMSDF(&Globals->OpenSansRegular, AssetRootDir + "Fonts/OpenSans/OpenSans_Regular", TransMemory, Assets);
			Globals->OpenSansRegular.LineHeight = 1.361f;

			LoadFontMSDF(&Globals->OpenSansBold, AssetRootDir + "Fonts/OpenSans/OpenSans_Bold", TransMemory, Assets);
			Globals->OpenSansBold.LineHeight = 1.361f;

			// setup font styles
			{
				Globals->FontStyleH1.Font = &Globals->RajBold;
				Globals->FontStyleH1.SizePoints = 35.0f;

				Globals->FontStyleH2.Font = &Globals->RajSemiBold;
				Globals->FontStyleH2.SizePoints = 25.0f;

				Globals->FontStyleH3.Font = &Globals->OpenSansBold;
				Globals->FontStyleH3.SizePoints = 11.0f;

				Globals->FontStyleButton.Font = &Globals->RajSemiBold;
				Globals->FontStyleButton.SizePoints = 18.0f;

				Globals->FontStyleP.Font = &Globals->OpenSansRegular;
				Globals->FontStyleP.SizePoints = 14.0f;

				Globals->FontStyleDropInfo.Font = &Globals->OpenSansBold;
				Globals->FontStyleDropInfo.SizePoints = 8.0f;

			}
		}

		// General
		{
		}
	}

	void UploadOneEntity(assets_list* AssetsList)
	{
		int i = AssetsList->EntitiesToUploadCount - 1;
		AssetsList->EntitiesToUploadCount--;

		UploadEntity(AssetsList->EntitiesToUpload[i]);
		AssetsList->EntitiesToUpload[i] = {};
	}

	void UploadAllQueuedEntities(assets_list* AssetsList)
	{
		while (AssetsList->EntitiesToUploadCount > 0) {
			UploadOneEntity(AssetsList);
		}
		AssetsList->EntitiesToUploadCount = 0;
	}

	void UploadOneImage(assets_list* AssetsList, memory_arena* TransMem)
	{
		int i = AssetsList->ImagesToLoadCount - 1;
		AssetsList->ImagesToLoadCount--;

		GLLoadPNG_Pointer(AssetsList->ImagesToLoad[i], TransMem);
		AssetsList->ImagesToLoad[i] = {};
	}

	void UploadAllQueuedImages(assets_list* AssetsList, memory_arena* TransMem)
	{
		while (AssetsList->ImagesToLoadCount > 0) {
			UploadOneImage(AssetsList, TransMem);
		}
		AssetsList->ImagesToLoadCount = 0;
	}

	void SaveMaterial(asset_material* MatAsset)
	{
		MatAsset->UniformsCount = 0;

		// Pack in the uniforms
		for (int x = 0; x < MatAsset->Material.Uniforms.Count; x++) {
			if (MatAsset->Material.Uniforms.Array[x]->Expose) {

				int FileIndex = MatAsset->UniformsCount;
				MatAsset->UniformsCount++;

				MatAsset->Uniforms[FileIndex].Name = MatAsset->Material.Uniforms.Array[x]->Name;
				MatAsset->Uniforms[FileIndex].Type = MatAsset->Material.Uniforms.Array[x]->Type;

				switch (MatAsset->Material.Uniforms.Array[x]->Type) {

					case glsl_type::gl_sampler2D: {
						for (int i = 0; i < Globals->AssetsList.ImagesCount; i++) {
							if (Globals->AssetsList.Images[i].LoadedImage.GLID == MatAsset->Material.Uniforms.Array[x]->Data.ImageID) {
								MatAsset->Uniforms[FileIndex].Data.ImageName = Globals->AssetsList.Images[i].ID;
								break;
							}
						}
					} break;

					case glsl_type::gl_float: {
						MatAsset->Uniforms[FileIndex].Data.Float = MatAsset->Material.Uniforms.Array[x]->Data.Float;
					} break;

					case glsl_type::gl_mat4: {
						MatAsset->Uniforms[FileIndex].Data.Mat4 = MatAsset->Material.Uniforms.Array[x]->Data.Mat4;
					} break;

					case glsl_type::gl_vec3: {
						MatAsset->Uniforms[FileIndex].Data.Vec3 = MatAsset->Material.Uniforms.Array[x]->Data.Vec3;
					} break;

					default: {
						// Unsupported glsl type
						Assert(0);
					} break;
				}
			}
		}

		string Dest = Globals->AssetsList.AssetRootDir + MATERIAL_DIR + MatAsset->Name + ".material";
		PlatformApi.WriteFile(&Dest.CharArray[0], (void*)MatAsset, sizeof(asset_material));
	}
}