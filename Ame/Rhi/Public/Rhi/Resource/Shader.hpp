#pragma once

#include <Core/String.hpp>
#include <Core/Coroutine.hpp>

#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Get the shader entry point name
    /// </summary>
    [[nodiscard]] const char* GetShaderEntryPoint(
        ShaderType Stage);

    /// <summary>
    /// Get the shader entry point name
    /// </summary>
    [[nodiscard]] const wchar_t* GetShaderEntryPointWide(
        ShaderType Stage);

    class ShaderBytecode
    {
    public:
        ShaderBytecode() = default;
        ShaderBytecode(
            uint8_t*   Bytecode,
            size_t     Size,
            ShaderType Stage,
            bool       Owning = true) :
            m_Bytecode(Bytecode),
            m_Size(Size),
            m_Stage(Stage),
            m_Owning(Owning)
        {
        }

        ShaderBytecode(const ShaderBytecode&) = delete;
        ShaderBytecode(
            ShaderBytecode&& Other) noexcept :
            m_Bytecode(Other.m_Bytecode),
            m_Size(Other.m_Size),
            m_Stage(Other.m_Stage),
            m_Owning(Other.m_Owning)
        {
            Other.m_Bytecode = nullptr;
            Other.m_Size     = 0;
            Other.m_Stage    = ShaderType::VERTEX_SHADER;
            Other.m_Owning   = false;
        }

        ShaderBytecode& operator=(const ShaderBytecode&) = delete;
        ShaderBytecode& operator=(
            ShaderBytecode&& Other) noexcept
        {
            if (this != &Other)
            {
                m_Bytecode = Other.m_Bytecode;
                m_Size     = Other.m_Size;
                m_Stage    = Other.m_Stage;
                m_Owning   = Other.m_Owning;

                Other.m_Bytecode = nullptr;
                Other.m_Size     = 0;
                Other.m_Stage    = ShaderType::VERTEX_SHADER;
                Other.m_Owning   = false;
            }

            return *this;
        }

        ~ShaderBytecode()
        {
            if (m_Owning)
            {
                delete[] m_Bytecode;
            }
        }

    public:
        operator bool() const
        {
            return m_Bytecode != nullptr;
        }

        [[nodiscard]] ShaderDesc GetDesc() const
        {
            return ShaderDesc{
                .stage          = m_Stage,
                .bytecode       = m_Bytecode,
                .size           = m_Size,
                .entryPointName = GetShaderEntryPoint(m_Stage)
            };
        }

        [[nodiscard]] const uint8_t* GetBytecode() const
        {
            return m_Bytecode;
        }

        [[nodiscard]] size_t GetSize() const
        {
            return m_Size;
        }

        [[nodiscard]] ShaderType GetStage() const
        {
            return m_Stage;
        }

    private:
        uint8_t*   m_Bytecode = nullptr;
        size_t     m_Size     = 0;
        ShaderType m_Stage    = ShaderType::NONE;
        bool       m_Owning   = false;
    };
} // namespace Ame::Rhi