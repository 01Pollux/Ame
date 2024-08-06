#include <Graphics/RenderGraph/Passes/RecordIndirectCommands.hpp>

#include <Ecs/World.hpp>
#include <Graphics/RenderGraph/Common/EntityIterator.hpp>

namespace Ame::Gfx
{
    RecordIndirectCommandsPass::RecordIndirectCommandsPass(
        Ecs::World* world) :
        m_World(world)
    {
        Name("Record Indirect Commands Pass")
            .Flags(RG::PassFlags::Graphics)
            .Build(std::bind_front(&RecordIndirectCommandsPass::Build, this))
            .Execute(std::bind_front(&RecordIndirectCommandsPass::Execute, this));
    }

    void RecordIndirectCommandsPass::Build(
        RG::Resolver& resolver)
    {
    }

    void RecordIndirectCommandsPass::Execute(
        const RG::ResourceStorage&,
        Dg::IDeviceContext*)
    {
        auto& world       = *m_World;
        auto& entityGroup = world->get<Gfx::EntityDrawCommandGroup>()->get();

        for (auto& entities : entityGroup)
        {
            for (auto& entity : entities)
            {
            }
        }
    }
} // namespace Ame::Gfx