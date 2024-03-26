#pragma once

#include <Engine/Engine.hpp>

namespace Ame::Framework
{
    template<typename BaseEngine>
        requires std::is_base_of_v<Ame::BaseEngine, BaseEngine>
    class HeadlessApplication
    {
    public:
        struct Builder;

        void Run()
        {
            m_Engine.Run();
        }

    private:
        AmeEngine m_Engine;
    };

    template<typename BaseEngine>
        requires std::is_base_of_v<Ame::BaseEngine, BaseEngine>
    struct HeadlessApplication<BaseEngine>::Builder
    {
        const char* Name;

        auto SetName(
            const char* CurName) -> Builder&
        {
            Name = CurName;
            return *this;
        }

        HeadlessApplication Build();
    };
} // namespace Ame::Framework
