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
            nri::Buffer* NriBuffer = nullptr;

            const void* CpuView_Or_Offset = nullptr;

            uint32_t Num    = 0;
            uint32_t Stride = 0;

            static BufferView Local(
                void*  Data,
                size_t Count,
                size_t Stride);

            static BufferView Shared(
                nri::Buffer* Buffer,
                size_t       Offset,
                size_t       Count,
                size_t       Stride);

            /// <summary>
            /// Get the offset of the buffer.
            /// The buffer MUST be unique to access the offset.
            /// </summary>
            [[nodiscard]] size_t Offset() const;

            /// <summary>
            /// Get the cpu data of the buffer.
            /// The buffer MUST be local to access the cpu data.
            /// </summary>
            [[nodiscard]] const void* CpuData() const;

            /// <summary>
            /// Check to see if the buffer is unique.
            /// </summary>
            [[nodiscard]] bool HasUniqueBuffer() const;

            /// <summary>
            /// Check to see if the buffer is local and dynamic.
            /// </summary>
            [[nodiscard]] bool IsLocal() const;
        };

        // must be either uint16_t or uint32_t
        BufferView Index;

        // must be of type VertexInput
        BufferView Vertex;

        Ptr<Gfx::Shading::Material> Material;

        uint32_t CameraMask = 0xFFFF'FFFF;
    };
} // namespace Ame::Ecs::Component