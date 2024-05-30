#include <Rhi/Core.hpp>

#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    const FormatProps& GetFormatProps(
        ResourceFormat format)
    {
        return nri::nriGetFormatProps(format);
    }
} // namespace Ame::Rhi