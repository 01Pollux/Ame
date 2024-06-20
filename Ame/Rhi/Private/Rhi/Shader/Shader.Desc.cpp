#include <Rhi/Shader/Shader.Desc.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Get the shader entry point.
    /// </summary>
    [[nodiscard]] const wchar_t* GetCommonEntryPoint(
        ShaderCompileStage type)
    {
        switch (type)
        {
        case ShaderCompileStage::Vertex:
            return L"VS_Main";
        case ShaderCompileStage::Domain:
            return L"DS_Main";
        case ShaderCompileStage::Hull:
            return L"HS_Main";
        case ShaderCompileStage::Geometry:
            return L"GS_Main";
        case ShaderCompileStage::Pixel:
            return L"PS_Main";
        case ShaderCompileStage::Compute:
            return L"CS_Main";
        case ShaderCompileStage::Amplification:
            return L"AS_Main";
        case ShaderCompileStage::Mesh:
            return L"MS_Main";
        default:
            std::unreachable();
        }
    }

    //

    bool ShaderCompileDesc::RequiresShaderValidation() const
    {
        using namespace EnumBitOperators;

        bool noValidation        = (Flags & ShaderCompileFlags::NoValidation) == ShaderCompileFlags::NoValidation;
        bool libraryCantValidate = (Stage == ShaderCompileStage::Library) && (Profile <= ShaderProfile::_6_2);

        // dxc failed : Must disable validation for unsupported lib_6_1 or lib_6_2 targets.
        return !(noValidation || libraryCantValidate);
    }

    ShaderType ShaderCompileDesc::GetShaderType() const
    {
        return CompileStageToShaderType(Stage);
    }

    void ShaderCompileDesc::SetAsShader(
        ShaderCompileStage stage)
    {
        Stage      = stage;
        EntryPoint = GetCommonEntryPoint(stage);
    }

    void ShaderCompileDesc::SetAsLibrary(
        ShaderCompileStage stage)
    {
        Stage      = ShaderCompileStage::Library;
        EntryPoint = GetCommonEntryPoint(stage);
    }
} // namespace Ame::Rhi