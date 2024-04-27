#pragma once

#include <Gfx/Draw/SpriteBatch/Desc.hpp>
#include <Rhi/Stream/Buffer.hpp>

namespace Ame::Gfx
{
    class SpriteBatch
    {
    public:
        SpriteBatch(
            SpriteBatchDesc& Desc);

    public:
        /// <summary>
        /// Begin the sprite batch on the provided pipeline.
        /// </summary>
        void Begin(
            Rhi::CommandList& CmdList);

    public:
        /// <summary>
        /// Draw a sprite with the provided points.
        /// </summary>
        void DrawPolygon(
            std::span<const Math::Vector3> Points,
            std::span<const uint16_t>      Indices);

        /// <summary>
        /// Draw a rectangle with the provided rectangle.
        /// </summary>
        void DrawRectangle(
            const Math::Vector3& TopLeft,
            const Math::Vector3& TopRight,
            const Math::Vector3& BottomRight,
            const Math::Vector3& BottomLeft);

        /// <summary>
        /// Draw a triangle with the provided points.
        /// </summary>
        void DrawTriangle(
            const Math::Vector3& Point1,
            const Math::Vector3& Point2,
            const Math::Vector3& Point3);

        /// <summary>
        /// Draw a line between the provided points.
        /// </summary>
        void DrawLine(
            const Math::Vector3& Point1,
            const Math::Vector3& Point2);

    public:
        /// <summary>
        /// End the sprite batch.
        /// </summary>
        void End();

    private:
        /// <summary>
        /// Flush the vertex and index buffers and draw the sprites.
        /// </summary>
        void Flush();

        /// <summary>
        /// Try to allocate the provided amount of vertices.
        /// Returns true if the allocation could be made, false otherwise.
        /// </summary>
        [[nodiscard]] bool TryAllocateVertices(
            size_t Size) const;

        /// <summary>
        /// Try to allocate the provided amount of indices.
        /// Returns true if the allocation could be made, false otherwise.
        /// </summary>
        [[nodiscard]] bool TryAllocateIndices(
            size_t Size) const;

        /// <summary>
        /// Reserve the provided amount of vertices.
        /// </summary>
        [[nodiscard]] void ReserveVertices(
            size_t Size);

        /// <summary>
        /// Reserve the provided amount of indices.
        /// </summary>
        [[nodiscard]] void ReserveIndices(
            size_t Size);

    private:
        SpriteBatchDesc m_Desc;

        Rhi::CommandList* m_CmdList = nullptr;

        Rhi::Buffer m_VertexBuffer;
        Rhi::Buffer m_IndexBuffer;

        Rhi::Streaming::BufferOStream m_VertexStream;
        Rhi::Streaming::BufferOStream m_IndexStream;

        size_t m_LastVertexOffset = 0;
        size_t m_LastIndexOffset  = 0;
    };
} // namespace Ame::Gfx