#pragma once
#ifndef RangeCPP
#define RangeCPP

#include "Range.h"

real64
GetPercentageInRange(real64 Current, range Range)
{
	Current = ClampValue(Range.Min, Range.Max, Current);

	if (Range.Min < 0) {
		real64 SignAdj = Abs(Range.Min);
		Range.Min = 0.0f;
		Range.Max += SignAdj;
		Current += SignAdj;
	}

	real64 TotalRange = Range.Max - Range.Min;
	real64 CurrRange = Abs(Range.Max - Current);
	return (Abs((Current / TotalRange)));
}

real64
GetValueAtPercentage(real64 Percentage, range Range)
{
	real64 SignAdj = 0.0f;
	if (Range.Min < 0) {
		SignAdj = Abs(Range.Min);
		Range.Min = 0.0f;
		Range.Max += SignAdj;
	}

	real64 TotalRange = Range.Max - Range.Min;
	return ((TotalRange - (Range.Min + (TotalRange * Percentage))) - SignAdj);
}

string
RangeToString(range Range)
{
	string Min = Range.Min;
	string Max = Range.Max;
	string FinalString = Min + "<->" + Max;
	return (FinalString);
}

bool32
RangeValid(range* Range)
{
	return (Range->Min != Range->Max);
}

#endif