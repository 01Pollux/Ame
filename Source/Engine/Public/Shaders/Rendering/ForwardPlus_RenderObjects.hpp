#pragma once

#include <Shaders/EngineShader.hpp>

#include <Shaders/Structs/Transform.hpp>
#include <Shaders/Structs/RenderInstance.hpp>
#include <Shaders/Structs/Light.hpp>
#include <Shaders/Structs/CameraFrameData.hpp>

#include <Shaders/Structs/Inputs/StdVertexInput.hpp>
#include <Shaders/Structs/Inputs/StdPixelInput.hpp>

#include <Shaders/Structs/Outputs/StdVertexOutput.hpp>
#include <Shaders/Structs/Outputs/StdMaterialFragment.hpp>
#include <Shaders/Structs/Outputs/ForwardPlus_PixelOutput.hpp>

#include <Shaders/Structs/Lighting/LightingResult.hpp>
#include <Shaders/Structs/Lighting/Operations.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API ForwardPlus_RenderObjects_VertexShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "ForwardPlus_RenderObjects_VS.hlsli.generated.hpp"
            ;

    public:
        ForwardPlus_RenderObjects_VertexShader()
        {
            Setup({ "R_Fwd+RenderObjects_VS", Dg::SHADER_TYPE_VERTEX }, c_SourceCode);
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

    class AME_ENGINE_API ForwardPlus_RenderObjects_PixelShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "ForwardPlus_RenderObjects_PS.hlsli.generated.hpp"
            ;

    public:
        ForwardPlus_RenderObjects_PixelShader()
        {
            Setup({ "R_Fwd+RenderObjects_PS", Dg::SHADER_TYPE_PIXEL }, c_SourceCode);
            LinkShaders();
        }

    private:
        void LinkShaders()
        {
            StructTransformShader       transformStruct;
            StructLightShader           lightStruct;
            StructCameraFrameDataShader cameraFrameDataStruct;

            StructStdPixelInputShader          pixelInputStruct;
            StructStdMaterialFragmentShader    materialOutputStruct;
            StructForwardPlusPixelOutputShader pixelOutputStruct;

            StructLightingResultShader     lightingResultStruct;
            StructLightingOperationsShader lightingOperationsStruct;

            m_LinkedShaders = Dg::CreateMemoryShaderSourceFactory(
                {
                    { transformStruct.GetMemoryShaderSourceFileInfo(transformStruct.Name) },
                    { lightStruct.GetMemoryShaderSourceFileInfo(lightStruct.Name) },
                    { cameraFrameDataStruct.GetMemoryShaderSourceFileInfo(cameraFrameDataStruct.Name) },

                    { pixelInputStruct.GetMemoryShaderSourceFileInfo(pixelInputStruct.Name) },
                    { materialOutputStruct.GetMemoryShaderSourceFileInfo(materialOutputStruct.Name) },
                    { pixelOutputStruct.GetMemoryShaderSourceFileInfo(pixelOutputStruct.Name) },

                    { lightingResultStruct.GetMemoryShaderSourceFileInfo(lightingResultStruct.Name) },
                    { lightingOperationsStruct.GetMemoryShaderSourceFileInfo(lightingOperationsStruct.Name) },
                },
                true);
            m_CreateInfo.pShaderSourceStreamFactory = m_LinkedShaders;
        }

    private:
        Ptr<Dg::IShaderSourceInputStreamFactory> m_LinkedShaders;
    };
} // namespace Ame::Rhi