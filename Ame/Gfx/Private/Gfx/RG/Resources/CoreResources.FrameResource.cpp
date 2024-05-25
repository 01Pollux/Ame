#include <Gfx/RG/Resources/CoreResources.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Util/ResourceSize.hpp>
#include <Math/Common.hpp>

namespace Ame::Gfx::RG
{
    void CoreResources::UpdateFrameResource(
        float                        engineTime,
        float                        gameTime,
        float                        deltaTime,
        const Ecs::Entity&           cameraEntity,
        const Math::TransformMatrix& transform,
        const Math::Matrix4x4&       projection,
        const Math::Vector2&         viewport)
    {
        auto view = glm::lookAt(
            transform.GetPosition(),
            transform.GetPosition() + transform.GetLookDir(),
            transform.GetUpDir());

        m_FrameResource.World = transform.ToMat4x4Transposed();

        m_FrameResource.View           = glm::transpose(view);
        m_FrameResource.Projection     = glm::transpose(projection);
        m_FrameResource.ViewProjection = m_FrameResource.View * m_FrameResource.Projection;

        m_FrameResource.ViewInverse           = glm::inverse(m_FrameResource.View);
        m_FrameResource.ProjectionInverse     = glm::inverse(m_FrameResource.Projection);
        m_FrameResource.ViewProjectionInverse = glm::inverse(m_FrameResource.ViewProjection);

        m_FrameResource.Viewport = viewport;

        m_FrameResource.EngineTime = engineTime;
        m_FrameResource.GameTime   = gameTime;
        m_FrameResource.DeltaTime  = deltaTime;

        m_FrameResource.CurrentCamera = cameraEntity;

        //

        auto&   deviceDesc = m_Device.get().GetDesc();
        uint8_t frameIndex = m_Device.get().GetFrameIndex();
        size_t  offset     = Rhi::GetConstantBufferSize(deviceDesc, sizeof(FrameResourceGPU), frameIndex);
        std::memcpy(m_FrameResourceBuffer.GetPtr(offset), &m_FrameResource, sizeof(FrameResourceGPU));
    }

    //

    Rhi::BufferViewDesc CoreResources::GetFrameResourceViewDesc() const
    {
        auto&   deviceDesc = m_Device.get().GetDesc();
        uint8_t frameIndex = m_Device.get().GetFrameIndex();
        size_t  size       = Rhi::GetConstantBufferSize(deviceDesc, sizeof(FrameResourceGPU), 0);
        size_t  offset     = size * frameIndex;

        return Rhi::BufferViewDesc{ .Range{ offset, size }, .Type = Rhi::BufferViewType::ConstantBuffer };
    }

    //

    Rhi::Buffer CoreResources::AllocateFrameResource(
        Rhi::Device& rhiDevice)
    {
        auto&   deviceDesc = rhiDevice.GetDesc();
        uint8_t frameCount = rhiDevice.GetFrameCountInFlight();
        size_t  bufferSize = Rhi::GetConstantBufferSize(deviceDesc, sizeof(FrameResourceGPU), frameCount);

        return Rhi::Buffer(rhiDevice, Rhi::MemoryLocation::HOST_UPLOAD, { .size = bufferSize, .usageMask = Rhi::BufferUsageBits::CONSTANT_BUFFER });
    }
} // namespace Ame::Gfx::RG