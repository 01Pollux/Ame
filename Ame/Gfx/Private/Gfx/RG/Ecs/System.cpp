#include <Gfx/RG/Ecs/System.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>

namespace Ame::Gfx::RG
{
    EcsSystemHooks::EcsSystemHooks(
        Ecs::Universe& universe,
        CoreResources& coreResources) :
        m_Universe(universe),
        m_CoreResources(coreResources),
        m_OnWorldChange(
            m_Universe.get().OnWorldChange(
                [this](auto& changeData)
                {
                    m_WorldData = {};
                    if (changeData.NewWorld)
                    {
                        auto& newWorld = *changeData.NewWorld;
                        RegisterModules(newWorld);
                        CreateTransformObserver(newWorld);
                        CreateCameraRule(newWorld);
                    }
                }))
    {
    }
} // namespace Ame::Gfx::RG