#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Descs/View.hpp>

namespace Ame::Rhi
{
    class DescriptorSet
    {
    public:
        DescriptorSet() = default;

        DescriptorSet(
            nri::DescriptorSet* Set) :
            m_Set(Set)
        {
        }

        operator bool() const noexcept
        {
            return m_Set != nullptr;
        }

    public:
        /// <summary>
        /// Get the nri descriptor set.
        /// </summary>
        [[nodiscard]] nri::DescriptorSet* Unwrap() const;

    private:
        nri::DescriptorSet* m_Set = nullptr;
    };
} // namespace Ame::Rhi