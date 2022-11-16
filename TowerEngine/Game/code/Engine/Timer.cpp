#pragma once

#ifndef TimerCPP
#define TimerCPP

#include "Timer.h"

void SetFrameTimer(frame_timer * Timer, int64 Length, int64 FrameStart)
{
	Timer->Complete = false;
	Timer->Running = true;
	Timer->Length = Length;
	Timer->FrameStart = FrameStart;
}

void UpdateFrameTimer(frame_timer * Timer, int64 CurrFrame)
{
	if (Timer->Running) {
		if (Timer->FrameStart + Timer->Length <= CurrFrame) {
			Timer->Complete = true;
			Timer->Running = false;
		}
	}
}

void SetSecondsTimer(seconds_timer* Timer, real64 LengthSeconds)
{
	Timer->Complete = false;
	Timer->Running = true;
	Timer->LengthMS = LengthSeconds * 1000.0f;
	Timer->TimeAccumMS = 0;
}

void UpdateSecondsTimer(seconds_timer* Timer, real64 DeltaTimeMS)
{
	if (Timer->Running) {
		Timer->TimeAccumMS += DeltaTimeMS;
		if (Timer->TimeAccumMS >= Timer->LengthMS) {
			Timer->Complete = true;
			Timer->Running = false;
		}
	}
}

#endif