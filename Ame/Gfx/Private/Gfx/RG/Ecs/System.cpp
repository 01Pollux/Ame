#include <Gfx/RG/Ecs/System.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>

#include <Ecs/Component/Renderable/Renderable.hpp>
#include <Ecs/Component/Math/Transform.hpp>

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
                OnWorldChange();
            }
        };
    }

    EcsSystemHooks::EcsSystemHooks(
        EcsSystemHooks&& Other) noexcept :
        m_Universe(std::move(Other.m_Universe)),
        m_CoreResources(std::move(Other.m_CoreResources)),
        m_OnWorldChange(std::move(Other.m_OnWorldChange)),
        m_TransformObserver(std::move(Other.m_TransformObserver))
    {
    }

    EcsSystemHooks& EcsSystemHooks::operator=(
        EcsSystemHooks&& Other)
    {
        if (this != &Other)
        {
            Reset();
            m_Universe          = std::move(Other.m_Universe);
            m_CoreResources     = std::move(Other.m_CoreResources);
            m_OnWorldChange     = std::move(Other.m_OnWorldChange);
            m_TransformObserver = std::move(Other.m_TransformObserver);
        }
        return *this;
    }

    EcsSystemHooks::~EcsSystemHooks()
    {
        Reset();
    }

    //

    void EcsSystemHooks::Reset()
    {
        m_TransformObserver = {};
    }

    void EcsSystemHooks::ApplyHooks()
    {
        ApplyTransformObserver();
    }

    //

    void EcsSystemHooks::ApplyTransformObserver()
    {
        auto& World = *m_Universe.get().GetActiveWorld();

        auto TransformObserverCallback =
            [this](Ecs::Iterator& Iter, const Ecs::Component::Transform* Transforms)
        {
            auto& TransformBuffer = m_CoreResources.get().GetTransformBuffer();
            for (size_t i : Iter)
            {
                auto&       Transform = Transforms[i];
                Ecs::Entity Entity(Iter.entity(i));

                auto& InstanceInfo = Entity.GetComponentMut<Ecs::Gfx::Component::RenderInstance>();
                if (Iter.event() == flecs::OnSet)
                {
                    if (InstanceInfo.TransformIndex == TransformBuffer.InvalidIndex)
                    {
                        InstanceInfo.TransformIndex = TransformBuffer.Rent(Transform);
                    }
                    TransformBuffer.Write(InstanceInfo.TransformIndex, Transform);
                }
                else
                {
                    if (InstanceInfo.TransformIndex != TransformBuffer.InvalidIndex)
                    {
                        TransformBuffer.Return(InstanceInfo.TransformIndex);
                        InstanceInfo.TransformIndex = TransformBuffer.InvalidIndex;
                    }
                }
            }
        };

        m_TransformObserver =
            World.CreateObserver<const Ecs::Component::Transform>()
                .with<const Ecs::Component::Renderable>()
                .event(flecs::OnSet)
                .event(flecs::OnRemove)
                .iter(TransformObserverCallback);
    }

    //

    void EcsSystemHooks::OnWorldChange()
    {
        Reset();
        ApplyHooks();
    }
} // namespace Ame::Gfx::RG