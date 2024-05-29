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
        ShaderType stage);

    /// <summary>
    /// Get the shader entry point name
    /// </summary>
    [[nodiscard]] const wchar_t* GetShaderEntryPointWide(
        ShaderType stage);

    class ShaderBytecode
    {
    public:
        ShaderBytecode() = default;
        ShaderBytecode(
            std::byte* bytecode,
            size_t     size,
            ShaderType stage,
            bool       owning = true) :
            m_Bytecode(bytecode),
            m_Size(size),
            m_Stage(stage),
            m_Owning(owning)
        {
        }

        ShaderBytecode(const ShaderBytecode&) = delete;
        ShaderBytecode(
            ShaderBytecode&& other) noexcept :
            m_Bytecode(other.m_Bytecode),
            m_Size(other.m_Size),
            m_Stage(other.m_Stage),
            m_Owning(other.m_Owning)
        {
            other.m_Bytecode = nullptr;
            other.m_Size     = 0;
            other.m_Stage    = ShaderType::VERTEX_SHADER;
            other.m_Owning   = false;
        }

        ShaderBytecode& operator=(const ShaderBytecode&) = delete;
        ShaderBytecode& operator=(
            ShaderBytecode&& other) noexcept
        {
            if (this != &other)
            {
                m_Bytecode = other.m_Bytecode;
                m_Size     = other.m_Size;
                m_Stage    = other.m_Stage;
                m_Owning   = other.m_Owning;

                other.m_Bytecode = nullptr;
                other.m_Size     = 0;
                other.m_Stage    = ShaderType::VERTEX_SHADER;
                other.m_Owning   = false;
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

        /// <summary>
        /// Get reference of this bytecode without owning it
        /// </summary>
        [[nodiscard]] ShaderBytecode Borrow() const
        {
            return ShaderBytecode{
                m_Bytecode,
                m_Size,
                m_Stage,
                false
            };
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

        [[nodiscard]] const std::byte* GetBytecode() const
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
        std::byte* m_Bytecode = nullptr;
        size_t     m_Size     = 0;
        ShaderType m_Stage    = ShaderType::NONE;
        bool       m_Owning   = false;
    };
} // namespace Ame::Rhi