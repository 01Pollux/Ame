#include <RG/Resources/CoreResources.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Util/ResourceSize.hpp>
#include <Math/Common.hpp>

namespace Ame::RG
{
    CoreResources::CoreResources(
        Rhi::Device&   rhiDevice) :
        m_Device(rhiDevice),
        m_FrameResourceBuffer(AllocateFrameResource(rhiDevice))
    {
    }

    //

    const Rhi::Buffer& CoreResources::GetFrameResource() const
    {
        return m_FrameResourceBuffer;
    }

    const FrameResourceCPU& CoreResources::GetFrameResourceData() const
    {
        return m_FrameResource;
    }
} // namespace Ame::RG