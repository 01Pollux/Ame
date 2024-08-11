#include <Shading/ShaderComposer.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// The format of material shader source code is:
    ///
    /// Prologue:
    /// void pre_main(inout vs_input __input)
    /// {
    ///     // User code
    /// }
    ///
    /// Epilogue:
    /// void post_main(in vs_input __input, inout vs_output __output)
    /// {
    ///     // User code
    /// }
    ///
    /// Main:
    /// void do_main(in vs_input __input, out vs_output __output)
    /// {
    ///     // User code
    /// }
    ///
    /// void main(in vs_input __input, out vs_output __output)
    /// {
    ///     pre_main(__input);
    ///     do_main(__input, __output);
    ///     post_main(__input, __output);
    /// }
    ///
    /// </summary>

    //

    // MUST match the order of VertexInputConstants
    static constexpr const char s_DefineInputString[]  = "#define VS_INPUT_LAYOUT uint vertex_id:SV_VertexID;uint instance_id:SV_InstanceID; ";
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
            sizeof(s_StructElementCode) * std::size(c_InputVertexAttributes) * 2 +
            sizeof(s_StructElementCode) * std::size(c_OutputVertexAttributes) * 2 +
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