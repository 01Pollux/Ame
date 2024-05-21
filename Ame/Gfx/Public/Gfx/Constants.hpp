#pragma once

#include <cstdint>
#include <Rhi/Descs/Pipeline.hpp>
#include <Ecs/Component/Renderable/VertexInput.hpp>

namespace Ame::Gfx::Constants
{
    namespace Shading
    {
        using MaterialVertex = Ecs::Component::VertexInput;

        struct MaterialVertexDesc : Rhi::VertexInputDesc
        {
            static constexpr uint32_t MaxVertexAttributes = 4;

            MaterialVertexDesc()
            {
                Fill();
            }

        private:
            void Fill()
            {
                this->attributes   = m_Attributes;
                this->attributeNum = MaxVertexAttributes;
                this->streams      = m_Streams;
                this->streamNum    = 1;
            }

            nri::VertexAttributeDesc m_Attributes[MaxVertexAttributes]{
                { .d3d{ "POSITION" }, .vk{ 0 }, .offset = offsetof(MaterialVertex, Position), .format = Rhi::ResourceFormat::RGB32_SFLOAT },
                { .d3d{ "NORMAL" }, .vk{ 1 }, .offset = offsetof(MaterialVertex, Normal), .format = Rhi::ResourceFormat::RGB32_SFLOAT },
                { .d3d{ "TANGENT" }, .vk{ 2 }, .offset = offsetof(MaterialVertex, Normal), .format = Rhi::ResourceFormat::RGB32_SFLOAT },
                { .d3d{ "TEXCOORD" }, .vk{ 3 }, .offset = offsetof(MaterialVertex, TexCoord), .format = Rhi::ResourceFormat::RG32_SFLOAT }
            };

            nri::VertexStreamDesc m_Streams[1]{
                { .stride = sizeof(MaterialVertex) }
            };
        };
    } // namespace Shading

    namespace DescriptorRanges
    {
        static constexpr uint32_t MaterialData_SetIndex      = 20;
        static constexpr uint32_t MaterialData_RegisterSpace = 20;

        //

        static constexpr uint32_t FrameData_RegisterIndex = 0;
        static constexpr uint32_t FrameData_SetIndex      = 0;
        static constexpr uint32_t FrameData_RegisterSpace = 1;

        static constexpr Rhi::DescriptorRangeDesc FrameRangeDesc{
            .baseRegisterIndex = FrameData_RegisterIndex,
            .descriptorNum     = 1,
            .descriptorType    = Rhi::DescriptorType::CONSTANT_BUFFER,
            .shaderStages      = Rhi::ShaderType::COMPUTE_SHADER
        };

        static constexpr Rhi::DescriptorSetDesc FrameSetDesc{
            .registerSpace = FrameData_RegisterSpace,
            .ranges        = &FrameRangeDesc,
            .rangeNum      = 1
        };

        //

        static constexpr uint32_t EntityData_RegisterIndex = 0;
        static constexpr uint32_t EntityData_SetIndex      = 1;
        static constexpr uint32_t EntityData_RegisterSpace = 2;

        enum class EntityDataTypes : uint8_t
        {
            Transforms,
            RenderInstances,

            Count
        };

        static constexpr Rhi::DescriptorRangeDesc EntityRangeDesc{
            .baseRegisterIndex = EntityData_RegisterIndex,
            .descriptorNum     = std::to_underlying(EntityDataTypes::Count),
            .descriptorType    = Rhi::DescriptorType::STRUCTURED_BUFFER,
            .shaderStages      = Rhi::ShaderType::COMPUTE_SHADER
        };

        static constexpr Rhi::DescriptorSetDesc EntitySetDesc{
            .registerSpace = EntityData_RegisterSpace,
            .ranges        = &EntityRangeDesc,
            .rangeNum      = 1
        };
    } // namespace DescriptorRanges
} // namespace Ame::Gfx::Constants