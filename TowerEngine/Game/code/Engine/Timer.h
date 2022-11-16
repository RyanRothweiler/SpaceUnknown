#pragma once
#ifndef TimerH
#define TimerH

struct frame_timer {
	int64 FrameStart;
	int64 Length;

	bool32 Running;
	bool32 Complete;
};

struct seconds_timer {
	real64 TimeAccumMS;
	real64 LengthMS;

	bool32 Running;
	bool32 Complete;
};

#endif