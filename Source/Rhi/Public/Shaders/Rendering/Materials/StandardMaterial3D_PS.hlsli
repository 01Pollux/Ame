#include "Structs/Inputs/StdPixelInput.hlsli"
#include "Structs/Outputs/StdMaterialFragment.hlsli"

#include "Structs/Rendering/StdMaterial3D_Data.hlsli"
#include "Structs/Rendering/StdSamplers.hlsli"

#define _HAS_PREMAIN
bool pre_main(in ps_input psIn, out material_fragment fragment)
{
	float2 roughness_metallic = Roughness_MetallicMap.Sample(Sampler_LinearWrap, psIn.tex_coord);
	fragment.base_color = BaseColorMap.Sample(Sampler_LinearWrap, psIn.tex_coord);
	fragment.normal = NormalMap.Sample(Sampler_LinearWrap, psIn.tex_coord);
	fragment.ao = AOMap.Sample(Sampler_LinearWrap, psIn.tex_coord);
	fragment.emissive = EmissiveMap.Sample(Sampler_LinearWrap, psIn.tex_coord);
	fragment.specular = SpecularMap.Sample(Sampler_LinearWrap, psIn.tex_coord);
	fragment.roughness = roughness_metallic.x;
	fragment.metallic = roughness_metallic.y;
	fragment.height = HeightMap.Sample(Sampler_LinearWrap, psIn.tex_coord);
	return true;
}
