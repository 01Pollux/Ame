#pragma once

#include <cstdint>
#include <Rhi/Descs/Pipeline.hpp>
#include <Ecs/Component/Renderable/VertexInput.hpp>

namespace Ame::Gfx::Shading
{
    using Ecs_VSInput = Ecs::Component::VertexInput;

    struct MaterialVertexDesc : Rhi::VertexInputDesc
    {
        static constexpr uint32_t c_MaxVertexAttributes = 4;
        static constexpr uint32_t c_MaxVertexStreams    = 1;

        MaterialVertexDesc()
        {
            Fill();
        }

    private:
        void Fill()
        {
            this->attributes   = m_Attributes;
            this->attributeNum = c_MaxVertexAttributes;
            this->streams      = m_Streams;
            this->streamNum    = c_MaxVertexStreams;
        }

    private:
        Rhi::VertexAttributeDesc m_Attributes[c_MaxVertexAttributes]{
            { .d3d{ "POSITION" }, .vk{ 0 }, .offset = offsetof(Ecs_VSInput, Position), .format = Rhi::ResourceFormat::RGB32_SFLOAT, .streamIndex = 0 },
            { .d3d{ "NORMAL" }, .vk{ 1 }, .offset = offsetof(Ecs_VSInput, Normal), .format = Rhi::ResourceFormat::RGB32_SFLOAT, .streamIndex = 0 },
            { .d3d{ "TANGENT" }, .vk{ 2 }, .offset = offsetof(Ecs_VSInput, Tangent), .format = Rhi::ResourceFormat::RGB32_SFLOAT, .streamIndex = 0 },
            { .d3d{ "TEXCOORD" }, .vk{ 3 }, .offset = offsetof(Ecs_VSInput, TexCoord), .format = Rhi::ResourceFormat::RG32_SFLOAT, .streamIndex = 0 }
        };

        Rhi::VertexStreamDesc m_Streams[c_MaxVertexStreams]{
            { .stride = sizeof(Ecs_VSInput), .bindingSlot = 0, .stepRate = Rhi::VertexStreamStepRate::PER_VERTEX }
        };
    };
} // namespace Ame::Gfx::Shading
