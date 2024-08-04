#pragma once

#include <Core/Serializable.hpp>
#include <Math/Matrix.hpp>
#include <Geometry/AABB.hpp>

#include <Rhi/Core.hpp>
#include <Shading/Material.hpp>

namespace Ame::Ecs
{
    using Vertex_Position = Math::Vector3;
    using Vertex_Normal   = Math::Vector3;
    using Vertex_TexCoord = Math::Vector2;
    using Vertex_Tangent  = Math::Vector3;

    //

    struct RenderableVertex
    {
        Dg::IBuffer* Buffer = nullptr;
        uint32_t     Offset = std::numeric_limits<uint32_t>::max();

        auto operator<=>(const RenderableVertex& other) const noexcept
        {
            return std::tie(Buffer, Offset) <=> std::tie(other.Buffer, other.Offset);
        }
    };

    struct RenderableVertices
    {
        RenderableVertex Position; // Vertex_Position
        RenderableVertex Normal;   // Vertex_Normal
        RenderableVertex TexCoord; // Vertex_TexCoord
        RenderableVertex Tangent;  // Vertex_Tangent

        auto operator<=>(const RenderableVertices& other) const noexcept
        {
            return std::tie(Position, Normal, TexCoord, Tangent) <=>
                   std::tie(other.Position, other.Normal, other.TexCoord, other.Tangent);
        }
    };

    struct RenderableIndices
    {
        Dg::IBuffer*   Buffer = nullptr;
        uint64_t       Offset = std::numeric_limits<uint32_t>::max();
        uint32_t       Count  = std::numeric_limits<uint32_t>::max();
        Dg::VALUE_TYPE Type   = Dg::VT_UNDEFINED;

        auto operator<=>(const RenderableIndices& other) const noexcept
        {
            return std::tie(Buffer, Offset, Count, Type) <=>
                   std::tie(other.Buffer, other.Offset, other.Count, other.Type);
        }
    };

    struct RenderableDesc
    {
        Rhi::Material*     Material = nullptr;
        RenderableVertices Vertices;
        RenderableIndices  Indices;

        auto operator<=>(const RenderableDesc& other) const noexcept
        {
            return std::tie(Material, Vertices, Indices) <=>
                   std::tie(other.Material, other.Vertices, other.Indices);
        }
    };

    class IBaseRenderable : public ISerializable
    {
    public:
        [[nodiscard]] const RenderableDesc& GetRenderableDesc()
        {
            return m_RenderableDesc;
        }

    protected:
        RenderableDesc m_RenderableDesc;
    };

    /// <summary>
    /// Automatically set when a renderable is added to an entity.
    /// the component will be set as pair, where the first pair is the component type, and the second pair is the renderable.
    /// (ComponentType, BaseRenderable)
    /// </summary>
    struct RenderableComponent
    {
        IBaseRenderable* Object;
    };
} // namespace Ame::Ecs