#pragma once

#include <Core/String.hpp>
#include <Core/Coroutine.hpp>

#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi
{
    class ShaderBytecode
    {
    public:
        struct Extern
        {
        };

        ShaderBytecode() = default;
        ShaderBytecode(
            Extern,
            String           entryPoint,
            const std::byte* bytecode,
            size_t           size,
            ShaderType       stage) :
            m_EntryPoint(std::move(entryPoint)),
            m_Bytecode(bytecode),
            m_Size(size),
            m_Stage(stage)
        {
        }

        ShaderBytecode(
            String                       entryPoint,
            std::unique_ptr<std::byte[]> bytecode,
            size_t                       size,
            ShaderType                   stage) :
            m_EntryPoint(std::move(entryPoint)),
            m_Bytecode(bytecode.release()),
            m_Size(size),
            m_Stage(stage),
            m_Owning(true)
        {
        }

        ShaderBytecode(const ShaderBytecode&) = delete;
        ShaderBytecode(
            ShaderBytecode&& other) noexcept :
            m_EntryPoint(std::move(other.m_EntryPoint)),
            m_Bytecode(std::exchange(other.m_Bytecode, nullptr)),
            m_Size(std::exchange(other.m_Size, 0)),
            m_Stage(other.m_Stage),
            m_Owning(std::exchange(other.m_Owning, false))
        {
        }

        ShaderBytecode& operator=(const ShaderBytecode&) = delete;
        ShaderBytecode& operator=(
            ShaderBytecode&& other) noexcept
        {
            if (this != &other)
            {
                m_EntryPoint = std::move(other.m_EntryPoint);
                m_Bytecode   = std::exchange(other.m_Bytecode, nullptr);
                m_Size       = std::exchange(other.m_Size, 0);
                m_Stage      = other.m_Stage;
                m_Owning     = std::exchange(other.m_Owning, false);
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
        explicit operator bool() const noexcept
        {
            return m_Bytecode != nullptr;
        }

        /// <summary>
        /// Get reference of this bytecode without owning it
        /// </summary>
        [[nodiscard]] ShaderBytecode Borrow() const
        {
            return ShaderBytecode{
                Extern{},
                m_EntryPoint,
                m_Bytecode,
                m_Size,
                m_Stage
            };
        }

        [[nodiscard]] ShaderDesc GetDesc() const
        {
            return ShaderDesc{
                .stage          = m_Stage,
                .bytecode       = m_Bytecode,
                .size           = m_Size,
                .entryPointName = m_EntryPoint.c_str()
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
        String           m_EntryPoint;
        const std::byte* m_Bytecode = nullptr;
        size_t           m_Size     = 0;
        ShaderType       m_Stage    = ShaderType::NONE;
        bool             m_Owning   = false;
    };
} // namespace Ame::Rhi