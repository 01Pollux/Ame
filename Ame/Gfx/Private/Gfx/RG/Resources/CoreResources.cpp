#include <Gfx/RG/Resources/CoreResources.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Util/ResourceSize.hpp>
#include <Math/Common.hpp>

namespace Ame::Gfx::RG
{
    CoreResources::CoreResources(
        Rhi::Device&   Device,
        Ecs::Universe& Universe) :
        m_Device(Device),
        m_Universe(Universe),
        m_FrameResourceBuffer(AllocateFrameResource(Device)),
        m_AABBBuffer(Device),
        m_TransformBuffer(Device),
        m_EcsSystemHooks(Universe, *this),
        m_CameraCullResult(Device)
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

    //

    uint32_t CoreResources::GetEntitiesCount() const
    {
        return m_CameraCullResult.GetEntitiesCount();
    }

    EntityStore::RowGenerator CoreResources::GetEntities() const
    {
        return m_CameraCullResult.GetEntities();
    }

    //

    const AABBBuffer& CoreResources::GetAABBBuffer() const
    {
        return m_AABBBuffer;
    }

    const TransformBuffer& CoreResources::GetTransformBuffer() const
    {
        return m_TransformBuffer;
    }

    const InstanceBuffer& CoreResources::GetInstancesTableBuffer() const
    {
        return m_CameraCullResult.GetInstancesTableBuffer();
    }

    //

    AABBBuffer& CoreResources::GetAABBBuffer()
    {
        return m_AABBBuffer;
    }

    TransformBuffer& CoreResources::GetTransformBuffer()
    {
        return m_TransformBuffer;
    }

    InstanceBuffer& CoreResources::GetInstancesTableBuffer()
    {
        return m_CameraCullResult.GetInstancesTableBuffer();
    }
} // namespace Ame::Gfx::RG