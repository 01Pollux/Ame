#include <Rhi/Resource/Set.hpp>

namespace Ame::Rhi
{
    nri::DescriptorSet* DescriptorSet::Unwrap() const
    {
        return m_Set;
    }
} // namespace Ame::Rhi