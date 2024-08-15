#pragma once

#include <Shaders/EngineShader.hpp>

#include <Shaders/Structs/Transform.hpp>
#include <Shaders/Structs/RenderInstance.hpp>
#include <Shaders/Structs/CameraFrameData.hpp>

#include <Shaders/Structs/Inputs/StdVertexInput.hpp>
#include <Shaders/Structs/Inputs/StdPixelInput.hpp>

#include <Shaders/Structs/Outputs/StdVertexOutput.hpp>
#include <Shaders/Structs/Outputs/StdMaterialFragment.hpp>
#include <Shaders/Structs/Outputs/ForwardPlus_PixelOutput.hpp>

namespace Ame::Rhi
{
    class ForwardPlus_RenderObjects_VertexShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] = R"(
    #include "Structs/Transform.hlsli"
    #include "Structs/RenderInstance.hlsli"
    #include "Structs/CameraFrameData.hlsli"

    #include "Structs/Inputs/StdVertexInput.hlsli"
	#include "Structs/Outputs/StdVertexOutput.hlsli"

    cbuffer FrameDataBuffer
    {
        CameraFrameData FrameData;
    };

    StructuredBuffer<Transform> Transforms;
    StructuredBuffer<RenderInstance> RenderInstances;

    //

    void main(in vs_input vsIn, out vs_output vsOut)
    {
#ifdef _HAS_PREMAIN
		if (pre_main(vsIn, vsOut) == true)
		{
			return;
		}
#endif

        RenderInstance instance = RenderInstances[0];
        //RenderInstance instance = RenderInstances[vsIn.instance_id];

        float4x4 world = Transforms[0].World;
        //float4x4 world = Transforms[instance.TransformId].World;
        float4 position = mul(float4(vsIn.position, 1.0), world);

        vsOut.screen_position   = mul(FrameData.ViewProjection, position);
        vsOut.world_position    = position.xyz;
        vsOut.world_normal      = mul(float4(vsIn.normal, 0.0), world).xyz;
        vsOut.world_tangent     = mul(float4(vsIn.tangent, 0.0), world).xyz;
        vsOut.tex_coord         = vsIn.tex_coord;

#ifdef _HAS_POSTMAIN
		post_main(vsIn, vsOut);
#endif
    }
)";

    public:
        ForwardPlus_RenderObjects_VertexShader()
        {
            Setup({ "VS_Fwd+RenderObjects", Dg::SHADER_TYPE_VERTEX, true }, c_SourceCode);
            LinkShaders();
        }

    private:
        void LinkShaders()
        {
            StructTransformShader       transformStruct;
            StructRenderInstanceShader  renderInstanceStruct;
            StructCameraFrameDataShader cameraFrameDataStruct;

            StructStdVertexInputShader  vertexInputStruct;
            StructStdVertexOutputShader vertexOutputStruct;

            m_LinkedShaders = Dg::CreateMemoryShaderSourceFactory(
                {
                    { transformStruct.GetMemoryShaderSourceFileInfo(transformStruct.Name) },
                    { renderInstanceStruct.GetMemoryShaderSourceFileInfo(renderInstanceStruct.Name) },
                    { cameraFrameDataStruct.GetMemoryShaderSourceFileInfo(cameraFrameDataStruct.Name) },
                    { vertexInputStruct.GetMemoryShaderSourceFileInfo(vertexInputStruct.Name) },
                    { vertexOutputStruct.GetMemoryShaderSourceFileInfo(vertexOutputStruct.Name) },
                },
                true);
            m_CreateInfo.pShaderSourceStreamFactory = m_LinkedShaders;
        }

    private:
        Ptr<Dg::IShaderSourceInputStreamFactory> m_LinkedShaders;
    };

    class ForwardPlus_RenderObjects_PixelShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] = R"(
    #include "Structs/Inputs/StdPixelInput.hlsli"
	#include "Structs/Outputs/StdMaterialFragment.hlsli"
	#include "Structs/Outputs/ForwardPlus_StdPixelOutput.hlsli"

	void main(in ps_input psIn, out ps_output psOut)
	{
        material_fragment fragment;

#ifdef _HAS_PREMAIN
        psOut.color = (float4)0;
		if (pre_main(psIn, fragment) == false)
		{
			return;
		}

        psOut.color = fragment.base_color;
#else
        psOut.color = float4(1.0, 1.0, 1.0, 1.0);
#endif
	}
)";

    public:
        ForwardPlus_RenderObjects_PixelShader()
        {
            Setup({ "PS_Fwd+RenderObjects", Dg::SHADER_TYPE_PIXEL, true }, c_SourceCode);
            LinkShaders();
        }

    private:
        void LinkShaders()
        {
            StructStdPixelInputShader          pixelInputStruct;
            StructStdMaterialFragmentShader    materialOutputStruct;
            StructForwardPlusPixelOutputShader pixelOutputStruct;

            m_LinkedShaders = Dg::CreateMemoryShaderSourceFactory(
                {
                    { pixelInputStruct.GetMemoryShaderSourceFileInfo(pixelInputStruct.Name) },
                    { materialOutputStruct.GetMemoryShaderSourceFileInfo(materialOutputStruct.Name) },
                    { pixelOutputStruct.GetMemoryShaderSourceFileInfo(pixelOutputStruct.Name) },
                },
                true);
            m_CreateInfo.pShaderSourceStreamFactory = m_LinkedShaders;
        }

    private:
        Ptr<Dg::IShaderSourceInputStreamFactory> m_LinkedShaders;
    };
} // namespace Ame::Rhi