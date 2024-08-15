#pragma once

#include <EcsComponent/Renderables/3D/Model.hpp>

namespace Ame::Ecs
{
    // {75B8DFC4-B5C5-4991-8230-D37C53E814FC}
    static constexpr UId IID_StaticMesh{ 0x75b8dfc4, 0xb5c5, 0x4991, { 0x82, 0x30, 0xd3, 0x7c, 0x53, 0xe8, 0x14, 0xfc } };

    class StaticMesh : public BaseObject<IBaseRenderable3D>
    {
    public:
        using Base = BaseObject<IBaseRenderable3D>;
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_StaticMesh, Base);

        StaticMesh(
            IReferenceCounters* counters,
            MeshModel*          model,
            uint32_t            subMeshIndex) :
            Base(counters),
            m_Model(model),
            m_SubMeshIndex(subMeshIndex)
        {
            UpdateRenderableDesc();
        }

    public:
        void Serialize(BinaryOArchiver& ar) const override
        {
        }

        void Deserialize(BinaryIArchiver& ar) override
        {
        }

    public:
        [[nodiscard]] MeshModel* GetModel() const
        {
            return m_Model;
        }

        uint32_t GetSubMeshIndex() const
        {
            return m_SubMeshIndex;
        }

    public:
        void SetMaterialOverride(
            Ptr<Rhi::Material> material)
        {
            m_OverrideMaterial = std::move(material);
            UpdateRenderableDesc();
        }

        [[nodiscard]] Rhi::Material* GetMaterialOverride() const noexcept
        {
            return m_OverrideMaterial;
        }

    public:
        [[nodiscard]] Rhi::Material* GetMaterial() const noexcept
        {
            if (m_OverrideMaterial)
            {
                return m_OverrideMaterial;
            }
            else
            {
                auto& submeshData = m_Model->GetSubMeshes()[m_SubMeshIndex];
                return m_Model->GetMaterials()[submeshData.MaterialIndex];
            }
        }

    private:
        void UpdateRenderableDesc()
        {
            auto& submeshData = m_Model->GetSubMeshes()[m_SubMeshIndex];

            m_RenderableDesc.Material = GetMaterial();
            m_RenderableDesc.Vertices = {
                .Position{ m_Model->GetPositionBuffer() },
                .Normal{ m_Model->GetNormalBuffer() },
                .TexCoord{ m_Model->GetTexCoordBuffer() },
                .Tangent{ m_Model->GetTangentBuffer() },
                .Offset{ submeshData.VertexOffset },
                .Desc{ m_Model->GetVertexInputDesc() }
            };
            m_RenderableDesc.Indices = {
                m_Model->GetIndexBuffer(),
                submeshData.IndexOffset,
                submeshData.IndexCount,
                m_Model->GetIndexType()
            };
        }

    private:
        Ptr<MeshModel>     m_Model;
        Ptr<Rhi::Material> m_OverrideMaterial;
        uint32_t           m_SubMeshIndex = 0;
    };

    //

    struct StaticMeshComponent
    {
        Ptr<StaticMesh> Object;

        StaticMeshComponent(
            Ptr<StaticMesh> object = {}) :
            Object(std::move(object))
        {
        }
    };
} // namespace Ame::Ecs