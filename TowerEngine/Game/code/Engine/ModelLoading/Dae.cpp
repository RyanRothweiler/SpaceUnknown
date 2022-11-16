#ifndef daecpp
#define daecpp

namespace Dae {

	struct attribute {
		string Name;
		str Value;
	};

	// Rename this. This actually represents a line. A set of opening and closing tags and the contents between.
	struct tag {
		bool32 Closing;

		// tag
		string Name;
		attribute Attributes[10];
		int64 AttributesCount;

		// Contents
		str Contents;
	};

	struct tokenizer  {
		char* Curr;
		char* End;
	};

	string GetAttributeValue(tag* Tag, string ID)
	{
		for (int a = 0; a < Tag->AttributesCount; a++) {
			attribute* Attr = &Tag->Attributes[a];
			if (Attr->Name == ID) {
				return Attr->Value.GetString();
			}
		}

		// Could not find that ID
		Assert(0);
		return "";
	}

	int32 GetAttributeValueInt(tag* Tag, string ID)
	{
		string str = GetAttributeValue(Tag, ID);
		return StringToInt32(str);
	}

	// Hard limit of no more than 200 objectgs in one file
	model* Models[200] = {};
	int32 ModelsCount = 0;

	// Could use some map here
	model* FindModel(string Url)
	{
		for (int m = 0; m < ModelsCount; m++) {
			string murl = "#" + Models[m]->URL;
			if (murl == Url) {
				return Models[m];
			}
		}

		// Could not find model
		Assert(false);
		return GameNull;
	}

	skin_controller* SkinControllers[200] = {};
	int32 SkinControllersCount = 0;

	tag GetNextTag(tokenizer* Tokenizer)
	{
		tag Tag = {};

		// Remove whitesapce
		while (IsWhitespace(Tokenizer->Curr[0])) { Tokenizer->Curr++; }

		if (Tokenizer->Curr[0] == '<') {
			Tokenizer->Curr++;

			if (Tokenizer->Curr[0] == '/') { Tag.Closing = true; }
			if (Tokenizer->Curr[0] == '?' || Tokenizer->Curr[0] == '/') { Tokenizer->Curr++; }

			// Get tag name
			char* NameStart = Tokenizer->Curr;
			uint32 NameCount = 0;
			while (Tokenizer->Curr[0] != ' ' && Tokenizer->Curr[0] != '>') {
				NameCount++;
				Tokenizer->Curr++;
			}
			Tag.Name = BuildString(NameStart, NameCount);

			// Get attributes here
			if (Tokenizer->Curr[0] != '>') {
				Tokenizer->Curr++;

				while (true) {
					attribute* Attr = &Tag.Attributes[Tag.AttributesCount];
					Tag.AttributesCount++;
					Assert(Tag.AttributesCount < ArrayCount(Tag.Attributes));

					// Get id
					{
						char* Start = Tokenizer->Curr;
						uint32 Count = 0;
						while (Tokenizer->Curr[0] != '=') {
							Count++;
							Tokenizer->Curr++;
						}
						Attr->Name = BuildString(Start, Count);
					}

					Tokenizer->Curr += 2;

					// Get value
					{
						Attr->Value.Data = Tokenizer->Curr;
						Attr->Value.Count = 0;
						while (Tokenizer->Curr[0] != '"') {
							Attr->Value.Count++;
							Tokenizer->Curr++;
						}
					}

					Tokenizer->Curr++;

					// Are we at the end?
					if (Tokenizer->Curr[0] == '?' || Tokenizer->Curr[0] == '>' || Tokenizer->Curr[0] == '/') { break; }

					// If not at end then move past empty space
					Tokenizer->Curr++;
				}
			}

			// Move to end of tag
			while (Tokenizer->Curr[0] != '>') { Tokenizer->Curr++; }
			Tokenizer->Curr++;
		} else {

			// Get contents
			if (Tokenizer->Curr[0] != '>') {
				Tag.Contents.Data = Tokenizer->Curr;
				Tag.Contents.Count = 0;
				while (Tokenizer->Curr[0] != '<') {
					Tag.Contents.Count++;
					Tokenizer->Curr++;
				}
			}
		}

		return Tag;
	}

	tag PeekNextTag(tokenizer* Tokenizer)
	{
		char* Before = Tokenizer->Curr;
		tag Tag = GetNextTag(Tokenizer);
		Tokenizer->Curr = Before;
		return Tag;
	}

	bool32 CheckForID(string IDEnding, attribute* Attr)
	{
		if (Attr->Name == "id") {

			int64 SourceLen = Attr->Value.Count;
			int64 EndLen = StringLength(IDEnding);

			if (SourceLen == 0 || EndLen == 0) { return false; }
			if (EndLen > SourceLen) { return false; }

			for (int x = 0; x < EndLen; x++) {
				char S = Attr->Value.Data[SourceLen - 1 - x];
				char En = IDEnding.CharArray[EndLen - 1 - x];
				if (S != En) {
					return false;
				}
			}

			return true;
		}

		return false;
	}

	int32 GetCount(tag* Tag, int32 IDIndex)
	{
		attribute* CountAttr = &Tag->Attributes[IDIndex + 1];
		Assert(CountAttr->Name == "count");

		string C = BuildString(CountAttr->Value.Data, (uint32)CountAttr->Value.Count);
		return StringToInt32(C);
	}

	struct numizer {
		char* Curr;
	};
	int32 PullNextNum(numizer* Numizer)
	{
		while (Numizer->Curr[0] == ' ' || Numizer->Curr[0] == '\n' || Numizer->Curr[0] == '\t') { Numizer->Curr++; }
		char* Start = Numizer->Curr;
		int32 Count = 0;
		while (Numizer->Curr[0] != ' ' && Numizer->Curr[0] != '<') {
			Numizer->Curr++;
			Count++;
		}

		string C = BuildString(Start, Count);
		return StringToInt32(C);
	}

	void ToArray(real64* Dest, char* Input, int32 InputCount)
	{
		for (int x = 0; x < InputCount; x++) {

			while (Input[0] == ' ' || Input[0] == '\n' || Input[0] == '\t') { Input++; }

			char* Start = Input;
			int32 Count = 0;

			bool32 sci = false;
			while (Input[0] != ' ' && Input[0] != '<') {
				Input++;
				Count++;

				// Any scientific notation is probably just basically 0 anyways.
				if (Input[0] == 'e' && Input[1] == '-') {
					Dest[x] = 0;
					sci = true;
				}
			}

			if (!sci) {
				string C = BuildString(Start, Count);
				Dest[x] = StringToReal64(C);
			}

		}
	}

	m4y4 Pull4y4(char* Start)
	{
		real64 FileMat[16];
		ToArray(&FileMat[0], Start, 16);

		m4y4 Result = {};
		int x = 0;
		int y = 0;
		for (int i = 0; i < 16; i++) {

			Result.E[y][x] = (real32)FileMat[i];

			x++;
			if (x % 4 == 0) {
				x = 0;
				y++;
			}
		}

		return Result;
	}

	void Pull4y4Array(m4y4* Dest, int32 NumbersCount, tag* DataTag, memory_arena* Memory)
	{
		int32 MatriciesCount = NumbersCount / 16;

		real64* Data = (real64*)ArenaAllocate(Memory, NumbersCount * sizeof(*Data));
		ToArray(&Data[0], DataTag->Contents.Data, NumbersCount);

		for (int m = 0; m < MatriciesCount; m++) {

			int x = 0;
			int y = 0;
			for (int i = 0; i < 16; i++) {
				Dest[m].E[y][x] = (real32)Data[(m * 16) + i];

				x++;
				if (x % 4 == 0) {
					x = 0;
					y++;
				}
			}
		}
	}

	bool32 TagIsBone(tag* Tag)
	{
		if (Tag->Closing) { return false; }
		if (Tag->Name != "node") { return false; }

		string type = GetAttributeValue(Tag, "type");
		return type == "JOINT";
	}

	void SkipToClosing(string Name, tokenizer* Tokenizer)
	{
		tag Next = GetNextTag(Tokenizer);
		while (true) {
			if (Next.Closing && Next.Name == Name) {
				break;
			}
			Next = GetNextTag(Tokenizer);
		}
	}


	tag SkipToOpening(string Name, tokenizer* Tokenizer)
	{
		tag Next = GetNextTag(Tokenizer);
		while (true) {
			if (!Next.Closing && Next.Name == Name) {
				return Next;
			}
			Next = GetNextTag(Tokenizer);
		}
	}

	// We only support one armature at this time
	armature* TargetArmature;

	void LoadBoneNode(bone* Dest, tokenizer* Tokenizer)
	{
		tag BoneNode = GetNextTag(Tokenizer);

		if (TagIsBone(&BoneNode)) {

			TargetArmature->BonesCount++;

			string sid = GetAttributeValue(&BoneNode, "sid");

			Dest->ID = GetAttributeValue(&BoneNode, "id");
			Dest->SID = sid;
			Dest->Name = GetAttributeValue(&BoneNode, "name");

			Dest->TipPos = {};
			Dest->Leaf = false;

			tag MatrixOpening = GetNextTag(Tokenizer);
			tag MatrixData = GetNextTag(Tokenizer);
			tag MatrixClosing = GetNextTag(Tokenizer);

			// Put matrix info into dest
			Dest->BindTrans = Pull4y4(MatrixData.Contents.Data);

			// This assumes a blender profile
			tag Extra = PeekNextTag(Tokenizer);
			if (Extra.Name == "extra") {
				// extra data that blender will add. Keep this as its useful for debugging sometimes.
				// Needs work, the data in extra isn't always the same.
				/*
				tag ExtraOpening = GetNextTag(Tokenizer);

				tag TechniqueOpening = GetNextTag(Tokenizer);

				tag ConnectOpening = GetNextTag(Tokenizer);
				tag ConnectData = GetNextTag(Tokenizer);
				tag ConnectClosing = GetNextTag(Tokenizer);

				tag LayerOpening = GetNextTag(Tokenizer);
				tag LayerData = GetNextTag(Tokenizer);
				tag LayerClosing = GetNextTag(Tokenizer);

				if (PeekNextTag(Tokenizer).Name == "roll") {
					tag RollOpening = GetNextTag(Tokenizer);
					tag RollData = GetNextTag(Tokenizer);
					tag RollClosing = GetNextTag(Tokenizer);
				}

				tag TipXOpening = GetNextTag(Tokenizer);
				tag TipXData = GetNextTag(Tokenizer);
				tag TipXClosing = GetNextTag(Tokenizer);

				tag TipYOpening = GetNextTag(Tokenizer);
				tag TipYData = GetNextTag(Tokenizer);
				tag TipYClosing = GetNextTag(Tokenizer);

				tag TipZOpening = GetNextTag(Tokenizer);
				tag TipZData = GetNextTag(Tokenizer);
				tag TipZClosing = GetNextTag(Tokenizer);

				tag TechniqueClosing = GetNextTag(Tokenizer);

				tag ExtraClosing = GetNextTag(Tokenizer);

				// save the extra stuff
				Dest->Leaf = true;
				Dest->TipPos.X = StringToReal64(BuildString(TipXData.Contents.Data, (uint32)TipXData.Contents.Count));
				Dest->TipPos.Y = StringToReal64(BuildString(TipYData.Contents.Data, (uint32)TipYData.Contents.Count));
				Dest->TipPos.Z = StringToReal64(BuildString(TipZData.Contents.Data, (uint32)TipZData.Contents.Count));
				*/

				SkipToClosing("extra", Tokenizer);
			}

			while (true) {
				tag NT = PeekNextTag(Tokenizer);

				if (TagIsBone(&NT)) {
					// Allocate bone
					Dest->Children[Dest->ChildrenCount] = (bone*)ArenaAllocate(GlobalPermMem, sizeof(bone));
					Dest->ChildrenCount++;
					Assert(Dest->ChildrenCount < ArrayCount(Dest->Children));

					// Get next bone
					LoadBoneNode(Dest->Children[Dest->ChildrenCount - 1], Tokenizer);

				} else {
					break;
				}
			}

			// Move to closing node
			SkipToClosing("node", Tokenizer);
		}
	}

	void LoadBoneAnim(armature* Armature, tokenizer* Tokenizer, memory_arena* TransMem)
	{
		tag AnimationOpen = GetNextTag(Tokenizer);
		string ArmName = GetAttributeValue(&AnimationOpen, "name");

		bone_anim* Bone = &Armature->BoneAnims[Armature->BoneAnimsCount++];

		tag SourceAnim = GetNextTag(Tokenizer);

		real64* Times;
		m4y4* Trans;

		// Get keyframe times
		{
			tag FloatOpen = GetNextTag(Tokenizer);
			Bone->KeyframesCount = GetAttributeValueInt(&FloatOpen, "count");

			tag KeyframeTimes = GetNextTag(Tokenizer);

			Bone->Keyframes = (keyframe*)ArenaAllocate(GlobalPermMem, Bone->KeyframesCount * sizeof(keyframe));

			Times = (real64*)ArenaAllocate(TransMem, Bone->KeyframesCount * sizeof(real64));
			ToArray(&Times[0], KeyframeTimes.Contents.Data, Bone->KeyframesCount);

			// Get anim length
			for (int i = 0; i < Bone->KeyframesCount; i++) {
				if (Times[i] > Armature->LengthSeconds) {
					Armature->LengthSeconds = Times[i];
				}
			}
		}

		// Get transforms
		{
			tag Source = SkipToOpening("source", Tokenizer);
			tag FloatOpen = GetNextTag(Tokenizer);
			int32 Count = GetAttributeValueInt(&FloatOpen, "count");

			tag MatrixData = GetNextTag(Tokenizer);

			Trans = (m4y4*)ArenaAllocate(TransMem, Bone->KeyframesCount * sizeof(m4y4));

			Pull4y4Array(Trans, Count, &MatrixData, TransMem);
		}

		// Fill keyframes
		for (int x = 0; x < Bone->KeyframesCount; x++) {
			keyframe* Key = &Bone->Keyframes[x];
			Key->Time = Times[x];
			Key->Trans = Trans[x];
		}

		// Get target bone
		//NAME_BONETARGET/transform
		{
			tag Channel = SkipToOpening("channel", Tokenizer);
			string Path = GetAttributeValue(&Channel, "target");

			int Start = StringLength(ArmName);
			int Length = 0;
			for (int x = Start; x < StringLength(Path); x++) {
				if (Path.CharArray[x] == '/') { break; }
				Length++;
			}

			Bone->Target = BuildString(&Path.CharArray[Start + 1], Length - 1);
		}

		SkipToClosing("animation", Tokenizer);
	}

	void CalcTangents(model* Model)
	{
		// Calc normal tangent and bitangents
		for (int x = 0; x < Model->IndeciesCount; x += 3) {
			v3 Normal = Model->Normals[x];

			v3 PointOne = Model->Points[x];
			v3 PointTwo = Model->Points[x + 1];
			v3 PointThree = Model->Points[x + 2];

			v2 UVOne = Model->UVs[x];
			v2 UVTwo = Model->UVs[x + 1];
			v2 UVThree = Model->UVs[x + 2];

			v3 EdgeOne = PointTwo - PointOne;
			v3 EdgeTwo = PointThree - PointOne;
			v2 UVDeltaOne = UVTwo - UVOne;
			v2 UVDeltaTwo = UVThree - UVOne;

			real32 f = 1.0f / ((UVDeltaOne.X * UVDeltaTwo.Y) - (UVDeltaOne.Y * UVDeltaTwo.X));

			v3 Tan = {};
			v3 BiTan = {};

			Tan.X = f * ((UVDeltaTwo.Y * EdgeOne.X) - (UVDeltaOne.Y * EdgeTwo.X));
			Tan.Y = f * ((UVDeltaTwo.Y * EdgeOne.Y) - (UVDeltaOne.Y * EdgeTwo.Y));
			Tan.Z = f * ((UVDeltaTwo.Y * EdgeOne.Z) - (UVDeltaOne.Y * EdgeTwo.Z));

			BiTan.X = f * ((UVDeltaOne.X * EdgeTwo.X) - (UVDeltaTwo.X * EdgeOne.X));
			BiTan.Y = f * ((UVDeltaOne.X * EdgeTwo.Y) - (UVDeltaTwo.X * EdgeOne.Y));
			BiTan.Z = f * ((UVDeltaOne.X * EdgeTwo.Z) - (UVDeltaTwo.X * EdgeOne.Z));

			Model->NormalTans[x] = Tan;
			Model->NormalTans[x + 1] = Tan;
			Model->NormalTans[x + 2] = Tan;
			Model->NormalBiTans[x] = BiTan;
			Model->NormalBiTans[x + 1] = BiTan;
			Model->NormalBiTans[x + 2] = BiTan;
		}
	}

	void LoadGeometry(model* Model, tokenizer* Tokenizer, memory_arena* TransMem)
	{
		// NOTE these counts are elements. packed. so each vertex will have 3 elements. x,y,z. A mesh with one vertex will have a vertcount of 3
		int32 UVsCount = 0;
		int32 VertsCount = 0;
		int32 NormalsCount = 0;

		// Mesh
		tag NormalsTag = {};
		tag VertsTag = {};
		tag UVsTag = {};
		tag IndeciesTag = {};

		int32 PointsOffset = 0;
		int32 UVsOffset = 0;
		int32 NormalsOffset = 0;
		int32 OffsetsCount = 0;

		int32 IndeciesCount = 0;

		// Walk the xml tree
		while (true) {
			tag Tag = GetNextTag(Tokenizer);

			for (int a = 0; a < Tag.AttributesCount; a++) {
				attribute* Attr = &Tag.Attributes[a];

				if (CheckForID("mesh-positions-array", Attr)) {
					VertsCount = GetCount(&Tag, a);
					VertsTag = GetNextTag(Tokenizer);
				}
				if (CheckForID("mesh-normals-array", Attr)) {
					NormalsCount = GetCount(&Tag, a);
					NormalsTag = GetNextTag(Tokenizer);
				}
				if (CheckForID("mesh-map-0-array", Attr)) {
					UVsCount = GetCount(&Tag, a);
					UVsTag = GetNextTag(Tokenizer);
				}
			}

			if (!Tag.Closing && Tag.Name == "triangles") {

				// This count is number of triangles, so 3 points per triangle
				IndeciesCount = GetAttributeValueInt(&Tag, "count") * 3;

				while (true) {

					tag Next = GetNextTag(Tokenizer);

					if (Next.Name == "p") {
						break;
					} else if (Next.Name == "input")  {
						string Semantic = GetAttributeValue(&Next, "semantic");
						int32 Offset = GetAttributeValueInt(&Next, "offset");

						if (Semantic == "VERTEX") {
							PointsOffset = Offset;
						} else if (Semantic == "TEXCOORD") {
							UVsOffset = Offset;
						} else if (Semantic == "NORMAL") {
							NormalsOffset = Offset;
						}

						OffsetsCount++;
					}
				}

				IndeciesTag = GetNextTag(Tokenizer);
			}

			if (Tag.Closing && Tag.Name == "geometry") {  break; }
		}


		// Assemble data

		Model->AABB = {};

		Model->IndeciesCount = IndeciesCount;
		Model->Points = (v3*)ArenaAllocate(GlobalPermMem, IndeciesCount * sizeof(*Model->Points));
		Model->UVs = (v2*)ArenaAllocate(GlobalPermMem, IndeciesCount * sizeof(*Model->UVs));
		Model->Normals = (v3*)ArenaAllocate(GlobalPermMem, IndeciesCount * sizeof(*Model->Normals));
		Model->NormalTans = (v3*)ArenaAllocate(GlobalPermMem, IndeciesCount * sizeof(*Model->NormalTans));
		Model->NormalBiTans = (v3*)ArenaAllocate(GlobalPermMem, IndeciesCount * sizeof(*Model->NormalBiTans));
		Model->Indecies = (uint32*)ArenaAllocate(GlobalPermMem, IndeciesCount * sizeof(*Model->Indecies));
		Model->IndeciesFromFile = (int32*)ArenaAllocate(GlobalPermMem, IndeciesCount * sizeof(*Model->IndeciesFromFile));

		//Model->Armature.FinalBoneTransforms = (m4y4*)ArenaAllocate(&Memory->PermanentMemory, Model->Armature.BonesCount * sizeof(*Model->Armature.FinalBoneTransforms));

		//Model->BoneWeights = (v3*)ArenaAllocate(&Memory->PermanentMemory, IndeciesCount * sizeof(*Model->BoneWeights));
		//Model->BoneIndecies = (v3*)ArenaAllocate(&Memory->PermanentMemory, IndeciesCount * sizeof(*Model->BoneIndecies));

		real64* FilePoints = (real64*)ArenaAllocate(TransMem, VertsCount * sizeof(real64));
		ToArray(FilePoints, VertsTag.Contents.Data, VertsCount);

		real64* FileNormals = (real64*)ArenaAllocate(TransMem, NormalsCount * sizeof(real64));
		ToArray(FileNormals, NormalsTag.Contents.Data, NormalsCount);

		real64* FileUVs = (real64*)ArenaAllocate(TransMem, UVsCount * sizeof(real64));
		ToArray(FileUVs, UVsTag.Contents.Data, UVsCount);

		// Handle model info
		{
			numizer N = {};
			N.Curr = IndeciesTag.Contents.Data;

			int i = 0;
			while (true) {

				for (int x = 0; x < OffsetsCount; x++) {

					int NextTriNum = PullNextNum(&N);

					if (x == PointsOffset) {
						int viFile = 3 * NextTriNum;
						Model->Points[i] = v3{(real32)FilePoints[viFile], (real32)FilePoints[viFile + 1], (real32)FilePoints[viFile + 2]};
						Model->IndeciesFromFile[i] = NextTriNum;
					} else if (x == UVsOffset) {
						int uvi = 2 * NextTriNum;
						Model->UVs[i] = v2{(real32)FileUVs[uvi], (real32)FileUVs[uvi + 1]};
					} else if (x == NormalsOffset) {
						int ni = 3 * NextTriNum;
						Model->Normals[i] = v3{(real32)FileNormals[ni], (real32)FileNormals[ni + 1], (real32)FileNormals[ni + 2]};
					}

					// Update AABB
					if (Model->Points[i].X < Model->AABB.XMin) {
						Model->AABB.XMin = Model->Points[i].X;
					}
					if (Model->Points[i].X > Model->AABB.XMax) {
						Model->AABB.XMax = Model->Points[i].X;
					}
					if (Model->Points[i].Y < Model->AABB.YMin) {
						Model->AABB.YMin = Model->Points[i].Y;
					}
					if (Model->Points[i].Y > Model->AABB.YMax) {
						Model->AABB.YMax = Model->Points[i].Y;
					}
					if (Model->Points[i].Z < Model->AABB.ZMin) {
						Model->AABB.ZMin = Model->Points[i].Z;
					}
					if (Model->Points[i].Z > Model->AABB.ZMax) {
						Model->AABB.ZMax = Model->Points[i].Z;
					}

					// Update bounding radius
					if (Abs(Model->Points[i].X) > Model->BoundingRadius) {
						Model->BoundingRadius = (real32)Abs(Model->Points[i].X);
					}
					if (Abs(Model->Points[i].Y) > Model->BoundingRadius) {
						Model->BoundingRadius = (real32)Abs(Model->Points[i].Y);
					}
					if (Abs(Model->Points[i].Z) > Model->BoundingRadius) {
						Model->BoundingRadius = (real32)Abs(Model->Points[i].Z);
					}
				}

				Model->Indecies[i] = i;
				i++;

				if (N.Curr[0] == '<') {
					break;
				}
			}
		}

		CalcTangents(Model);
	}

	void LoadNode(entity* Parent, tokenizer* Tokenizer)
	{
		entity* NewEntity = (entity*)ArenaAllocate(GlobalPermMem, sizeof(entity));
		Parent->AddChild(NewEntity);

		// !!! this is pretty suspect. Only works because we only support one armature.
		NewEntity->Armature = TargetArmature;

		tag Node = GetNextTag(Tokenizer);
		tag MatrixOpening = GetNextTag(Tokenizer);
		tag MatrixData = GetNextTag(Tokenizer);

		m4y4 Trans = Pull4y4(MatrixData.Contents.Data);
		NewEntity->Transform.LocalPos = Trans.GetTranslation();

		quat Q = {};
		Q.FromMat(&Trans);
		NewEntity->Transform.LocalRot = Q.ToEuler();

		tag MatrixClosing = GetNextTag(Tokenizer);

		while (true) {

			tag Next = PeekNextTag(Tokenizer);
			if (Next.Name == "node") {
				if (!Next.Closing) {

					string NodeType = GetAttributeValue(&Next, "type");
					if (NodeType == "NODE") {
						LoadNode(NewEntity, Tokenizer);

					} else if (NodeType == "JOINT") {
						LoadBoneNode(&TargetArmature->RootBone, Tokenizer);

					} else {
						// Unknown node type
						Assert(false);
					}

				} else {
					// Consume the tag
					GetNextTag(Tokenizer);
					break;
				}
			} else if (!Next.Closing && Next.Name == "instance_controller") {

				// Consume the tag
				tag InstanceControllerOpen = GetNextTag(Tokenizer);
				tag SkeletonOpen = GetNextTag(Tokenizer);
				tag SkeletonData = GetNextTag(Tokenizer);
				tag SkeletonClose = GetNextTag(Tokenizer);
				tag InstanceControllerClose = GetNextTag(Tokenizer);

				string SkinUrl = GetAttributeValue(&InstanceControllerOpen, "url");

				// If we want to support multiple armatures then we would use the BonesUrl here to find the correct one.
				// Assume parent has the armature. Will this always be true?
				string BonesUrl = SkeletonData.Contents.GetString();
				Assert(BonesUrl == "#" + TargetArmature->RootBone.ID);

				for (int m = 0; m < SkinControllersCount; m++) {
					string murl = "#" + SkinControllers[m]->URL;
					if (murl == SkinUrl) {
						NewEntity->SkinController = *SkinControllers[m];
						m = SkinControllersCount + 1;
					}
				}

				// Assign mesh
				NewEntity->Model = FindModel(NewEntity->SkinController.MeshUrl);

			} else if (!Next.Closing && Next.Name == "instance_geometry") {

				// Consume the tag
				GetNextTag(Tokenizer);

				NewEntity->Model = FindModel(GetAttributeValue(&Next, "url"));
			} else {
				// Consume the peeked tag
				GetNextTag(Tokenizer);
			}
		}
	}

	/*
	NOTE This is not a general purpose loader. Some catches
		-  object must be named "Armature"
	*/
	void Load(entity* Entity, string File, bool32 AutoReload, memory_arena* TransMem)
	{

		// Reset some global state
		ModelsCount = 0;
		SkinControllersCount = 0;

		Entity->AABBLocal = {};
		Entity->BoundingRadius = {};

		Entity->Armature = (armature*)ArenaAllocate(GlobalPermMem, sizeof(armature));
		TargetArmature = Entity->Armature;
		TargetArmature->BoneAnimsCount = 0;

		read_file_result FileResult = {};
		FileResult = PlatformApi.ReadFile(&File.CharArray[0], TransMem);
		if (FileResult.ContentsSize == 0) {
			ConsoleLog("Could not find file " + string{File});
			return;
		}

		tokenizer Tokenizer = {};
		Tokenizer.Curr = (char*)FileResult.Contents;
		Tokenizer.End = (char*)(Tokenizer.Curr + FileResult.ContentsSize);


		while (true) {
			tag Tag = GetNextTag(&Tokenizer);

			// Load animations
			if (Tag.Name == "library_animations" && !Tag.Closing) {

				// Limit of 50 bones
				TargetArmature->BoneAnims = (bone_anim*)ArenaAllocate(GlobalPermMem, 50 * sizeof(bone_anim));

				tag Armatures = GetNextTag(&Tokenizer);

				tag Next = PeekNextTag(&Tokenizer);
				while (Next.Name == "animation" && !Next.Closing) {
					LoadBoneAnim(TargetArmature, &Tokenizer, TransMem);
					Next = PeekNextTag(&Tokenizer);
				}
			}

			// Load geometries
			if (Tag.Name == "library_geometries" && !Tag.Closing) {

				tag Next = GetNextTag(&Tokenizer);
				while (Next.Name == "geometry" && !Next.Closing) {

					Models[ModelsCount] = (model*)ArenaAllocate(GlobalPermMem, sizeof(model));

					Models[ModelsCount]->URL = GetAttributeValue(&Next, "id");
					LoadGeometry(Models[ModelsCount], &Tokenizer, TransMem);

					// Update entity aabb
					if (Models[ModelsCount]->AABB.XMax > Entity->AABBLocal.XMax) {
						Entity->AABBLocal.XMax = Models[ModelsCount]->AABB.XMax;
					}
					if (Models[ModelsCount]->AABB.XMin < Entity->AABBLocal.XMin) {
						Entity->AABBLocal.XMin = Models[ModelsCount]->AABB.XMin;
					}
					if (Models[ModelsCount]->AABB.YMax > Entity->AABBLocal.YMax) {
						Entity->AABBLocal.YMax = Models[ModelsCount]->AABB.YMax;
					}
					if (Models[ModelsCount]->AABB.YMin < Entity->AABBLocal.YMin) {
						Entity->AABBLocal.YMin = Models[ModelsCount]->AABB.YMin;
					}
					if (Models[ModelsCount]->AABB.ZMax > Entity->AABBLocal.ZMax) {
						Entity->AABBLocal.ZMax = Models[ModelsCount]->AABB.ZMax;
					}
					if (Models[ModelsCount]->AABB.ZMin < Entity->AABBLocal.ZMin) {
						Entity->AABBLocal.ZMin = Models[ModelsCount]->AABB.ZMin;
					}

					// Update entity bounding radius
					if (Models[ModelsCount]->BoundingRadius > Entity->BoundingRadius) {
						Entity->BoundingRadius = Models[ModelsCount]->BoundingRadius;
					}

					ModelsCount++;
					Assert(ModelsCount < ArrayCount(Models));

					Next = GetNextTag(&Tokenizer);
				}
			}

			if (Tag.Name == "library_visual_scenes" && !Tag.Closing) {

				// Assume only one scene
				tag SceneOpening = GetNextTag(&Tokenizer);

				tag Next = PeekNextTag(&Tokenizer);
				while (Next.Name == "node") {
					LoadNode(Entity, &Tokenizer);
					Next = PeekNextTag(&Tokenizer);
				}

				tag SceneClosing = GetNextTag(&Tokenizer);
			}

			// Load Skin
			if (Tag.Name == "library_controllers" && !Tag.Closing) {

				tag Next = PeekNextTag(&Tokenizer);
				while (Next.Name == "controller") {

					real64* WeightsList = {};
					int32 JointsCountLength = 0; // length of array
					real64* JointsCount = {}; // real64, but these are actually all just ints
					real64* WeightsIndecies = {}; // real64, but these are actually all just ints


					SkinControllers[SkinControllersCount] = (skin_controller*)ArenaAllocate(GlobalPermMem, sizeof(skin_controller));
					skin_controller* SkinCon = SkinControllers[SkinControllersCount];
					SkinControllersCount++;

					tag Controller = GetNextTag(&Tokenizer);
					tag Skin = GetNextTag(&Tokenizer);

					SkinCon->URL = GetAttributeValue(&Controller, "id");

					// Find geometry
					SkinCon->MeshUrl = GetAttributeValue(&Skin, "source");

					tag BoneOrderOpening = SkipToOpening("Name_array", &Tokenizer);
					SkinCon->BonesCount = GetAttributeValueInt(&BoneOrderOpening, "count");

					tag BoneOrder = GetNextTag(&Tokenizer);

					SkinCon->BoneNameList.Data = (char*)ArenaAllocate(GlobalPermMem, BoneOrder.Contents.Count * sizeof(char));
					MemoryCopy(SkinCon->BoneNameList.Data, BoneOrder.Contents.Data, BoneOrder.Contents.Count);
					SkinCon->BoneNameList.Count = BoneOrder.Contents.Count;

					// skip skin joints
					SkipToClosing("source", &Tokenizer);

					// Get inverse bind transforms
					{
						tag SourceOpen = GetNextTag(&Tokenizer);
						tag FloatOpen = GetNextTag(&Tokenizer);
						int Count = GetAttributeValueInt(&FloatOpen, "count");

						tag Transforms = GetNextTag(&Tokenizer);
						SkinCon->InvBindTrans = (m4y4*)ArenaAllocate(GlobalPermMem, SkinCon->BonesCount * sizeof(*SkinCon->InvBindTrans));
						Pull4y4Array(SkinCon->InvBindTrans, Count, &Transforms, TransMem);
						SkipToClosing("source", &Tokenizer);
					}

					SkinCon->FinalBoneTransforms = (m4y4*)ArenaAllocate(GlobalPermMem, SkinCon->BonesCount * sizeof(*SkinCon->FinalBoneTransforms));

					// Get weights
					{
						tag SourceOpen = GetNextTag(&Tokenizer);
						tag FloatOpen = GetNextTag(&Tokenizer);

						int32 WeightsCount = GetAttributeValueInt(&FloatOpen, "count");
						WeightsList = (real64*)ArenaAllocate(TransMem, WeightsCount * sizeof(real64));

						tag WeightsTag = GetNextTag(&Tokenizer);

						ToArray(WeightsList, WeightsTag.Contents.Data, WeightsCount);

						// skip to end
						SkipToClosing("source", &Tokenizer);
					}

					SkipToClosing("joints", &Tokenizer);

					// Get weights indecies
					{
						tag VertexWeightsOpening = GetNextTag(&Tokenizer);
						JointsCountLength = GetAttributeValueInt(&VertexWeightsOpening, "count");
						JointsCount = (real64*)ArenaAllocate(TransMem, JointsCountLength * sizeof(real64));

						tag inputJoint = GetNextTag(&Tokenizer);
						tag inputWeight = GetNextTag(&Tokenizer);
						tag vcountOpen = GetNextTag(&Tokenizer);

						tag JCount = GetNextTag(&Tokenizer);
						ToArray(JointsCount, JCount.Contents.Data, JointsCountLength);

						tag vcountClose = GetNextTag(&Tokenizer);
						tag vOpen = GetNextTag(&Tokenizer);

						// Weights indecies
						tag WeightsIndeciesTag = GetNextTag(&Tokenizer);
						int32 WeightsIndeciesCount = 0;
						for (int x = 0; x < JointsCountLength; x++) {
							WeightsIndeciesCount += (int32)JointsCount[x];
						}
						WeightsIndeciesCount = WeightsIndeciesCount * 2;

						WeightsIndecies = (real64*)ArenaAllocate(TransMem, WeightsIndeciesCount * sizeof(real64));
						ToArray(WeightsIndecies, WeightsIndeciesTag.Contents.Data, WeightsIndeciesCount);

						tag vClose = GetNextTag(&Tokenizer);
						tag VertexWeightsClosing = GetNextTag(&Tokenizer);
					}

					// Assemble bone weights
					v3* FileWeights = (v3*)ArenaAllocate(GlobalPermMem, JointsCountLength * sizeof(v3));
					v3* FileWeightIndecies = (v3*)ArenaAllocate(GlobalPermMem, JointsCountLength * sizeof(v3));
					{
						int CurrWeightsIndex = 0;
						for (int v = 0; v < JointsCountLength; v++) {

							int32 JCount = (int32)JointsCount[v];

							real64* WeightsSorted = (real64*)ArenaAllocate(TransMem, JCount * sizeof(real64));
							int32* IndeciesSorted = (int32*)ArenaAllocate(TransMem, JCount * sizeof(int32));

							ClearMemory((uint8*)(WeightsSorted), JCount * sizeof(*WeightsSorted));
							ClearMemory((uint8*)(IndeciesSorted), JCount * sizeof(*IndeciesSorted));

							for (int j = 0; j < JCount; j++) {
								// Get data
								int32 BoneNum = (int32)WeightsIndecies[CurrWeightsIndex];
								int32 WeightIndex = (int32)WeightsIndecies[CurrWeightsIndex + 1];
								real64 Weight = WeightsList[WeightIndex];
								CurrWeightsIndex += 2;

								// Insert into sorted list
								for (int s = 0; s < JCount; s++) {
									if (WeightsSorted[s] < Weight) {

										for (int r = JCount - 1; r > s; r--) {
											WeightsSorted[r] = WeightsSorted[r - 1];
											IndeciesSorted[r] = IndeciesSorted[r - 1];
										}

										WeightsSorted[s] = Weight;
										IndeciesSorted[s] = BoneNum;
										break;
									}
								}
							}

							// Use top 3 weights, they must equal 1
							real64 Tot = WeightsSorted[0] + WeightsSorted[1] + WeightsSorted[2];
							FileWeights[v] = v3{
								(real32)(WeightsSorted[0] / Tot),
								(real32)(WeightsSorted[1] / Tot),
								(real32)(WeightsSorted[2] / Tot)
							};
							FileWeightIndecies[v] = v3{
								(real32)IndeciesSorted[0],
								(real32)IndeciesSorted[1],
								(real32)IndeciesSorted[2]
							};
						}
					}

					// Assemble vertex info
					{
						model* Model = FindModel(SkinCon->MeshUrl);

						// Assemble data
						Assert(Model->IndeciesCount != 0);

						SkinCon->BoneWeights = (v3*)ArenaAllocate(GlobalPermMem, Model->IndeciesCount * sizeof(*SkinCon->BoneWeights));
						SkinCon->BoneIndecies = (v3*)ArenaAllocate(GlobalPermMem, Model->IndeciesCount * sizeof(*SkinCon->BoneIndecies));

						for (int i = 0; i < Model->IndeciesCount; i++) {
							int32 vi = Model->IndeciesFromFile[i];

							SkinCon->BoneWeights[i] = FileWeights[vi];
							SkinCon->BoneIndecies[i] = FileWeightIndecies[vi];
						}
					}
					SkipToClosing("controller", &Tokenizer);

					Next = PeekNextTag(&Tokenizer);
				}
			}

			if (Tag.Closing && Tag.Name == "COLLADA") {  break; }
		}

		// Link bone anims
		for (int x = 0; x < TargetArmature->BoneAnimsCount; x++) {
			if (TargetArmature->BoneAnims[x].KeyframesCount > 0) {
				bone_anim* Bone = &TargetArmature->BoneAnims[x];
				Bone->Bone = TargetArmature->FindBone(&Bone->Target);
				Assert(Bone->Bone != GameNull);
			}
		}
	}
}

#endif