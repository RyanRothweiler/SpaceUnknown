#pragma once

#ifndef RandomCPP
#define RandomCPP

#include "RandomTable.cpp"

// do this??
//	https://en.wikipedia.org/wiki/Mersenne_Twister

real64
RandomRangeFloat(real64 Bottom, real64 Top)
{
	// Attemp at true random number generation here
	/*
	Assert(Bottom < Top);
	real64 Result = 0;

	uint32 RandomMax = 1000;
	uint32 RandomInt = ((RandomPackage->RandomGenState + 3) % RandomMax);
	real64 RandomScalar = (real64)RandomInt / (real64)RandomMax;

	real64 ScaledNum = (real64)((Top - Bottom) * RandomScalar);
	Result = ScaledNum + Bottom;

	RandomPackage->RandomGenState = (RandomPackage->RandomGenState * RandomPackage->RandomGenState) +
	                                (RandomPackage->RandomGenState >> RandomPackage->PlatformGetClock());
	if (RandomPackage->RandomGenState == 0) {
		RandomPackage->RandomGenState = RandomPackage->PlatformGetClock() * RandomPackage->PlatformGetClock();
	}
	// GameState->RandomGenState = GameState->RandomGenState >> 1;
	// *RandomGenState += *RandomGenState;

	return (Result);
	*/


	// Works on android
	// seed if 0
	/*
	if (RandomPackage->TableIndex == 0) {
		//		RandomPackage->TableIndex = ((RandomPackage->TableIndex + 1) * RandomPackage->PlatformGetClock() * RandomPackage->PlatformGetClock()) % ArrayCount(RandomTable);
	}
	RandomPackage->TableIndex = (RandomPackage->TableIndex + 1) % ArrayCount(RandomTable);

	uint32 TableVal = RandomTable[RandomPackage->TableIndex];
	real64 Unilateral = (real64)TableVal / (real64)RandomTableMax;

	real64 ScaledNum = (real64)((Top - Bottom) * Unilateral);
	real64 Result = ScaledNum + Bottom;
	*/


	real64 RandPerc = PlatformApi.RandomFloat();

	real64 ScaledNum = (real64)((Top - Bottom) * RandPerc);
	real64 Result = ScaledNum + Bottom;


	// this is broken by android
	//Assert(0);


	return (Result);
}

real64
RandomRangeFloat(range Range)
{
	return (RandomRangeFloat(Range.Min, Range.Max));
}


// Bottom is inclusive, top is exclusive
int64
RandomRangeInt(int32 Bottom, int32 Top)
{
	real64 Result = RandomRangeFloat((real32)Bottom, (real32)Top);
	return ((int64)Result);
}

int64
RandomRangeInt(range Range)
{
	return (RandomRangeInt((int32)Range.Min, (int32)Range.Max));
}

bool32 RandomBool()
{
	// Top is not included, so this gives 0 through 9
	int32 Rand = (int32)RandomRangeInt(0, 10);
	return Rand <= 4;
}

color RandomColor()
{
	color Ret = {};
	Ret.R = (real32)RandomRangeFloat(0.0f, 1.0f);
	Ret.G = (real32)RandomRangeFloat(0.0f, 1.0f);
	Ret.B = (real32)RandomRangeFloat(0.0f, 1.0f);
	Ret.A = 1.0f;
	return Ret;
}

vector2 RandomRadius(real64 Radius)
{
	vector2 Ret = {};
	real64 Theta = RandomRangeFloat(0.0f, 1.0f);
	Ret.X = Radius * cos(Theta);
	Ret.Y = Radius * sin(Theta);
	return Ret;
}

/*
// NOTE this is my attempts at a custom bezier quality curve. It never really worked correctly.

// A random number between 0 and 1 with numbers close to 1 being exponentially unlikely
real64 RandomExponential()
{
	real64 Rand = RandomRangeFloat(0, 9);
	return (1.0f / (Rand + 0.9f)) - 0.1f;
}

// https://stackoverflow.com/questions/785097/how-do-i-implement-a-b%C3%A9zier-curve-in-c
// https://stackoverflow.com/questions/37642168/how-to-convert-quadratic-bezier-curve-code-into-cubic-bezier-curve/37642695#37642695
namespace bezier  {
	real64 getPt(real64 n1, real64 n2, real64 perc)
	{
		real64 diff = n2 - n1;

		return n1 + ( diff * perc );
	}

	vector2 Calc(real64 i, vector2 p1, vector2 p2, vector2 p3, vector2 p4)
	{
		// The Green Lines
		real64 xa = getPt( p1.X , p2.X , i );
		real64 ya = getPt( p1.Y , p2.Y , i );
		real64 xb = getPt( p2.X , p3.X , i );
		real64 yb = getPt( p2.Y , p3.Y , i );
		real64 xc = getPt( p3.X , p4.X , i );
		real64 yc = getPt( p3.Y , p4.Y , i );

		// The Blue Line
		real64 xm = getPt( xa , xb , i );
		real64 ym = getPt( ya , yb , i );
		real64 xn = getPt( xb , xc , i );
		real64 yn = getPt( yb , yc , i );

		// The Black Dot
		return vector2 {
			getPt( xm , xn , i ),
			getPt( ym , yn , i )
		};
	}
}

vector2 QualityCurve[100];
void PrecalcQualityCurve()
{
	// this is the quality bezier curve
	for (int i = 0; i < ArrayCount(QualityCurve); i++) {
		QualityCurve[i] = bezier::Calc((float)i / ArrayCount(QualityCurve), vector2{0, 0}, vector2{0.566f, 0.194f}, vector2{0.964f, 0.77f}, vector2{1, 1});
	}

	// Lowest value is 1
	QualityCurve[0] = vector2{0.01f, 0.01f};
}
*/

real64 RandomQuality()
{
	return RandomRangeFloat(0.01f, 1.0f);
}

/*

#define GeneratedGridSize 2048
real32 PerlinNoise[GeneratedGridSize][GeneratedGridSize];

void GeneratePerlinNoise(int nOctaves, real32 bias, random_package* Package, memory_arena* Memory)
{
	// Generate seed gird

	// NOTE must be multiple of two
	int32 SeedGridSize = 2048;
	real32* SeedGrid = (real32*)ArenaAllocate(Memory, sizeof(real32) * SeedGridSize * SeedGridSize);
	for (int32 x = 0; x < SeedGridSize; x++) {
		for (int32 y = 0; y < SeedGridSize; y++) {
			SeedGrid[y * SeedGridSize + x] = (real32)RandomRangeFloat(-1.0f, 1.0f, Package);
		}
	}

	// Generate noise grid
	for (int x = 0; x < GeneratedGridSize; x++) {
		for (int y = 0; y < GeneratedGridSize; y++) {

			float noise = 0.0f;
			float scaleAccum = 0.0f;
			float scale = 1.0f;

			for (int o = 0; o < nOctaves; o++) {
				int pitch = GeneratedGridSize >> o;
				int sampleX1 = (x / pitch) * pitch;
				int sampleY1 = (y / pitch) * pitch;

				int sampleX2 = (sampleX1 + pitch) % GeneratedGridSize;
				int sampleY2 = (sampleY1 + pitch) % GeneratedGridSize;

				float blendX = (float)(x - sampleX1) / (float)pitch;
				float blendY = (float)(y - sampleY1) / (float)pitch;

				double sampleT = (1.0 - blendX) * SeedGrid[sampleY1 * GeneratedGridSize + sampleX1] + blendX * SeedGrid[sampleY1 * GeneratedGridSize + sampleX2];
				double sampleB = (1.0 - blendX) * SeedGrid[sampleY2 * GeneratedGridSize + sampleX1] + blendX * SeedGrid[sampleY2 * GeneratedGridSize + sampleX2];

				scaleAccum += scale;
				noise += (float)((blendY * (sampleB - sampleT) + sampleT) * scale);
				scale = scale / bias;
			}

			PerlinNoise[x][y] = noise / scaleAccum;
		}
	}
}
*/

// Unit Tests

void
RandomUnitTests()
{
	return;

	int32 CountToCheck = 2000;

	// int64
	for (int index = 0; index < 5000; index++) {
		int64 num = RandomRangeInt(0, 3);
		Assert(num >= 0 && num < 3);
	}

	// int64
	for (int index = 0; index < CountToCheck; index++) {
		int64 num = RandomRangeInt(CountToCheck, CountToCheck * 2);
		Assert(num >= CountToCheck && num < CountToCheck * 2);
	}

	// int64
	for (int index = 0; index < CountToCheck; index++) {
		int64 num = RandomRangeInt(20, 72);
		Assert(num >= 20 && num < 72);
	}

	// real64
	for (int index = 0; index < CountToCheck; index++) {
		real64 num = RandomRangeFloat(0.0, 100.0);
		Assert(num >= 0 && num < 100);
	}
}

namespace Perlin {

	vector2* SeedGrid;
	uint32 SeedGridSize;
	bool32 SeedGridGenerated;

	void GenerateSeedGrid(uint32 GSize, game_memory* Memory)
	{
		// Generate grid vectors
		SeedGrid = (vector2*)ArenaAllocate(&Memory->PermanentMemory, sizeof(vector2) * GSize * GSize);
		for (uint32 index = 0; index <  GSize * GSize; index++) {
			SeedGrid[index].X = RandomRangeFloat(-1.0f, 1.0f);
			SeedGrid[index].Y = RandomRangeFloat(-1.0f, 1.0f);
			// SeedGrid[index] = Vector2Normalize(SeedGrid[index]);
		}

		SeedGridGenerated = true;
		SeedGridSize = GSize;
	}

	real64 PerlinFadeFunction(real64 Input)
	{
		return (6 * Input * Input * Input * Input * Input) - (15 * Input * Input * Input * Input) + (10 * Input * Input * Input);
	}

	real64 GetNoise(vector2 Pos, uint32 ImageSize)
	{
		real64 Value = 0.0f;

		if (SeedGridGenerated) {

			Pos.X = (real64)((int32)Pos.X);
			Pos.Y = (real64)((int32)Pos.Y);

			int32 PixelsPerSeedGrid = (int32)((real64)ImageSize / (real64)SeedGridSize);

			vector2 PosWrapped = vector2{
				(real64)((int32)Pos.X % (PixelsPerSeedGrid * PixelsPerSeedGrid)),
				(real64)((int32)Pos.Y % (PixelsPerSeedGrid * PixelsPerSeedGrid))
			};

			// Find nearest corners and get seed vectors
			vector2 TopLeftPixelPos = vector2{
				(Pos.X) - ((int32)Pos.X % PixelsPerSeedGrid),
				(Pos.Y) - ((int32)Pos.Y % PixelsPerSeedGrid)
			};
			vector2 TopRightPixelPos = vector2{
				(Pos.X + PixelsPerSeedGrid) - ((int32)Pos.X % PixelsPerSeedGrid),
				(Pos.Y) - ((int32)Pos.Y % PixelsPerSeedGrid)
			};
			vector2 BottomRightPixelPos = vector2{
				(Pos.X + PixelsPerSeedGrid) - ((int32)Pos.X % PixelsPerSeedGrid),
				(Pos.Y + PixelsPerSeedGrid) - ((int32)Pos.Y % PixelsPerSeedGrid)
			};
			vector2 BottomLeftPixelPos = vector2{
				(Pos.X) - ((int32)Pos.X % PixelsPerSeedGrid),
				(Pos.Y + PixelsPerSeedGrid) - ((int32)Pos.Y % PixelsPerSeedGrid)
			};

			vector2 TopLeftSeed = vector2{(real64)((int32)(TopLeftPixelPos.X / SeedGridSize) % SeedGridSize), (real64)((int32)(TopLeftPixelPos.Y / SeedGridSize) % SeedGridSize)};
			vector2 BottomRightSeed = vector2{(real64)((int32)(BottomRightPixelPos.X / SeedGridSize) % SeedGridSize), (real64)((int32)(BottomRightPixelPos.Y / SeedGridSize) % SeedGridSize)};

			uint32 TopLeftIndex = ((int)TopLeftSeed.X * SeedGridSize) + (int)TopLeftSeed.Y;
			uint32 TopRightIndex = ((int)BottomRightSeed.X * SeedGridSize) + (int)TopLeftSeed.Y;
			uint32 BottomLeftIndex = ((int)TopLeftSeed.X * SeedGridSize) + (int)BottomRightSeed.Y;
			uint32 BottomRightIndex = ((int)BottomRightSeed.X * SeedGridSize) + (int)BottomRightSeed.Y;

			Assert(TopLeftIndex >= 0 && TopLeftIndex < (SeedGridSize * SeedGridSize));
			Assert(TopRightIndex >= 0 && TopRightIndex < (SeedGridSize * SeedGridSize));
			Assert(BottomLeftIndex >= 0 && BottomLeftIndex < (SeedGridSize * SeedGridSize));
			Assert(BottomRightIndex >= 0 && BottomRightIndex < (SeedGridSize * SeedGridSize));

			vector2 TopLeftSeedVector = SeedGrid[TopLeftIndex];
			vector2 TopRightSeedVector = SeedGrid[TopRightIndex];
			vector2 BottomLeftSeedVector = SeedGrid[BottomLeftIndex];
			vector2 BottomRightSeedVector = SeedGrid[BottomRightIndex];

			// Get vectors from corners to new point
			vector2 DirTopLeft = vector2{(TopLeftPixelPos.X - Pos.X) / PixelsPerSeedGrid,
			                             (TopLeftPixelPos.Y - Pos.Y) / PixelsPerSeedGrid
			                            };
			vector2 DirTopRight = vector2{(TopRightPixelPos.X - Pos.X) / PixelsPerSeedGrid,
			                              (TopRightPixelPos.Y - Pos.Y) / PixelsPerSeedGrid
			                             };
			vector2 DirBottomLeft = vector2{(BottomLeftPixelPos.X - Pos.X) / PixelsPerSeedGrid,
			                                (BottomLeftPixelPos.Y - Pos.Y) / PixelsPerSeedGrid
			                               };
			vector2 DirBottomRight = vector2{(BottomRightPixelPos.X - Pos.X) / PixelsPerSeedGrid,
			                                 (BottomRightPixelPos.Y - Pos.Y) / PixelsPerSeedGrid
			                                };

			// For testing
			real64 TopLeftLen = Vector2Length(DirTopLeft);
			real64 TopRightLen = Vector2Length(DirTopRight);
			real64 BottomLeftLen = Vector2Length(DirBottomLeft);
			real64 BottomRightLen = Vector2Length(DirBottomRight);

			// Get dot product from each corner
			real64 TopLeftDot = DotProduct(DirTopLeft, TopLeftSeedVector);
			real64 TopRightDot = DotProduct(DirTopRight, TopRightSeedVector);
			real64 BottomLeftDot = DotProduct(DirBottomLeft, BottomLeftSeedVector);
			real64 BottomRightDot = DotProduct(DirBottomRight, BottomRightSeedVector);

			// Lerp the top and bottom, then lerp the results to get the Value
			real64 HorPerc = (Pos.X - TopLeftPixelPos.X) / PixelsPerSeedGrid;
			real64 VerPerc = (Pos.Y - TopLeftPixelPos.Y) / PixelsPerSeedGrid;
			real64 TopLerp = Coserp(TopLeftDot, TopRightDot, HorPerc);
			real64 BottomLerp = Coserp(BottomLeftDot, BottomRightDot, HorPerc);

			Value = Coserp(TopLerp, BottomLerp, VerPerc);

			// This actually generates value between -1 and 1. Put it between 0 and 1
			Value = (Value + 1) / 2.0f;
			Value = PerlinFadeFunction(Value);
		}

		return Value;
	}

}

#endif