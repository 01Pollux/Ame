#include <Gfx/Ecs/System.hpp>
#include <Gfx/Ecs/Modules.hpp>
#include <Gfx/Compositor/EcsSystemDesc.hpp>

namespace Ame::Gfx
{
    EcsSystemHooks::EcsSystemHooks(
        Rhi::Device&         rhiDevice,
        Ecs::Universe&       universe,
        const EcsSystemDesc& desc) :
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

    void EcsSystemHooks::RegisterModules(
        Ecs::World& world)
    {
        world.ImportModule<RGModule>();
    }
} // namespace Ame::Gfx