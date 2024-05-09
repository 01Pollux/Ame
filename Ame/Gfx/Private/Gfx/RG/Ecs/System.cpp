#include <Gfx/RG/Ecs/System.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>

namespace Ame::Gfx::RG
{
    EcsSystemHooks::EcsSystemHooks(
        Ecs::Universe& Universe,
        CoreResources& Resources) :
        m_Universe(Universe),
        m_CoreResources(Resources)
    {
        m_OnWorldChange = {
            m_Universe.get().OnWorldChange().ObjectSignal(),
            [this](auto& Universe, auto& ChangeData)
            {
                m_WorldData = {};
                if (ChangeData.NewWorld)
                {
                    CreateTransformObserver();
                    CreateCameraRule();
                }
            }
        };
    }
} // namespace Ame::Gfx::RG