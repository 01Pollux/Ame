#pragma once

#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    class Fence
    {
    public:
        Fence() = default;
        Fence(nullptr_t)
        {
        }

        Fence(
            nri::Fence*    fence,
            uint64_t       value,
            nri::StageBits stages) noexcept :
            m_Fence(fence),
            m_Value(value),
            m_Stages(stages)
        {
        }

        /// <summary>
        /// Get the nri fence.
        /// </summary>
        [[nodiscard]] nri::Fence* const& Unwrap() const noexcept
        {
            return m_Fence;
        }

        /// <summary>
        /// Get the fence value.
        /// </summary>
        [[nodiscard]] uint64_t GetValue() const noexcept
        {
            return m_Value;
        }

        /// <summary>
        /// Get the fence stages.
        /// </summary>
        [[nodiscard]] nri::StageBits GetStages() const noexcept
        {
            return m_Stages;
        }

    private:
        nri::Fence*    m_Fence  = nullptr;
        uint64_t       m_Value  = 0;
        nri::StageBits m_Stages = nri::StageBits::NONE;
    };
} // namespace Ame::Rhi