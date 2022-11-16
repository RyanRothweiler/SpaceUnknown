#ifndef objcpp
#define objcpp


namespace OBJ {

	v3
	V3Cross(v3 A, v3 B)
	{
		v3 Output = {};

		Output.X = (A.Y * B.Z) - (A.Z * B.Y);
		Output.Y = (A.Z * B.X) - (A.X * B.Z);
		Output.Z = (A.X * B.Y) - (A.Y * B.X);

		return Output;
	}

	enum class token_type {
		unknown, vertex, normal, comment, eof, face, uv
	};

	struct tokenizer  {
		char* Curr;
		char* End;
	};

	struct token {

		token_type Type;

		union {
			v3 Vertex;
			v3 Normal;
			v2 UV;
			string Comment;

			// NOTE this requires triangulation. no quads
			struct {
				// NOTE this is zero based index. (obj is natively one based index)
				int64 VertexIndex;
				int64 TextureIndex;
				int64 NormalIndex;
			} Face[3];
		};
	};

	void MoveToNextLine(tokenizer* Tokenizer)
	{
		while (*Tokenizer->Curr != '\n' && Tokenizer->Curr < Tokenizer->End) {
			Tokenizer->Curr++;
		}
		Tokenizer->Curr++;
	}

	bool IsFloat(char c)
	{
		if (c == '-') { return true; }
		if (c == '.') { return true; }
		return (c >= '0') && (c <= '9');
	}

	real64 GetNextNumber(tokenizer* Tokenizer)
	{
		char* Start = Tokenizer->Curr;
		while (IsFloat(*Tokenizer->Curr)) {
			Tokenizer->Curr++;
		}

		string Num = BuildString(Start, (uint32)(Tokenizer->Curr - Start));
		real64 N = StringToReal64(Num);
		return N;
	}

	token GetNextToken(tokenizer* Tokenizer)
	{
		if (Tokenizer->Curr >= Tokenizer->End)  {
			token Token = {};
			Token.Type = token_type::eof;
			return Token;
		}

		if (*Tokenizer->Curr == '#') {
			char* Start = Tokenizer->Curr;
			MoveToNextLine(Tokenizer);

			int64 S = Tokenizer->Curr - Start;

			token Token = {};
			Token.Type = token_type::comment;
			Token.Comment = BuildString(Start, (uint32)S);

			return Token;

		} else if (Tokenizer->Curr[0] == 'v' && Tokenizer->Curr[1] == 't') {
			// vertex texture
			//vt 0.625000 0.250000

			Tokenizer->Curr += 3;

			token Token = {};
			Token.Type = token_type::uv;

			Token.UV.X = (real32)GetNextNumber(Tokenizer);
			Tokenizer->Curr++;

			Token.UV.Y = (real32)GetNextNumber(Tokenizer);
			Tokenizer->Curr++;

			return Token;

		} else if (Tokenizer->Curr[0] == 'v' && Tokenizer->Curr[1] == 'n') {
			// vertex normal
			//vn -0.4002 0.0572 0.9147

			Tokenizer->Curr += 3;

			token Token = {};
			Token.Type = token_type::normal;

			Token.Normal.X = (real32)GetNextNumber(Tokenizer);
			Tokenizer->Curr++;

			Token.Normal.Y = (real32)GetNextNumber(Tokenizer);
			Tokenizer->Curr++;

			Token.Normal.Z = (real32)GetNextNumber(Tokenizer);
			Tokenizer->Curr++;

			return Token;

		} else if (*Tokenizer->Curr == 'f') {
			// face
			//f 5/1/1 3/2/1 1/3/1

			Tokenizer->Curr += 2;

			token Token = {};
			Token.Type = token_type::face;

			for (int i = 0; i < 3; i++) {
				Token.Face[i].VertexIndex = (int64)GetNextNumber(Tokenizer) - 1;
				Tokenizer->Curr++;

				Token.Face[i].TextureIndex = (int64)GetNextNumber(Tokenizer) - 1;
				Tokenizer->Curr++;

				Token.Face[i].NormalIndex = (int64)GetNextNumber(Tokenizer) - 1;
				Tokenizer->Curr++;
			}

			// Triangulation required
			if (*Tokenizer->Curr == ' ') {
				Assert(0);
			}

			return Token;

		} else if (*Tokenizer->Curr == 'v') {
			// vertex
			//v - 1.000000 1.000000 1.000000

			Tokenizer->Curr += 2;

			token Token = {};
			Token.Type = token_type::vertex;

			Token.Vertex.X = (real32)GetNextNumber(Tokenizer);
			Tokenizer->Curr++;

			Token.Vertex.Y = (real32)GetNextNumber(Tokenizer);
			Tokenizer->Curr++;

			Token.Vertex.Z = (real32)GetNextNumber(Tokenizer);
			Tokenizer->Curr++;

			return Token;
		} else {
			// Could not determine token of that type
		}

		MoveToNextLine(Tokenizer);

		token Ret = {};
		return Ret;
	}

	void LoadOBJ(model* Obj, string FileDir, game_memory* Memory, bool32 AutoReload, game_assets* Assets)
	{
		memory_arena* TempMem = &Memory->TransientMemory;
		memory_arena* PermMem = &Memory->PermanentMemory;

		read_file_result FileResult = {};
		FileResult = PlatformApi.ReadFile(&FileDir.CharArray[0], TempMem);
		if (FileResult.ContentsSize == 0) {
			ConsoleLog("Could not find file " + string{FileDir});
			return;
			Assert(0);
		}

		tokenizer Tokenizer = {};
		Tokenizer.Curr = (char*)FileResult.Contents;
		Tokenizer.End = (char*)(Tokenizer.Curr + FileResult.ContentsSize);


		int32 VerticesCount = 0;
		int32 NormalsCount = 0;
		int32 UVsCount = 0;

		// Count points. Could trade speed for memory complexity here
		while (true) {
			token Token = GetNextToken(&Tokenizer);
			if (Token.Type == token_type::eof) { break; }

			switch (Token.Type) {
				case (token_type::vertex): {
					VerticesCount++;
				} break;

				case (token_type::normal): {
					NormalsCount++;
				} break;

				case (token_type::uv): {
					UVsCount++;
				} break;

				case (token_type::face): {
					Obj->IndeciesCount += ArrayCount(Token.Face);
				} break;

				default: { } break;
			}
		}

		// Allocate
		Obj->Points = (v3*)ArenaAllocate(PermMem, sizeof(v3) * Obj->IndeciesCount);
		Obj->UVs = (v2*)ArenaAllocate(PermMem, sizeof(v2) * Obj->IndeciesCount);
		Obj->Indecies = (uint32*)ArenaAllocate(PermMem, sizeof(uint32) * Obj->IndeciesCount);
		Obj->Normals = (v3*)ArenaAllocate(PermMem, sizeof(v3) * Obj->IndeciesCount);
		Obj->NormalTans = (v3*)ArenaAllocate(PermMem, sizeof(v3) * Obj->IndeciesCount);
		Obj->NormalBiTans = (v3*)ArenaAllocate(PermMem, sizeof(v3) * Obj->IndeciesCount);

		Tokenizer.Curr = (char*)FileResult.Contents;


		v3* VerticesFile = (v3*)ArenaAllocate(TempMem, sizeof(v3) * VerticesCount);
		v3* NormalsFile = (v3*)ArenaAllocate(TempMem, sizeof(v3) * NormalsCount);
		v2* UVsFile = (v2*)ArenaAllocate(TempMem, sizeof(v2) * UVsCount);
		vector3* NormalsAccum = (vector3*)ArenaAllocate(TempMem, sizeof(vector3) * Obj->IndeciesCount);

		Obj->IndeciesCount = 0;
		int vi = 0;
		int vni = 0;
		int uvi = 0;

		// Get points
		// This assumes that faces come last in the file. Is that always safe?
		while (true) {
			token Token = GetNextToken(&Tokenizer);
			if (Token.Type == token_type::eof) { break; }

			switch (Token.Type) {

				case (token_type::comment): { } break;

				case (token_type::vertex): {
					VerticesFile[vi] = Token.Vertex;
					vi++;
				} break;

				case (token_type::normal): {
					NormalsFile[vni] = Token.Normal;
					vni++;
				} break;

				case (token_type::uv): {
					UVsFile[uvi] = Token.UV;
					uvi++;
				} break;

				case (token_type::face): {
					for (int x = 0; x < ArrayCount(Token.Face); x++) {
						Obj->Indecies[Obj->IndeciesCount] = Obj->IndeciesCount;

						int64 PointIndex = Token.Face[x].VertexIndex;
						int64 NormalIndex = Token.Face[x].NormalIndex;
						int64 UVIndex = Token.Face[x].TextureIndex;

						Obj->Points[Obj->IndeciesCount] = VerticesFile[PointIndex];

						Obj->Normals[Obj->IndeciesCount] = NormalsFile[NormalIndex];

						Obj->Normals[Obj->IndeciesCount].X = NormalsFile[NormalIndex].X;
						Obj->Normals[Obj->IndeciesCount].Y = NormalsFile[NormalIndex].Y;
						Obj->Normals[Obj->IndeciesCount].Z = NormalsFile[NormalIndex].Z;

						Obj->UVs[Obj->IndeciesCount] = UVsFile[UVIndex];

						Obj->IndeciesCount++;
					}
				} break;

				default: { } break;
			}
		}

		// Normalize normals
		for (int x = 0; x < Obj->IndeciesCount; x++) {
			vector3 n = Vector3Normalize(NormalsAccum[x]);
			//Ret.Normals[x] = v3{(real32)n.X, (real32)n.Y, (real32)n.Z};
		}

		Dae::CalcTangents(Obj);

		Obj->FileWriteTime = PlatformApi.GetFileWriteTime(FileDir.CharArray);

		if (AutoReload) {
			Obj->FileDir = FileDir;

			Assert(Assets->ModelPolling.Count < ArrayCount(Assets->ModelPolling.Models));
			Assets->ModelPolling.Models[Assets->ModelPolling.Count] = Obj;
			Assets->ModelPolling.Count++;
		}
	}
}
#endif