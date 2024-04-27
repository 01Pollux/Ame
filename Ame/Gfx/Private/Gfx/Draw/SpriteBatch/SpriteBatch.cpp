#include <Gfx/Draw/SpriteBatch/SpriteBatch.hpp>

#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/Resource/VertexView.hpp>

namespace Ame::Gfx
{
    SpriteBatch::SpriteBatch(
        SpriteBatchDesc& Desc) :
        m_Desc(Desc)
    {
    }

    //

    void SpriteBatch::Begin(
        Rhi::CommandList& CmdList)
    {
        m_CmdList = &CmdList;
    }

    //

    void SpriteBatch::DrawPolygon(
        std::span<const Math::Vector3> Points,
        std::span<const uint16_t>      Indices)
    {
        const bool VertexBufferNoSpaceLeft = TryAllocateVertices(Points.size_bytes());
        const bool IndexBufferNoSpaceLeft  = TryAllocateIndices(Indices.size_bytes());

        const bool ShouldFlush = VertexBufferNoSpaceLeft || IndexBufferNoSpaceLeft;
        if (ShouldFlush)
        {
            Flush();
            if (VertexBufferNoSpaceLeft)
            {
                ReserveVertices(Points.size_bytes());
            }
            if (IndexBufferNoSpaceLeft)
            {
                ReserveIndices(Indices.size_bytes());
            }
        }

        m_VertexStream.write(std::bit_cast<const char*>(Points.data()), Points.size_bytes());
        m_IndexStream.write(std::bit_cast<const char*>(Indices.data()), Indices.size_bytes());
    }

    void SpriteBatch::DrawRectangle(
        const Math::Vector3& TopLeft,
        const Math::Vector3& TopRight,
        const Math::Vector3& BottomRight,
        const Math::Vector3& BottomLeft)
    {
        std::array Vertices{
            TopLeft,
            TopRight,
            BottomRight,
            BottomLeft
        };
        std::array<uint16_t, 6> Indices{
            0, 1, 2,
            0, 2, 3
        };
        DrawPolygon(Vertices, Indices);
    }

    void SpriteBatch::DrawTriangle(
        const Math::Vector3& Point1,
        const Math::Vector3& Point2,
        const Math::Vector3& Point3)
    {
        std::array Vertices{
            Point1,
            Point2,
            Point3
        };
        std::array<uint16_t, 3> Indices{
            0, 1, 2
        };
        DrawPolygon(Vertices, Indices);
    }

    void SpriteBatch::DrawLine(
        const Math::Vector3& Point1,
        const Math::Vector3& Point2)
    {
        std::array Vertices{
            Point1,
            Point2
        };
        std::array<uint16_t, 2> Indices{
            0, 1
        };
        DrawPolygon(Vertices, Indices);
    }

    //

    void SpriteBatch::End()
    {
        Flush();
        m_CmdList = nullptr;
    }

    //

    void SpriteBatch::Flush()
    {
        size_t DeltaVertices = static_cast<size_t>(m_VertexStream.tellp()) - m_LastVertexOffset;
        size_t DeltaIndices  = static_cast<size_t>(m_IndexStream.tellp()) - m_LastIndexOffset;

        if (!DeltaVertices || !DeltaIndices) [[unlikely]]
        {
            return;
        }

        m_VertexStream.flush();
        m_IndexStream.flush();

        Rhi::VertexBufferView VertexView{
            .Buffer = m_VertexBuffer,
            .Offset = m_LastVertexOffset
        };

        Rhi::IndexBufferView IndexView{
            .Buffer = m_IndexBuffer,
            .Offset = m_LastIndexOffset
        };

        Rhi::DrawIndexedDesc DrawDesc{
            .indexNum    = static_cast<uint32_t>(DeltaIndices / sizeof(uint16_t)),
            .instanceNum = 1,
        };

        m_CmdList->SetVertexBuffer(VertexView);
        m_CmdList->SetIndexBuffer(IndexView);
        m_CmdList->Draw(DrawDesc);

        m_LastVertexOffset = static_cast<size_t>(m_VertexStream.tellp());
        m_LastIndexOffset  = static_cast<size_t>(m_IndexStream.tellp());
    }

    //

    bool SpriteBatch::TryAllocateVertices(
        size_t Size) const
    {
        return m_Desc.EstimatedVertexBufferSize - m_LastVertexOffset >= Size;
    }

    bool SpriteBatch::TryAllocateIndices(
        size_t Size) const
    {
        return m_Desc.EstimatedIndexBufferSize - m_LastIndexOffset >= Size;
    }

    void SpriteBatch::ReserveVertices(
        size_t Size)
    {
        m_VertexBuffer = Rhi::Buffer(
            m_Desc.Device.get(),
            Rhi::MemoryLocation::HOST_UPLOAD,
            Rhi::BufferDesc{
                .size      = m_Desc.EstimatedVertexBufferSize,
                .usageMask = Rhi::BufferUsageBits::VERTEX_BUFFER });

        Rhi::Streaming::BufferView View(m_VertexBuffer, Rhi::EntireBuffer);
        m_VertexStream.open(View);
        m_LastVertexOffset = 0;
    }

    void SpriteBatch::ReserveIndices(
        size_t Size)
    {
        m_IndexBuffer = Rhi::Buffer(
            m_Desc.Device.get(),
            Rhi::MemoryLocation::HOST_UPLOAD,
            Rhi::BufferDesc{
                .size      = m_Desc.EstimatedIndexBufferSize,
                .usageMask = Rhi::BufferUsageBits::INDEX_BUFFER });

        Rhi::Streaming::BufferView View(m_IndexBuffer, Rhi::EntireBuffer);
        m_IndexStream.open(View);
        m_LastIndexOffset = 0;
    }
} // namespace Ame::Gfx