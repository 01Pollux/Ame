#pragma once

#include <Math/Colors.hpp>
#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::Shading
{
    class Material;
} // namespace Ame::Gfx::Shading

namespace Ame::Ecs::Component
{
    struct Sprite
    {
        Ptr<Gfx::Shading::Material> Material = nullptr;

        uint32_t CameraMask = 0xFFFF'FFFF;

        std::vector<VertexInput> Vertices;
        std::vector<uint16_t>    Indices;

        Math::Color4  ModulationColor = Colors::White;
        Math::Vector2 UVStart         = Math::Vector2(0.0f, 0.0f);
        Math::Vector2 UVEnd           = Math::Vector2(1.0f, 1.0f);

        [[nodiscard]] static Sprite Quad() noexcept
        {
            return {
                .Vertices{
                    {
                        .Position = Math::Vector3(-0.5f, -0.5f, 0.0f),
                        .Normal   = Math::Vector3(0.0f, 0.0f, 1.0f),
                        .Tangent  = Math::Vector3(1.0f, 0.0f, 0.0f),
                        .TexCoord = Math::Vector2(0.0f, 0.0f),
                    },
                    {
                        .Position = Math::Vector3(-0.5f, 0.5f, 0.0f),
                        .Normal   = Math::Vector3(0.0f, 0.0f, 1.0f),
                        .Tangent  = Math::Vector3(1.0f, 0.0f, 0.0f),
                        .TexCoord = Math::Vector2(0.0f, 1.0f),
                    },
                    {
                        .Position = Math::Vector3(0.5f, 0.5f, 0.0f),
                        .Normal   = Math::Vector3(0.0f, 0.0f, 1.0f),
                        .Tangent  = Math::Vector3(1.0f, 0.0f, 0.0f),
                        .TexCoord = Math::Vector2(1.0f, 1.0f),
                    },
                    {
                        .Position = Math::Vector3(0.5f, -0.5f, 0.0f),
                        .Normal   = Math::Vector3(0.0f, 0.0f, 1.0f),
                        .Tangent  = Math::Vector3(1.0f, 0.0f, 0.0f),
                        .TexCoord = Math::Vector2(1.0f, 0.0f),
                    } },
                .Indices{ 0, 1, 2, 2, 3, 0 }
            };
        }
    };
} // namespace Ame::Ecs::Component