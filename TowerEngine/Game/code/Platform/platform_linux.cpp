// NOTE this requires opengl stuff
// get with
// sudo apt-get install libgl-dev
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <dlfcn.h>


#include <X11/Xresource.h>
#include <X11/Xlib.h>
#include <sys/times.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Game.h"

// NOTE this pretty much all works. Only thing missing is sound.


bool32 GlobalRunning = true;

global_variable real64 GlobalScrollPos = 0.0f;
global_variable real64 PrevGlobalScrollPos;

struct win32_game_code
{
	// TODO fix this
	void *GameCodeDLL;
	game_update_and_render *GameLoop;
	game_window_closing *GameWindowClosing;

	bool32 IsValid;
};

void
DebugLine(int64 *Output)
{
	char NumChar[MAX_STRING_SIZE] = {};
	IntToCharArray(*Output, NumChar);

	char FinalOutput[MAX_STRING_SIZE] = {};
	ConcatCharArrays(NumChar, "\n", FinalOutput);
	printf("%s\n", FinalOutput);
}

void
DebugLine(int64 Output)
{
	DebugLine(&Output);
}

void
DebugLine(char *Output)
{
	char FinalOutput[MAX_STRING_SIZE] = {};
	ConcatCharArrays(Output, "\n", FinalOutput);
	printf("%s\n", FinalOutput);
}


void
ConcatIntChar(char *CharInput, int64 IntInput, char *CharOutput)
{
	char IntInputAsChar[] = {};
	IntToCharArray(IntInput, IntInputAsChar);
	ConcatCharArrays(IntInputAsChar, CharInput, CharOutput);
}

// TODO fix this for input
// real64
// CheckStickDeadzone(short Value, SHORT DeadZoneThreshold)
// {
// 	real64 Result = 0;

// 	if (Value < -DeadZoneThreshold)
// 	{
// 		Result = (real64)(Value + DeadZoneThreshold) / (32768.0f - DeadZoneThreshold);
// 	}
// 	else if (Value > DeadZoneThreshold)
// 	{
// 		// this explicit number is pulled from my ass.
// 		Result = (real64)(Value  + DeadZoneThreshold) / (47467.0f - DeadZoneThreshold);
// 	}

// 	return (Result);
// }

void
ProcessInputState(input_state *ButtonProcessing, bool32 NewState)
{
	if (NewState)
	{
		if (ButtonProcessing->IsDown)
		{
			ButtonProcessing->OnDown = false;
		}
		else
		{
			ButtonProcessing->IsDown = true;
			ButtonProcessing->OnDown = true;
			ButtonProcessing->IsUp = false;
			ButtonProcessing->OnUp = false;
		}
	}
	else
	{
		if (ButtonProcessing->IsUp)
		{
			ButtonProcessing->OnUp = false;
		}
		else
		{
			ButtonProcessing->IsUp = true;
			ButtonProcessing->OnUp = true;
			ButtonProcessing->IsDown = false;
			ButtonProcessing->OnDown = false;
		}
	}
}

void
ProcessTriggerInput(input_state *Trigger, int32 TriggerValue)
{
	if (TriggerValue > 200)
	{
		ProcessInputState(Trigger, true);
	}
	else
	{
		ProcessInputState(Trigger, false);
	}
}

win32_game_code
LoadGameCode()
{
	win32_game_code Result = {};

	// NOTE this seems to take a few loops before it works. Not really sure why.
	// TODO fix that.
	system("cp Game.so Game_temp.so -f");
	Result.GameCodeDLL = dlopen("./Game.so", RTLD_NOW);
	if (Result.GameCodeDLL)
	{
		Result.GameLoop = (game_update_and_render *)dlsym(Result.GameCodeDLL, "GameLoop");
		Result.GameWindowClosing = (game_window_closing *)dlsym(Result.GameCodeDLL, "GameWindowClosing");
		Result.IsValid = true;
	}
	else
	{
		Result.IsValid = false;
		fprintf(stderr, "dlopen failed: %s\n", dlerror());
	}

	return (Result);
}

void
UnloadGameCode(win32_game_code *GameCode)
{
	if (GameCode->GameCodeDLL)
	{
		dlclose(GameCode->GameCodeDLL);
	}

	GameCode->IsValid = false;
	GameCode->GameLoop = GameLoopStub;
}

// TODO fix this for sound
// #define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
// typedef DIRECT_SOUND_CREATE(direct_sound_create);

// TODO fix this to enable sound
// void
// LoadDirectSound(HWND WindowHandle, win32_audio_output *SoundOutput, LPDIRECTSOUNDBUFFER *SoundSecondaryBuffer)
// {
// 	HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
// 	if (DSoundLibrary)
// 	{
// 		direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

// 		LPDIRECTSOUND DirectSound;
// 		if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
// 		{
// 			WAVEFORMATEX WaveFormat = {};
// 			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;

// 			// NOTE this is forced mono
// 			WaveFormat.nChannels = 1;

// 			WaveFormat.nSamplesPerSec = SoundOutput->SamplesPerSecond;
// 			WaveFormat.wBitsPerSample = 16;
// 			WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
// 			WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
// 			WaveFormat.cbSize = 0;

// 			if (SUCCEEDED(DirectSound->SetCooperativeLevel(WindowHandle, DSSCL_PRIORITY)))
// 			{
// 				DSBUFFERDESC BufferDescription = {};
// 				BufferDescription.dwSize = sizeof(BufferDescription);
// 				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

// 				LPDIRECTSOUNDBUFFER PrimaryBuffer;
// 				if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
// 				{
// 					if (SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat)))
// 					{
// 						// NOTE primary format format has been set
// 					}
// 					else
// 					{
// 						// diagnostics
// 					}
// 				}
// 				else
// 				{
// 					// diagnostics
// 				}
// 			}
// 			else
// 			{
// 				// diagnostics
// 			}

// 			//secondary buffer stuff

// 			DSBUFFERDESC BufferDescription = {};
// 			BufferDescription.dwSize = sizeof(BufferDescription);
// 			BufferDescription.dwFlags = 0;
// 			BufferDescription.dwBufferBytes = SoundOutput->SecondaryBufferSize;
// 			BufferDescription.lpwfxFormat = &WaveFormat;

// 			if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, SoundSecondaryBuffer, 0)))
// 			{
// 				// NOTE the secondary sound buffer was created successfully
// 			}
// 			else
// 			{
// 				// diagnostics
// 			}
// 		}
// 	}
// 	else
// 	{
// 		// didn't load the library
// 		// diagnostics
// 	}
// }

// void
// FillSoundOutput(game_audio_output_buffer *GameAudio, win32_audio_output *SoundOutput,
//                 DWORD ByteToLock, DWORD BytesToWrite, LPDIRECTSOUNDBUFFER SoundSecondaryBuffer)
// {
// 	VOID *Region1;
// 	DWORD Region1Size;
// 	VOID *Region2;
// 	DWORD Region2Size;

// 	if (SUCCEEDED(SoundSecondaryBuffer->Lock(ByteToLock, BytesToWrite,
// 	              &Region1, &Region1Size,
// 	              &Region2,  &Region2Size,
// 	              0)))
// 	{
// 		DWORD Region1SampleCount = Region1Size / SoundOutput->BytesPerSample;
// 		int16 *DestSample = (int16 *)Region1;
// 		int16 *SourceSamples = GameAudio->Samples;
// 		for (DWORD SampleIndex = 0;
// 		        SampleIndex < Region1SampleCount;
// 		        SampleIndex++)
// 		{
// 			*DestSample++ = *SourceSamples++;
// 			*DestSample++ = *SourceSamples++;
// 			SoundOutput->RunningSampleIndex++;
// 		}

// 		DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
// 		DestSample = (int16 *)Region2;
// 		for (DWORD SampleIndex = 0;
// 		        SampleIndex < Region2SampleCount;
// 		        SampleIndex++)
// 		{
// 			*DestSample++ = *SourceSamples++;
// 			*DestSample++ = *SourceSamples++;
// 			SoundOutput->RunningSampleIndex++;
// 		}

// 		SoundSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
// 	}
// }

inline uint64
GetWallClock()
{
	// LARGE_INTEGER Count;
	// QueryPerformanceCounter(&Count);
	// return (Count);

	timespec TimeSpec;
	uint32 ReturnVal = clock_gettime(CLOCK_REALTIME, &TimeSpec);
	return (TimeSpec.tv_nsec);
}

void
SaveState(char *FileName, game_memory *GameMemory)
{
	DebugLine("Saving State");

	char FinalFileName[MAX_STRING_SIZE] = {};
	int FileHandle = open(FileName, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
	write(FileHandle, GameMemory->GameMemoryBlock, GameMemory->TotalSize);

	close(FileHandle);

	DebugLine("Save Complete");
}

void
LoadState(char *FileName, game_memory *GameMemory)
{
	DebugLine("Loading State");

	char FinalFileName[MAX_STRING_SIZE] = {};
	int FileHandle = open(FileName, O_RDWR);
	read(FileHandle, GameMemory->GameMemoryBlock, GameMemory->TotalSize);
	close(FileHandle);

	DebugLine("Load Complete");
}

read_file_result
LoadFileData(char *FileName, memory_arena *Memory)
{
	read_file_result Result = {};

	int FileHandle = open(FileName, O_RDWR);
	DebugLine(FileHandle);
	if (FileHandle != -1)
	{
		struct stat FileStats = {};
		fstat(FileHandle, &FileStats);

		int x = 0;
		Result.Contents = ArenaAllocate(Memory, FileStats.st_size);
		read(FileHandle, Result.Contents, FileStats.st_size);
		Result.ContentsSize = FileStats.st_size;
	}
	else
	{
		Assert(0);
		// Counld not open file
	}
	close(FileHandle);

	return (Result);
}

PLATFORM_FILE_EXISTS(PlatformFileExists)
{
	int FileHandle = open(FilePath, O_RDWR);
	if (FileHandle != -1)
	{
		return (true);
	}
	return (false);
}

PLATFORM_READ_FILE(PlatformReadFile)
{
	read_file_result Result = LoadFileData(Path, Memory);
	return (Result);
}

PLATFORM_WRITE_FILE(PlatformWriteFile)
{
	int FileHandle = open(FileDestination, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
	Assert(FileHandle != -1);

	int BytesWritten = write(FileHandle, Data, DataSize);
	Assert(Success != -1);

	close(FileHandle);
}

inline timespec
GetGameCodeLastWriteTime()
{
	struct stat FileStats = {};
	stat("Game.so", &FileStats);
	return (FileStats.st_atim);
}

void
CheckSaveState(char *FilePath, input_state *ButtonChecking, bool32 SelectIsDown, game_memory *GameMemory)
{
	if (ButtonChecking->OnDown && SelectIsDown)
	{
		SaveState(FilePath, GameMemory);
	}
	if (ButtonChecking->OnDown && !SelectIsDown)
	{
		LoadState(FilePath, GameMemory);
	}
}

void
RenderLine(gl_line *Line)
{
	glColor4f((GLfloat)Line->Color.R, (GLfloat)Line->Color.G, (GLfloat)Line->Color.B, (GLfloat)Line->Color.A);
	glBegin(GL_LINES);
	{
		// NOTE line width isn't working
		// glLineWidth(10.0f);

		glVertex2d(Line->Start.X, Line->Start.Y);
		glVertex2d(Line->End.X, Line->End.Y);
	}
	glEnd();
}

void
RenderTexture(gl_texture *TextureRendering)
{
	glPushMatrix();

	vector2 Center = TextureRendering->Center;
	vector2 Scale = TextureRendering->Scale;
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, TextureRendering->Image->GLID);
	glBegin(GL_QUADS);
	{
		glColor4f((GLfloat)TextureRendering->Color.R, (GLfloat)TextureRendering->Color.G,
		          (GLfloat)TextureRendering->Color.B, (GLfloat)TextureRendering->Color.A);

		real64 Radians = TextureRendering->RadiansAngle;

		vector2 RotatedPoint = {};
		vector2 OrigPoint = {};

		OrigPoint = {Center.X - Scale.X, Center.Y - Scale.Y};
		RotatedPoint = Vector2RotatePoint(OrigPoint, Center, Radians);
		glTexCoord2f(0, 1);
		glVertex2f((GLfloat)RotatedPoint.X, (GLfloat)RotatedPoint.Y);

		OrigPoint = {Center.X + Scale.X, Center.Y - Scale.Y};
		RotatedPoint = Vector2RotatePoint(OrigPoint, Center, Radians);
		glTexCoord2f(1, 1);
		glVertex2f((GLfloat)RotatedPoint.X, (GLfloat)RotatedPoint.Y);

		OrigPoint = {Center.X + Scale.X, Center.Y + Scale.Y};
		RotatedPoint = Vector2RotatePoint(OrigPoint, Center, Radians);
		glTexCoord2f(1, 0);
		glVertex2f((GLfloat)RotatedPoint.X, (GLfloat)RotatedPoint.Y);

		OrigPoint = {Center.X - Scale.X, Center.Y + Scale.Y};
		RotatedPoint = Vector2RotatePoint(OrigPoint, Center, Radians);
		glTexCoord2f(0, 0);
		glVertex2f((GLfloat)RotatedPoint.X, (GLfloat)RotatedPoint.Y);
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void
RenderSquare(gl_square *Square)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
	{
		glColor4f((GLfloat)Square->Color.R, (GLfloat)Square->Color.G, (GLfloat)Square->Color.B, (GLfloat)Square->Color.A);
		// NOTE the order of this can't be changed. Though I can't find any documentation on why or what the correct order is, but this works.
		glVertex2d(Square->TopRight.X, Square->TopRight.Y);
		glVertex2d(Square->TopLeft.X, Square->TopLeft.Y);
		glVertex2d(Square->BottomLeft.X, Square->BottomLeft.Y);
		glVertex2d(Square->BottomRight.X, Square->BottomRight.Y);
	}
	glEnd();
}

void
GLMakeTexture(loaded_image *Image, uint32 *BitmapPixels)
{
	GLuint textureID;
	// glGenTextures(1, (GLuint *)&Image->GLID);
	glGenTextures(1, &textureID);
	Image->GLID = (uint32)textureID;
	glBindTexture(GL_TEXTURE_2D, Image->GLID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
	             Image->Width, Image->Height,
	             0, GL_RGBA, GL_UNSIGNED_BYTE, BitmapPixels);
}

int main(void)
{

	bmp_header Header = {};

	FILE *BMPFile = fopen("../../assets/Circle.bmp", "rb");

	fread(&Header.FileType, 2, 1, BMPFile);
	fread(&Header.FileSize, 4, 1, BMPFile);
	fread(&Header.Reserved1, 2, 1, BMPFile);
	fread(&Header.Reserved2, 2, 1, BMPFile);
	fread(&Header.BitmapOffset, 4, 1, BMPFile);
	fread(&Header.Size, 4, 1, BMPFile);
	fread(&Header.Width, 4, 1, BMPFile);
	fread(&Header.Height, 4, 1, BMPFile);


	window_info WindowInfo = {};
	WindowInfo.Width = 1366;
	WindowInfo.Height = 800;

	Display *OpenDisplay = {};
	OpenDisplay = XOpenDisplay(NULL);

	int Screen = DefaultScreen(OpenDisplay);

	Window OpenWindow = {};
	OpenWindow = XCreateSimpleWindow(OpenDisplay, RootWindow(OpenDisplay, Screen),
	                                 0, 0, WindowInfo.Width, WindowInfo.Height - 40,
	                                 1, BlackPixel(OpenDisplay, Screen), WhitePixel(OpenDisplay, Screen));
	XSelectInput(OpenDisplay, OpenWindow, ExposureMask | KeyPressMask);
	XMapWindow(OpenDisplay, OpenWindow);

	// uint32 DispWidth = DisplayWidth(OpenDisplay, Screen);
	// uint32 DispHeight = DisplayHeight(OpenDisplay, Screen);
	// XMoveWindow(OpenDisplay, OpenWindow, (DispWidth - WindowInfo.Width) / 2, (DispHeight - WindowInfo.Height) / 2);

	Atom WM_DELETE_WINDOW = XInternAtom(OpenDisplay, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(OpenDisplay, OpenWindow, &WM_DELETE_WINDOW, 1);

	GLint VisualAttributesList[] = {GLX_RGBA, GLX_ALPHA_SIZE, 8, GLX_RED_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_DOUBLEBUFFER};
	XVisualInfo *VisualInfo = glXChooseVisual(OpenDisplay, 0, VisualAttributesList);
	GLXContext OpenContext = glXCreateContext(OpenDisplay, VisualInfo, NULL, true);
	glXMakeCurrent(OpenDisplay, OpenWindow, OpenContext);

	game_input GameInput = {};

	timespec ClockRes = {};
	clock_getres(CLOCK_REALTIME, &ClockRes);
	uint64 PerfCountFrequency = ClockRes.tv_nsec;

	int32 GameUpdateHz = 60;
	real64 TargetSecondsElapsedPerFrame = 1.0f / (real64)GameUpdateHz;

	uint64 PreviousFrameCount = GetWallClock();

	// 	win32_audio_output SoundOutput = {};
	// 	// NOTE I don't know why this * 2 is necessary. There is a bug somewhere with the audio that this fixes.
	// 	// There are twice as many samples as we expect?
	// 	SoundOutput.SamplesPerSecond = 48000 * 2;
	// 	SoundOutput.RunningSampleIndex = 0;
	// 	SoundOutput.BytesPerSample = sizeof(int16) * 2;
	// 	SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample;
	// 	bool SoundIsInit = false;

	// 	int16 *AudioSamplesMemory = (int16 *)VirtualAlloc(NULL, SoundOutput.SecondaryBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	// 	LPDIRECTSOUNDBUFFER SoundSecondaryBuffer;
	// 	LoadDirectSound(Window, &SoundOutput, &SoundSecondaryBuffer);
	// 	SoundSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

	win32_game_code GameCode = LoadGameCode();
	timespec GameCodeLastWriteTime = GetGameCodeLastWriteTime();

	#if INTERNAL
	void *BaseAddress = (LPVOID)Megabytes((uint64)2);
	#else
	void *BaseAddress = 0;
	#endif
	game_memory GameMemory = {};
	GameMemory.PermanentMemory.Size = Megabytes(64);
	GameMemory.TransientMemory.Size = Megabytes(64);
	GameMemory.TotalSize = GameMemory.PermanentMemory.Size + GameMemory.TransientMemory.Size;

	GameMemory.GameMemoryBlock = malloc((size_t)GameMemory.TotalSize);
	GameMemory.PermanentMemory.Memory = GameMemory.GameMemoryBlock;
	GameMemory.TransientMemory.Memory = (uint8 *)GameMemory.PermanentMemory.Memory + GameMemory.PermanentMemory.Size;

	GameMemory.PlatformReadFile = PlatformReadFile;
	GameMemory.PlatformWriteFile = PlatformWriteFile;
	GameMemory.PlatformFileExists = PlatformFileExists;
	GameMemory.PlatformMakeTexture = GLMakeTexture;

	XSelectInput(OpenDisplay, OpenWindow, ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask);

	bool32 LeftMouseDown = false;
	bool32 RightMouseDown = false;
	bool32 MiddleMouseDown = false;

	GlobalRunning = true;
	while (GlobalRunning)
	{
		GameInput.MouseScrollDelta = 0.0f;

		XEvent Event = {};
		while (XPending(OpenDisplay) > 0)
		{
			XNextEvent(OpenDisplay, &Event);

			if (Event.type == ButtonPress)
			{
				switch (Event.xbutton.button)
				{
					case 1:
					{
						LeftMouseDown = true;
					}
					break;

					case 2:
					{
						MiddleMouseDown = true;
					}
					break;

					case 3:
					{
						RightMouseDown = true;
					}
					break;

					case 4:
					{
						GameInput.MouseScrollDelta = 1;
					}
					break;

					case 5:
					{
						GameInput.MouseScrollDelta = -1;
					}
					break;
				}
			}
			if (Event.type == ButtonRelease)
			{
				switch (Event.xbutton.button)
				{
					case 1:
					{
						LeftMouseDown = false;
					}
					break;

					case 2:
					{
						MiddleMouseDown = false;
					}
					break;

					case 3:
					{
						RightMouseDown = false;
					}
					break;
				}
			}

			if (Event.type == ClientMessage)
			{
				GlobalRunning = false;
			}
		}

		ProcessInputState(&GameInput.MouseLeft, LeftMouseDown);
		ProcessInputState(&GameInput.MouseRight, RightMouseDown);
		ProcessInputState(&GameInput.MouseMiddle, MiddleMouseDown);

		tms Times = {};
		int64 FlipWallClock = times(&Times);

		timespec NewDLLWriteTime = GetGameCodeLastWriteTime();
		bool AreEqual = NewDLLWriteTime.tv_sec == GameCodeLastWriteTime.tv_sec &&
		                NewDLLWriteTime.tv_nsec == GameCodeLastWriteTime.tv_nsec;
		if (!AreEqual)
		{
			DebugLine("Loading the new code");
			UnloadGameCode(&GameCode);
			GameCode = LoadGameCode();
			GameCodeLastWriteTime = NewDLLWriteTime;
		}

		// 		DWORD dwResult;
		// 		for (DWORD ControllerIndex = 0;
		// 		        ControllerIndex < XUSER_MAX_COUNT;
		// 		        ControllerIndex++)
		// 		{
		// 			XINPUT_STATE ControllerState;
		// 			ZeroMemory(&ControllerState, sizeof(XINPUT_STATE));
		// 			dwResult = XInputGetState(ControllerIndex, &ControllerState);

		// 			if (dwResult == ERROR_SUCCESS)
		// 			{
		// 				XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

		// 				ProcessInputState(&GameInput.AButton, Pad->wButtons & XINPUT_GAMEPAD_A);
		// 				ProcessInputState(&GameInput.BButton, Pad->wButtons & XINPUT_GAMEPAD_B);
		// 				ProcessInputState(&GameInput.XButton, Pad->wButtons & XINPUT_GAMEPAD_X);
		// 				ProcessInputState(&GameInput.YButton, Pad->wButtons & XINPUT_GAMEPAD_Y);

		// 				// NOTE this works, just commented out to turn on keyboard controls
		// 				// ProcessInputState(&GameInput.DUp, Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
		// 				// ProcessInputState(&GameInput.DDown, Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
		// 				// ProcessInputState(&GameInput.DLeft, Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
		// 				// ProcessInputState(&GameInput.DRight, Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

		// 				ProcessInputState(&GameInput.R1, Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
		// 				ProcessInputState(&GameInput.L1, Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
		// 				ProcessTriggerInput(&GameInput.R2, Pad->bRightTrigger);
		// 				ProcessTriggerInput(&GameInput.L2, Pad->bLeftTrigger);

		// 				ProcessInputState(&GameInput.Start, Pad->wButtons & XINPUT_GAMEPAD_START);
		// 				ProcessInputState(&GameInput.Select, Pad->wButtons & XINPUT_GAMEPAD_BACK);

		// 				GameInput.LeftStick.X = ClampValue(-0.9f, 0.9f, CheckStickDeadzone(Pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
		// 				GameInput.LeftStick.Y = ClampValue(-0.9f, 0.9f, CheckStickDeadzone(Pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) * -1;

		// 			}
		// 			else
		// 			{
		// 				// diagnostics
		// 				// Controller is not connected
		// 			}
		// 		}

		Window RootWindow = {};
		Window WindowChild = {};
		int XReturn;
		int YReturn;
		int XRootReturn;
		int YRootReturn;
		unsigned int ButtonStateReturn;

		XQueryPointer(OpenDisplay, OpenWindow, &RootWindow, &WindowChild,
		              &XRootReturn, &YRootReturn,
		              &XReturn, &YReturn, &ButtonStateReturn);
		GameInput.MousePos = vector2{(real64)XReturn, (real64)YReturn};

		// TODO test these, they should work though
		CheckSaveState("StateSlot1.ts", &GameInput.R1, GameInput.Select.IsDown, &GameMemory);
		CheckSaveState("StateSlot2.ts", &GameInput.L1, GameInput.Select.IsDown, &GameMemory);
		CheckSaveState("StateSlot3.ts", &GameInput.R2, GameInput.Select.IsDown, &GameMemory);
		CheckSaveState("StateSlot4.ts", &GameInput.L2, GameInput.Select.IsDown, &GameMemory);


		// 		DWORD PlayCursor;
		// 		DWORD WriteCursor;
		game_audio_output_buffer GameAudio = {};
		// 		DWORD BytesToWrite = 0;
		// 		DWORD ByteToLock = 0;
		// 		if (SoundSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor) == DS_OK)
		// 		{
		// 			ByteToLock = (SoundOutput.RunningSampleIndex * SoundOutput.BytesPerSample) % SoundOutput.SecondaryBufferSize;

		// 			real32 BytesPerSecond = ((real32)(SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample));
		// 			DWORD ExpectedSoundBytesPerFrame = (int)(BytesPerSecond / GameUpdateHz);

		// 			DWORD TargetCursor = 0;
		// 			TargetCursor = (WriteCursor + ExpectedSoundBytesPerFrame) % SoundOutput.SecondaryBufferSize;

		// 			if (ByteToLock > TargetCursor)
		// 			{
		// 				BytesToWrite = (SoundOutput.SecondaryBufferSize - ByteToLock) + TargetCursor;
		// 			}
		// 			else
		// 			{
		// 				BytesToWrite = TargetCursor - ByteToLock;
		// 			}

		// 			GameAudio.SamplesPerSecond = SoundOutput.SamplesPerSecond;
		// 			GameAudio.SampleCount = BytesToWrite / SoundOutput.BytesPerSample;
		// 			GameAudio.Samples = AudioSamplesMemory;
		// 			GameAudio.BytesPerSample = SoundOutput.BytesPerSample;
		// 		}

		engine_state *GameStateFromMemory = (engine_state *)GameMemory.PermanentMemory.Memory;


		// #if DEBUG_AUDIO
		// 		float BytesCountScale = 500.0f;

		// 		gl_line StartLine = {};
		// 		StartLine.Start = vector2{20, 40};
		// 		StartLine.End = vector2{20, 140};
		// 		StartLine.Color = COLOR_BLACK;
		// 		RenderLine(&StartLine);

		// 		gl_line EndLine = {};
		// 		EndLine.Start = vector2{20 + (SoundOutput.SecondaryBufferSize / BytesCountScale), 40};
		// 		EndLine.End = vector2{20 + (SoundOutput.SecondaryBufferSize / BytesCountScale), 140};
		// 		EndLine.Color = COLOR_BLACK;
		// 		RenderLine(&EndLine);

		// 		gl_line PlayLine = {};
		// 		PlayLine.Start = vector2{20 + (PlayCursor / BytesCountScale), 40};
		// 		PlayLine.End = vector2{20 + (PlayCursor / BytesCountScale), 140};
		// 		PlayLine.Color = COLOR_BLUE;
		// 		RenderLine(&PlayLine);

		// 		gl_line WriteLine = {};
		// 		WriteLine.Start = vector2{20 + (WriteCursor / BytesCountScale), 40};
		// 		WriteLine.End = vector2{20 + (WriteCursor / BytesCountScale), 140};
		// 		WriteLine.Color = COLOR_BLUE;
		// 		RenderLine(&WriteLine);

		// 		gl_line LockLine = {};
		// 		LockLine.Start = vector2{20 + (ByteToLock / BytesCountScale), 40};
		// 		LockLine.End = vector2{20 + (ByteToLock / BytesCountScale), 140};
		// 		LockLine.Color = COLOR_GREEN;
		// 		RenderLine(&LockLine);
		// #endif

		time_t SystemTime = time(NULL);
		GameStateFromMemory->RandomGenState += SystemTime;
		if (GameStateFromMemory->RandomGenState > 100000)
		{
			GameStateFromMemory->RandomGenState = 0;
		}


		if (GameCode.IsValid)
		{
			GameCode.GameLoop(&GameMemory, &GameInput, &WindowInfo, &GameAudio);
		}

		// FillSoundOutput(&GameAudio, &SoundOutput, ByteToLock, BytesToWrite, SoundSecondaryBuffer);


		GameStateFromMemory = (engine_state *)GameMemory.PermanentMemory.Memory;
		char *EmptyChar = "";
		if (GameStateFromMemory->DebugOutput &&
		    GameStateFromMemory->DebugOutput != EmptyChar)
		{
			DebugLine(GameStateFromMemory->DebugOutput);
			GameStateFromMemory->DebugOutput = EmptyChar;
		}


		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glOrtho(0, WindowInfo.Width, WindowInfo.Height, 0, -10, 10);
		glViewport(0, -40, WindowInfo.Width, WindowInfo.Height);

		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


		for (int32 LayerIndex = 0;
		     LayerIndex < RENDER_LAYER_COUNT;
		     LayerIndex++)
		{
			glEnable(GL_STENCIL_TEST);
			glColorMask(0, 0, 0, 0);
			glStencilFunc(GL_ALWAYS, 1, 1);
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

			gl_square ClearSquare = {};
			ClearSquare.Color = COLOR_WHITE;
			ClearSquare.TopLeft = vector2{0, 0};
			ClearSquare.TopRight = vector2{(real64)WindowInfo.Width, 0};
			ClearSquare.BottomLeft = vector2{0, (real64)WindowInfo.Height};
			ClearSquare.BottomRight = vector2{(real64)WindowInfo.Width, (real64)WindowInfo.Height};
			RenderSquare(&ClearSquare);

			glStencilFunc(GL_ALWAYS, 0, 0);

			for (uint32 RenderIndex = 1;
			     RenderIndex <= (uint32)GameStateFromMemory->StencilObjects[LayerIndex].LinkCount;
			     RenderIndex++)
			{
				list_link *LinkRendering = GetLink(&GameStateFromMemory->StencilObjects[LayerIndex], RenderIndex);

				if (LinkRendering->DataType != LINKTYPE_GL_SQUARE)
				{
					AssertM(0, "Can only render squares to the stencil buffer");
				}

				RenderSquare((gl_square *)LinkRendering->Data);
			}

			glColorMask(1, 1, 1, 1);
			glStencilFunc(GL_EQUAL, 1, 1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


			for (uint32 RenderIndex = 1;
			     RenderIndex <= (uint32)GameStateFromMemory->RenderObjects[LayerIndex].LinkCount;
			     RenderIndex++)
			{
				list_link *LinkRendering = GetLink(&GameStateFromMemory->RenderObjects[LayerIndex], RenderIndex);
				switch (LinkRendering->DataType)
				{
					case LINKTYPE_GL_TEXTURE:
					{
						RenderTexture((gl_texture *)LinkRendering->Data);
						break;
					}

					case LINKTYPE_GL_LINE:
					{
						RenderLine((gl_line *)LinkRendering->Data);
						break;
					}

					case LINKTYPE_GL_SQUARE:
					{
						RenderSquare((gl_square *)LinkRendering->Data);
						break;
					}
				}
			}
		}

		GLXDrawable TheDrawable = glXGetCurrentDrawable();
		glXSwapBuffers(OpenDisplay, TheDrawable);

		uint64 AfterWorkNS = GetWallClock();
		uint64 ElapsedNSCount = AfterWorkNS - PreviousFrameCount;
		uint64 TargetNSperFrame = TargetSecondsElapsedPerFrame * (1000000000);
		while (ElapsedNSCount < TargetNSperFrame)
		{
			AfterWorkNS = GetWallClock();
			ElapsedNSCount = AfterWorkNS - PreviousFrameCount;
		}

		// NOTE game is forced at 60 fps. Anything smaller doesn't work.
		AfterWorkNS = GetWallClock();
		ElapsedNSCount = AfterWorkNS - PreviousFrameCount;

		double SecondsForFrame = ((double)ElapsedNSCount / (double)1000000000);
		real64 FPS = 1.0f / SecondsForFrame;
		char charFPS[MAX_STRING_SIZE] = {};
		ConcatIntChar(FPS, " FPS", charFPS);
		#if 0
		if (GameStateFromMemory->PrintFPS)
		{
			DebugLine(FPS);
		}
		#endif
		GameStateFromMemory->PrevFrameFPS = FPS;

		PreviousFrameCount = AfterWorkNS;
		GameMemory.ElapsedCycles = PreviousFrameCount;

		FlipWallClock = GetWallClock();
	}

	XCloseDisplay(OpenDisplay);
	return (0);

}