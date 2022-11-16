
struct shader_loader {

private:

	material* MaterialsWatching[100];
	int32 MaterialWatchingCount;

	shader* ShadersWatching[100];
	int32 ShaderWatchingCount;

	struct tokenizer {
		char* Curr;
		char* End;
	};

	enum class token_type {
		uniform, layout
	};

	struct token {
		bool32 Valid;
		token_type Type;
		union {
			shader_uniform Uniform;
			shader_layout Layout;
		};
	};

	struct struct_member {
		bool32 Valid;

		glsl_type Type;
		string Name;
	};

	struct gl_struct {
		string TypeName;

		struct_member Members[50];
		int32 MembersCount;
	};

	gl_struct Structs[10];
	int32 StructsCount;

	glsl_type GetGLSLType(char* Start)
	{
		// Skip whitespace
		while (IsWhitespace(*Start)) { Start++; }

		for (int x = 0; x < ArrayCount(glsl_type_NAME); x++) {
			glsl_type Type = (glsl_type)x;
			bool32 Found = true;

			// The 3 here is becuse our type names are gl_mat4. So the 3 is to skip the gl_
			int Len = StringLength(glsl_type_NAME[x]);
			for (int s = 3; s < Len; s++) {
				if (glsl_type_NAME[x].CharArray[s] != Start[s - 3]) {
					Found = false;
					s = Len + 1;
				}
			}

			if (Found) {
				return Type;
			}
		}

		// Couldn't find that type.
		// Probably a custom struct type. We can do more here but might not really be worth it
		// It would be good to store the struct type to validate that this is actually a valid custom type.
		return glsl_type::none;
	}

	token GetNextToken(tokenizer* Tokenizer, shader* Shader)
	{
		token Token = {};

		bool32 ExposeNext = false;

		while (Tokenizer->Curr < Tokenizer->End) {

			if (Tokenizer->Curr[0] == 'u' && Tokenizer->Curr[1] == 'n' && Tokenizer->Curr[2] == 'i' && Tokenizer->Curr[3] == 'f' && Tokenizer->Curr[4] == 'o' && Tokenizer->Curr[5] == 'r' && Tokenizer->Curr[6] == 'm') {
				Token.Type = token_type::uniform;

				Tokenizer->Curr += 8;

				Token.Uniform.Type = GetGLSLType(&Tokenizer->Curr[0]);

				// Check for struct
				if (Token.Uniform.Type == glsl_type::none) {

					char* Start = Tokenizer->Curr;
					int Count = 0;
					while (!IsWhitespace(Tokenizer->Curr[0])) {
						Tokenizer->Curr++;
						Count++;
					}
					string Type = BuildString(Start, Count);

					gl_struct * Str = GameNull;
					for (int x = 0; x < StructsCount; x++) {
						if (Structs[x].TypeName == Type) {
							Structs[x] = {};
							Str = &Structs[x];
							break;
						}
					}

					Tokenizer->Curr++;

					Start = Tokenizer->Curr;
					Count = 0;
					while (Tokenizer->Curr[0] != ';' && Tokenizer->Curr[0] != '[') {
						Tokenizer->Curr++;
						Count++;
					}
					string VarName = BuildString(Start, Count);

					Assert(Str != GameNull); // Could not find a valid type

					// Unwrap all the types
					for (int x = 0; x < Str->MembersCount; x++) {

						shader_uniform Uniform = {};
						Uniform.Expose = false;
						Uniform.Type = Str->Members[x].Type;
						Uniform.Name = VarName + "." + Str->Members[x].Name;

						// !!!!!!!!!!! this will leak !!!!!!!!!!!!!!!

						Shader->Uniforms.Array[Shader->Uniforms.Count] = (shader_uniform*)ArenaAllocate(GlobalPermMem, sizeof(shader_uniform));
						*Shader->Uniforms.Array[Shader->Uniforms.Count] = Uniform;

						Shader->Uniforms.Count++;
						Assert(Shader->Uniforms.Count < ArrayCount(Shader->Uniforms.Array));
					}

					Token.Valid = false;
					return Token;

				} else {

					Token.Uniform.Expose = ExposeNext;

					// Skip past glsl type
					while (Tokenizer->Curr[0] != ' ') { Tokenizer->Curr++; }
					Tokenizer->Curr++;

					char* Start = Tokenizer->Curr;
					int Count = 0;
					while (Tokenizer->Curr[0] != ';' && Tokenizer->Curr[0] != '[') {
						Tokenizer->Curr++;
						Count++;
					}

					Token.Uniform.Name = BuildString(Start, Count);

					Token.Valid = true;
					return Token;
				}

			} else if (Tokenizer->Curr[0] == 's' && Tokenizer->Curr[1] == 't' && Tokenizer->Curr[2] == 'r' && Tokenizer->Curr[3] == 'u' && Tokenizer->Curr[4] == 'c' && Tokenizer->Curr[5] == 't') {
				// struct

				Tokenizer->Curr += 7;

				gl_struct* Str = &Structs[StructsCount];
				StructsCount++;

				// Get struct names
				{
					char* Start = Tokenizer->Curr;
					int Count = 0;
					while (Tokenizer->Curr[0] != ' ') {
						Tokenizer->Curr++;
						Count++;
					}

					Str->TypeName = BuildString(Start, Count);
				}

				while (Tokenizer->Curr[0] != '{') { Tokenizer->Curr++; }
				Tokenizer->Curr++;

				while (true) {
					struct_member* Member = &Str->Members[Str->MembersCount];
					Str->MembersCount++;

					Member->Type = GetGLSLType(&Tokenizer->Curr[0]);
					Assert(Member->Type != glsl_type::none); // This requires a valid type

					// Skip past glsl type
					while (IsWhitespace(Tokenizer->Curr[0])) { Tokenizer->Curr++; }
					while (Tokenizer->Curr[0] != ' ') { Tokenizer->Curr++; }
					Tokenizer->Curr++;

					char* Start = Tokenizer->Curr;
					int Count = 0;
					while (Tokenizer->Curr[0] != ';' && Tokenizer->Curr[0] != '[') {
						Tokenizer->Curr++;
						Count++;
					}

					Member->Name = BuildString(Start, Count);

					Tokenizer->Curr++;
					while (IsWhitespace(Tokenizer->Curr[0])) { Tokenizer->Curr++; }

					if (Tokenizer->Curr[0] == '}') { break; }
				}

			} else if (Tokenizer->Curr[0] == 'E' && Tokenizer->Curr[1] == 'x' && Tokenizer->Curr[2] == 'p' && Tokenizer->Curr[3] == 'o' && Tokenizer->Curr[4] == 's' && Tokenizer->Curr[5] == 'e') {
				// Expose

				ExposeNext = true;
				Tokenizer->Curr += 6;

			} else if (Tokenizer->Curr[0] == 'l' && Tokenizer->Curr[1] == 'a' && Tokenizer->Curr[2] == 'y' && Tokenizer->Curr[3] == 'o' && Tokenizer->Curr[4] == 'u' && Tokenizer->Curr[5] == 't') {
				// Layout

				Token.Type = token_type::layout;

				// layout
				Tokenizer->Curr += 6;
				// (
				Tokenizer->Curr += 2;
				// location =
				Tokenizer->Curr += 11;

				// Pull layout
				{
					int Len = 0;
					char* Start = Tokenizer->Curr;
					while (IsNumeric(Tokenizer->Curr[0])) {
						Tokenizer->Curr++;
						Len++;
					}

					string Str = BuildString(Start, Len);
					Token.Layout.Loc = StringToInt32(Str);
				}

				Tokenizer->Curr += 2;

				if (Tokenizer->Curr[0] == 'i' && Tokenizer->Curr[1] == 'n') {
					// in
					Token.Layout.In = true;
					Tokenizer->Curr += 3;
				} else if (Tokenizer->Curr[0] == 'o' && Tokenizer->Curr[1] == 'u' && Tokenizer->Curr[2] == 't') {
					// out
					Token.Layout.In = false;
					Tokenizer->Curr += 4;
				} else {
					// Unkown direction?? Should always be either in or out
					Assert(0);
				}

				Token.Layout.Type = GetGLSLType(&Tokenizer->Curr[0]);
				while (Tokenizer->Curr[0] != ' ') { Tokenizer->Curr++; }
				Tokenizer->Curr++;

				// Pull name
				{
					char* Start = Tokenizer->Curr;
					int Count = 0;
					while (Tokenizer->Curr[0] != ';') {
						Tokenizer->Curr++;
						Count++;
					}

					Token.Layout.Name = BuildString(Start, Count);
				}

				Token.Valid = true;
				return Token;

			} else if (Tokenizer->Curr[0] == 'm' && Tokenizer->Curr[1] == 'a' && Tokenizer->Curr[2] == 'i' && Tokenizer->Curr[3] == 'n') {
				// Look for main to quickly exit so we don't parse the entire file
				Token.Valid = false;
				return Token;
			} else {
				Tokenizer->Curr++;
			}
		}

		Token.Valid = false;
		return Token;
	}

	// Returns source and fills in the shader with the uniforms
	char* LoadShader(shader* Shader, string Path, memory_arena* Memory)
	{
		tokenizer Tok = {};

		for (int x = 0; x < ArrayCount(Structs); x++) {
			Structs[x] = {};
		}
		StructsCount = 0;

		char* Source = {};

		read_file_result FileResult = {};
		FileResult = PlatformApi.ReadFile(Path.CharArray, Memory);
		if (FileResult.ContentsSize != 0) {

			// Copy file so we can null terminate it
			Source = (char*)ArenaAllocate(Memory, FileResult.ContentsSize + 1);
			ClearMemory((uint8*)Source, FileResult.ContentsSize + 1);
			MemoryCopy(Source, (char*)FileResult.Contents, FileResult.ContentsSize);
			Source[FileResult.ContentsSize + 1] = GameNull;

			Tok.Curr = Source;
			Tok.End = Source + FileResult.ContentsSize;
		} else {
			ConsoleLog("Could not find file " + Path);
			return GameNull;
		}

		while (true) {
			token Token = GetNextToken(&Tok, Shader);
			if (Token.Valid) {

				if (Token.Type == token_type::uniform) {
					// !!!!!!!! we need to free this if it already exists.
					// Shader reloading will leak here

					Shader->Uniforms.Array[Shader->Uniforms.Count] = (shader_uniform*)ArenaAllocate(GlobalPermMem, sizeof(shader_uniform));
					*Shader->Uniforms.Array[Shader->Uniforms.Count] = Token.Uniform;

					Shader->Uniforms.Count++;
					Assert(Shader->Uniforms.Count < ArrayCount(Shader->Uniforms.Array));
				} else if (Token.Type == token_type::layout) {

					Shader->Layouts[Shader->LayoutsCount] = Token.Layout;
					Shader->LayoutsCount++;
					Assert(Shader->LayoutsCount < ArrayCount(Shader->Layouts));

				} else {
					// Unknown type
					Assert(0);
				}
			} else {
				break;
			}
		}

		return Source;
	}


public:

	void Load(shader* Shader, string VertexShaderPath, string FragmentShaderPath, memory_arena* Memory)
	{
		ConsoleLog("Processing shader ----------------- \n " + VertexShaderPath + " \n " + FragmentShaderPath);

		Shader->Uniforms.Count = 0;
		Shader->LayoutsCount = 0;

		Shader->VertSource = LoadShader(Shader, VertexShaderPath, Memory);
		Shader->FragSource = LoadShader(Shader, FragmentShaderPath, Memory);

		if (Shader->VertSource == GameNull) {
			Shader->VertError = true;
			return;
		}

		if (Shader->FragSource == GameNull) {
			Shader->FragError = true;
			return;
		}

		Shader->VertPath = VertexShaderPath;
		Shader->VertWriteTime = PlatformApi.GetFileWriteTime(VertexShaderPath.CharArray);
		Shader->FragPath = FragmentShaderPath;
		Shader->FragWriteTime = PlatformApi.GetFileWriteTime(FragmentShaderPath.CharArray);

		if (Shader->FragError || Shader->VertError || Shader->LinkError) {
			//ConsoleLog("Shader build error ------------------ \n ");
		}
	}

	void SetAutoReloadMaterial(material* Material)
	{
		Assert(MaterialWatchingCount < ArrayCount(MaterialsWatching));
		MaterialsWatching[MaterialWatchingCount] = Material;
		MaterialWatchingCount++;
	}

	void SetAutoReloadShader(shader* Shader)
	{
		Assert(ShaderWatchingCount < ArrayCount(ShadersWatching));
		ShadersWatching[ShaderWatchingCount] = Shader;
		ShaderWatchingCount++;
	}

	void PollReload()
	{
		/*
			bool32 ShowError = false;

			// Materials
			for (int x = 0; x < MaterialWatchingCount; x++) {
				material* Mat = MaterialsWatching[x];

				uint64 VertWriteTime = PlatformApi.GetFileWriteTime(Mat->Shader->VertPath.CharArray);
				uint64 FragWriteTime = PlatformApi.GetFileWriteTime(Mat->Shader->FragPath.CharArray);

				if (VertWriteTime != Mat->Shader->VertWriteTime || FragWriteTime != Mat->Shader->FragWriteTime) {
					Load(Mat->Shader, Mat->Shader->VertPath, Mat->Shader->FragPath);


					// Update locations for any materials that use this shader

					for (int m = 0; m < MaterialWatchingCount; m++) {
						material* M = MaterialsWatching[m];
						if (M->Shader == Mat->Shader) {

							for (int y = 0; y < M->Uniforms.Count; y++) {
								shader_uniform* MatUni = M->Uniforms.Array[y];

								// Find the uniform from the shader
								for (int z = 0; z < M->Shader->Uniforms.Count; z++) {
									shader_uniform* ShaderUni = M->Shader->Uniforms.Array[z];

									if (ShaderUni->Name == MatUni->Name) {

										shader_uniform::uniform_data OrigData = MatUni->Data;
										*MatUni = *ShaderUni;
										MatUni->Data = OrigData;

										int xxx = 0;
									}
								}
							}
						}
					}
				}

				if (!Mat->Shader->Valid) {
					ShowError = true;
				}
			}

			// Shaders
			for (int x = 0; x < ShaderWatchingCount; x++) {
				shader* Shader = ShadersWatching[x];

				uint64 VertWriteTime = PlatformApi.GetFileWriteTime(Shader->VertPath.CharArray);
				uint64 FragWriteTime = PlatformApi.GetFileWriteTime(Shader->FragPath.CharArray);

				if (VertWriteTime != Shader->VertWriteTime || FragWriteTime != Shader->FragWriteTime) {
					Load(Shader, Shader->VertPath, Shader->FragPath);
				}

				if (!Shader->Valid) {
					ShowError = true;
				}
			}

			if (ShowError) {
				ImGui::Begin("Shader Error");
				for (int x = 0; x < MaterialWatchingCount; x++) {
					shader* Shader = MaterialsWatching[x]->Shader;
					if (!Shader->Valid) {
						ImGui::Separator();

						if (Shader->FragError) {
							ImGui::Text(Shader->FragPath.CharArray);
						}

						if (Shader->VertError) {
							ImGui::Text(Shader->VertPath.CharArray);
						}

						ImGui::Text(Shader->ErrorInfo);
					}
				}

				for (int x = 0; x < ShaderWatchingCount; x++) {
					shader* Shader = ShadersWatching[x];
					if (!Shader->Valid) {
						ImGui::Separator();

						if (Shader->FragError) {
							ImGui::Text(Shader->FragPath.CharArray);
						}

						if (Shader->VertError) {
							ImGui::Text(Shader->VertPath.CharArray);
						}

						ImGui::Text(Shader->ErrorInfo);
					}
				}

				ImGui::Separator();
				ImGui::End();
			}
		*/
	}
};