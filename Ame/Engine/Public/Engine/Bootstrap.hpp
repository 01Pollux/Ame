#pragma once

#include <Engine/Engine.hpp>

namespace Ame::Bootstrap
{
    template<typename Ty>
    concept EngineConcept = requires {
        std::is_base_of_v<BaseAmeEngine, Ty>;
    };

    template<EngineConcept Ty = BaseAmeEngine>
    class Application
    {
    public:
        template<typename Ty, typename... ArgsTy>
        auto& With(
            ArgsTy&&... args)
        {
            m_Container.emplace<Ty>(std::forward<ArgsTy>(args)...);
            return *this;
        }

        [[nodiscard]] Ty Build() &&
        {
            return Ty(std::move(m_Container));
        }

        [[nodiscard]] Ty Build() const&
        {
            return Ty(m_Container.fork());
        }

    private:
        kgr::container m_Container;
    };
} // namespace Ame::Bootstrap