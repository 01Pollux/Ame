#ifndef SRGBA_TO_LINEAR
#define SRGBA_TO_LINEAR(x)
#endif
Texture2D Texture;
SamplerState Texture_sampler;
struct ps_input
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 uv : TEX_COORD;
};
float4 main(in ps_input psIn) : SV_Target
{
	float4 col = Texture.Sample(Texture_sampler, psIn.uv) * psIn.col;
	col.rgb *= col.a;
	SRGBA_TO_LINEAR(col) 
	return col;
}