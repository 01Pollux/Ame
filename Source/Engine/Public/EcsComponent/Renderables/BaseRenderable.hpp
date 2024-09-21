#pragma once

#include <Core/Serializable.hpp>
#include <EcsComponent/Core.hpp>

#include <Math/Matrix.hpp>
#include <Geometry/AABB.hpp>

#include <Rhi/Core.hpp>
#include <Shading/Material.hpp>

namespace Ame::Ecs
{
    template<typename Ty> struct RenderableVertex
    {
        Dg::IBuffer*              Buffer = nullptr;
        static constexpr uint64_t Stride = sizeof(Ty);

        auto operator<=>(const RenderableVertex& other) const noexcept
        {
            return Buffer <=> other.Buffer;
        }
    };

    struct RenderableVertices
    {
        RenderableVertex<Rhi::Vertex_Position> Position; // Vertex_Position
        RenderableVertex<Rhi::Vertex_Normal>   Normal;   // Vertex_Normal
        RenderableVertex<Rhi::Vertex_TexCoord> TexCoord; // Vertex_TexCoord
        RenderableVertex<Rhi::Vertex_Tangent>  Tangent;  // Vertex_Tangent
        uint32_t                               Offset = std::numeric_limits<uint32_t>::max();
        Rhi::MaterialVertexDesc                Desc;

        auto operator<=>(const RenderableVertices& other) const noexcept
        {
            return std::tie(Position, Normal, TexCoord, Tangent) <=>
                   std::tie(other.Position, other.Normal, other.TexCoord, other.Tangent);
        }
    };

    struct RenderableIndices
    {
        Dg::IBuffer*   Buffer = nullptr;
        uint32_t       Offset = std::numeric_limits<uint32_t>::max();
        uint32_t       Count  = std::numeric_limits<uint32_t>::max();
        Dg::VALUE_TYPE Type   = Dg::VT_UNDEFINED;

        auto operator<=>(const RenderableIndices& other) const noexcept
        {
            return std::tie(Buffer, Offset, Count, Type) <=>
                   std::tie(other.Buffer, other.Offset, other.Count, other.Type);
        }

        [[nodiscard]] uint32_t AbsoluteOffset() const noexcept
        {
            return (Type == Dg::VT_UINT16 ? sizeof(uint16_t) : sizeof(uint32_t)) * Offset;
        }
    };

    struct RenderableDesc
    {
        Rhi::Material*     Material = nullptr;
        RenderableVertices Vertices;
        RenderableIndices  Indices;

        auto operator<=>(const RenderableDesc& other) const noexcept
        {
            return std::tie(Material, Indices, Vertices) <=> std::tie(other.Material, other.Indices, other.Vertices);
        }
    };

    class IBaseRenderable : public ISerializable
    {
    public:
        using ISerializable::ISerializable;

        [[nodiscard]] const RenderableDesc& GetRenderableDesc()
        {
            return m_RenderableDesc;
        }

    protected:
        RenderableDesc m_RenderableDesc;
    };

    /// <summary>
    /// Automatically set when a renderable is added to an entity.
    /// the component will be set as pair, where the first pair is the component type, and the second pair is the
    /// renderable. (ComponentType, BaseRenderable)
    /// </summary>
    AME_ECS_WRAP_COMPONENT_REF(IBaseRenderable);
} // namespace Ame::Ecs