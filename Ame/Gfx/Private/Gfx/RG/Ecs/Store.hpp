#pragma once

#include <set>
#include <Ecs/World.hpp>

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/PipelineState.hpp>

namespace Ame::Gfx::RG
{
    struct EcsPipelineEntity
    {
        uint32_t InstanceId;
        uint32_t VertexOffset;
        uint32_t IndexOffset;
        uint32_t IndexCount;
        float    Depth = 0.f;

        auto operator<=>(const EcsPipelineEntity& Other) const noexcept
        {
            return std::tie(Depth, Other.InstanceId) <=> std::tie(Other.Depth, Other.InstanceId);
        }
    };

    struct EcsPipelineRow
    {
        std::set<EcsPipelineEntity> Entities;
        Ptr<Rhi::PipelineState>     PipelineState;
        Rhi::Buffer                 VertexBuffer;
        Rhi::Buffer                 IndexBuffer;
        float                       EffectiveDepth = 0.f; // sqrt(sum(depth^2) / count)

        EcsPipelineRow(
            Ptr<Rhi::PipelineState> PipelineState,
            const Rhi::Buffer&      VertexBuffer,
            const Rhi::Buffer&      IndexBuffer) :
            PipelineState(std::move(PipelineState)),
            VertexBuffer(VertexBuffer),
            IndexBuffer(IndexBuffer)
        {
            if (!Entities.empty()) [[unlikely]]
            {
                for (const auto& Entity : Entities)
                {
                    EffectiveDepth += Entity.Depth * Entity.Depth;
                }
                EffectiveDepth = std::sqrt(EffectiveDepth / Entities.size());
            }
        }

        auto operator<=>(const EcsPipelineRow& Other) const noexcept
        {
            return std::tie(EffectiveDepth, Entities, PipelineState) <=> std::tie(Other.EffectiveDepth, Other.Entities, PipelineState);
        }
    };

    class EcsStore
    {
    public:
        EcsStore(
            Ecs::World& World);

    private:
        Ecs::UniqueRule<> m_RenderableRule;
    };
} // namespace Ame::Gfx::RG