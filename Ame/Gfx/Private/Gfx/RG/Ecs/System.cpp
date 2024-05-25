#include <Gfx/RG/Ecs/System.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>

namespace Ame::Gfx::RG
{
    EcsSystemHooks::EcsSystemHooks(
        Ecs::Universe& universe,
        CoreResources& coreResources) :
        m_Universe(universe),
        m_CoreResources(coreResources)
    {
        m_OnWorldChange = {
            m_Universe.get().OnWorldChange().ObjectSignal(),
            [this](auto& universe, auto& changeData)
            {
                m_WorldData = {};
                if (changeData.NewWorld)
                {
                    CreateTransformObserver();
                    CreateCameraRule();
                }
            }
        };
    }
} // namespace Ame::Gfx::RG