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
        static constexpr const char c_SourceCode[] =
#include "DepthPrepass_VS.hlsli.generated.hpp"
            ;

    public:
        DepthPrepass_VertexShader()
        {
            Setup({ "R_DepthPrepass_VS", Dg::SHADER_TYPE_VERTEX }, c_SourceCode);
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