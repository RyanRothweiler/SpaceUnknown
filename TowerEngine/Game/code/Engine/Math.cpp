#pragma once

#ifndef MathCPP
#define MathCPP

#include <math.h>
//#include <xmmintrin.h>

#include <cmath>


uint64
Pow(uint64 Num, uint32 Expo)
{
	if (Expo == 0) {
		return (1);
	} else {
		return (Num * Pow(Num, Expo - 1));
	}
}

real64
Lerp(real64 Start, real64 End, real64 Percentage)
{
	return (Start + (Percentage * (End - Start)));
}


real64
LerpCurve(real64 Start, real64 End, real64 CurveExp, real64 Percentage)
{
	real64 P = std::pow(Percentage, CurveExp);
	return Lerp(Start, End, P);
}


// ----------------------------------------------------------------------------- square
int64
Square(int64 input)
{
	return (input * input);
}

real64
Square(real64 input)
{
	return (input * input);
}


// ----------------------------------------------------------------------------- clamp
real64
ClampValue(real64 Bottom, real64 Top, real64 Value)
{
	if (Value <= Bottom) {
		return (Bottom);
	}
	if (Value >= Top) {
		return (Top);
	}
	return (Value);
}

int
ClampValue(int Bottom, int Top, int Value)
{
	if (Value <= Bottom) {
		return (Bottom);
	}
	if (Value >= Top) {
		return (Top);
	}
	return (Value);
}

int64
ClampValueInt64(int64 Bottom, int64 Top, int64 Value)
{
	if (Value <= Bottom) {
		return (Bottom);
	}
	if (Value >= Top) {
		return (Top);
	}
	return (Value);
}

uint64
ClampValue(uint64 Bottom, uint64 Top, uint64 Value)
{
	if (Value <= Bottom) {
		return (Bottom);
	}
	if (Value >= Top) {
		return (Top);
	}
	return (Value);
}

// ----------------------------------------------------------------------------- abs
real64
Abs(real64 A)
{
	if (A >= 0) {
		return (A);
	} else {
		return (A * -1);
	}
}

int64
Abs(int64 A)
{
	if (A >= 0) {
		return (A);
	} else {
		return (A * -1);
	}
}

uint64
Abs(uint64 A)
{
	if (A >= 0) {
		return (A);
	} else {
		return (A * -1);
	}
}

int32
Abs(int32 A)
{
	if (A >= 0) {
		return (A);
	} else {
		return (A * -1);
	}
}

// ----------------------------------------------------------------------------- max
real64
Max(real64 A, real64 B)
{
	if (A > B) {
		return (A);
	} else {
		return (B);
	}
}

uint64
Max(uint64 A, uint64 B)
{
	if (A > B) {
		return (A);
	} else {
		return (B);
	}
}

int64
Max(int64 A, int64 B)
{
	if (A > B) {
		return (A);
	} else {
		return (B);
	}
}


bool32 CloseEnough(real32 A, real32 B, real32 Dif = 0.0001f)
{
	if (Abs(A - B) < Dif) {
		return true;
	}
	return false;
}

bool32 CloseEnough(real64 A, real64 B, real64 Dif = 0.0001f)
{
	if (Abs(A - B) < Dif) {
		return true;
	}
	return false;
}


// ----------------------------------------------------------------------------- min
real64
Min(real64 A, real64 B)
{
	if (A < B) {
		return (A);
	} else {
		return (B);
	}
}

uint64
Min(uint64 A, uint64 B)
{
	if (A < B) {
		return (A);
	} else {
		return (B);
	}
}

// ----------------------------------------------------------------------------- trig

const real64 PI = 3.1415926535897932384626433832795;
const real64 E = 2.7182818284590452353602875;

real64 RadToDeg(real64 Rads)
{
	return (Rads * 180.0f) / PI;
}

real64 DegToRad(real64 Degs)
{
	return (Degs * PI) / 180.0f;
}

real64
SquareRoot(real64 num)
{
	return sqrt(num);
}

real64
Cos(real64 Input)
{
	return cos(Input);
}

real64
ArcCos(real64 Input)
{
	return acos(Input);
}

real64
ArcTan(real64 Input)
{
	return atan(Input);
}

real64
Sin(real64 Input)
{
	return sin(Input);
}

int64 Round(real64 Input)
{
	if (Input < 0.0f) {
		return (int64)(Input - 0.5f);
	} else {
		return (int64)(Input + 0.5f);
	}
}

int64 RoundUp(real64 Input)
{
	return (int64)(Input + 1);
}

real64 Coserp(real64 y1, real64 y2, real64 mu)
{
	real64 mu2;

	mu2 = (1 - cos(mu * PI)) / 2;
	return (y1 * (1 - mu2) + y2 * mu2);
}


// ----------------------------------------------------------------------------- vector3
struct vector3 {
	real64 X, Y, Z;
};

vector3 operator+(vector3 A, vector3 B)
{
	vector3 Ret = {};
	Ret.X = A.X + B.X;
	Ret.Y = A.Y + B.Y;
	Ret.Z = A.Z + B.Z;
	return Ret;
}

vector3 operator-(vector3 A, vector3 B)
{
	vector3 Ret = {};
	Ret.X = A.X - B.X;
	Ret.Y = A.Y - B.Y;
	Ret.Z = A.Z - B.Z;
	return Ret;
}

vector3 operator/(vector3 A, real64 B)
{
	vector3 Ret = {};
	Ret.X = A.X / B;
	Ret.Y = A.Y / B;
	Ret.Z = A.Z / B;
	return Ret;
}

vector3 operator*(vector3 A, real32 B)
{
	vector3 Output;
	Output.X = A.X * B;
	Output.Y = A.Y * B;
	Output.Z = A.Z * B;
	return (Output);
}

vector3
Vector3Lerp(vector3 Start, vector3 End, real64 Percentage)
{
	vector3 Result = {};
	Result.X = Lerp(Start.X, End.X, Percentage);
	Result.Y = Lerp(Start.Y, End.Y, Percentage);
	Result.Z = Lerp(Start.Z, End.Z, Percentage);
	return (Result);
}

real32
Vector3Dot(vector3 A, vector3 B)
{
	real64 Result;
	Result = (A.X * B.X) + (A.Y * B.Y) + (A.Z * B.Z);
	return (real32)Result;
}

vector3
Vector3Cross(vector3 A, vector3 B)
{
	vector3 Output = {};

	Output.X = (A.Y * B.Z) - (A.Z * B.Y);
	Output.Y = (A.Z * B.X) - (A.X * B.Z);
	Output.Z = (A.X * B.Y) - (A.Y * B.X);

	return Output;
}


real64
Vector3Length(vector3 A)
{
	return (SquareRoot((A.X * A.X) + (A.Y * A.Y) + (A.Z * A.Z)));
}

vector3
Vector3Normalize(vector3 Input)
{
	vector3 Output = {};

	real64 Length = Vector3Length(Input);
	Output.X = Input.X / Length;
	Output.Y = Input.Y / Length;
	Output.Z = Input.Z / Length;

	return (Output);
}

real64
Vector3Distance(vector3 A, vector3 B)
{
	real64 Output = 0;
	real64 ASideLength = Abs(A.X - B.X);
	real64 BSideLength = Abs(A.Y - B.Y);
	real64 CSideLength = Abs(A.Z - B.Z);
	Output = SquareRoot((ASideLength * ASideLength) + (BSideLength * BSideLength) + (CSideLength * CSideLength));
	return (Abs(Output));
}

// ----------------------------------------------------------------------------- vector2

struct vector2 {
	real64 X;
	real64 Y;
};

const vector2 VECTOR2_ZERO = vector2{0, 0};

vector2
operator+(vector2 A, vector2 B)
{
	vector2 Output;
	Output.X = A.X + B.X;
	Output.Y = A.Y + B.Y;
	return (Output);
}

vector2
operator+(real64 B, vector2 A)
{
	vector2 Output;
	Output.X = A.X + B;
	Output.Y = A.Y + B;
	return (Output);
}

vector2
operator-(vector2 A, vector2 B)
{
	vector2 Output;
	Output.X = A.X - B.X;
	Output.Y = A.Y - B.Y;
	return (Output);
}

vector2
operator/(vector2 A, int32 B)
{
	vector2 Output;
	Output.X = A.X / B;
	Output.Y = A.Y / B;
	return (Output);
}

vector2
operator/(vector2 A, real64 B)
{
	vector2 Output;
	Output.X = A.X / B;
	Output.Y = A.Y / B;
	return (Output);
}

vector2
operator/(real64 B, vector2 A)
{
	vector2 Output;
	Output.X = A.X / B;
	Output.Y = A.Y / B;
	return (Output);
}

vector2
operator*(vector2 A, real64 B)
{
	vector2 Output;
	Output.X = A.X * B;
	Output.Y = A.Y * B;
	return (Output);
}

vector2
operator*(vector2 A, float B)
{
	vector2 Output;
	Output.X = A.X * B;
	Output.Y = A.Y * B;
	return (Output);
}

vector2
operator*(float B, vector2 A)
{
	vector2 Output;
	Output.X = A.X * B;
	Output.Y = A.Y * B;
	return (Output);
}

vector2
operator*(vector2 A, int64 B)
{
	vector2 Output;
	Output.X = A.X * B;
	Output.Y = A.Y * B;
	return (Output);
}

vector2
operator*(vector2 A, vector2 B)
{
	vector2 Output;
	Output.X = A.X * B.X;
	Output.Y = A.Y * B.Y;
	return (Output);
}

bool32
operator>(vector2 A, int32 B)
{
	if (A.X > B &&
	        A.Y > B) {
		return (true);
	}
	return (false);
}

bool32
operator==(vector2 A, vector2 B)
{
	return (A.X == B.X) && (A.Y == B.Y);
}

real64
DotProduct(vector2 A, vector2 B)
{
	real64 Result;
	Result = (A.X * B.X) + (A.Y * B.Y);
	return (Result);
}

real64
DotProduct(vector3 A, vector3 B)
{
	real64 Result;
	Result = (A.X * B.X) + (A.Y * B.Y) + (A.Z * B.Z);
	return (Result);
}

real64
Vector2Length(vector2 A)
{
	return (SquareRoot((A.X * A.X) + (A.Y * A.Y)));
}

vector2
Vector2Perp(vector2 Input)
{
	vector2 Output = {};
	Output.X = -Input.Y;
	Output.Y = Input.X;
	return Output;
}

vector2
Vector2Normalize(vector2 Input)
{
	if (Input.X == 0 && Input.Y == 0) {
		return Input;
	}

	vector2 Output = {};

	real64 Length = Vector2Length(Input);
	Output.X = Input.X / Length;
	Output.Y = Input.Y / Length;

	return (Output);
}

real64
Vector2Distance(vector2 A, vector2 B)
{
	real64 Output = 0;
	real64 ASideLength = Abs(A.X - B.X);
	real64 BSideLength = Abs(A.Y - B.Y);
	Output = SquareRoot((ASideLength * ASideLength) + (BSideLength * BSideLength));
	return (Abs(Output));
}

// http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html
real64
Vector2DiagionalDistance(vector2 A, vector2 B)
{
	// Cost to move N,S,E,W
	real32 D = 1;
	// Cost to move diagionally
	real32 D2 = 1;

	real64 dx = Abs(A.X - B.X);
	real64 dy = Abs(A.Y - B.Y);

	return D * (dx + dy) + (D2 - 2 * D) * Min(dx, dy);
}

uint32
Vector2TaxiDistance(vector2 A, vector2 B)
{
	return (uint32)(Abs(A.X - B.X) + Abs(A.Y - B.Y));
}

// Returns in radians
real64
Vector2AngleBetween(vector2 A, vector2 B)
{
	// Vectors can't be the same
	if (A.X != B.X || A.Y != B.Y) {

		real64 ALen = Vector2Length(A);
		real64 BLen = Vector2Length(B);

		// Vectors can't by zero
		if (ALen > 0 && BLen > 0) {

			real64 Result = 0;
			Result = acos(DotProduct(A, B) / (ALen * BLen));
			return (Result);
		}
	}

	return 0;
}

real64
Vector2GetDimension(uint8 Dimension, vector2 Vector)
{
	if (Dimension == 0) {
		return (Vector.X);
	}
	if (Dimension == 1) {
		return (Vector.Y);
	}

	// We probably tried to get a dimension which is not 0 or 1

	Assert(0);
	return (0);
}

vector2
Vector2RotatePoint(vector2 OriginalPoint, vector2 Center, real64 AngleRadians)
{
	if (AngleRadians == 0) {
		return (OriginalPoint);
	}

	vector2 Result = {};
	Result.X = Center.X + ((OriginalPoint.X - Center.X) * cos(AngleRadians)) + ((OriginalPoint.Y - Center.Y) * sin(AngleRadians));
	Result.Y = Center.Y - ((OriginalPoint.X - Center.X) * sin(AngleRadians)) + ((OriginalPoint.Y - Center.Y) * cos(AngleRadians));
	return (Result);
}

//starting as Base, this returns the cardinal direction you would need to move to get to To
vector2
Vector2GetCardinalDirection(vector2 Base, vector2 To, uint32 WidthSum)
{
	if (Base.X > (To.X + WidthSum)) {
		return (vector2{1, 0});
	}
	if (Base.X < (To.X - WidthSum)) {
		return (vector2{ -1, 0});
	}
	if (Base.Y > (To.Y + WidthSum)) {
		return (vector2{0, 1});
	}
	if (Base.Y < (To.Y - WidthSum)) {
		return (vector2{0, -1});
	}

	return (vector2{0, 0});
}

vector2
Vector2Lerp(vector2 Start, vector2 End, real64 Percentage)
{
	vector2 Result = {};
	Result.X = Lerp(Start.X, End.X, Percentage);
	Result.Y = Lerp(Start.Y, End.Y, Percentage);
	return (Result);
}

vector2
ClampValue(real64 Bottom, real64 Top, vector2 Value)
{
	vector2 Output;
	Output.X = ClampValue(Bottom, Top, Value.X);
	Output.Y = ClampValue(Bottom, Top, Value.Y);
	return (Output);
}


real64 TriangleArea(vector2 APoint, vector2 BPoint, vector2 CPoint)
{
	real64 ASideLength = Vector2Distance(APoint, CPoint);
	real64 BSideLength = Vector2Distance(BPoint, CPoint);

	vector2 ASideDir = Vector2Normalize(CPoint - APoint);
	vector2 BSideDir = Vector2Normalize(CPoint - BPoint);
	real64 ABDeg = Vector2AngleBetween(ASideDir, BSideDir);

	real64 Area = 0.5f * ASideLength * BSideLength * Sin(ABDeg);
	return Area;
}

bool32 RectContainsPoint(vector2 Point, vector2 A, vector2 B, vector2 C, vector2 D, real64 RectArea)
{
	real64 APD = TriangleArea(A, Point, D);
	real64 DPC = TriangleArea(D, Point, C);
	real64 CPB = TriangleArea(C, Point, B);
	real64 PBA = TriangleArea(Point, B, A);

	return APD + DPC + CPB + PBA < RectArea;
}

// Between line AB and line CD
struct intersection_point {
	bool32 Valid;
	vector2 Point;
};
intersection_point GetIntersection(vector2 A, vector2 B, vector2 C, vector2 D)
{
	intersection_point P = {};

	double a1 = B.Y - A.Y;
	double b1 = A.X - B.X;
	double c1 = (a1 * A.X) + (b1 * A.Y);

	double a2 = D.Y - C.Y;
	double b2 = C.X - D.X;
	double c2 = (a2 * C.X) + (b2 * C.Y);

	double determinant = (a1 * b2) - (a2 * b1);

	// There is no intersection
	if (determinant == 0) {
		P.Valid = false;
		return P;
	}

	double xx = (b2 * c1 - b1 * c2) / determinant;
	double yy = (a1 * c2 - a2 * c1) / determinant;

	P.Valid = true;
	P.Point = vector2{xx, yy};
	return P;
}

bool32 TriContainsPoint(vector2 Point, vector2 A, vector2 B, vector2 C)
{
	real64 abc = TriangleArea(A, B, C);

	real64 pbc = TriangleArea(Point, B, C);
	real64 apc = TriangleArea(A, Point, C);
	real64 abp = TriangleArea(A, B, Point);

	// Include a little buffer for floats
	return Abs(abc - (pbc + apc + abp)) < 0.1f;
}

// ----------------------------------------------------------------------------- m3y3
struct m3y3 {
	real32 E[3][3];
};

m3y3 Transpose3y3(m3y3 Matrix)
{
	m3y3 New = {};

	New.E[0][0] = Matrix.E[0][0];
	New.E[0][1] = Matrix.E[1][0];
	New.E[0][2] = Matrix.E[2][0];

	New.E[1][0] = Matrix.E[0][1];
	New.E[1][1] = Matrix.E[1][1];
	New.E[1][2] = Matrix.E[2][1];

	New.E[2][0] = Matrix.E[0][2];
	New.E[2][1] = Matrix.E[1][2];
	New.E[2][2] = Matrix.E[2][2];

	return New;
};

m3y3 MatrixMultiply(m3y3 A, m3y3 B)
{
	m3y3 Ret = {};

	// Row 0
	Ret.E[0][0] = (A.E[0][0] * B.E[0][0]) + (A.E[1][0] * B.E[0][1]) + (A.E[2][0] * B.E[0][2]);
	Ret.E[1][0] = (A.E[0][0] * B.E[1][0]) + (A.E[1][0] * B.E[1][1]) + (A.E[2][0] * B.E[1][2]);
	Ret.E[2][0] = (A.E[0][0] * B.E[2][0]) + (A.E[1][0] * B.E[2][1]) + (A.E[2][0] * B.E[2][2]);

	// row 1
	Ret.E[0][1] = (A.E[0][1] * B.E[0][0]) + (A.E[1][1] * B.E[0][1]) + (A.E[2][1] * B.E[0][2]);
	Ret.E[1][1] = (A.E[0][1] * B.E[1][0]) + (A.E[1][1] * B.E[1][1]) + (A.E[2][1] * B.E[1][2]);
	Ret.E[2][1] = (A.E[0][1] * B.E[2][0]) + (A.E[1][1] * B.E[2][1]) + (A.E[2][1] * B.E[2][2]);

	// row 2
	Ret.E[0][2] = (A.E[0][2] * B.E[0][0]) + (A.E[1][2] * B.E[0][1]) + (A.E[2][2] * B.E[0][2]);
	Ret.E[1][2] = (A.E[0][2] * B.E[1][0]) + (A.E[1][2] * B.E[1][1]) + (A.E[2][2] * B.E[1][2]);
	Ret.E[2][2] = (A.E[0][2] * B.E[2][0]) + (A.E[1][2] * B.E[2][1]) + (A.E[2][2] * B.E[2][2]);
	return Ret;

	/*
	// TODO maybe finish this simd change. I started this, but it wasn't the cause of the slowdown so I didn't finish it.

	__m128 X = {A.E[0][0], A.E[1][0], A.E[2][0], A.E[0][0]};
	__m128 Y = {B.E[0][0], B.E[0][1], B.E[0][2], B.E[1][0]};
	__m128 ResultOne = _mm_mul_ps(X, Y);

	__m128 Z = {A.E[1][0], A.E[2][0], A.E[0][0], A.E[1][0]};
	__m128 W = {B.E[1][1], B.E[1][1], B.E[2][0], B.E[2][1]};
	__m128 ResultTwo = _mm_mul_ps(Z, W);

	__m128 C = {A.E[0][1], A.E[1][1], A.E[2][1], A.E[0][1]};
	__m128 D = {B.E[0][0], B.E[0][1], B.E[0][2], B.E[1][0]};
	__m128 ResultThree = _mm_mul_ps(C, D);

	__m128 EE = {A.E[1][1], A.E[2][0], A.E[2][1], A.E[0][1]};
	__m128 F = {B.E[1][1], B.E[2][2], B.E[1][2], B.E[2][0]};
	__m128 ResultFour = _mm_mul_ps(Z, W);

	// row 0
	Ret.E[0][0] = (ResultOne.m128_f32[0]) + (ResultOne.m128_f32[1]) + (ResultOne.m128_f32[2]);
	Ret.E[1][0] = (ResultOne.m128_f32[3]) + (ResultTwo.m128_f32[0]) + (ResultTwo.m128_f32[1]);
	Ret.E[2][0] = (ResultTwo.m128_f32[2]) + (ResultTwo.m128_f32[3]) + (ResultFour.m128_f32[1]);

	// row 1
	Ret.E[0][1] = (ResultThree.m128_f32[0]) + (ResultThree.m128_f32[1]) + (ResultThree.m128_f32[2]);
	Ret.E[1][1] = (ResultThree.m128_f32[3]) + (ResultFour.m128_f32[0]) + (ResultFour.m128_f32[2]);
	Ret.E[2][1] = (ResultFour.m128_f32[3]) + (A.E[1][1] * B.E[2][1]) + (A.E[2][1] * B.E[2][2]);

	// row 2
	Ret.E[0][2] = (A.E[0][2] * B.E[0][0]) + (A.E[1][2] * B.E[0][1]) + (A.E[2][2] * B.E[0][2]);
	Ret.E[1][2] = (A.E[0][2] * B.E[1][0]) + (A.E[1][2] * B.E[1][1]) + (A.E[2][2] * B.E[1][2]);
	Ret.E[2][2] = (A.E[0][2] * B.E[2][0]) + (A.E[1][2] * B.E[2][1]) + (A.E[2][2] * B.E[2][2]);

	return Ret;
	*/
}

vector3 ApplyMatrix(m3y3 Matrix, vector3 Point)
{
	// Why do we need to tranpose?
	Matrix = Transpose3y3(Matrix);

	// Choice 0 is slightly slower??
	int choice = 1;

	if (choice == 0) {
		/*
		__m256 P1 = {Matrix.E[0][0], 	Matrix.E[1][0], 	Matrix.E[2][0], 	Matrix.E[0][1], 	Matrix.E[1][1], 	Matrix.E[2][1], 	Matrix.E[0][2], 	Matrix.E[1][2]};
		__m256 P2 = {(float)Point.X, 	(float)Point.Y, 	(float)Point.Z, 	(float)Point.X, 	(float)Point.Y, 	(float)Point.Z, 	(float)Point.X, 	(float)Point.Y};
		__m256 Results = _mm256_mul_ps(P1, P2);

		return vector3 {
			Results.m256_f32[0] + Results.m256_f32[1] + Results.m256_f32[2],
			Results.m256_f32[3] + Results.m256_f32[4] + Results.m256_f32[5],
			Results.m256_f32[6] + Results.m256_f32[7] + (Matrix.E[2][2] * Point.Z),
		};
		*/

	} else if (choice == 1) {
		/*
		__m128 P1 = {Matrix.E[1][1], 	Matrix.E[2][1], 	Matrix.E[0][2], 	Matrix.E[1][2]};
		__m128 P2 = {(float)Point.Y, 	(float)Point.Z, 	(float)Point.X, 	(float)Point.Y};
		__m128 ResultsOne = _mm_mul_ps(P1, P2);

		__m128 P3 = {Matrix.E[0][0], 	Matrix.E[1][0], 	Matrix.E[2][0], 	Matrix.E[0][1]};
		__m128 P4 = {(float)Point.X, 	(float)Point.Y, 	(float)Point.Z, 	(float)Point.X};
		__m128 ResultsTwo = _mm_mul_ps(P3, P4);

		return vector3 {
			ResultsTwo.m128_f32[0] + ResultsTwo.m128_f32[1] + ResultsTwo.m128_f32[2],
			ResultsTwo.m128_f32[3] + ResultsOne.m128_f32[0] + ResultsOne.m128_f32[1],
			ResultsOne.m128_f32[2] + ResultsOne.m128_f32[3] + (Matrix.E[2][2] * Point.Z),
		};
		*/
	}

	return vector3 {
		(Matrix.E[0][0] * Point.X) + (Matrix.E[1][0] * Point.Y) + (Matrix.E[2][0] * Point.Z),
		(Matrix.E[0][1] * Point.X) + (Matrix.E[1][1] * Point.Y) + (Matrix.E[2][1] * Point.Z),
		(Matrix.E[0][2] * Point.X) + (Matrix.E[1][2] * Point.Y) + (Matrix.E[2][2] * Point.Z),
	};
}

// ----------------------------------------------------------------------------- vector4
struct vector4 {
	real64 X, Y, Z, W;

	vector3 ToThree()
	{
		return {X, Y, Z};
	}
};

vector4 operator/(vector4 A, real64 B)
{
	vector4 Ret = {};
	Ret.X = A.X / B;
	Ret.Y = A.Y / B;
	Ret.Z = A.Z / B;
	Ret.W = A.W / B;
	return Ret;
}

// ----------------------------------------------------------------------------- m4y4
struct m4y4 {
	real32 E[4][4];

	vector3 GetTranslation()
	{
		vector3 Loc = {};
		Loc.X = E[0][3];
		Loc.Y = E[1][3];
		Loc.Z = E[2][3];
		return Loc;
	}

	vector3 GetScale()
	{
		vector3 X = vector3{E[0][0], E[1][0], E[2][0]};
		vector3 Y = vector3{E[0][1], E[1][1], E[2][1]};
		vector3 Z = vector3{E[0][2], E[1][2], E[2][2]};

		vector3 Ret = {};
		Ret.X = Vector3Length(X);
		Ret.Y = Vector3Length(Y);
		Ret.Z = Vector3Length(Z);

		return Ret;
	}
};

m4y4 m4y4Identity()
{
	m4y4 Ret = {};
	Ret.E[0][0] = 1;
	Ret.E[1][1] = 1;
	Ret.E[2][2] = 1;
	Ret.E[3][3] = 1;
	return Ret;
}


vector4 Apply4y4_4(m4y4 Matrix, vector4 Point)
{
	vector4 Ret = {};

	Ret.X = (Matrix.E[0][0] * Point.X) + (Matrix.E[0][1] * Point.Y) + (Matrix.E[0][2] * Point.Z) + (Matrix.E[0][3] * Point.W);
	Ret.Y = (Matrix.E[1][0] * Point.X) + (Matrix.E[1][1] * Point.Y) + (Matrix.E[1][2] * Point.Z) + (Matrix.E[1][3] * Point.W);
	Ret.Z = (Matrix.E[2][0] * Point.X) + (Matrix.E[2][1] * Point.Y) + (Matrix.E[2][2] * Point.Z) + (Matrix.E[2][3] * Point.W);
	Ret.W = (Matrix.E[3][0] * Point.X) + (Matrix.E[3][1] * Point.Y) + (Matrix.E[3][2] * Point.Z) + (Matrix.E[3][3] * Point.W);

	return Ret;
}

vector3 Apply4y4(m4y4 Matrix, vector3 Point, real32 W = 1.0f)
{
	vector3 Ret = {};

	Ret.X = (Matrix.E[0][0] * Point.X) + (Matrix.E[0][1] * Point.Y) + (Matrix.E[0][2] * Point.Z) + (Matrix.E[0][3] * W);
	Ret.Y = (Matrix.E[1][0] * Point.X) + (Matrix.E[1][1] * Point.Y) + (Matrix.E[1][2] * Point.Z) + (Matrix.E[1][3] * W);
	Ret.Z = (Matrix.E[2][0] * Point.X) + (Matrix.E[2][1] * Point.Y) + (Matrix.E[2][2] * Point.Z) + (Matrix.E[2][3] * W);

	return Ret;
}

m4y4 MatrixMultiply(m4y4 A, m4y4 B)
{
	m4y4 Ret = {};

	// Row 0
	Ret.E[0][0] = (A.E[0][0] * B.E[0][0]) + (A.E[1][0] * B.E[0][1]) + (A.E[2][0] * B.E[0][2]) + (A.E[3][0] * B.E[0][3]);
	Ret.E[1][0] = (A.E[0][0] * B.E[1][0]) + (A.E[1][0] * B.E[1][1]) + (A.E[2][0] * B.E[1][2]) + (A.E[3][0] * B.E[1][3]);
	Ret.E[2][0] = (A.E[0][0] * B.E[2][0]) + (A.E[1][0] * B.E[2][1]) + (A.E[2][0] * B.E[2][2]) + (A.E[3][0] * B.E[2][3]);
	Ret.E[3][0] = (A.E[0][0] * B.E[3][0]) + (A.E[1][0] * B.E[3][1]) + (A.E[2][0] * B.E[3][2]) + (A.E[3][0] * B.E[3][3]);

	// row 1
	Ret.E[0][1] = (A.E[0][1] * B.E[0][0]) + (A.E[1][1] * B.E[0][1]) + (A.E[2][1] * B.E[0][2]) + (A.E[3][1] * B.E[0][3]);
	Ret.E[1][1] = (A.E[0][1] * B.E[1][0]) + (A.E[1][1] * B.E[1][1]) + (A.E[2][1] * B.E[1][2]) + (A.E[3][1] * B.E[1][3]);
	Ret.E[2][1] = (A.E[0][1] * B.E[2][0]) + (A.E[1][1] * B.E[2][1]) + (A.E[2][1] * B.E[2][2]) + (A.E[3][1] * B.E[2][3]);
	Ret.E[3][1] = (A.E[0][1] * B.E[3][0]) + (A.E[1][1] * B.E[3][1]) + (A.E[2][1] * B.E[3][2]) + (A.E[3][1] * B.E[3][3]);

	// row 2
	Ret.E[0][2] = (A.E[0][2] * B.E[0][0]) + (A.E[1][2] * B.E[0][1]) + (A.E[2][2] * B.E[0][2]) + (A.E[3][2] * B.E[0][3]);
	Ret.E[1][2] = (A.E[0][2] * B.E[1][0]) + (A.E[1][2] * B.E[1][1]) + (A.E[2][2] * B.E[1][2]) + (A.E[3][2] * B.E[1][3]);
	Ret.E[2][2] = (A.E[0][2] * B.E[2][0]) + (A.E[1][2] * B.E[2][1]) + (A.E[2][2] * B.E[2][2]) + (A.E[3][2] * B.E[2][3]);
	Ret.E[3][2] = (A.E[0][2] * B.E[3][0]) + (A.E[1][2] * B.E[3][1]) + (A.E[2][2] * B.E[3][2]) + (A.E[3][2] * B.E[3][3]);

	// row 2
	Ret.E[0][3] = (A.E[0][3] * B.E[0][0]) + (A.E[1][3] * B.E[0][1]) + (A.E[2][3] * B.E[0][2]) + (A.E[3][3] * B.E[0][3]);
	Ret.E[1][3] = (A.E[0][3] * B.E[1][0]) + (A.E[1][3] * B.E[1][1]) + (A.E[2][3] * B.E[1][2]) + (A.E[3][3] * B.E[1][3]);
	Ret.E[2][3] = (A.E[0][3] * B.E[2][0]) + (A.E[1][3] * B.E[2][1]) + (A.E[2][3] * B.E[2][2]) + (A.E[3][3] * B.E[2][3]);
	Ret.E[3][3] = (A.E[0][3] * B.E[3][0]) + (A.E[1][3] * B.E[3][1]) + (A.E[2][3] * B.E[3][2]) + (A.E[3][3] * B.E[3][3]);

	return Ret;
}

m4y4 m4y4Transpose(m4y4 Matrix)
{
	m4y4 New = {};

	New.E[0][0] = Matrix.E[0][0];
	New.E[0][1] = Matrix.E[1][0];
	New.E[0][2] = Matrix.E[2][0];
	New.E[0][3] = Matrix.E[3][0];

	New.E[1][0] = Matrix.E[0][1];
	New.E[1][1] = Matrix.E[1][1];
	New.E[1][2] = Matrix.E[2][1];
	New.E[1][3] = Matrix.E[3][1];

	New.E[2][0] = Matrix.E[0][2];
	New.E[2][1] = Matrix.E[1][2];
	New.E[2][2] = Matrix.E[2][2];
	New.E[2][3] = Matrix.E[3][2];

	New.E[3][0] = Matrix.E[0][3];
	New.E[3][1] = Matrix.E[1][3];
	New.E[3][2] = Matrix.E[2][3];
	New.E[3][3] = Matrix.E[3][3];

	return New;
};

m4y4 Translate(m4y4 Matrix, vector3 Translation)
{
	m4y4 Trans = m4y4Identity();
	Trans.E[0][3] = (real32)Translation.X;
	Trans.E[1][3] = (real32)Translation.Y;
	Trans.E[2][3] = (real32)Translation.Z;

	return MatrixMultiply(Matrix, Trans);
}

m4y4 Scale(m4y4 Matrix, vector3 Scale)
{
	m4y4 S = {
		{
			{(real32)Scale.X, 	0,  				0, 					0},
			{0, 				(real32)Scale.Y, 	0, 					0},
			{0, 				0, 					(real32)Scale.Z, 	0},
			{0, 				0, 					0, 					1},
		}
	};

	return MatrixMultiply(Matrix, S);
}
// ----------------------------------------------------------------------------- Quaternions
struct quat {
	real64 x, y, z, w;

	void Normalize()
	{
		real64 Len = SquareRoot((x * x) + (y * y) + (z * z) + (w * w));
		x = x / Len;
		y = y / Len;
		z = z / Len;
		w = w / Len;
	}

	m4y4 ToMatrix()
	{
		real32 qx = (real32)x;
		real32 qy = (real32)y;
		real32 qz = (real32)z;
		real32 qw = (real32)w;

		real32 n = (real32)(1.0f / sqrt(qx * qx + qy * qy + qz * qz + qw * qw));
		qx *= n;
		qy *= n;
		qz *= n;
		qw *= n;

		m4y4 Mat = {};

		Mat.E[0][0] = 1.0f - 2.0f * qy * qy - 2.0f * qz * qz;
		Mat.E[1][0] = 2.0f * qx * qy - 2.0f * qz * qw;
		Mat.E[2][0] = 2.0f * qx * qz + 2.0f * qy * qw;
		Mat.E[3][0] = 0.0f;

		Mat.E[0][1] = 2.0f * qx * qy + 2.0f * qz * qw;
		Mat.E[1][1] = 1.0f - 2.0f * qx * qx - 2.0f * qz * qz;
		Mat.E[2][1] = 2.0f * qy * qz - 2.0f * qx * qw;
		Mat.E[3][1] = 0.0f;

		Mat.E[0][2] = 2.0f * qx * qz - 2.0f * qy * qw;
		Mat.E[1][2] = 2.0f * qy * qz + 2.0f * qx * qw;
		Mat.E[2][2] = 1.0f - 2.0f * qx * qx - 2.0f * qy * qy;
		Mat.E[3][2] = 0.0f;

		Mat.E[0][3] = 0.0f;
		Mat.E[1][3] = 0.0f;
		Mat.E[2][3] = 0.0f;
		Mat.E[3][3] = 1.0f;

		return m4y4Transpose(Mat);
	}

	vector3 ToEuler()
	{
		vector3 angles;

		// roll (x-axis rotation)
		double sinr_cosp = 2 * (w * x + y * z);
		double cosr_cosp = 1 - 2 * (x * x + y * y);
		angles.X = atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = 2 * (w * y - z * x);
		if (Abs(sinp) >= 1) {
			angles.Y = copysign(PI / 2, sinp); // use 90 degrees if out of range
		} else {
			angles.Y = asin(sinp);
		}

		// yaw (z-axis rotation)
		double siny_cosp = 2 * (w * z + x * y);
		double cosy_cosp = 1 - 2 * (y * y + z * z);
		angles.Z = atan2(siny_cosp, cosy_cosp);

		return angles;
	}

	void FromMat(m4y4* Mat)
	{
		w = SquareRoot(1.0f + Mat->E[0][0] + Mat->E[1][1] + Mat->E[2][2]) / 2.0f;
		real64 w4 = (4.0f * w);
		x = (Mat->E[2][1] - Mat->E[1][2]) / w4;
		y = (Mat->E[0][2] - Mat->E[2][0]) / w4;
		z = (Mat->E[1][0] - Mat->E[0][1]) / w4;
	}

	void FromEuler(vector3 Euler)
	{
		real64 cy = cos(Euler.Z * 0.5f);
		real64 sy = sin(Euler.Z * 0.5f);
		real64 cp = cos(Euler.Y * 0.5f);
		real64 sp = sin(Euler.Y * 0.5f);
		real64 cr = cos(Euler.X * 0.5f);
		real64 sr = sin(Euler.X * 0.5f);

		w = cr * cp * cy + sr * sp * sy;
		x = sr * cp * cy - cr * sp * sy;
		y = cr * sp * cy + sr * cp * sy;
		z = cr * cp * sy - sr * sp * cy;
	}

	void AngleAxis( real64 AngleRad, vector3 Axis)
	{
		x = Axis.X * sin(AngleRad / 2);
		y = Axis.Y * sin(AngleRad / 2);
		z = Axis.Z * sin(AngleRad / 2);
		w = cos(AngleRad / 2);
	}
};

quat operator *(quat A, real64 X)
{
	quat Ret = {};
	Ret.x = A.x * X;
	Ret.y = A.y * X;
	Ret.z = A.z * X;
	Ret.w = A.w * X;
	return Ret;
}

quat operator *(real64 X, quat A)
{
	quat Ret = {};
	Ret.x = A.x * X;
	Ret.y = A.y * X;
	Ret.z = A.z * X;
	Ret.w = A.w * X;
	return Ret;
}

quat operator +(quat A, quat B)
{
	quat Ret = {};
	Ret.x = A.x + B.x;
	Ret.y = A.y + B.y;
	Ret.z = A.z + B.z;
	Ret.w = A.w + B.w;
	return Ret;
}

real64 DotProduct(quat A, quat B)
{
	return (A.x * B.x) + (A.y * B.y) + (A.z * B.z) + (A.w * B.w);
}

quat QuatLerp(quat Start, quat End, real64 T)
{
	quat Result = {};
	Result.x = Lerp(Start.x, End.x, T);
	Result.y = Lerp(Start.y, End.y, T);
	Result.z = Lerp(Start.z, End.z, T);
	Result.w = Lerp(Start.w, End.w, T);
	return (Result);
}

quat RotationBetweenVectors(vector3 start, vector3 dest)
{
	start = Vector3Normalize(start);
	dest = Vector3Normalize(dest);

	float cosTheta = Vector3Dot(start, dest);
	vector3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = Vector3Cross(vector3{1.0f, 0.0f, 0.0f}, start);

		/*
		if (gtx::norm::length2(rotationAxis) < 0.01 ) {
			// bad luck, they were parallel, try again!
			rotationAxis = Vector3Cross(vector3(1.0f, 0.0f, 0.0f), start);
		}
		*/

		rotationAxis = Vector3Normalize(rotationAxis);

		quat Ret = {};
		Ret.AngleAxis(DegToRad(180.0f), rotationAxis);
		return Ret;
	}

	rotationAxis = Vector3Cross(start, dest);

	real64 s = sqrt( (1 + cosTheta) * 2 );
	real64 invs = 1 / s;

	return quat{
		rotationAxis.X * invs,
		rotationAxis.Y * invs,
		rotationAxis.Z * invs,
		s * 0.5f
	};
}

quat LookAt(vector3 Direction, vector3 Up = {0, 0, 1})
{
	quat rot1 = RotationBetweenVectors(Up, Direction);

	/*
	// Recompute desiredUp so that it's perpendicular to the direction
	// You can skip that part if you really want to force desiredUp
	vector3 right = Vector3Cross(Direction, up);
	up = Vector3Cross(right, Direction);

	// Because of the 1rst rotation, the up is probably completely screwed up.
	// Find the rotation between the "up" of the rotated object, and the desired up

	vector3 newUp = rot1 * vector3{0.0f, 1.0f, 0.0f};
	quat rot2 = RotationBetweenVectors(newUp, up);

	quat targetOrientation = rot2 * rot1; // remember, in reverse order.
	return targetOrientation;
	*/

	return rot1;
}

/*
m4y4 RotationMatrix(real64 X, real64 Y, real64 Z)
{
	m4y4 xMat = {{
			{1, 		0, 					0, 					0},
			{0, 		(real32)cos(X), 	(real32) - sin(X), 	0},
			{0, 		(real32)sin(X), 	(real32)cos(X), 	0},
			{0, 		0, 					0, 					1}
		}
	};

	m4y4 zMat = {{
			{(real32)cos(Z), 	(real32) - sin(Z), 	0, 		0},
			{(real32)sin(Z), 	(real32)cos(Z), 	0, 		0},
			{0, 				0, 					1, 		0},
			{0, 				0, 					0, 		1}
		}
	};

	return zMat;
}
*/

m4y4 Rotate(m4y4 Matrix, vector3 Rot)
{
	quat Q = {};
	Q.FromEuler(Rot);
	return MatrixMultiply(Q.ToMatrix(), Matrix);
}

struct aabb {
	real64 XMax, XMin;
	real64 YMax, YMin;
	real64 ZMax, ZMin;
};

vector3 AABBCenter(aabb* Input)
{
	vector3 Ret = {};
	Ret.X = (Input->XMax + Input->XMin) * 0.5f;
	Ret.Y = (Input->YMax + Input->YMin) * 0.5f;
	Ret.Z = (Input->ZMax + Input->ZMin) * 0.5f;
	return Ret;
}

aabb AABBApplyPosition(aabb A, vector3 Pos)
{
	A.XMax += Pos.X;
	A.XMin += Pos.X;

	A.YMax += Pos.Y;
	A.YMin += Pos.Y;

	A.ZMax += Pos.Z;
	A.ZMin += Pos.Z;

	return A;
}

real64 AABBSizeX(aabb* Input)
{
	return Abs(Input->XMax - Input->XMin);
}

real64 AABBSizeY(aabb* Input)
{
	return Abs(Input->YMax - Input->YMin);
}

real64 AABBSizeZ(aabb* Input)
{
	return Abs(Input->ZMax - Input->ZMin);
}

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool RayIntersectsTriangle(vector3 rayOrigin,
                           vector3 rayVector,
                           vector3 vertex0, vector3 vertex1, vector3 vertex2,
                           vector3* outIntersectionPoint)
{
	const double EPSILON = 0.0000001;

	vector3 edge1, edge2, h, s, q;
	real64 a, f, u, v;

	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;

	h = Vector3Cross(rayVector, edge2);
	a = DotProduct(edge1, h);
	if (a > -EPSILON && a < EPSILON) {
		return false;    // This ray is parallel to this triangle.
	}

	f = 1.0 / a;
	s = rayOrigin - vertex0;
	u = f * DotProduct(s, h);
	if (u < 0.0 || u > 1.0) {
		return false;
	}

	q = Vector3Cross(s, edge1);
	v = f * DotProduct(rayVector, q);
	if (v < 0.0 || u + v > 1.0) {
		return false;
	}

	// At this stage we can compute t to find out where the intersection point is on the line.
	real32 t = (real32)(f * DotProduct(edge2, q));
	if (t > EPSILON) { // ray intersection
		vector3 Point = rayOrigin + (rayVector * t);
		outIntersectionPoint->X = Point.X;
		outIntersectionPoint->Y = Point.Y;
		outIntersectionPoint->Z = Point.Z;
		return true;
	} else {
		// This means that there is a line intersection but not a ray intersection.
		return false;
	}
}

// Remove an index from an array and slide everything aftr it down
void RemoveSlideArray(void* ArrayStart, uint32 ArrayCount, uint32 IndexSize, uint32 IndexRemoving)
{
	char* Array = (char*)ArrayStart;

	for (uint32 j = IndexRemoving; j < ArrayCount - 1; j++) {
		char* Current = Array + (IndexSize * j);
		char* Next = Array + (IndexSize * (j + 1));
		MemoryCopy(Current, Next, IndexSize);
	}

	// Clear end of array
	ClearMemory((uint8*)ArrayStart + ((ArrayCount - 1) * IndexSize), IndexSize);
}

#endif