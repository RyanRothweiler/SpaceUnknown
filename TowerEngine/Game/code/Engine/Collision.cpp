/*
	This site has lots of easy explination
	http://www.jeffreythompson.org/collision-detection/line-line.php
*/

bool32 CollisionLineVPoint(line Line, vector2 Point, real32 Dif = 0.01f)
{
	vector2 Nearest = NearestOnLine(Line, Point);

	real64 Full = Vector2Distance(Line.Start, Line.End);
	real64 A = Vector2Distance(Line.Start, Nearest);
	real64 B = Vector2Distance(Line.End, Nearest);

	if (CloseEnough(A + B, Full)) {
		real64 NDist = Vector2Distance(Nearest, Point);
		if (NDist < Dif) {
			return true;
		}
	}

	return false;
}

bool32 CollisionLineVCircle(line Line, vector2 Center, real32 R)
{
	vector2 Nearest = NearestOnLine(Line, Center);
	if (CollisionLineVPoint(Line, Nearest)) {
		real64 Dist = Vector2Distance(Nearest, Center);
		if (Dist < R) {
			return true;
		}
	}

	return false;
}

//http://www.jeffreythompson.org/collision-detection/line-line.php
bool32 CollisionLineVLine(line A, line B)
{
	real64 x1 = A.Start.X;
	real64 y1 = A.Start.Y;

	real64 x2 = A.End.X;
	real64 y2 = A.End.Y;

	real64 x3 = B.Start.X;
	real64 y3 = B.Start.Y;

	real64 x4 = B.End.X;
	real64 y4 = B.End.Y;


	real64 uA = (
	                ((x4 - x3) * (y1 - y3)) -
	                ((y4 - y3) * (x1 - x3))
	            ) /
	            (
	                ((y4 - y3) * (x2 - x1)) -
	                ((x4 - x3) * (y2 - y1))
	            );

	real64 uB = (
	                ((x2 - x1) * (y1 - y3)) -
	                ((y2 - y1) * (x1 - x3))
	            ) /
	            (
	                ((y4 - y3) * (x2 - x1)) -
	                ((x4 - x3) * (y2 - y1))
	            );

	if (
	    uA >= 0 && uA <= 1 &&
	    uB >= 0 && uB <= 1
	) {
		return true;
	}

	return false;
}

bool CollisionAABBVAABB(aabb A, aabb B)
{
	if (A.XMax < B.XMin) { return false; }
	if (A.XMin > B.XMax) { return false; }

	if (A.YMax < B.YMin) { return false; }
	if (A.YMin > B.YMax) { return false; }

	if (A.ZMax < B.ZMin) { return false; }
	if (A.ZMin > B.ZMax) { return false; }

	return true;
};

//https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
bool CollisionAABBVRay(aabb A, vector3 Origin, vector3 Dir)
{
	real64 t = 0;

	// r.dir is unit direction vector of ray
	vector3 dirfrac = {};
	dirfrac.X = 1.0f / Dir.X;
	dirfrac.Y = 1.0f / Dir.Y;
	dirfrac.Z = 1.0f / Dir.Z;

	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray
	real64 t1 = (A.XMin - Origin.X) * dirfrac.X;
	real64 t2 = (A.XMax - Origin.X) * dirfrac.X;
	real64 t3 = (A.YMin - Origin.Y) * dirfrac.Y;
	real64 t4 = (A.YMax - Origin.Y) * dirfrac.Y;
	real64 t5 = (A.ZMin - Origin.Z) * dirfrac.Z;
	real64 t6 = (A.ZMax - Origin.Z) * dirfrac.Z;

	real64 tmin = Max(Max(Min(t1, t2), Min(t3, t4)), Min(t5, t6));
	real64 tmax = Min(Min(Max(t1, t2), Max(t3, t4)), Max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
	if (tmax < 0) {
		t = tmax;
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax) {
		t = tmax;
		return false;
	}

	t = tmin;
	return true;
}