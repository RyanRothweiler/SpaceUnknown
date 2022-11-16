#pragma once
#ifndef profiler_h
#define profiler_h

#if WINDOWS
	#define TIME_BEGIN_ZERO(NAME, FILENAME, LINE_NUMBER, FUNC_NAME) timed_block TimedBlock##NAME(COUNTER, FILENAME, LINE_NUMBER, FUNC_NAME);
	#define TIME_BEGIN_ONE(NAME, FILENAME, LINE_NUMBER, FUNC_NAME) TIME_BEGIN_ZERO(NAME, FILENAME, LINE_NUMBER, FUNC_NAME)
	#define TIME_BEGIN TIME_BEGIN_ONE(__LINE__, __FILE__, __LINE__, __func__)
#else
	#define TIME_BEGIN
#endif

enum class graph_type {
	histogram, line
};

struct graph_data {

	// graph data
	float Data[100];
	float Max;

	int32 I;

	real32 Average;
	real32 Sum;

	// graph settings
	graph_type Type;
};

struct accum_method {
	char* FileName;
	int LineNumber;
	char* FunctionName;

	uint32 CallsCount;
	uint64 CycleStart;
	uint64 TotalCycles;

	uint64 PerfCounterStart;
	uint64 PerfCounterDuration;
};

void AllocateProfiler();

struct profiler_data {
	// These are allocated at the very bottom on of all code. Bottom of EngineCore.cpp
	accum_method* AccumulatedMethods;
	graph_data* AccumulatedMethodsGraphs;

	uint32 TimesCount;

	platform::api* Platform;

	int32 DrawCallCount;
};

profiler_data* GlobalProfilerData = {};

struct timed_block {

	accum_method* Info;

	timed_block(uint32 ID, char* FileName, int LineNumber, char* FunctionName)
	{
		Info = &GlobalProfilerData->AccumulatedMethods[ID];

		Info->CallsCount++;
		Info->CycleStart = GlobalProfilerData->Platform->GetCycle();
		Info->PerfCounterStart = GlobalProfilerData->Platform->QueryPerformanceCounter();

		Info->FileName = FileName;
		Info->LineNumber = LineNumber;
		Info->FunctionName = FunctionName;

	}

	~timed_block()
	{
		Info->TotalCycles += GlobalProfilerData->Platform->GetCycle() - Info->CycleStart;
		Info->PerfCounterDuration += GlobalProfilerData->Platform->QueryPerformanceCounter() - Info->PerfCounterStart;
	}
};

#endif