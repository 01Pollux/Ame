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
    /*
    #include "Structs/DrawIndexedIndirectCommand.hlsli"
    #include "Structs/Transform.hlsli"
    #include "Structs/RenderInstance.hlsli"
    #include "Structs/CameraFrameData.hlsli"

    cbuffer FrameDataBuffer
    {
        CameraFrameData FrameData;
    };

    //StructuredBuffer<Transform> Transforms;
    StructuredBuffer<RenderInstance> RenderInstances;

    StructuredBuffer<float3> PositionBuffer;
    StructuredBuffer<float3> NormalBuffer;
    StructuredBuffer<float2> TexCoordBuffer;
    StructuredBuffer<float3> TangentBuffer;

    //

    struct vs_input{VS_INPUT_LAYOUT};
    struct vs_output{VS_OUTPUT_LAYOUT};

    void main(in vs_input vsIn, out vs_output vsOut)
    {
        RenderInstance instance = RenderInstances[vsIn.instance_id];

        float3 position = PositionBuffer[vsIn.vertex_id + instance.PositionOffset];
        float3 normal = NormalBuffer[vsIn.vertex_id + instance.NormalOffset];
        float2 tex_coord = TexCoordBuffer[vsIn.vertex_id + instance.TexCoordOffset];
        float3 tangent = TangentBuffer[vsIn.vertex_id + instance.TangentOffset];

        #if 0
        float4x4 world = Transforms[instance.TransformId].World;
        vsOut.screen_position = mul(float4(position, 1.0), world);
        vsOut.world_position = vsOut.screen_position.xyz;
        vsOut.world_normal = mul(float4(normal, 0.0), world).xyz;
        vsOut.world_tangent = mul(float4(tangent, 0.0), world).xyz;
        vsOut.tex_coord = tex_coord;
        #endif

        vsOut.screen_position = mul(float4(position, 1.0), FrameData.ViewProjection);
        vsOut.world_position  = position;
        vsOut.world_normal    = normal;
        vsOut.world_tangent   = tangent;
        vsOut.tex_coord       = tex_coord;
    }
    */
    class ForwardPlus_RenderObjects_VertexShader : public EngineShader
    {
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

    //StructuredBuffer<Transform> Transforms;
    StructuredBuffer<RenderInstance> RenderInstances;

    //

    void main(in vs_input vsIn, out vs_output vsOut)
    {
        RenderInstance instance = RenderInstances[vsIn.instance_id];

        #if 0
        float4x4 world = Transforms[instance.TransformId].World;
        vsOut.screen_position = mul(float4(position, 1.0), world);
        vsOut.world_position = vsOut.screen_position.xyz;
        vsOut.world_normal = mul(float4(normal, 0.0), world).xyz;
        vsOut.world_tangent = mul(float4(tangent, 0.0), world).xyz;
        vsOut.tex_coord = tex_coord;
        #endif

        vsOut.screen_position = mul(FrameData.ViewProjection, float4(vsIn.position, 1.0));
        vsOut.world_position  = vsIn.position;
        vsOut.world_normal    = vsIn.normal;
        vsOut.world_tangent   = vsIn.tangent;
        vsOut.tex_coord       = vsIn.tex_coord;
    }
)";

    public:
        ForwardPlus_RenderObjects_VertexShader()
        {
            Setup({ "Render Indirect Commands VS", Dg::SHADER_TYPE_VERTEX, true }, c_SourceCode);
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

    /*
    struct ps_input{VS_OUTPUT_LAYOUT};
    struct ps_output{float4 color : SV_TARGET;};

    void main(in ps_input psIn, out ps_output psOut)
    {
        psOut.color = float4(1.0, 0.0, 0.0, 1.0);
    }
    */
    class ForwardPlus_RenderObjects_PixelShader : public EngineShader
    {
        //// clang-format off
        // static constexpr const uint8_t c_SourceCode[] {
        //     0x1F, 0x8B, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x2B, 0x2E, 0x29, 0x2A, 0x4D, 0x2E,
        //     0x51, 0x28, 0x28, 0x8E, 0xCF, 0xCC, 0x2B, 0x28, 0x2D, 0xA9, 0x0E, 0x0B, 0x8E, 0xF7, 0x0F, 0x0D,
        //     0x09, 0x08, 0x0D, 0x89, 0xF7, 0x71, 0x8C, 0x04, 0xB2, 0x6A, 0xAD, 0x79, 0xB9, 0x8A, 0xE1, 0x6A,
        //     0xF2, 0x4B, 0x4B, 0x40, 0x8A, 0xD2, 0x72, 0xF2, 0x13, 0x4B, 0x4C, 0x14, 0x92, 0xF3, 0x73, 0xF2,
        //     0x8B, 0x14, 0xAC, 0x14, 0x82, 0xC3, 0xE2, 0x43, 0x1C, 0x83, 0xDC, 0x5D, 0x43, 0xAC, 0x41, 0xAA,
        //     0x79, 0xB9, 0xCA, 0xF2, 0x33, 0x53, 0x14, 0x72, 0x13, 0x33, 0xF3, 0x34, 0x32, 0xF3, 0xE0, 0x26,
        //     0x03, 0x19, 0x9E, 0x79, 0x3A, 0x0A, 0xF9, 0xA5, 0x48, 0x06, 0x01, 0x59, 0xFE, 0xA5, 0x25, 0x9A,
        //     0xBC, 0x5C, 0xD5, 0xBC, 0x5C, 0x9C, 0x60, 0xB6, 0x1E, 0xC4, 0x4C, 0x5B, 0x05, 0x88, 0x15, 0x1A,
        //     0x86, 0x7A, 0x06, 0x3A, 0x0A, 0x06, 0x70, 0x02, 0xC8, 0xD5, 0x04, 0x5A, 0x51, 0x0B, 0x00, 0xD0,
        //     0xC6, 0xD4, 0x9C, 0xB6, 0x00, 0x00, 0x00
        // };
        //// clang-format on

    public:
        static constexpr const char c_SourceCode[] = R"(
    #include "Structs/Inputs/StdPixelInput.hlsli"
	#include "Structs/Outputs/StdMaterialFragment.hlsli"
	#include "Structs/Outputs/ForwardPlus_StdPixelOutput.hlsli"

	void main(in ps_input psIn, out ps_output psOut)
	{
		psOut.color = float4(1.0, 0.0, 0.0, 1.0);
	}
)";

    public:
        ForwardPlus_RenderObjects_PixelShader()
        {
            Setup({ "Render Indirect Commands PS", Dg::SHADER_TYPE_PIXEL, true }, c_SourceCode);
            LinkShaders();
        }

    private:
        void LinkShaders()
        {
            StructTransformShader       transformStruct;
            StructRenderInstanceShader  renderInstanceStruct;
            StructCameraFrameDataShader cameraFrameDataStruct;

            StructStdPixelInputShader          pixelInputStruct;
            StructStdMaterialFragmentShader    materialOutputStruct;
            StructForwardPlusPixelOutputShader pixelOutputStruct;

            m_LinkedShaders = Dg::CreateMemoryShaderSourceFactory(
                {
                    { transformStruct.GetMemoryShaderSourceFileInfo(transformStruct.Name) },
                    { renderInstanceStruct.GetMemoryShaderSourceFileInfo(renderInstanceStruct.Name) },
                    { cameraFrameDataStruct.GetMemoryShaderSourceFileInfo(cameraFrameDataStruct.Name) },
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