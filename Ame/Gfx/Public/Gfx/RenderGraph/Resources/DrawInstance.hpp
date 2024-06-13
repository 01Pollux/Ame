#pragma once

#include <Gfx/RenderGraph/Resources/RenderInstance.hpp>
#include <Rhi/Resource/Buffer.hpp>

namespace nri
{
    struct Buffer;
} // namespace nri

namespace Ame::Gfx::Shading
{
    class Material;
} // namespace Ame::Gfx::Shading

//

namespace Ame::Gfx
{
    struct DrawInstance
    {
        /// <summary>
        /// TODO: optional material, will default to DEV material
        /// </summary>
        Shading::Material* Material = nullptr;
        nri::Buffer*       VertexBuffer;
        nri::Buffer*       IndexBuffer;

        size_t VertexOffset;
        size_t IndexOffset;
        size_t IndexCount;

        Rhi::IndexType IndexType;
    };

    struct DrawInstanceOrder
    {
        /// <summary>
        /// Used for sorting render instances, the lower the value, the earlier the instance is rendered
        /// Can be thought of as the Z-Index of the instance
        /// </summary>
        DrawInstance Instance;
        float        Order;

        constexpr auto operator<=>(const DrawInstanceOrder& other) const
        {
            // we should sort by [material, vertexBuffer, indexBuffer, order]
            return std::tie(Instance.Material, Instance.VertexBuffer, Instance.IndexBuffer, Order) <=>
                   std::tie(other.Instance.Material, other.Instance.VertexBuffer, other.Instance.IndexBuffer, other.Order);
        }
    };
} // namespace Ame::Gfx