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
    class DepthPrepass_VertexShader : public EngineShader
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

    void main(in vs_input vsIn, out vs_output_pos_only vsOut)
    {
#ifdef _HAS_PREMAIN_DEPTH_PREPASS
		if (pre_main_depth_prepass(vsIn, vsOut) == true)
		{
			return;
		}
#endif

        RenderInstance instance = RenderInstances[vsIn.instance_id];

        float4x4 world = Transforms[instance.TransformId].World;
        float4 position = mul(float4(vsIn.position, 1.0), world);

        vsOut.screen_position   = mul(FrameData.ViewProjection, position);

#ifdef _HAS_POSTMAIN_DEPTH_PREPASS
		post_main_depth_prepass(vsIn, vsOut);
#endif
    }
)";

    public:
        DepthPrepass_VertexShader()
        {
            Setup({ "VS_DepthPrepass", Dg::SHADER_TYPE_VERTEX, true }, c_SourceCode);
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
} // namespace Ame::Rhi