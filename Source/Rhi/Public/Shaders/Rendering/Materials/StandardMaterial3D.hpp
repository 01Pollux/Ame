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
    class StandardMaterial3D_PixelShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] = R"(
    #include "Structs/Inputs/StdPixelInput.hlsli"
	#include "Structs/Outputs/StdMaterialFragment.hlsli"
	#include "Structs/Outputs/ForwardPlus_StdPixelOutput.hlsli"

    #define _HAS_PREMAIN
	bool pre_main(in ps_input psIn, out material_fragment fragment)
	{
        fragment = (material_fragment)0;
		fragment.base_color = float4(1.0, 0.0, 1.0, 1.0);
        return true;
	}
)";

    public:
        StandardMaterial3D_PixelShader()
        {
            Setup({ "PS_StdMaterial3D", Dg::SHADER_TYPE_PIXEL, true }, c_SourceCode);
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