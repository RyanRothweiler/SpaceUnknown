
#define Assert(Expression) if (!(Expression)) {*(int *)0 = 0;}

#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>

#define GameNull 0

#include "Engine/Types.cpp"
#include "Engine/MemoryManager.h"
#include "Engine/MemoryManager.cpp"
#include "Engine/String.cpp"

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

struct read_file_result {
	uint32 ContentsSize;
	void *Contents;
};


// This is recursive and will include all child files also.
// Returns the end of the list
struct path_list {
	string Path;
	path_list* Next;

	int32 GetCount()
	{
		int32 Count = 0;

		if (Next != GameNull) {
			Count++;
			path_list* P = Next;

			while (P->Next != GameNull) {
				Count++;
				P = P->Next;
			}
		}

		return Count;
	}
};
path_list* GetPathsForFileType(string FileType, string Root, memory_arena* Memory, path_list* PathList)
{
	path_list* NextPath = PathList;

	HANDLE hFind = {};
	WIN32_FIND_DATA FileData = {};

	string Path = Root + "*";
	hFind = FindFirstFileA(Path.CharArray, &FileData);

	do {
		if ((FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && FileData.cFileName[0] != '.') {
			string P = Root + FileData.cFileName + "/";
			NextPath = GetPathsForFileType(FileType, P, Memory, NextPath);
		} else {
			if (StringEndsWith(FileData.cFileName, FileType)) {
				NextPath->Path = Root + FileData.cFileName;
				NextPath->Next = (path_list*)ArenaAllocate(Memory, sizeof(path_list));
				ClearMemory((uint8*)NextPath->Next, sizeof(path_list));
				NextPath = NextPath->Next;
			}

		}
	} while (FindNextFileA(hFind, &FileData) != 0);

	FindClose(hFind);
	return NextPath;
}

read_file_result
LoadFileData(char *FileName)
{
	read_file_result Result = {};

	HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (FileHandle != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER FileSize;
		if (GetFileSizeEx(FileHandle, &FileSize)) {
			uint32 FileSize32 = (uint32)FileSize.QuadPart;
			Result.Contents = malloc(FileSize32);
			// Result.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (Result.Contents) {
				DWORD BytesRead;
				if (ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0)) {
					// File read successfully
					Result.ContentsSize = FileSize32;
				} else {
					VirtualFree(Result.Contents, 0, MEM_RELEASE);
					Result.Contents = 0;
				}

				//Add null terimator
				char* Contents = (char*)Result.Contents;
				Contents[FileSize32] = NULL;

			} else {

			}
		} else {

		}

		CloseHandle(FileHandle);
	} else {

	}

	return (Result);
}

enum token_type {
	Token_MetaStruct,
	Token_MetaMethod,
	// TODO add Token_MetaEnum to use the code in MetaStruct

	Token_Struct,
	Token_Identifier,
	Token_Number,
	Token_OpenParen,
	Token_ClosedParen,
	Token_OpenCurly,
	Token_ClosedCurly,
	Token_SemiColon,
	Token_Colon,
	Token_String,
	Token_Star,
	Token_Comma,
	Token_OpenSquare,
	Token_ClosedSquare,
	Token_Class,
	Token_Enum,
	Token_Unknown,
};

struct token {
	token_type Type;
	char* Contents;
	uint32 ContentsLength;

	char* Extra;
	uint32 ExtraLength;
};

struct tokenizer {
	char* Current;
	char* End;
};

struct meta_method_parameter {
	token Type;
	bool32 IsPointer;
	token ID;
};

struct meta_method_info {
	token MethodName;

	token ReturnType;
	bool32 ReturnTypeIsPointer;

	meta_method_parameter Params[10];
	int32 ParamsCount;
};

// Just a lazy/stupid hash map
struct method_signature {
	meta_method_info Methods[100];
	uint32 MethodsCount;

	token SignatureID;
};

std::vector<std::string> PrimitiveTypes = {
	"uint32", "uint16", "uint8",
	"int32", "int16", "int8", "int64",
	"real32", "real64",

	// not a real primitive, but kinda a primitive for our purposes. For custom types like struct / class
	"custom",
};

//std::vector<std::string> StructTypes;

method_signature SignatureTable[100];
int32 SignatureTableCount;

meta_method_info* SignatureGetNewMethod(token* SignatureID)
{
	method_signature* Sig = NULL;

	for (int x = 0; x < SignatureTableCount; x++) {
		if (SignatureTable[x].SignatureID.ExtraLength == SignatureID->ExtraLength) {
			bool32 Valid = true;
			for (uint32 i = 0; i < SignatureID->ExtraLength; i++) {
				if (SignatureID->Extra[i] != SignatureTable[x].SignatureID.Extra[i]) {
					Valid = false;
				}
			}
			if (Valid) {
				Sig = &SignatureTable[x];
			}
		}
	}

	if (Sig == NULL) {
		Sig = &SignatureTable[SignatureTableCount];
		SignatureTableCount++;
		Sig->SignatureID = *SignatureID;
	}

	meta_method_info* Method = &Sig->Methods[Sig->MethodsCount];
	Sig->MethodsCount++;

	if (Sig->MethodsCount >= ArrayCount(Sig->Methods)) { printf("Too many meta methods"); }

	return Method;
}


void EatAllWhitespace(tokenizer* Tokenizer)
{
	while (Tokenizer->Current[0]) {

		if (Tokenizer->Current[0] == ' ' || Tokenizer->Current[0] == '\t' || Tokenizer->Current[0] == '\n' || Tokenizer->Current[0] == '\r') {
			Tokenizer->Current++;

		} else if (Tokenizer->Current[0] == '/' && Tokenizer->Current[1] == '*') {
			Tokenizer->Current += 2;
			while (Tokenizer->Current[0] && !(Tokenizer->Current[0] == '*' && Tokenizer->Current[1] == '/')) {
				Tokenizer->Current++;
			}
			if (Tokenizer->Current[0] == '*') {
				Tokenizer->Current += 2;
			}

		} else if (Tokenizer->Current[0] == '/' && Tokenizer->Current[1] == '/') {
			Tokenizer->Current += 2;
			while (Tokenizer->Current[0] && Tokenizer->Current[0] != '\n') {
				Tokenizer->Current++;
			}

		} else {
			break;
		}
	}
}

bool IsAlpha(char c)
{
	bool Result = ((c >= 'a') && (c <= 'z')) ||
	              ((c >= 'A') && (c <= 'Z'));
	return Result;
}

token GetNextToken(tokenizer* Tokenizer)
{
	EatAllWhitespace(Tokenizer);

	token NewToken = {};

	NewToken.Contents = Tokenizer->Current;
	NewToken.ContentsLength = 1;

	switch (Tokenizer->Current[0]) {

		case '{': {
			NewToken.Type = Token_OpenCurly;
			Tokenizer->Current++;
		} break;

		case '}': {
			NewToken.Type = Token_ClosedCurly;
			Tokenizer->Current++;
		} break;

		case '(': {
			NewToken.Type = Token_OpenParen;
			Tokenizer->Current++;
		} break;

		case ')': {
			NewToken.Type = Token_ClosedParen;
			Tokenizer->Current++;
		} break;

		case ';': {
			NewToken.Type = Token_SemiColon;
			Tokenizer->Current++;
		} break;

		case ':': {
			NewToken.Type = Token_Colon;
			Tokenizer->Current++;
		} break;

		case '*': {
			NewToken.Type = Token_Star;
			Tokenizer->Current++;
		} break;

		case ',': {
			NewToken.Type = Token_Comma;
			Tokenizer->Current++;
		} break;

		case '[': {
			NewToken.Type = Token_OpenSquare;
			Tokenizer->Current++;
		} break;

		case ']': {
			NewToken.Type = Token_ClosedSquare;
			Tokenizer->Current++;
		} break;

		default: {

			// Check for struct
			if (Tokenizer->Current[0] == 's' &&
			        Tokenizer->Current[1] == 't' &&
			        Tokenizer->Current[2] == 'r' &&
			        Tokenizer->Current[3] == 'u' &&
			        Tokenizer->Current[4] == 'c' &&
			        Tokenizer->Current[5] == 't'
			   ) {
				NewToken.Type = Token_Struct;
				NewToken.ContentsLength = 6;
				Tokenizer->Current += 6;

			} else if (Tokenizer->Current[0] == 'M' &&
			           Tokenizer->Current[1] == 'e' &&
			           Tokenizer->Current[2] == 't' &&
			           Tokenizer->Current[3] == 'a' &&
			           Tokenizer->Current[4] == 'S' &&
			           Tokenizer->Current[5] == 't' &&
			           Tokenizer->Current[6] == 'r' &&
			           Tokenizer->Current[7] == 'u' &&
			           Tokenizer->Current[8] == 'c' &&
			           Tokenizer->Current[9] == 't'
			          ) {
				NewToken.Type = Token_MetaStruct;
				NewToken.ContentsLength = 10;
				Tokenizer->Current += 10;

			} else if (Tokenizer->Current[0] == 'M' &&
			           Tokenizer->Current[1] == 'e' &&
			           Tokenizer->Current[2] == 't' &&
			           Tokenizer->Current[3] == 'a' &&
			           Tokenizer->Current[4] == 'M' &&
			           Tokenizer->Current[5] == 'e' &&
			           Tokenizer->Current[6] == 't' &&
			           Tokenizer->Current[7] == 'h' &&
			           Tokenizer->Current[8] == 'o' &&
			           Tokenizer->Current[9] == 'd'
			          ) {
				NewToken.Type = Token_MetaMethod;
				NewToken.ContentsLength = 10;
				Tokenizer->Current += 10;

				Tokenizer->Current++;
				NewToken.Extra = Tokenizer->Current;
				while (Tokenizer->Current[0] != '\n' && Tokenizer->Current[0] != '\t' && Tokenizer->Current[0] != '\r') {
					Tokenizer->Current++;
					NewToken.ExtraLength++;
				}

			} else if (Tokenizer->Current[0] == 'e' &&
			           Tokenizer->Current[1] == 'n' &&
			           Tokenizer->Current[2] == 'u' &&
			           Tokenizer->Current[3] == 'm'
			          ) {
				NewToken.Type = Token_Enum;
				NewToken.ContentsLength = 4;
				Tokenizer->Current += 4;

			} else if (Tokenizer->Current[0] == 'c' &&
			           Tokenizer->Current[1] == 'l' &&
			           Tokenizer->Current[2] == 'a' &&
			           Tokenizer->Current[3] == 's' &&
			           Tokenizer->Current[4] == 's'
			          ) {
				NewToken.Type = Token_Class;
				NewToken.ContentsLength = 5;
				Tokenizer->Current += 5;

			} else if (IsAlpha(Tokenizer->Current[0])) {
				while (IsAlpha(Tokenizer->Current[0]) || IsNumeric(Tokenizer->Current[0]) || Tokenizer->Current[0] == '_') {
					Tokenizer->Current++;
				}
				NewToken.Type = Token_Identifier;
				NewToken.ContentsLength = (uint32)(Tokenizer->Current - NewToken.Contents);
			} else if (IsNumeric(Tokenizer->Current[0])) {
				while (IsNumeric(Tokenizer->Current[0])) {
					Tokenizer->Current++;
				}
				NewToken.Type = Token_Number;
				NewToken.ContentsLength = (uint32)(Tokenizer->Current - NewToken.Contents);
			} else {
				NewToken.Type = Token_Unknown;
				Tokenizer->Current++;
			}

		} break;
	}

	return NewToken;
}

void MoveUntil(tokenizer* Tokenizer, std::vector<token_type> Types)
{
	token Next = GetNextToken(Tokenizer);
	if (std::find(Types.begin(), Types.end(), Next.Type) == Types.end()) {
		Next = GetNextToken(Tokenizer);
	}
}

token PeekNextToken(tokenizer* Tokenizer)
{
	char* Curr = Tokenizer->Current;
	token Token = GetNextToken(Tokenizer);
	Tokenizer->Current = Curr;
	return Token;
}

void OutputFolders(std::string AssetRootDir, std::string RelativePath)
{
	HANDLE hFind = {};
	WIN32_FIND_DATA FileData = {};

	std::string StarPath = AssetRootDir + RelativePath + "*";
	hFind = FindFirstFileA(StarPath.c_str(), &FileData);

	do {
		if ((FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && FileData.cFileName[0] != '.') {
			//string P = Root + FileData.cFileName + "/";
			std::string P = RelativePath + FileData.cFileName + "/";

			printf("\"");
			printf(P.c_str());
			printf("\",");
			printf("\n");

			OutputFolders(AssetRootDir, P);
		}
	} while (FindNextFileA(hFind, &FileData) != 0);

	FindClose(hFind);
}

void ProcessFile(char* Path)
{
	read_file_result FileResults = LoadFileData(Path);

	tokenizer Tokenizer = {};
	Tokenizer.Current = (char*)FileResults.Contents;
	Tokenizer.End = Tokenizer.Current + FileResults.ContentsSize;

	while (Tokenizer.Current < Tokenizer.End) {
		token NextToken = GetNextToken(&Tokenizer);

		switch (NextToken.Type) {
			case Token_Unknown: {
			} break;

			case Token_MetaMethod: {

				//void AbilityBite(actor * Source, actor * Dest)

				meta_method_info* Info = SignatureGetNewMethod(&NextToken);

				Info->ReturnType = GetNextToken(&Tokenizer);
				token PointerOrMethodName = GetNextToken(&Tokenizer);
				if (PointerOrMethodName.Type == Token_Identifier) {
					Info->MethodName = PointerOrMethodName;
				} else {
					Info->ReturnTypeIsPointer = true;
					Info->MethodName = GetNextToken(&Tokenizer);
				}
				token OpenParen = GetNextToken(&Tokenizer);

				if (Info->ReturnType.Type != Token_Identifier) { printf("Expected identifier for return type."); }
				if (Info->MethodName.Type != Token_Identifier) { printf("Expected identifier for method name."); }
				if (OpenParen.Type != Token_OpenParen) { printf("Expected open paren."); }

				if (PeekNextToken(&Tokenizer).Type != Token_ClosedParen) {
					while (true) {
						meta_method_parameter* Param = &Info->Params[Info->ParamsCount];
						Info->ParamsCount++;
						if (Info->ParamsCount > ArrayCount(Info->Params)) {
							printf("Error. Too many parameters.");
						}

						Param->Type = GetNextToken(&Tokenizer);
						token PointerOrID = GetNextToken(&Tokenizer);
						if (PointerOrID.Type == Token_Star) {
							Param->IsPointer = true;
							Param->ID = GetNextToken(&Tokenizer);
						} else if (PointerOrID.Type == Token_Identifier) {
							Param->ID = PointerOrID;
						} else if (PointerOrID.Type == Token_ClosedParen) {
							// Probably a macro in the parameters
							break;
						} else {
							printf("Invalid type when parsing parameters.");
						}

						token End = GetNextToken(&Tokenizer);
						if (End.Type == Token_ClosedParen) { break; }
					}
				}

			} break;

			case Token_MetaStruct: {

				token Struct = GetNextToken(&Tokenizer);
				if (Struct.Type == Token_Struct) {
					token StructType = GetNextToken(&Tokenizer);

					static int TypeID = 0;
					TypeID++;

					printf("meta_member %.*s_META[] { \n", StructType.ContentsLength, StructType.Contents);

					token OpenCurly = GetNextToken(&Tokenizer);
					bool32 Parsing = true;
					while (true) {

						bool32 Skipping = false;

						token VarType = GetNextToken(&Tokenizer);
						if (VarType.Type == Token_ClosedCurly) {
							// Found the end of the struct break out
							break;
						}

						token VarName = GetNextToken(&Tokenizer);
						token AfterVar = GetNextToken(&Tokenizer);

						// Checks / validation
						if (
						    VarType.Type != Token_Identifier ||
						    VarName.Type != Token_Identifier ||

						    // Skip pointers
						    AfterVar.Type == Token_Star
						) {
							Skipping = true;
							MoveUntil(&Tokenizer, {Token_SemiColon});
							continue;
						}

						// Check for arrays
						uint32 ArrayLength = 0;
						// Setup the token default, for no arrays
						token ArrayLengthToken = {};
						ArrayLengthToken.Contents = "0";
						ArrayLengthToken.ContentsLength = 1;
						if (AfterVar.Type == Token_OpenSquare) {
							ArrayLengthToken = GetNextToken(&Tokenizer);
						}

						// Consume more tokens if the member uses a default initialization
						if (AfterVar.Type != Token_SemiColon) {
							while (Tokenizer.Current[0] != ';') { Tokenizer.Current++; }
							Tokenizer.Current++;
						}

						// Check variable type
						std::string TypeStr;
						bool32 Found = false;
						for (int i = 0; i < PrimitiveTypes.size() && !Found; i++) {
							const char* Type = PrimitiveTypes[i].c_str();
							if (VarType.ContentsLength == PrimitiveTypes[i].length()) {
								bool32 Valid = true;

								for (uint32 c = 0; c < VarType.ContentsLength && Valid; c++) {
									if (VarType.Contents[c] != Type[c]) {
										Valid = false;
									}
								}

								if (Valid) {
									Found = true;
									TypeStr = "meta_member_type::" + PrimitiveTypes[i];
								}

							}
						}

						// If not found then assume its a struct type
						if (!Found) {
							TypeStr = "meta_member_type::custom";
						}

						// Print the line
						printf("{%.*s, \"%.*s\", \"%.*s\", (uint64)&((%.*s *)0)->%.*s, %.*s, sizeof(%.*s),",

						       // Type
						       (int)TypeStr.length(), TypeStr.c_str(),

						       // Type String
						       VarType.ContentsLength, VarType.Contents,

						       // Name
						       VarName.ContentsLength, VarName.Contents,

						       // Offset
						       StructType.ContentsLength, StructType.Contents,
						       VarName.ContentsLength, VarName.Contents,

						       // Array Length
						       ArrayLengthToken.ContentsLength, ArrayLengthToken.Contents,

						       // Size
						       VarType.ContentsLength, VarType.Contents
						      );

						// Print struct to string method for custon types
						if (!Found) {
							printf("&StructMetaFill_%.*s, &JsonFillStructShim_%.*s, &%.*s_META[0], ArrayCount(%.*s_META) ",

							       // To string method
							       VarType.ContentsLength, VarType.Contents,

							       // Json struct fill method
							       VarType.ContentsLength, VarType.Contents,

							       // Meta array
							       VarType.ContentsLength, VarType.Contents,

							       // Meta array count
							       VarType.ContentsLength, VarType.Contents
							      );
						} else {
							printf("{},{},{},{}");
						}

						printf("},\n");
					}
					printf("}; \n \n");

					// Generate the safe allocate for this
					{
						printf("void M_ALLOC__%.*s(s_void* SafeVoid, memory_arena* Memory) { \n", StructType.ContentsLength, StructType.Contents);
						printf("	SafeVoid->IsAllocated = true; \n");
						printf("	SafeVoid->Type = %.i; \n", TypeID);
						printf("	SafeVoid->DataSize = sizeof(%.*s); \n", StructType.ContentsLength, StructType.Contents);
						printf("	SafeVoid->Data = ArenaAllocate(Memory, sizeof(%.*s)); \n", StructType.ContentsLength, StructType.Contents);
						printf("	ClearMemory((uint8*)SafeVoid->Data, sizeof(%.*s)); \n", StructType.ContentsLength, StructType.Contents);
						printf("};\n\n");
					}

					// The get method
					{
						printf("%.*s* M_GET__%.*s(s_void* SafeVoid) { \n",
						       StructType.ContentsLength, StructType.Contents,
						       StructType.ContentsLength, StructType.Contents);
						printf("	if (SafeVoid->IsAllocated && SafeVoid->Type == %.i) { \n", TypeID);
						printf("		return (%.*s*)SafeVoid->Data; \n", StructType.ContentsLength, StructType.Contents);
						printf("	} \n");
						printf("	return GameNull; \n");
						printf("};\n\n");
					}

					// struct to string for this
					{
						printf("void StructMetaFill_%.*s (struct_string_return* Dest,  void* AccData){\n", StructType.ContentsLength, StructType.Contents);
						//void StructMetaFill(struct_string_return* Dest, meta_member* MetaInfo, uint32 MetaInfoCount, void* AccData)
						printf("return StructMetaFill(Dest, &%.*s_META[0], ArrayCount(%.*s_META), AccData);\n", StructType.ContentsLength, StructType.Contents, StructType.ContentsLength, StructType.Contents);
						printf("}\n\n");
					}

					// json to struct for this
					{
						//typedef void(*json_fill_struct_shim)(json::json_data* JsonData, string KeyParent, meta_member * MetaInfo, uint32 MetaInfoCount, void* DataDest);
						printf("void JsonFillStructShim_%.*s (json::json_data* JsonData, string KeyParent, void* DataDest){\n", StructType.ContentsLength, StructType.Contents);
						//json::json_data* JsonData, string KeyParent, meta_member * MetaInfo, uint32 MetaInfoCount, void* DataDest
						//void FillStruct(json_data* JsonData, string KeyParent, meta_member * MetaInfo, uint32 MetaInfoCount, void* DataDest)
						printf("return json::FillStruct(JsonData, KeyParent, &%.*s_META[0], ArrayCount(%.*s_META), DataDest);\n", StructType.ContentsLength, StructType.Contents, StructType.ContentsLength, StructType.Contents);
						printf("}\n\n");
					}

					/*
					// Full data struct
					{
						printf("meta_member_data %.*s_DATA_META { \n", StructType.ContentsLength, StructType.Contents);

						printf("{{}, &StructToString_%.*s}, \n",

						       // To String func
						       StructType.ContentsLength, StructType.Contents
						      );

						printf("}; \n \n");

					}
					*/


					//struct_string_return Ret = StructToString(meta_member * MetaInfo, uint32 MetaInfoCount, void* AccData, memory_arena * Memory);

				} else if (Struct.Type == Token_Enum) {

					int32 Count = 0;

					token Name = GetNextToken(&Tokenizer);

					// Skip class, if its there
					if (Name.Type == Token_Class) { Name = GetNextToken(&Tokenizer); }

					token OpenCurly = GetNextToken(&Tokenizer);

					printf("string %.*s_NAME[] { \n", Name.ContentsLength, Name.Contents);

					while (true) {

						token Entry = GetNextToken(&Tokenizer);
						if (Entry.Type == Token_ClosedCurly) {
							// Found the end of the struct break out
							break;
						}

						printf("\"%.*s\",\n", Entry.ContentsLength, Entry.Contents);
						Count++;

						token Comma = GetNextToken(&Tokenizer);
						if (Comma.Type == Token_ClosedCurly) {
							break;
						} else if (Comma.Type != Token_Comma) {
							printf("Preprocessor error. Expected comma. \n");
						}
					}


					printf("}; \n");

				} else {
					printf("Preprocessor error. Expected struct. \n");
				}
			} break;

			default: {
				// printf("%d: %.*s \n", NextToken.Type, NextToken.ContentsLength, NextToken.Contents);
			} break;
		}
	}

}

int main(int argc, char* ars[])
{

	/*
	int32 Platform = -1;

	// Get platform. Set using an environment variable
	{
		int32 BuffSize = 4096;
		LPTSTR pszOldVal = (LPTSTR) malloc(BuffSize * sizeof(TCHAR));
		DWORD ret = GetEnvironmentVariable("TowerPlatform", pszOldVal, BuffSize);
		if (ret == 0) {
			printf("Platform variable does not exist.");
		} else {
			printf("\n");
			if (pszOldVal[0] == PLATFORM_WIN64_c) {
				Platform = PLATFORM_WIN64;
			} else if (pszOldVal[0] == PLATFORM_ANDROID_c) {
				Platform = PLATFORM_ANDROID;
			} else {
				printf("UNKNOWN PLATFORM ");
				printf(pszOldVal);
				printf("\n");
				return -1;
			}
		}
	}
	*/

	printf("#ifndef GENERATED \n");
	printf("#define GENERATED \n \n");

	// Save the assets folder structure for Android. Maybe skip this for other platforms to save time.
	/*
	{
		printf("char* AssetsFolderStructure[] { \n");
		OutputFolders("T:/Game/assets/", "");
		printf("}; \n \n");
	}
	*/

	memory_arena PathsMemory = {};
	int64 PathsMemorySize = Megabytes(10);
	PathsMemory.Memory = malloc(PathsMemorySize);
	PathsMemory.Head = (uint8*)PathsMemory.Memory;
	PathsMemory.EndOfMemory = (uint8*)PathsMemory.Memory + PathsMemorySize;

	{
		path_list Paths = {};
		GetPathsForFileType(".cpp", "T:/Game/code/Game/", &PathsMemory, &Paths);
		path_list* P = &Paths;
		while (P != GameNull && StringLength(P->Path) > 0) {
			ProcessFile(P->Path.CharArray);
			P = P->Next;
		}
	}

	{
		path_list Paths = {};
		GetPathsForFileType(".h", "T:/Game/code/Game/", &PathsMemory, &Paths);
		path_list* P = &Paths;
		while (P != GameNull && StringLength(P->Path) > 0) {
			ProcessFile(P->Path.CharArray);
			P = P->Next;
		}
	}


	// Manually process files
	char* FilesToRead[] = {
		"T:/Game/code/Engine/EngineCore.h",
		"T:/Game/code/Engine/Renderer/Renderer.h",
		"T:/Game/code/Game/AssetList.cpp",
	};
	for (int i = 0; i < ArrayCount(FilesToRead); i++) {
		ProcessFile(FilesToRead[i]);
	}

	// Build method stubs, so that we can include the generated file anywhere
	printf("\n");
	for (int32 x = 0; x < SignatureTableCount; x++) {

		method_signature* Sig = &SignatureTable[x];

		for (uint32 i = 0; i < Sig->MethodsCount; i++) {

			// Assume all the meta methods have the same signature. This would be a compiler error if not, so no big deal.
			meta_method_info* I = &Sig->Methods[i];

			// return type
			printf("%.*s", I->ReturnType.ContentsLength, I->ReturnType.Contents);
			if (I->ReturnTypeIsPointer) {
				printf("*");
			}

			// Name
			printf(" %.*s (",
			       I->MethodName.ContentsLength, I->MethodName.Contents
			      );


			for (int y = 0; y < I->ParamsCount; y++) {
				meta_method_parameter* Type = &I->Params[y];
				if (Type->IsPointer) {
					printf("%.*s* %.*s",
					       Type->Type.ContentsLength, Type->Type.Contents,
					       Type->ID.ContentsLength, Type->ID.Contents
					      );
				} else {
					printf("%.*s %.*s",
					       Type->Type.ContentsLength, Type->Type.Contents,
					       Type->ID.ContentsLength, Type->ID.Contents
					      );
				}

				if (y != I->ParamsCount - 1) {
					printf(",");
				}
			}

			printf("); \n");
		}
	}

	printf("\n");

	// typedef the function signature
	for (int32 x = 0; x < SignatureTableCount; x++) {
		//typedef void (*Ability_Begin_Func)(AbilityParams);

		method_signature* Sig = &SignatureTable[x];

		// Assume all the meta methods have the same signature. This would be a compiler error if not, so no big deal.
		meta_method_info* I = &Sig->Methods[0];

		printf("typedef ");

		// return type
		printf("%.*s", I->ReturnType.ContentsLength, I->ReturnType.Contents);
		if (I->ReturnTypeIsPointer) {
			printf("*");
		}

		// Name
		printf(" (*METAFUNC_%.*s)(",
		       Sig->SignatureID.ExtraLength, Sig->SignatureID.Extra
		      );

		// Parameters
		for (int y = 0; y < I->ParamsCount; y++) {
			meta_method_parameter* Type = &I->Params[y];
			if (Type->IsPointer) {
				printf("%.*s* %.*s",
				       Type->Type.ContentsLength, Type->Type.Contents,
				       Type->ID.ContentsLength, Type->ID.Contents
				      );
			} else {
				printf("%.*s %.*s",
				       Type->Type.ContentsLength, Type->Type.Contents,
				       Type->ID.ContentsLength, Type->ID.Contents
				      );
			}

			if (y != I->ParamsCount - 1) {
				printf(",");
			}
		}

		printf("); \n");
	}

	// Print out the methods names
	//struct meta_method { string Name; void(*AbilityPointer)(actor* Source, actor*Dest); };
	for (int32 x = 0; x < SignatureTableCount; x++) {

		method_signature* Sig = &SignatureTable[x];

		// Create struct for this method signature
		{

			// Assume all the meta methods have the same signature. This would be a compiler error if not, so no big deal.
			meta_method_info* I = &Sig->Methods[0];

			printf("\n struct meta_method_%.*s { string Name; ",
			       Sig->SignatureID.ExtraLength, Sig->SignatureID.Extra
			      );

			printf("%.*s", I->ReturnType.ContentsLength, I->ReturnType.Contents);
			if (I->ReturnTypeIsPointer) {
				printf("*");
			}
			printf(" ");

			printf("(*Method)(");

			for (int y = 0; y < I->ParamsCount; y++) {
				meta_method_parameter* Type = &I->Params[y];
				if (Type->IsPointer) {
					printf("%.*s* %.*s",
					       Type->Type.ContentsLength, Type->Type.Contents,
					       Type->ID.ContentsLength, Type->ID.Contents
					      );
				} else {
					printf("%.*s %.*s",
					       Type->Type.ContentsLength, Type->Type.Contents,
					       Type->ID.ContentsLength, Type->ID.Contents
					      );
				}

				if (y != I->ParamsCount - 1) {
					printf(",");
				}
			}

			printf(");}; \n");
		}

		// Build array for all the methods
		printf("meta_method_%.*s methods_%.*s[] { \n",
		       Sig->SignatureID.ExtraLength, Sig->SignatureID.Extra,
		       Sig->SignatureID.ExtraLength, Sig->SignatureID.Extra
		      );

		for (uint32 i = 0; i < Sig->MethodsCount; i++) {
			meta_method_info* I = &Sig->Methods[i];
			printf("{\"%.*s\", &%.*s}, \n",

			       // Name
			       I->MethodName.ContentsLength, I->MethodName.Contents,

			       // Ability method Pointer
			       I->MethodName.ContentsLength, I->MethodName.Contents
			      );
		}
		printf("}; \n \n");



		// Build get method
		{
			// Assume all the meta methods have the same signature. This would be a compiler error if not, so no big deal.
			meta_method_info* I = &Sig->Methods[0];

			printf("METAFUNC_%.*s",
			       Sig->SignatureID.ExtraLength, Sig->SignatureID.Extra
			      );


			printf(" MetaFindMethod_%.*s(string ID)",
			       Sig->SignatureID.ExtraLength, Sig->SignatureID.Extra
			      );
			printf("{ \n");
			printf("	for (int i = 0; i < ArrayCount(");
			printf("methods_%.*s", Sig->SignatureID.ExtraLength, Sig->SignatureID.Extra);
			printf("); i++) { \n");

			printf("		if (methods_%.*s[i].Name == ID) { \n",
			       Sig->SignatureID.ExtraLength, Sig->SignatureID.Extra
			      );
			printf("			return methods_%.*s[i].Method; \n",
			       Sig->SignatureID.ExtraLength, Sig->SignatureID.Extra);
			printf("		} \n");

			printf("	}\n");
			printf("	return {}; \n");
			printf("}\n");
		}
	}

	printf("\n #endif \n");

}