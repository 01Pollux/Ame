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

#include <Shaders/Structs/Rendering/StandardSamplers.hpp>
#include <Shaders/Structs/Rendering/StandardMaterial3D_Data.hpp>

namespace Ame::Rhi
{
    class StdMaterial3D_PixelShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "StandardMaterial3D_PS.hlsli.generated.hpp"
            ;

    public:
        StdMaterial3D_PixelShader()
        {
            Setup({ "R_StandardMaterial3D_PS", Dg::SHADER_TYPE_PIXEL }, c_SourceCode);
            LinkShaders();
        }

    private:
        void LinkShaders()
        {
            StructStdPixelInputShader       pixelInputStruct;
            StructStdMaterialFragmentShader materialOutputStruct;

            StructStdSamplersShader        samplersStruct;
            StructStdMaterial3D_DataShader materialDataStruct;

            m_LinkedShaders = Dg::CreateMemoryShaderSourceFactory(
                {
                    { pixelInputStruct.GetMemoryShaderSourceFileInfo(pixelInputStruct.Name) },
                    { materialOutputStruct.GetMemoryShaderSourceFileInfo(materialOutputStruct.Name) },
                    { samplersStruct.GetMemoryShaderSourceFileInfo(samplersStruct.Name) },
                    { materialDataStruct.GetMemoryShaderSourceFileInfo(materialDataStruct.Name) },
                },
                true);
            m_CreateInfo.pShaderSourceStreamFactory = m_LinkedShaders;
        }

    private:
        Ptr<Dg::IShaderSourceInputStreamFactory> m_LinkedShaders;
    };
} // namespace Ame::Rhi