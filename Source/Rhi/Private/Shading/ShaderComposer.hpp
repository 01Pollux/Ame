#pragma once

#include <Core/String.hpp>

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
        void WriteShader(
            StringView header,
            StringView sourceCode);

    private:
        String m_SourceCode;
    };
} // namespace Ame::Rhi