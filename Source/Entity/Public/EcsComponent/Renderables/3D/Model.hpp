#pragma once

#include <EcsComponent/Renderables/BaseRenderable3D.hpp>

#include <Rhi/Core.hpp>

namespace Ame::Ecs
{
    // {E1E74357-EB73-4AF0-BA9D-4D17F7B89CF6}
    static constexpr UId IID_Model{ 0xe1e74357, 0xeb73, 0x4af0, { 0xba, 0x9d, 0x4d, 0x17, 0xf7, 0xb8, 0x9c, 0xf6 } };

    static constexpr uint32_t c_InvalidIndex = std::numeric_limits<uint32_t>::max();

    struct MeshNode
    {
        uint32_t              Parent    = c_InvalidIndex;
        Math::Matrix4x4       Transform = Math::Matrix4x4::Constants::Identity;
        String                Name;
        std::vector<uint32_t> Children;
        std::vector<uint32_t> SubMeshes;
    };

    struct SubMeshData
    {
        Math::Matrix4x4 Transform = Math::Matrix4x4::Constants::Identity;
        Geometry::AABB  AABB;

        uint32_t PositionOffset = c_InvalidIndex;
        uint32_t NormalOffset   = c_InvalidIndex;
        uint32_t TangentOffset  = c_InvalidIndex;
        uint32_t TexCoordOffset = c_InvalidIndex;

        uint32_t IndexOffset = c_InvalidIndex;
        uint32_t IndexCount  = c_InvalidIndex;

        uint32_t MaterialIndex = c_InvalidIndex;
    };

    class MeshModel : public BaseObject<IBaseRenderable3D>
    {
    public:
        using MeshNodeList    = std::vector<MeshNode>;
        using SubMeshDataList = std::vector<SubMeshData>;

        struct CreateDesc
        {
            MeshModel::MeshNodeList    MeshNodes;
            MeshModel::SubMeshDataList SubMeshes;

            Ptr<Dg::IBuffer> PositionBuffer;
            Ptr<Dg::IBuffer> NormalBuffer;
            Ptr<Dg::IBuffer> TangentBuffer;
            Ptr<Dg::IBuffer> TexCoordBuffer;
            Ptr<Dg::IBuffer> IndexBuffer;
            bool             SmallIndexBuffer = false;
        };

    public:
        using Base = BaseObject<IBaseRenderable3D>;
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_Model, Base);

        MeshModel(
            IReferenceCounters* counters,
            CreateDesc          desc) :
            Base(counters),
            m_MeshNodes(std::move(desc.MeshNodes)),
            m_SubMeshes(std::move(desc.SubMeshes)),
            m_PositionBuffer(std::move(desc.PositionBuffer)),
            m_NormalBuffer(std::move(desc.NormalBuffer)),
            m_TangentBuffer(std::move(desc.TangentBuffer)),
            m_TexCoordBuffer(std::move(desc.TexCoordBuffer)),
            m_IndexBuffer(std::move(desc.IndexBuffer)),
            m_SmallIndexBuffer(desc.SmallIndexBuffer)
        {
        }

    public:
        void Serialize(BinaryOArchiver& ar) const override
        {
        }

        void Deserialize(BinaryIArchiver& ar) override
        {
        }

    public:
        [[nodiscard]] Dg::IBuffer* GetPositionBuffer() const noexcept
        {
            return m_PositionBuffer;
        }
        [[nodiscard]] Dg::IBuffer* GetNormalBuffer() const noexcept
        {
            return m_NormalBuffer;
        }
        [[nodiscard]] Dg::IBuffer* GetTangentBuffer() const noexcept
        {
            return m_TangentBuffer;
        }
        [[nodiscard]] Dg::IBuffer* GetTexCoordBuffer() const noexcept
        {
            return m_TexCoordBuffer;
        }

        [[nodiscard]] Dg::IBuffer* GetIndexBuffer() const noexcept
        {
            return m_IndexBuffer;
        }
        [[nodiscard]] Dg::VALUE_TYPE GetIndexType() const noexcept
        {
            return m_SmallIndexBuffer ? Dg::VT_UINT16 : Dg::VT_UINT32;
        }

        [[nodiscard]] const MeshNodeList& GetMeshNodes() const noexcept
        {
            return m_MeshNodes;
        }
        [[nodiscard]] const SubMeshDataList& GetSubMeshes() const noexcept
        {
            return m_SubMeshes;
        }

    private:
        MeshNodeList    m_MeshNodes;
        SubMeshDataList m_SubMeshes;

        Ptr<Dg::IBuffer> m_PositionBuffer; // VI3_Position
        Ptr<Dg::IBuffer> m_NormalBuffer;   // VI3_Normal
        Ptr<Dg::IBuffer> m_TangentBuffer;  // VI3_Tangent
        Ptr<Dg::IBuffer> m_TexCoordBuffer; // VI_TexCoord
        Ptr<Dg::IBuffer> m_IndexBuffer;    // uint32_t | uint16_t=
        bool             m_SmallIndexBuffer = false;
    };
} // namespace Ame::Ecs