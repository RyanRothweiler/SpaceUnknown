#pragma once

#ifndef AudioCPP
#define AudioCPP

#include "EngineCore.h"

struct wave_header {
	uint32 RiffID;
	uint32 Size;
	uint32 WaveID;
};

#define WAVE_CODE(a, b, c, d) (((uint32)(a) << 0) | ((uint32)(b) << 8) | ((uint32)(c) << 16) | ((uint32)(d) << 24))
enum {
	WAVE_ChunkID_fmt = WAVE_CODE('f', 'm', 't', ' '),
	WAVE_ChunkID_data = WAVE_CODE('d', 'a', 't', 'a'),
	WAVE_ChunkID_RIFF = WAVE_CODE('R', 'I', 'F', 'F'),
	WAVE_ChunkID_WAVE = WAVE_CODE('W', 'A', 'V', 'E'),
};

struct wave_chunk {
	uint32 ID;
	uint32 Size;
};

struct wave_fmt {
	uint16 Format;
	uint16 NumberOfChannels;
	uint32 NumSamplesPerSecond;
	uint32 AverageBytesPerSec;
	uint16 BlockAlign;
	uint16 BitsPerSample;
	uint16 CBSize;
	uint16 ValidBitsPerSample;
	uint32 ChannelMask;
	uint8 SubFormat[16];
};

struct riff_iterator {
	uint8 *At;
	uint8 *Stop;
};


inline riff_iterator
ParseChunkAt(void *At, void *Stop)
{
	riff_iterator Iter;

	Iter.At = (uint8 *)At;
	Iter.Stop = (uint8 *)Stop;

	return (Iter);
}

inline riff_iterator
NextChunk(riff_iterator Iter)
{
	wave_chunk *Chunk = (wave_chunk *)Iter.At;
	uint32 Size = (Chunk->Size + 1) & ~1;
	Iter.At += sizeof(wave_chunk) + Size;

	return (Iter);
}

inline bool32
IsValid(riff_iterator Iter)
{
	bool32 Result = (Iter.At < Iter.Stop);
	return (Result);
}

inline void *
GetChunkData(riff_iterator Iter)
{
	void *Result = (Iter.At + sizeof(wave_chunk));
	return (Result);
}

inline uint32
GetType(riff_iterator Iter)
{
	wave_chunk *Chunk = (wave_chunk *)Iter.At;
	uint32 Result = Chunk->ID;

	return (Result);
}

uint32
GetChunkDataSize(riff_iterator Iter)
{
	wave_chunk *Chunk = (wave_chunk *)Iter.At;
	uint32 Result = Chunk->Size;

	return (Result);
}

loaded_sound
LoadWave(char *FilePath, memory_arena *Memory)
{
	loaded_sound Result = {};

	read_file_result WaveResult = PlatformApi.ReadFile(FilePath, Memory);
	wave_header *WaveHeader = (wave_header *)WaveResult.Contents;
	Assert(WaveHeader->RiffID == WAVE_ChunkID_RIFF);
	Assert(WaveHeader->WaveID == WAVE_ChunkID_WAVE);

	uint32 ChannelCount = 0;
	uint32 SampleDataSize = 0;
	int16 *SampleData = 0;
	for (riff_iterator Iter = ParseChunkAt(WaveHeader + 1, (uint8 *)(WaveHeader + 1) + WaveHeader->Size - 4);
	        IsValid(Iter);
	        Iter = NextChunk(Iter)) {
		switch (GetType(Iter)) {
			case WAVE_ChunkID_fmt: {
				wave_fmt *fmt = (wave_fmt *)GetChunkData(Iter);

				// NOTE Assert that this file is in a supported format
				// Using PCM format;
				Assert(fmt->Format == 1);
				Assert(fmt->NumSamplesPerSecond == 48000);
				Assert(fmt->BitsPerSample == 16);
				Assert(fmt->BlockAlign == (sizeof(int16) * fmt->NumberOfChannels));
				ChannelCount = fmt->NumberOfChannels;
			}
			break;

			case WAVE_ChunkID_data: {
				SampleData = (int16 *)GetChunkData(Iter);
				SampleDataSize = GetChunkDataSize(Iter);
			}
			break;
		}
	}

	Result.ChannelCount = ChannelCount;
	Result.SampleCount = SampleDataSize / (ChannelCount * sizeof(int16));

	if (ChannelCount == 1) {
		Result.Samples[0] = SampleData;
		Result.Samples[1] = 0;
	} else if (ChannelCount == 2) {
		Assert(0);
	} else {
		// Invalid channel count
		Assert(0);
	}

	return (Result);
}

uint64
GetSampleIndexAtSongTime(real64 SecondsFromStart, uint64 SamplesPerSecond)
{
	// NOTE why the 2?
	return ((uint64)(SecondsFromStart * (SamplesPerSecond / 2)));
}

real64
GetSampleValueAtIndex(uint64 Index, loaded_sound *Sound)
{
	Assert(Index < Sound->SampleCount)
	return (Sound->Samples[0][Index]);
}

real64
GetSampleValueAtTime(real64 SecondsFromStart, uint32 SamplesPerSecond, loaded_sound *Sound)
{
	uint64 IndexAtTime = GetSampleIndexAtSongTime(SecondsFromStart, SamplesPerSecond);
	return (GetSampleValueAtIndex(IndexAtTime, Sound));
}

real64
GetAvgSampleAroundIndex(uint64 MiddleIndex, uint32 Range, loaded_sound *Sound)
{
	int64 LeftBound = MiddleIndex - (Range / 2);
	int64 RightBound = MiddleIndex + (Range / 2);
	if (LeftBound < 0) {
		LeftBound = 0;
	}
	if (RightBound > Sound->SampleCount) {
		RightBound = Sound->SampleCount - 1;
	}

	real64 Accumulator = 0;
	for (uint64 SampleIndex = LeftBound;
	        SampleIndex < (uint64)RightBound;
	        SampleIndex++) {
		real64 SampleValue = GetSampleValueAtIndex(SampleIndex, Sound);
		if (SampleValue > 0) {
			Accumulator += SampleValue;
		}
	}

	real64 FinalAverage = Accumulator / (Range / 2);
	real64 test = 0;

	return (FinalAverage);
}

real64
GetAvgSampleAroundTime(real64 SecondsFromStart, uint32 Range, uint32 SamplesPerSecond, loaded_sound *Sound)
{
	uint64 MiddleIndex = GetSampleIndexAtSongTime(SecondsFromStart, SamplesPerSecond);
	real64 AverageSample = GetAvgSampleAroundIndex(MiddleIndex, Range, Sound);
	return (AverageSample);
}

#endif