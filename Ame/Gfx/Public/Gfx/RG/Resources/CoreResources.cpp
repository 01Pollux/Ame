#include <Gfx/RG/Resources/CoreResources.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Util/ResourceSize.hpp>
#include <Math/Common.hpp>

namespace Ame::Gfx::RG
{
    CoreResources::CoreResources(
        Rhi::Device& Device) :
        m_Device(Device)
    {
        AllocateFrameResource();
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

    void CoreResources::UpdateFrameResource(
        float                        EngineTime,
        float                        GameTime,
        float                        DeltaTime,
        const Math::TransformMatrix& Transform,
        const Math::Matrix4x4&       Projection,
        const Math::Vector2&         Viewport)
    {
        auto View = glm::lookAt(
            Transform.GetPosition(),
            Transform.GetPosition() + Transform.GetLookDir(),
            Transform.GetUpDir());

        m_FrameResource.World = Transform.ToMat4x4Transposed();

        m_FrameResource.View           = glm::transpose(View);
        m_FrameResource.Projection     = glm::transpose(Projection);
        m_FrameResource.ViewProjection = m_FrameResource.View * m_FrameResource.Projection;

        m_FrameResource.ViewInverse           = glm::inverse(m_FrameResource.View);
        m_FrameResource.ProjectionInverse     = glm::inverse(m_FrameResource.Projection);
        m_FrameResource.ViewProjectionInverse = glm::inverse(m_FrameResource.ViewProjection);

        m_FrameResource.Viewport = Viewport;

        m_FrameResource.EngineTime = EngineTime;
        m_FrameResource.GameTime   = GameTime;
        m_FrameResource.DeltaTime  = DeltaTime;

        auto&   DeviceDesc = m_Device.get().GetDesc();
        uint8_t FrameIndex = m_Device.get().GetFrameIndex();
        size_t  Offset     = Rhi::GetConstantBufferSize(DeviceDesc, sizeof(FrameResourceGPU), FrameIndex);
        std::memcpy(m_FrameResourceBuffer.GetPtr(Offset), &m_FrameResource, sizeof(FrameResourceGPU));
    }

    //

    void CoreResources::AllocateFrameResource()
    {
        auto&   DeviceDesc = m_Device.get().GetDesc();
        uint8_t FrameCount = m_Device.get().GetFrameCountInFlight();
        size_t  BufferSize = Rhi::GetConstantBufferSize(DeviceDesc, sizeof(FrameResourceGPU), FrameCount);

        m_FrameResourceBuffer = Rhi::Buffer(m_Device.get(), Rhi::MemoryLocation::HOST_UPLOAD, { .size = BufferSize, .usageMask = Rhi::BufferUsageBits::CONSTANT_BUFFER });
    }
} // namespace Ame::Gfx::RG