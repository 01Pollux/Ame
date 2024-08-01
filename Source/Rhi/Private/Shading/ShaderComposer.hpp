#pragma once

#include <Shading/VertexInput.hpp>

namespace Ame::Rhi
{
    // Helper class to compose shader source code
    class MaterialShaderComposer
    {
    public:
        [[nodiscard]] StringView Write(
            StringView prologue,
            StringView sourceCode,
            StringView epilogue);

    private:
        void WriteHeader(
            StringView                           defineTag,
            std::span<const VertexAttributeDesc> attributes);

        void WriteShader(
            StringView sourceCode);

    private:
        String m_SourceCode;
    };
} // namespace Ame::Rhi