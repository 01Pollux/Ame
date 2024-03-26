#if VERTEX_SHADER
#ifndef QUAD_Z
        #error QUAD_Z must be defined
#endif

#ifndef FULL_SCREEN
        #error FULL_SCREEN must be defined
#endif

#elif PIXEL_SHADER

#ifndef TEXTURE_ARRAY
        #error TEXTURE_ARRAY must be defined
#endif

#ifndef BLIT_SHARPEN
        #error BLIT_SHARPEN must be defined
#endif
#endif

//

#include "../Core/Ame.hlsli"

//

struct BlitConstants
{
	float4 Src;
	float4 Dst;
	float SharpenFactor;
};

struct VSInput
{
	uint VertexId : SV_VertexID;
};

struct PSInput
{
	float4 PosClip : SV_Position;
	float2 UV : UV;
};

//

PUSH_CONSTANT BlitConstants g_BlitConstants : register(b0);

#if TEXTURE_ARRAY
Texture2DArray g_Texture : register(t0);
#else
Texture2D g_Texture : register(t0);
#endif

SamplerState g_Sampler : register(s0);

//

//
#if VERTEX_SHADER

PSInput VS_Main(
	VSInput Vs)
{
	PSInput Ps;
    Ps.UV = float2(Vs.VertexId & 1, (Vs.VertexId >> 1) & 1);

#if FULL_SCREEN

#define SrcUV Ps.UV

#else

    float2 SrcOrigin = g_BlitConstants.Src.xy;
    float2 DstOrigin = g_BlitConstants.Dst.xy;

    float2 SrcSize = g_BlitConstants.Dst.zw;
    float2 DstSize = g_BlitConstants.Src.zw;

    float2 SrcUV = Ps.UV * SrcSize + SrcOrigin;
    Ps.UV = Ps.UV * DstSize + DstOrigin;

#endif

	Ps.PosClip = float4(SrcUV.x * 2.f - 1.f, 1.f - SrcUV.y * 2.f, QUAD_Z, 1.f);
	return Ps;
}

#endif
//

//
#if PIXEL_SHADER


#if BLIT_SHARPEN
#define PS_Main_Blit PS_Main
#else
#define PS_Main_Sharpen PS_Main
#endif

float4 PS_Main_Blit(
    PSInput Ps) : SV_Target
{
#if TEXTURE_ARRAY
    return g_Texture.Sample(g_Sampler, float3(Ps.UV, 0));
#else
    return g_Texture.Sample(g_Sampler, Ps.UV);
#endif
}

float4 PS_Main_Sharpen(
    PSInput Ps) : SV_Target
{
#if TEXTURE_ARRAY
	float4 x = g_Texture.SampleLevel(g_Sampler, float3(Ps.UV, 0), 0);
	
	float4 a = g_Texture.SampleLevel(g_Sampler, float3(Ps.UV, 0), 0, int2(-1,  0));
	float4 b = g_Texture.SampleLevel(g_Sampler, float3(Ps.UV, 0), 0, int2( 1,  0));
	float4 c = g_Texture.SampleLevel(g_Sampler, float3(Ps.UV, 0), 0, int2( 0,  1));
	float4 d = g_Texture.SampleLevel(g_Sampler, float3(Ps.UV, 0), 0, int2( 0, -1));

	float4 e = g_Texture.SampleLevel(g_Sampler, float3(Ps.UV, 0), 0, int2(-1, -1));
	float4 f = g_Texture.SampleLevel(g_Sampler, float3(Ps.UV, 0), 0, int2( 1,  1));
	float4 g = g_Texture.SampleLevel(g_Sampler, float3(Ps.UV, 0), 0, int2(-1,  1));
	float4 h = g_Texture.SampleLevel(g_Sampler, float3(Ps.UV, 0), 0, int2( 1, -1));
#else
	float4 x = g_Texture.SampleLevel(g_Sampler, Ps.UV, 0);
	
	float4 a = g_Texture.SampleLevel(g_Sampler, Ps.UV, 0, int2(-1,  0));
	float4 b = g_Texture.SampleLevel(g_Sampler, Ps.UV, 0, int2( 1,  0));
	float4 c = g_Texture.SampleLevel(g_Sampler, Ps.UV, 0, int2( 0,  1));
	float4 d = g_Texture.SampleLevel(g_Sampler, Ps.UV, 0, int2( 0, -1));

	float4 e = g_Texture.SampleLevel(g_Sampler, Ps.UV, 0, int2(-1, -1));
	float4 f = g_Texture.SampleLevel(g_Sampler, Ps.UV, 0, int2( 1,  1));
	float4 g = g_Texture.SampleLevel(g_Sampler, Ps.UV, 0, int2(-1,  1));
	float4 h = g_Texture.SampleLevel(g_Sampler, Ps.UV, 0, int2( 1, -1));
#endif
	
	float4 Val = x * (6.828427 * g_BlitConstants.SharpenFactor + 1)
		- (a + b + c + d) * g_BlitConstants.SharpenFactor
		- (e + g + f + h) * g_BlitConstants.SharpenFactor * 0.7071;
	return Val;
}

#endif
//
