#include <Rhi/Resource/Shader.Desc.hpp>

namespace Ame::Rhi
{
    bool ShaderCompileDesc::ShouldValidate() const
    {
        using namespace EnumBitOperators;

        bool NoValidation = (Flags & ShaderCompileFlags::NoValidation) == ShaderCompileFlags::NoValidation;

        // dxc failed : Must disable validation for unsupported lib_6_1 or lib_6_2 targets.
        return !(NoValidation ||
                 (IsLibraryShader() && Profile <= ShaderProfile::_6_2));
    }

    bool ShaderCompileDesc::IsLibraryShader() const
    {
        using namespace EnumBitOperators;

        return (Flags & ShaderCompileFlags::LibraryShader) == ShaderCompileFlags::LibraryShader;
    }

    //

    ShaderType ShaderCompileDesc::GetStage() const
    {
        using namespace EnumBitOperators;

        bool IsLibrary = (Flags & ShaderCompileFlags::LibraryShader) == ShaderCompileFlags::LibraryShader;
        return IsLibrary ? LibraryShaderType : Stage;
    }

    ShaderType ShaderCompileDesc::GetStageUnchecked() const
    {
        return Stage;
    }

    void ShaderCompileDesc::SetStage(
        ShaderType Type)
    {
        Stage = Type;
    }
} // namespace Ame::Rhi