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
            Device&             RhiDevice,
            nri::DescriptorSet* Set) :
            m_Device(&RhiDevice),
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
        Device*             m_Device = nullptr;
        nri::DescriptorSet* m_Set    = nullptr;
    };
} // namespace Ame::Rhi