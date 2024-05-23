struct Ecs_PSInput
{
    float4 position : SV_POSITION;
};

struct PSOutput
{
    float4 color : SV_TARGET0;
};

struct MaterialFragment
{
    float4 color;
};

MaterialFragment PSM_Main(Ecs_PSInput input)
{
	MaterialFragment material = (MaterialFragment) 0;
	
	material.color = float4(1, 0, 0, 1);
	
	return material;


}

[shader("pixel")]
PSOutput PS_Main(Ecs_PSInput input)
{
    MaterialFragment material = PSM_Main(input);
    PSOutput output = (PSOutput) 0;
    
    output.color = material.color;
    
    return output;
}