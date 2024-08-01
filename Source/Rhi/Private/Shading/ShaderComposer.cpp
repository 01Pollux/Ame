#include <Shading/ShaderComposer.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// The format of material shader source code is:
    ///
    /// Prologue:
    /// void pre_main(inout vs_input input)
    /// {
    ///     // User code
    /// }
    ///
    /// Epilogue:
    /// void post_main(in vs_input input, inout vs_output output)
    /// {
    ///     // User code
    /// }
    ///
    /// Main:
    /// void do_main(in vs_input input, out vs_output output)
    /// {
    ///     // User code
    /// }
    ///
    /// void main(in vs_input input, out vs_output output)
    /// {
    ///     pre_main(input);
    ///     do_main(input, output);
    ///     post_main(input, output);
    /// }
    ///
    /// </summary>

    //

    static constexpr const char s_PrologueShaderCode[] = "void pre_main(inout vs_input input){";

    static constexpr const char s_EpilogueShaderCode[] = "void post_main(in vs_input input, inout vs_output output){";

    static constexpr const char s_DoMainShaderCode[] = "void do_main(in vs_input input, out vs_output output){";

    static constexpr const char s_MainShaderCode[] = "void main(in vs_input input, out vs_output output){pre_main(input);do_main(input, output);post_main(input, output);}";

    static constexpr const char s_CloseBrace[] = "}";

    //

    StringView MaterialShaderComposer::Write(
        StringView prologue,
        StringView sourceCode,
        StringView epilogue)
    {
        m_SourceCode.clear();
        m_SourceCode.reserve(
            prologue.size() +
            sourceCode.size() +
            epilogue.size() +
            sizeof(s_MainShaderCode) +
            sizeof(s_PrologueShaderCode) +
            sizeof(s_EpilogueShaderCode) +
            sizeof(s_CloseBrace) * 2);

        WriteShader(s_PrologueShaderCode, prologue);
        WriteShader(s_DoMainShaderCode, sourceCode);
        WriteShader(s_EpilogueShaderCode, epilogue);
        m_SourceCode.append(s_MainShaderCode);

        return m_SourceCode;
    }

    void MaterialShaderComposer::WriteShader(
        StringView header,
        StringView sourceCode)
    {
        m_SourceCode.append(header);
        m_SourceCode.append(sourceCode);
        m_SourceCode.append(s_CloseBrace);
    }
} // namespace Ame::Rhi