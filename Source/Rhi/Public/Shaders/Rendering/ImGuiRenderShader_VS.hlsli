cbuffer Constants
{
	float4x4 ProjectionMatrix;
};
struct vs_input
{
	float2 pos : ATTRIB0;
	float2 uv : ATTRIB1;
	float4 col : ATTRIB2;
};
struct vs_output
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 uv : TEX_COORD;
};
void main(in vs_input vsIn, out vs_output vsOut)
{
	vsOut.pos = mul(ProjectionMatrix, float4(vsIn.pos.xy, 0.0, 1.0));
	vsOut.col = vsIn.col;
	vsOut.uv = vsIn.uv;
}