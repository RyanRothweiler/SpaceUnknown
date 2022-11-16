/*
	NOTE
	Would be really cool to automatically upload but authenticating to use the api is a shit. So I give up.
*/

#define _CRT_SECURE_NO_WARNINGS
#define CURL_STATICLIB

#include <stdio.h>
#include <T:\Crossbow\curl\curl.h>
#include <iostream>
#include <fstream>
#include <time.h>

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define GameNull 0
#define Assert(Expression) if (!(Expression)) {*(int *)0 = 0;}

#include "T:\Game\code\Engine\Types.cpp"
#include "T:\Game\code\Engine\memorymanager.h"
#include "T:\Game\code\Engine\memorymanager.cpp"
#include "T:\Game\code\Engine\String.cpp"

#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// Source https://gist.github.com/darelf/0f96e1d313e1d0da5051e1a6eff8d329

/*
Base64 translates 24 bits into 4 ASCII characters at a time. First,
3 8-bit bytes are treated as 4 6-bit groups. Those 4 groups are
translated into ASCII characters. That is, each 6-bit number is treated
as an index into the ASCII character array.
If the final set of bits is less 8 or 16 instead of 24, traditional base64
would add a padding character. However, if the length of the data is
known, then padding can be eliminated.
One difference between the "standard" Base64 is two characters are different.
See RFC 4648 for details.
This is how we end up with the Base64 URL encoding.
*/

const char base64_url_alphabet[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'
};

std::string base64_encode(const std::string & in)
{
	std::string out;
	int val = 0, valb = -6;
	size_t len = in.length();
	unsigned int i = 0;
	for (i = 0; i < len; i++) {
		unsigned char c = in[i];
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back(base64_url_alphabet[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) {
		out.push_back(base64_url_alphabet[((val << 8) >> (valb + 8)) & 0x3F]);
	}
	return out;
}

std::string base64_decode(const std::string & in)
{
	std::string out;
	std::vector<int> T(256, -1);
	unsigned int i;
	for (i = 0; i < 64; i++) T[base64_url_alphabet[i]] = i;

	int val = 0, valb = -8;
	for (i = 0; i < in.length(); i++) {
		unsigned char c = in[i];
		if (T[c] == -1) break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			out.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}
// -----------------------------------------------------------------------------

uint64 GetClockMS()
{
	SYSTEMTIME SystemTime = {};
	GetSystemTime(&SystemTime);
	return ((uint64)SystemTime.wMilliseconds);
}

void PrintTimeElapsed(SYSTEMTIME Start)
{
	SYSTEMTIME CurrentTime = {};
	GetSystemTime(&CurrentTime);

	DWORD Hours = CurrentTime.wHour - Start.wHour;
	DWORD Minutes = CurrentTime.wMinute - Start.wMinute;
	DWORD Seconds = CurrentTime.wSecond - Start.wSecond;
	DWORD MS = CurrentTime.wMilliseconds - Start.wMilliseconds;

	if (Hours > 0) {
		string F = string{(uint64)Hours} + "h ";
		printf(F.CharArray);
	}
	if (Minutes > 0) {
		string F = string{(uint64)Minutes} + "m ";
		printf(F.CharArray);
	}
	if (Seconds > 0) {
		string F = string{(uint64)Seconds} + "s ";
		printf(F.CharArray);
	}
	if (MS > 0) {
		string F = string{(uint64)MS} + "ms ";
		printf(F.CharArray);
	}
	printf("\n");
}

DWORD RunBatch(char* Path, int FirstArg, int SecondArg)
{
	TCHAR systemDirPath[MAX_PATH] = _T("");
	GetSystemDirectory( systemDirPath, sizeof(systemDirPath) / sizeof(_TCHAR) );

	// path to cmd.exe, path to batch file, plus some space for quotes, spaces, etc.
	TCHAR commandLine[2 * MAX_PATH + 16] = _T("");

	_sntprintf( commandLine, sizeof(commandLine) / sizeof(_TCHAR),
	            _T("\"%s\\cmd.exe\" /C \"%s %i %i\""), systemDirPath, Path, FirstArg, SecondArg);

	STARTUPINFO si = {0}; // alternative way to zero array
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {0};

	if ( !CreateProcess( NULL,
	                     commandLine,
	                     NULL,
	                     NULL,
	                     FALSE,
	                     0,
	                     NULL,
	                     NULL,
	                     &si,
	                     &pi )
	   ) {
		_tprintf( _T("CROSSBOW -> CreateProcess failed (%d)\n"), GetLastError() );
		while (true) { }
		return 1;
	}

	WaitForSingleObject( pi.hProcess, INFINITE );

	DWORD BuildResult;
	GetExitCodeProcess(pi.hProcess, &BuildResult);

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	return (int)BuildResult;
}

enum class build_type {
	release, testing
};
enum class platform {
	windows, android
};
DWORD BuildGame(platform Platform, build_type BuildType)
{
	if (Platform == platform::android && BuildType == build_type::testing) {
		printf("Test version of android not supported. That doesn't even really make sense.");
		return 1;
	}

	int Testing = 0;
	if (BuildType == build_type::release) {
		Testing = 0;
	} else if (BuildType == build_type::testing) {
		Testing = 1;
	} else {
		// Unknown build type
		Assert(0);
	}

	char* BatchFilePath;
	switch (Platform) {
		case platform::windows: BatchFilePath = "T:/Game/build_scripts/build_win32.bat"; break;
		case platform::android: BatchFilePath = "T:/Game/build_scripts/build_android_releaseAAB.bat"; break;
		default: printf("Unsupported platform"); return 1;
	}

	return RunBatch(BatchFilePath, Testing, 0);
}

void main(int argc, char* argv[])
{
	// Test Build --------------------------------------------------------------
	{
		printf("CROSSBOW -> Build Windows automated testing version\n");

		DWORD BuildResult = BuildGame(platform::windows, build_type::testing);

		if (BuildResult != 0) {
			printf("CROSSBOW -> Build error \n");
			while (true) { }
		}

		printf("CROSSBOW -> Build success. \n");
	}
	// -------------------------------------------------------------------------

	// Run tests ---------------------------------------------------------------
	{
		printf("CROSSBOW -> Running automated tests\n");

		STARTUPINFO si = {0}; // alternative way to zero array
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi = {0};

		if ( !CreateProcess( NULL,
		                     "T:/Game/build/Windows/Platform_win32.exe",
		                     NULL,
		                     NULL,
		                     FALSE,
		                     0,
		                     NULL,
		                     NULL,
		                     &si,
		                     &pi )
		   ) {
			_tprintf( _T("CROSSBOW -> CreateProcess failed (%d)\n"), GetLastError() );
			while (true) { }
		}

		WaitForSingleObject( pi.hProcess, INFINITE );

		std::ifstream Stream("T:/Crossbow/build/TestingResults.txt");
		if (Stream.is_open()) {
			std::cout << Stream.rdbuf();
		}

		printf("CROSSBOW -> Tests successful. \n");
	}
	// -------------------------------------------------------------------------


	memory_arena TempMem = {};
	TempMem.Size = Megabytes(512);
	TempMem.Memory = VirtualAlloc(0, TempMem.Size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	TempMem.Head = (uint8*)TempMem.Memory;
	TempMem.EndOfMemory = (uint8 *)TempMem.Memory + TempMem.Size;

	string NewVersion;

	// Increment version.txt ---------------------------------------------------
	{
		printf("CROSSBOW -> Updating Version.txt \n");

		char* FilePath = "T:/Game/version.txt";

		HANDLE FileHandle = CreateFileA(FilePath, GENERIC_READ,  0, 0, OPEN_EXISTING, 0, 0);
		if (FileHandle) {

			LARGE_INTEGER FileSize;
			GetFileSizeEx(FileHandle, &FileSize);
			int32 FS = (uint32)FileSize.QuadPart;

			void* Contents = VirtualAlloc(0, FS, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			DWORD BytesRead;
			if (ReadFile(FileHandle, Contents, FS, &BytesRead, 0)) {
				CloseHandle(FileHandle);

				int32 Len = CharArrayLength((char*)Contents);
				string StringVer = BuildString((char*)Contents, Len);

				split_info Split = SplitString(StringVer, '.', false, true, &TempMem);

				int32 Major = StringToInt32(Split.Strings[0]);
				int32 Minor = StringToInt32(Split.Strings[1]);
				int32 Patch = StringToInt32(Split.Strings[2]);
				int32 Build = StringToInt32(Split.Strings[3]);

				int32 BuildNew = Build + 1;

				VirtualFree(Contents, FS, MEM_RELEASE);

				HANDLE WriteHandle = CreateFileA(FilePath, GENERIC_WRITE,  0, 0, OPEN_EXISTING, 0, 0);
				NewVersion = Major + string{"."} + Minor + string{"."} + Patch + string{"."} + BuildNew;
				DWORD BytesWritten;
				if (WriteHandle && WriteFile(WriteHandle, (void*)NewVersion.CharArray, sizeof(char) * StringLength(NewVersion), &BytesWritten, 0)) {
					CloseHandle(WriteHandle);

					printf("CROSSBOW -> New version is ");
					printf(string{NewVersion} .CharArray);
					printf("\n");
				} else {
					printf("CROSSBOW -> Error writing Version.txt");
				}

			} else {
				printf("CROSSBOW -> Error reading Version.txt");
				while (true) { }
			}
		} else {
			printf("CROSSBOW -> Unable to open Version.txt");
			while (true) { }
		}
	}
	// -------------------------------------------------------------------------

	// Build release windows ---------------------------------------------------
	{
		printf("CROSSBOW -> Building Windows release version \n");

		DWORD BuildResult = BuildGame(platform::windows, build_type::release);

		if (BuildResult != 0) {
			printf("CROSSBOW -> Build error \n");
			while (true) { }
		}

		printf("CROSSBOW -> Building successful. \n");
	}
	// -------------------------------------------------------------------------

	// Update android xml version ----------------------------------------------
	// NOTE this is pretty janky because I don't have a full xml serializer / deserializer. This would get much simpler if we had one of those.
	{
		printf("CROSSBOW -> Updating android manifest versions \n");

		char* FilePath = "T:/AndroidProject/app/src/main/AndroidManifest.xml";

		string VersionCodeLiteral = "versionCode";
		string VersionNameLiteral = "versionName";

		HANDLE OpenFileHandle = CreateFileA(FilePath, GENERIC_READ,  0, 0, OPEN_EXISTING, 0, 0);
		if (OpenFileHandle) {

			LARGE_INTEGER FileSize;
			GetFileSizeEx(OpenFileHandle, &FileSize);
			int32 FS = (uint32)FileSize.QuadPart;

			void* Contents = VirtualAlloc(0, FS, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			DWORD BytesRead;
			if (ReadFile(OpenFileHandle, Contents, FS, &BytesRead, 0)) {
				CloseHandle(OpenFileHandle);

				string NewVersionCode;

				// Needed so we can get the exact size of the new xml file
				int32 OrigVersionCodeLen;
				int32 OrigVersionNameLen;

				char* Curr = (char*)Contents;

				// Get versionCode
				{
					int32 TargetLen = StringLength(VersionCodeLiteral);
					while (true) {
						string Test = BuildString(Curr, TargetLen);
						if (Test == VersionCodeLiteral) { break; }
						Curr++;
					}

					Curr += TargetLen + 2;
					char* VersionCodeStart = Curr;
					int32 NumLen = 0;
					while (CharIsNumber(*Curr)) {
						Curr++;
						NumLen++;
					}
					string VersionCodeString = BuildString(VersionCodeStart, NumLen);
					OrigVersionCodeLen = StringLength(VersionCodeString);
					int32 VersionCode = StringToInt32(VersionCodeString);
					VersionCode++;
					NewVersionCode = VersionCode;
				}

				// Get versionName leng
				{
					int32 TargetLen = StringLength(VersionNameLiteral);
					while (true) {
						string Test = BuildString(Curr, TargetLen);
						if (Test == VersionNameLiteral) { break; }
						Curr++;
					}

					Curr += TargetLen + 2;
					char* VersionNameStart = Curr;
					int32 NumLen = 0;
					while (CharIsNumber(*Curr) || *Curr == '.') {
						Curr++;
						NumLen++;
					}
					string VersionNameString = BuildString(VersionNameStart, NumLen);
					OrigVersionNameLen = StringLength(VersionNameString);
				}

				// Update xml
				{
					int32 NewVersionCodeLen = StringLength(NewVersionCode);
					int32 NewVersionNameLen = StringLength(NewVersion);
					int32 TotalChars = (FS - OrigVersionCodeLen - OrigVersionNameLen) + NewVersionCodeLen + NewVersionNameLen;
					char* NewXML = (char*)ArenaAllocate(&TempMem, sizeof(char) * TotalChars);
					char* FinalXMl = NewXML;

					char* Orig = (char*)Contents;
					char* OrigEnd = (char*)Contents + FS;

					// Move to and set new version code
					{
						int32 TargetLen = StringLength(VersionCodeLiteral);
						while (true) {
							string Test = BuildString(Orig, TargetLen);
							if (Test == VersionCodeLiteral) { break; }

							*NewXML = *Orig;

							Orig++;
							NewXML++;
						}

						for (int x = 0; x < TargetLen; x++) {
							*NewXML = Orig[x];
							NewXML++;
						}

						Orig += TargetLen + 2 + OrigVersionCodeLen;

						*NewXML = '=';
						NewXML++;

						*NewXML = '\"';
						NewXML++;

						for (int x = 0; x < NewVersionCodeLen; x++) {
							*NewXML = NewVersionCode.CharArray[x];
							NewXML++;
						}
					}

					// Move to and set new version name
					{
						int32 TargetLen = StringLength(VersionNameLiteral);
						while (true) {
							string Test = BuildString(Orig, TargetLen);
							if (Test == VersionNameLiteral) { break; }

							*NewXML = *Orig;

							Orig++;
							NewXML++;
						}

						for (int x = 0; x < TargetLen; x++) {
							*NewXML = Orig[x];
							NewXML++;
						}

						Orig += TargetLen + 2 + OrigVersionNameLen;

						*NewXML = '=';
						NewXML++;

						*NewXML = '\"';
						NewXML++;

						for (int x = 0; x < NewVersionNameLen; x++) {
							*NewXML = NewVersion.CharArray[x];
							NewXML++;
						}
					}

					// Copy the rest of the file
					while (Orig < OrigEnd) {
						*NewXML = *Orig;
						NewXML++;
						Orig++;
					}

					// Write changes to xml
					HANDLE WriteHandle = CreateFileA(FilePath, GENERIC_WRITE,  0, 0, OPEN_EXISTING, 0, 0);
					DWORD BytesWritten;
					if (WriteHandle && WriteFile(WriteHandle, (void*)FinalXMl, sizeof(char) * TotalChars, &BytesWritten, 0)) {
						CloseHandle(WriteHandle);

						string CrossVersionCode = "CROSSBOW -> Version " + NewVersionCode + "\n";
						string CrossVersionName = "CROSSBOW -> VersionName " + NewVersion + "\n";
						printf(CrossVersionCode.CharArray);
						printf(CrossVersionName.CharArray);
						printf("CROSSBOW -> Android manifest versions updated. ");
						printf("\n");
					} else {
						printf("CROSSBOW -> Error writing Version.txt");
					}
				}

				VirtualFree(Contents, FS, MEM_RELEASE);
			} else {
				printf("CROSSBOW -> Error reading android manifest \n");
				while (true) { }
			}
		} else {
			printf("CROSSBOW -> Unable to open android manifest \n");
			while (true) { }
		}
	}
	// -------------------------------------------------------------------------

	// Build release andorid ---------------------------------------------------
	{
		printf("CROSSBOW -> Building Andorid release version \n");

		DWORD BuildResult = BuildGame(platform::android, build_type::release);

		if (BuildResult != 0) {
			printf("CROSSBOW -> Build error \n");
			while (true) { }
		}

		printf("CROSSBOW -> Building successful. ");
	}
	// -------------------------------------------------------------------------

	// Sign release andorid ---------------------------------------------------
	{
		printf("CROSSBOW -> Signing Andorid release version \n");

		DWORD BuildResult = RunBatch("T:/Game/build_scripts/sign_android.bat", 0, 0);

		if (BuildResult != 0) {
			printf("CROSSBOW -> Sign error \n");
			while (true) { }
		}

		printf("CROSSBOW -> Signing successful. \n");
	}
	// -------------------------------------------------------------------------

	// Upload andorid build ----------------------------------------------------
	/*
	{
		printf("\n");
		printf("CROSSBOW -> Uploading android build \n");
		printf("\n");

		printf("CROSSBOW -> TODO \n");


		printf("\n");
		printf("CROSSBOW -> Upload successful \n");
		printf("\n");
	}
	*/
	// -------------------------------------------------------------------------
	/*
	TODO
	- Android upload release
	*/


	printf("CROSSBOW -> Everything finished successfully \n");
	while (true) { }
}