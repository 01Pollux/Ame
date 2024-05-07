#include <Gfx/RG/Passes/EntityCollectPass.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG::Std
{
    EntityCollectPass::EntityCollectPass(
        Ecs::Universe& Universe) :
        m_Universe(Universe)
    {
        Name("EntityCollectPass")
            .SetFlags(PassFlags::Compute)
            .Build(
                [this](Resolver& RgResolver)
                {
                    auto& World       = *m_Universe.get().GetActiveWorld();
                    auto  EntityCount = std::max(World.CreateFilter<const Ecs::Component::BaseRenderable>().build().count(), MinEntities);

                    RgResolver.CreateBuffer(
                        Names::EntityDispatchCounter,
                        Rhi::BufferDesc{ Math::AlignUp(sizeof(int) * EntityCount, BufferAlignment) });
                    RgResolver.CreateBuffer(
                        Names::EntityDispatchBuffer,
                        Rhi::BufferDesc{ Math::AlignUp(sizeof(DispatchDesc) * EntityCount, BufferAlignment) });

                    RgResolver.WriteBuffer(
                        Names::EntityDispatchCounter("Main"),
                        Rhi::ShaderBits::COMPUTE_SHADER);
                    RgResolver.WriteBuffer(
                        Names::EntityDispatchBuffer("Main"),
                        Rhi::ShaderBits::COMPUTE_SHADER);
                })
            .Execute(
                [this](const ResourceStorage& RgStorage, Rhi::CommandList* CommandList) {
                });
    }
} // namespace Ame::Gfx::RG::Std