#include <Rhi/Resource/Shader.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Get the shader entry point.
    /// </summary>
    [[nodiscard]] const char* GetShaderEntryPoint(
        ShaderType type)
    {
        switch (type)
        {
        case ShaderType::COMPUTE_SHADER:
            return "CS_Main";
        case ShaderType::VERTEX_SHADER:
            return "VS_Main";
        case ShaderType::TESS_CONTROL_SHADER:
            return "HS_Main";
        case ShaderType::TESS_EVALUATION_SHADER:
            return "DS_Main";
        case ShaderType::GEOMETRY_SHADER:
            return "GS_Main";
        case ShaderType::FRAGMENT_SHADER:
            return "PS_Main";
        case ShaderType::MESH_CONTROL_SHADER:
            return "AS_Main";
        case ShaderType::MESH_EVALUATION_SHADER:
            return "MS_Main";
        default:
            std::unreachable();
        }
    }

    /// <summary>
    /// Get the shader entry point.
    /// </summary>
    [[nodiscard]] const wchar_t* GetShaderEntryPointWide(
        ShaderType type)
    {
        switch (type)
        {
        case ShaderType::COMPUTE_SHADER:
            return L"CS_Main";
        case ShaderType::VERTEX_SHADER:
            return L"VS_Main";
        case ShaderType::TESS_CONTROL_SHADER:
            return L"HS_Main";
        case ShaderType::TESS_EVALUATION_SHADER:
            return L"DS_Main";
        case ShaderType::GEOMETRY_SHADER:
            return L"GS_Main";
        case ShaderType::FRAGMENT_SHADER:
            return L"PS_Main";
        case ShaderType::MESH_CONTROL_SHADER:
            return L"AS_Main";
        case ShaderType::MESH_EVALUATION_SHADER:
            return L"MS_Main";
        default:
            std::unreachable();
        }
    }
} // namespace Ame::Rhi