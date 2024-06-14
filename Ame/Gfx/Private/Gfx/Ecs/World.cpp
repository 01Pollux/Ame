#include <Gfx/Ecs/World.hpp>
#include <Gfx/Ecs/Modules.hpp>
#include <Gfx/Compositor/EcsWorldResourcesDesc.hpp>

namespace Ame::Gfx
{
    EcsWorldResources::EcsWorldResources(
        Rhi::Device&                 rhiDevice,
        Ecs::Universe&               universe,
        const EcsWorldResourcesDesc& desc) :
        m_Universe(universe),
        m_OnWorldChange(
            m_Universe.get().OnWorldChange(
                [this](auto& changeData)
                {
                    m_WorldData = {};
                    if (changeData.NewWorld)
                    {
                        auto& newWorld = *changeData.NewWorld;
                        RegisterModules(newWorld);
                        CreateObservers(newWorld);
                    }
                })),
        m_TransformBuffer(rhiDevice, desc.TransformBufferDesc),
        m_AABBBuffer(rhiDevice, desc.AABBBufferDesc)
    {
    }

    void EcsWorldResources::RegisterModules(
        Ecs::World& world)
    {
        world.ImportModule<RGModule>();
    }
} // namespace Ame::Gfx