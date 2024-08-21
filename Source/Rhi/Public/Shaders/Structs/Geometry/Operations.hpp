#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class GeometryOperationsShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] = R"(
#ifndef AME_GEOMETRY_OPERATIONS
#define AME_GEOMETRY_OPERATIONS

int Geometry_FrustumIntersectsSphere(const in Sphere s, const in Frustum f);
int Geometry_SphereContainsFrustum(const in Sphere s, const in Frustum f);

//

int Geometry_FrustumIntersectsSphere(const in Sphere s, const in Frustum f)
{
	float4 planes[FRUSTUM_PLANE_COUNT];
	planes[0] = float4(0.f, 0.f, -1.f, f.n);
	planes[1] = float4(0.f, 0.f, 1.f, -f.f);
	planes[2] = float4(1.f, 0.f, -f.r, 0.f);
	planes[3] = float4(-1.f, 0.f, f.l, 0.f);
	planes[4] = float4(0.f, 1.f, -f.t, 0.f);
	planes[5] = float4(0.f, -1.f, f.b, 0.f);

	float r2 = s.r * s.r;
	float3 temp = mul(f.q, float4(s.o - f.o, 0.0f)).xyz;
	float4 o = float4(temp, 1.f);

	bool outside = false;
	bool inside_all = true;
	bool center_inside_all = true;

	float dist[FRUSTUM_PLANE_COUNT];
	for (int i = 0; i < FRUSTUM_PLANE_COUNT; i++)
	{
		dist[i] = dot(o, planes[i]);
		outside = outside || (dist[i] > s.r);
		inside_all = inside_all && (dist[i] <= -s.r);
		center_inside_all = center_inside_all && (dist[i] <= 0.f);
	}

	if (outside) return false;
	if (inside_all) return true;
	if (center_inside_all) return true;

	const int4 adj_faces[FRUSTUM_PLANE_COUNT];
	adj_faces[0] = adj_faces[1] = int4(2, 3, 4, 5);
	adj_faces[2] = adj_faces[3] = int4(0, 1, 4, 5);
	adj_faces[4] = adj_faces[5] = int4(0, 1, 2, 3);

	bool intersect = false;
	for (int i = 0; i < FRUSTUM_PLANE_COUNT && !intersect; i++)
	{
		float4 p = float4(o - dist[i] * planes[i].xyz, 1.f);
		bool inside = true;
		for (int j = 0; j < 4 && inside; j++)
		{
			int idx = adj_faces[i][j];
			inside = inside && (dot(p, planes[idx]) <= 0.f);
		}
	}
	if (intersect) return true;

	float right_top = float(f.r, f.t, 1.f);
	float right_bottom = float(f.r, f.b, 1.f);
	float left_top = float(f.l, f.t, 1.f);
	float left_bottom = float(f.l, f.b, 1.f);
	float near = f.n;
	float far = f.f;

	float3 corners[FRUSTUM_CORNER_COUNT];
	corners[0] = right_top * near;
	corners[1] = right_bottom * near;
	corners[2] = left_top * near;
	corners[3] = left_bottom * near;
	corners[4] = right_top * far;
	corners[5] = right_bottom * far;
	corners[6] = left_top * far;
	corners[7] = left_bottom * far;

	const int edges[FRUSTUM_EDGE_COUNT][2] = {
		{ 0, 1 }, { 2, 3 }, { 0, 2 }, { 1, 3 },    // Near plane
        { 4, 5 }, { 6, 7 }, { 4, 6 }, { 5, 7 },    // Far plane
        { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },    // Near to far
	};
	for (int i = 0; i < FRUSTUM_EDGE_COUNT && !intersect; i++)
	{
		int e1 = edges[i][0];
		int e2 = edges[i][1];

		float c1 = corners[e1];
		float c2 = corners[e2];
		float t = dot(s.o - c1, c2 - c1) / dot(c2 - c1, c2 - c1);
		float3 p = lerp(c1, c2, t);

		float3 d = s.o - p;
		float d2 = dot(d, d);

		intersect = intersect || (d2 <= r2);
	}
	return intersect;
}	

//

int Geometry_SphereContainsFrustum(const in Sphere s, const in Frustum f)
{
	[branch]
	if (Geometry_FrustumIntersectsSphere(s, f) == false)
	{
		return CONTAINMENT_TYPE_DISJOINT;
	}

    float r2 = s.r * s.r;
    float3 right_top = float3(f.r, f.t, 1.0f);
	float3 right_bottom = float3(f.r, f.b, 1.0f);
	float3 left_top = float3(f.l, f.t, 1.0f);
	float3 left_bottom = float3(f.l, f.b, 1.0f);
    float near = f.n;
	float far = f.f;

    float3 corners[FRUSTUM_CORNER_COUNT];
	corners[0] = right_top * near;
	corners[1] = right_bottom * near;
	corners[2] = left_top * near;
	corners[3] = left_bottom * near;
	corners[4] = right_top * far;
	corners[5] = right_bottom * far;
	corners[6] = left_top * far;
	corners[7] = left_bottom * far;

    bool inside_all = true;
	for (int i = 0; i < FRUSTUM_CORNER_COUNT && inside_all; i++)
	{
		float3 c = mul(f.q, float4(corners[i], 1.0f)).xyz + f.o;
		float d = dot(s.o - c, s.o - c);
		inside_all = inside_all && (d <= r2);
	}
	return inside_all ? CONTAINMENT_TYPE_CONTAINS : CONTAINMENT_TYPE_INTERSECTS;
}
#endif
)";

    public:
        static constexpr const char* Name = "Structs/Geometry/Operations.hlsli";

        GeometryOperationsShader()
        {
            Setup({ "G_Operations", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi