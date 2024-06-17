#pragma once

#include <variant>
#include <span>
#include <Ecs/VertexInput.hpp>

namespace Ame::Gfx::Shading
{
    class Material;
} // namespace Ame::Gfx::Shading

namespace Ame::Ecs::Component
{
    struct BaseRenderable
    {
        uint32_t CameraMask = 0xFFFF'FFFF;
    };

    struct BatchableRenderable
    {
    public:
        using IndexView16 = std::span<uint16_t>;
        using IndexView32 = std::span<uint32_t>;
        using VertexView  = std::span<VertexInput>;
        using IndexView   = std::variant<IndexView16, IndexView32>;

    public:
        Gfx::Shading::Material* Material = nullptr;
        VertexView              VertexBuffer;
        IndexView               IndexBuffer;
    };
} // namespace Ame::Ecs::Component