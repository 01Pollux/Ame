#include <Gfx/RG/Resources/CoreResources.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Util/ResourceSize.hpp>
#include <Math/Common.hpp>

namespace Ame::Gfx::RG
{
    CoreResources::CoreResources(
        Rhi::Device&   rhiDevice,
        Ecs::Universe& universe) :
        m_Device(rhiDevice),
        m_Universe(universe),
        m_FrameResourceBuffer(AllocateFrameResource(rhiDevice)),
        m_AABBBuffer(rhiDevice),
        m_TransformBuffer(rhiDevice),
        m_EcsSystemHooks(universe, *this),
        m_CameraCullResult(rhiDevice)
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

    const DynamicInstanceBuffer& CoreResources::GetInstancesTableBuffer() const
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

    DynamicInstanceBuffer& CoreResources::GetInstancesTableBuffer()
    {
        return m_CameraCullResult.GetInstancesTableBuffer();
    }
} // namespace Ame::Gfx::RG