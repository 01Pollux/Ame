#ifndef AME_GEOMETRY_OPERATIONS
#define AME_GEOMETRY_OPERATIONS

#include "Structs/Geometry/Common.hlsli"
#include "Structs/Geometry/Frustum.hlsli"
#include "Structs/Geometry/Plane.hlsli"
#include "Structs/Geometry/Sphere.hlsli"

//

float4 Geometry_QuaternionInverse(const in float4 q);
float3 Geometry_RotateQuaternion(const in float3 v, const in float4 q);
float Geometry_Length(const in float3 v);
float Geometry_LengthSqr(const in float3 v);

//

bool Geometry_FrustumIntersectsSphere(const in Sphere s, const in Frustum f);
int Geometry_SphereContainsFrustum(const in Sphere s, const in Frustum f);

//

float3 Geometry_PointOnLineSegmentNearestPoint(const in float3 S1, const in float3 S2, const in float3 P);

//
//

float4 Geometry_QuaternionInverse(const in float4 q)
{
	return float4(-q.x, -q.y, -q.z, q.w);
}

float3 Geometry_RotateQuaternion(const in float3 v, const in float4 q)
{
	float4 vq = float4(v, 0.0f);
	float4 qinv = Geometry_QuaternionInverse(q);
	float4 Result = q * vq * qinv;
	return Result.xyz;
}

float Geometry_Length(const in float3 v)
{
	return sqrt(Geometry_LengthSqr(v));
}

float Geometry_LengthSqr(const in float3 v)
{
	return dot(v, v);
}

//

float3 Geometry_PointOnLineSegmentNearestPoint(const in float3 S1, const in float3 S2, const in float3 P)
{
	float3 Dir = S2 - S1;
	float Projection = dot(P, Dir) - dot(S1, Dir);
	float LengthSq = dot(Dir, Dir);

	float t = Projection / LengthSq;
	float3 Point = t * Dir + S1;

    // t < 0
	Point = Projection < 0 ? S1 : Point;

    // t > 1
	Point = Projection > LengthSq ? S2 : Point;

	return Point;
}

//

bool Geometry_FrustumIntersectsSphere(const in Sphere sh, const in Frustum frustum)
{
	int i;
	float4 Zero = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Build the frustum planes.
	float4 Planes[6];
	Planes[0] = float4(0.0f, 0.0f, -1.0f, frustum.n);
	Planes[1] = float4(0.0f, 0.0f, 1.0f, -frustum.f);
	Planes[2] = float4(1.0f, 0.0f, -frustum.r, 0.0f);
	Planes[3] = float4(-1.0f, 0.0f, frustum.l, 0.0f);
	Planes[4] = float4(0.0f, 1.0f, -frustum.t, 0.0f);
	Planes[5] = float4(0.0f, -1.0f, frustum.b, 0.0f);

    // Normalize the planes so we can compare to the sphere radius.
	Planes[2] = normalize(Planes[2]);
	Planes[3] = normalize(Planes[3]);
	Planes[4] = normalize(Planes[4]);
	Planes[5] = normalize(Planes[5]);

    // Load origin and orientation of the frustum.
	float4 vOrigin = float4(frustum.o, 0.f);
	float4 vOrientation = frustum.q;

    // Load the sphere.
	float4 vCenter = float4(sh.o, 0.f);
	float vRadius = sh.r;

    // Transform the center of the sphere into the local space of frustum.
	vCenter = mul((vCenter - vOrigin), Geometry_QuaternionInverse(vOrientation));

    // Set w of the center to one so we can dot4 with the plane.
	vCenter.w = 1.0f;

    // Check against each plane of the frustum.
	bool Outside = false;
	bool InsideAll = true;
	bool CenterInsideAll = true;

	float Dist[6];

	for (i = 0; i < 6; ++i)
	{
		Dist[i] = dot(vCenter, Planes[i]);

        // Outside the plane?
		Outside = Outside || (Dist[i] > vRadius);

        // Fully inside the plane?
		InsideAll = InsideAll && (Dist[i] <= -vRadius);

        // Check if the center is inside the plane.
		CenterInsideAll = CenterInsideAll && (Dist[i] <= 0.0f);
	}

    // If the sphere is outside any of the planes it is outside.
	if (Outside)
		return false;

    // If the sphere is inside all planes it is fully inside.
	if (InsideAll)
		return true;

    // If the center of the sphere is inside all planes and the sphere intersects
    // one or more planes then it must intersect.
	if (CenterInsideAll)
		return true;

    // The sphere may be outside the frustum or intersecting the frustum.
    // Find the nearest feature (face, edge, or corner) on the frustum
    // to the sphere.

    // The faces adjacent to each face are:
	static const int adjacent_faces[6][4] =
	{
		{ 2, 3, 4, 5 }, // 0
		{ 2, 3, 4, 5 }, // 1
		{ 0, 1, 4, 5 }, // 2
		{ 0, 1, 4, 5 }, // 3
		{ 0, 1, 2, 3 }, // 4
		{ 0, 1, 2, 3 } // 5
	};

	bool Intersects = false;

    // Check to see if the nearest feature is one of the planes.
	for (i = 0; i < 6; ++i)
	{
        // Find the nearest point on the plane to the center of the sphere.
		float4 Point = -Planes[i] * Dist[i] + vCenter;

        // Set w of the point to one.
		Point.w = 1.0f;

        // If the point is inside the face (inside the adjacent planes) then
        // this plane is the nearest feature.
		bool InsideFace = true;

		for (int j = 0; j < 4; j++)
		{
			int plane_index = adjacent_faces[i][j];

			InsideFace = InsideFace && (dot(Point, Planes[plane_index]) <= 0.0f);
		}

        // Since we have already checked distance from the plane we know that the
        // sphere must intersect if this plane is the nearest feature.
		Intersects = Intersects || (Dist[i] > 0.0f && InsideFace);
	}

	if (Intersects)
		return true;

    // Build the corners of the frustum.
	float3 vRightTop = float3(frustum.r, frustum.t, 1.0f);
	float3 vRightBottom = float3(frustum.r, frustum.b, 1.0f);
	float3 vLeftTop = float3(frustum.l, frustum.t, 1.0f);
	float3 vLeftBottom = float3(frustum.l, frustum.b, 1.0f);
	float vNear = frustum.n;
	float vFar = frustum.f;

	float3 Corners[FRUSTUM_CORNER_COUNT];
	Corners[0] = vRightTop * vNear;
	Corners[1] = vRightBottom * vNear;
	Corners[2] = vLeftTop * vNear;
	Corners[3] = vLeftBottom * vNear;
	Corners[4] = vRightTop * vFar;
	Corners[5] = vRightBottom * vFar;
	Corners[6] = vLeftTop * vFar;
	Corners[7] = vLeftBottom * vFar;

    // The Edges are:
	static const int edges[12][2] =
	{
		{ 0, 1 },
		{ 2, 3 },
		{ 0, 2 },
		{ 1, 3 }, // Near plane
		{ 4, 5 },
		{ 6, 7 },
		{ 4, 6 },
		{ 5, 7 }, // Far plane
		{ 0, 4 },
		{ 1, 5 },
		{ 2, 6 },
		{ 3, 7 } // Near to far
	};

	float RadiusSq = vRadius * vRadius;

    // Check to see if the nearest feature is one of the edges (or corners).
	for (i = 0; i < 12; ++i)
	{
		int ei0 = edges[i][0];
		int ei1 = edges[i][1];

        // Find the nearest point on the edge to the center of the sphere.
        // The corners of the frustum are included as the endpoints of the edges.
		float3 Point = Geometry_PointOnLineSegmentNearestPoint(Corners[ei0], Corners[ei1], vCenter.xyz);

		float3 Delta = vCenter.xyz - Point;

		float DistSq = dot(Delta, Delta);

        // If the distance to the center of the sphere to the point is less than
        // the radius of the sphere then it must intersect.
		Intersects = Intersects || (DistSq <= RadiusSq);
	}

	return Intersects;
}

//

int Geometry_SphereContainsFrustum(const in Sphere s, const in Frustum f)
{
	if (Geometry_FrustumIntersectsSphere(s, f) == false)
	{
		return CONTAINMENT_TYPE_DISJOINT;
	}
	
	float3 vCenter = s.o;
	float vRadius = s.r;
	float RadiusSq = vRadius * vRadius;

	float3 vOrigin = f.o;
	float4 vOrientation = f.q;

    // Build the corners of the frustum.
	float3 vRightTop = float3(f.r, f.t, 1.0f);
	float3 vRightBottom = float3(f.r, f.b, 1.0f);
	float3 vLeftTop = float3(f.l, f.t, 1.0f);
	float3 vLeftBottom = float3(f.l, f.b, 1.0f);
	float vNear = f.n;
	float vFar = f.f;

	float3 Corners[FRUSTUM_CORNER_COUNT];
	Corners[0] = vRightTop * vNear;
	Corners[1] = vRightBottom * vNear;
	Corners[2] = vLeftTop * vNear;
	Corners[3] = vLeftBottom * vNear;
	Corners[4] = vRightTop * vFar;
	Corners[5] = vRightBottom * vFar;
	Corners[6] = vLeftTop * vFar;
	Corners[7] = vLeftBottom * vFar;

	bool InsideAll = true;
	for (uint i = 0; i < FRUSTUM_CORNER_COUNT; ++i)
	{
		float3 C = Geometry_RotateQuaternion(Corners[i], vOrientation) + vOrigin;
		float d = Geometry_LengthSqr(vCenter - C);
		InsideAll = InsideAll && (d <= RadiusSq);
	}

	return InsideAll ? CONTAINMENT_TYPE_CONTAINS : CONTAINMENT_TYPE_INTERSECTS;
}
#endif