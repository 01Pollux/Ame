#include <Shading/ShaderComposer.hpp>
#include <Shading/VertexInput.hpp>

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

    static constexpr const char s_DefineInputString[]  = "#define VS_INPUT_LAYOUT ";
    static constexpr const char s_DefineOutputString[] = "#define VS_OUTPUT_LAYOUT ";
    static constexpr const char s_StructElementCode[]  = "{} {}:{};";

    //

    StringView MaterialShaderComposer::Write(
        StringView prologue,
        StringView sourceCode,
        StringView epilogue)
    {
        m_SourceCode.clear();
        m_SourceCode.reserve(
            sizeof(s_DefineInputString) +
            sizeof(s_DefineOutputString) +
            sizeof(s_StructElementCode) * std::size(c_InputVertexAttributes) * 4 +
            sourceCode.size() +
            prologue.size() +
            epilogue.size());

        WriteHeader(s_DefineInputString, c_InputVertexAttributes);
        WriteHeader(s_DefineOutputString, c_OutputVertexAttributes);

        WriteShader(sourceCode);
        WriteShader(prologue);
        WriteShader(epilogue);

        return m_SourceCode;
    }

    void MaterialShaderComposer::WriteHeader(
        StringView                           defineTag,
        std::span<const VertexAttributeDesc> attributes)
    {
        m_SourceCode.append(defineTag);
        for (const auto& attribute : attributes)
        {
            std::format_to(
                std::back_inserter(m_SourceCode),
                s_StructElementCode,
                attribute.TypeString,
                attribute.Name,
                attribute.SemanticName);
        }
        m_SourceCode.append("\n");
    }

    void MaterialShaderComposer::WriteShader(
        StringView sourceCode)
    {
        if (!sourceCode.empty())
        {
            m_SourceCode.append(sourceCode);
            m_SourceCode.append("\n");
        }
    }
} // namespace Ame::Rhi