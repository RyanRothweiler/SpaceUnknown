#pragma once

#ifndef HexCPP
#define HexCPP

// TODO unit testing for this

#include "EngineCore.h"

struct cube_coords {
	int32 X;
	int32 Y;
	int32 Z;
};

struct axial {
	int32 Q;
	int32 R;
};


enum cell_availability {
	// Claimed
	CellAvail_Claimed,
	// Available but not claimed
	CellAvail_Unclaimed,
	// Not available at all
	CellAvail_Blocked,
};
string CellAvailabilityNames[] = {
	"Claimed",
	"Unclaimed",
	"Blocked"
};

struct cell_stage {
	int32 GridSize;
	real64 HexSize;
	real64 HexScale;
	vector2 CenterOffset;

	uint32 TableSize;
	axial CenterAxialOffset = axial{10, 10};
};

axial operator + (axial A, axial B)
{
	axial Final = {};
	Final.Q = A.Q + B.Q;
	Final.R = A.R + B.R;
	return (Final);
}

const axial AxialAdjacentOffsets[] = {
	axial{0, -1},
	axial{ -1, 0},
	axial{ -1, 1},
	axial{0, 1},
	axial{1, 0},
	axial{1, -1},
};
const uint32 AxialTotalAdjacentOffsets = 6;

cube_coords
AxialToCube(axial Axial)
{
	cube_coords Cube = {};
	Cube.X = Axial.Q;
	Cube.Z = Axial.R;
	Cube.Y = -Cube.X - Cube.Z;
	return (Cube);
}

int32
CubeDistance(cube_coords A, cube_coords B)
{
	int32 Dist = (Abs(A.X - B.X) + Abs(A.Y - B.Y) + Abs(A.Z - B.Z)) / 2;
	return (Dist);
}

int32
AxialDistance(axial A, axial B)
{
	return (CubeDistance(AxialToCube(A), AxialToCube(B)));
}


cube_coords
CubeRound(real64 X, real64 Y, real64 Z)
{
	int32 RoundedX = (int32)X;
	int32 RoundedY = (int32)Y;
	int32 RoundedZ = (int32)Z;

	real64 DiffX = Abs(RoundedX - X);
	real64 DiffY = Abs(RoundedY - Y);
	real64 DiffZ = Abs(RoundedZ - Z);

	if (DiffX > DiffY &&
	        DiffX > DiffZ) {
		RoundedX = -RoundedY - RoundedZ;
	} else if (DiffY > DiffZ) {
		RoundedY = -RoundedX - RoundedZ;
	} else {
		RoundedZ = -RoundedX - RoundedY;
	}

	return (cube_coords{RoundedX, RoundedY, RoundedZ});
}

// TODO remove the offset from here, should do that in the line, not in this method
vector2
AxialToPixel(real64 HexSize, axial Axial)
{
	vector2 Point = {};
	Point.X = HexSize * sqrt(3.0f) * (Axial.Q + (Axial.R / 2.0f));
	Point.Y = HexSize * (3.0f / 2.0f) * Axial.R;
	return (Point);
}


axial
CubeToAxial(cube_coords Cube)
{
	axial Axial = {};
	Axial.Q = Cube.X;
	Axial.R = Cube.Z;
	return (Axial);
}

axial
PixelToAxial(vector2 PixelPos, real64 HexSize)
{
	axial Axial = {};

	vector2 OffsetPos = PixelPos;

	real64 FloatingAxialQ = (((OffsetPos.X * (sqrt(3.0f) / 3.0f)) - (OffsetPos.Y / 3.0f)) / HexSize);
	real64 FloatingAxialR = ((OffsetPos.Y * (2.0f / 3.0f)) / HexSize);

	int32 AxialQ = (int32)FloatingAxialQ;
	int32 AxialR = (int32)FloatingAxialR;

	real64 RoundDiffQ = Abs(AxialQ - FloatingAxialQ);
	real64 RoundDiffR = Abs(AxialR - FloatingAxialR);

	if (RoundDiffQ > 0.5f) {
		if (FloatingAxialQ < 0) {
			AxialQ--;
		} else {
			AxialQ++;
		}
	}
	if (RoundDiffR > 0.5f) {
		if (FloatingAxialR < 0) {
			AxialR--;
		} else {
			AxialR++;
		}
	}

	axial FinalAxial = {AxialQ, AxialR};

	return (FinalAxial);
}

cube_coords
CubeRotateRight(cube_coords CubeInput)
{
	cube_coords Output = {};
	Output.X = -CubeInput.Z;
	Output.Y = -CubeInput.X;
	Output.Z = -CubeInput.Y;
	return (Output);
}

axial
AxialRotateRight(axial AxialInput)
{
	cube_coords CoordsCubed = AxialToCube(AxialInput);
	cube_coords CubeRotated = CubeRotateRight(CoordsCubed);
	return (CubeToAxial(CubeRotated));
}

void
FillAxialRing(axial Center, int32 Radius, axial* Dest)
{
	int32 NextRet = 0;

	axial Step = Center + axial{1 * Radius, 0};

	axial Adjacents[] = {
		axial{0, -1},
		axial{ -1, 0},
		axial{ -1, 1},
		axial{0, 1},
		axial{1, 0},
		axial{1, -1},
	};

	for (int i = 0; i < 6; i++) {
		for (int r = 0; r < Radius; r++) {
			Dest[NextRet] = Step;
			NextRet++;
			Step = Step + Adjacents[i];
		}
	}
}

axial*
GetAxialRing(axial Center, int32 Radius, memory_arena* Mem)
{
	axial* Ret = (axial*)ArenaAllocate(Mem, sizeof(axial) * Radius * 6);

	int32 NextRet = 0;

	axial Step = Center + axial{1 * Radius, 0};

	axial Adjacents[] = {
		axial{0, -1},
		axial{ -1, 0},
		axial{ -1, 1},
		axial{0, 1},
		axial{1, 0},
		axial{1, -1},
	};

	for (int i = 0; i < 6; i++) {
		for (int r = 0; r < Radius; r++) {
			Ret[NextRet] = Step;
			NextRet++;
			Step = Step + Adjacents[i];
		}
	}
	return (Ret);
}

// This is usefull for doing axial world wrapping. Always fills in seven centers.
void
HexGetMirrorCenters(axial* ArrayFilling, int32 WorldRadius)
{
	cube_coords FirstMirrorCenter = cube_coords{(2 * WorldRadius) + 1, -WorldRadius, -WorldRadius - 1};
	ArrayFilling[0] = CubeToAxial(FirstMirrorCenter);
	for (int index = 1; index < 6; index++) {
		FirstMirrorCenter = CubeRotateRight(FirstMirrorCenter);
		ArrayFilling[index] = CubeToAxial(FirstMirrorCenter);
	}
	ArrayFilling[6] = axial{0, 0};
}

// Returns the new axial position as if the world were circular. If not ouside the radius then returns the same position.
axial
HexGetCircularAxial(axial InputPos, axial* MirrorCenters, int32 WorldRadius)
{
	for (int32 I = 0; I < 7; I++) {
		int32 Dist = AxialDistance(InputPos, MirrorCenters[I]);
		if (Dist <= WorldRadius) {
			axial NewOutput = {};
			NewOutput.Q = InputPos.Q - MirrorCenters[I].Q;
			NewOutput.R = InputPos.R - MirrorCenters[I].R;
			return (NewOutput);
		}
	}

	// We should never get here. Position was probably off screen or something.
	return (axial{0, 0});
}

// Keep this. It might be useful at somepoint. Something is wrong with it though
/*
struct radius_hex_row {
	void* Array;
	uint32 RowLength;
};

struct radius_hex_store {
	radius_hex_row* Rows;
	uint32 RowsCount;
	uint32 Radius;
	uint32 DataSize;

	bool32 Allocated;
};

namespace RadiusHexStore
{

// NOTE the radius counts the center index. A radius of 1 is just 1 cell.
void Allocate(radius_hex_store* Store, uint32 Radius, uint32 IndexSize, memory_arena* Memory)
{
	Store->Radius = Radius;
	Store->DataSize = IndexSize;

	uint32 RowsCount = (Radius * 2) - 1;
	Store->RowsCount = RowsCount;

	Store->Rows = (radius_hex_row*)ArenaAllocate(Memory, sizeof(radius_hex_row) * RowsCount);

	// Allocate center row first (the longest row). Then work out from there allocating each top and bottom row.
	Store->Rows[Radius - 1].Array = ArenaAllocate(Memory, IndexSize * RowsCount);
	Store->Rows[Radius - 1].RowLength = Radius;
	for (uint32 R = 1; R <= Radius - 1; R++) {
		// -1 to offset for zero indexed
		Store->Rows[Radius - R - 1].Array = ArenaAllocate(Memory, IndexSize * (RowsCount - R));
		Store->Rows[Radius - R - 1].RowLength = RowsCount - R;
		Store->Rows[Radius + R - 1].Array = ArenaAllocate(Memory, IndexSize * (RowsCount - R));
		Store->Rows[Radius + R - 1].RowLength = RowsCount - R;

		ClearMemory((uint8*)Store->Rows[Radius - R - 1].Array, IndexSize * Store->Rows[Radius - R - 1].RowLength);
		ClearMemory((uint8*)Store->Rows[Radius + R - 1].Array, IndexSize * Store->Rows[Radius + R - 1].RowLength);
	}

	Store->Allocated = true;
}

void* GetData(axial Index, radius_hex_store* Store)
{
	// -1 because the arrays are zero indexed;
	Index.Q += 10000;
	Index.R += 10000;

	Assert(Index.R >= 0);
	Assert(Index.Q >= 0);

	Index.R = Index.R % Store->RowsCount;
	Assert((uint32)Index.R < Store->RowsCount);

	uint32 ArrayIndex = Index.Q % (Store->Rows[Index.R].RowLength);

	Assert(ArrayIndex < Store->Rows[Index.R].RowLength);

	char* ArrayStart = (char*)(&Store->Rows[Index.R].Array);
	ArrayStart = ArrayStart + (ArrayIndex * Store->DataSize);
	return ArrayStart;
}

real32 GetReal(axial Index, radius_hex_store* Store)
{
	void* Val = GetData(Index, Store);
	return *(real32*)Val;
}

void SetReal(axial Index, real32 Val, radius_hex_store* Store)
{
	Assert(Store->DataSize == sizeof(real32));

	real32* Data = (real32*)GetData(Index, Store);
	Data[0] = Val;
}

}
*/

#endif