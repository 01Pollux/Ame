#pragma once

#include <Ecs/Universe.hpp>

namespace Ame::Signals
{
    /// <summary>
    /// Helper class to destruct the object of type Ty when the world changes
    /// </summary>
    template<typename Ty>
    class OnWorldChangeHelper : public OnWorldChange::Handle
    {
    public:
        OnWorldChangeHelper() = default;

        template<std::invocable<Ecs::Universe&, const Data::WorldChange&> FnTy>
        OnWorldChangeHelper(
            OnWorldChange& Sig,
            FnTy&&         Delegate) :
            Handle(Sig,
                   [this, Delegate = std::move(Delegate)](auto& This, auto& SigData)
                   {
                       m_Object = {};
                       Delegate(This, SigData);
                   })
        {
        }

        [[nodiscard]] Ty* operator->() noexcept
        {
            return &m_Object;
        }

        [[nodiscard]] Ty& Get() noexcept
        {
            return m_Object;
        }

    private:
        Ty m_Object;
    };
} // namespace Ame::Signals
