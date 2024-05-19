#pragma once

#include <Rhi/Resource/Buffer.hpp>
#include <Ecs/Component/Renderable/VertexInput.hpp>

namespace Ame::Gfx::Shading
{
    class Material;
} // namespace Ame::Gfx::Shading

namespace Ame::Ecs::Component
{
    /// <summary>
    /// Internal component for entities that are renderable.
    ///
    /// For each renderable entity, if it has a its own buffer, it will be stored in RhiBuffer, and view will contains offset in the buffer.
    /// If the entity is small, RhiBuffer will be nullptr, and view will contains the cpu data directly.
    /// </summary>
    struct BaseRenderable
    {
        struct BufferView
        {
            const void* View  = nullptr;
            uint32_t    Count = 0;
            Rhi::Buffer RhiBuffer;

            [[nodiscard]] bool HasUniqueBuffer() const
            {
                return RhiBuffer;
            }

            [[nodiscard]] size_t GetUniqueBufferOffset() const
            {
                return std::bit_cast<size_t>(View);
            }

            [[nodiscard]] bool IsLocal() const
            {
                return !HasUniqueBuffer();
            }
        };

        // must be either uint16_t or uint32_t
        BufferView Index;

        // must be of type VertexInput
        BufferView Vertex;

        Ptr<Gfx::Shading::Material> Material;

        uint32_t CameraMask = 0xFFFF'FFFF;
    };
} // namespace Ame::Ecs::Component