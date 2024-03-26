#pragma once

#include <Engine/Engine.hpp>

namespace Ame::Framework
{
    template<typename EngineType>
        requires std::is_base_of_v<Ame::BaseEngine, EngineType>
    class HeadlessApplication
    {
    public:
        struct Builder;

        auto& Run()
        {
            m_Engine.Run();
            return *this;
        }
        
        auto& Close()
        {
            m_Engine.Close();
            return *this;
        }

    private:
        template<typename... ArgsTy>
        HeadlessApplication(
            ArgsTy&&... Args) :
            m_Engine(std::forward<ArgsTy>(Args)...)
        {
        }

    private:
        EngineType m_Engine;
    };

    template<typename EngineType>
        requires std::is_base_of_v<Ame::BaseEngine, EngineType>
    struct HeadlessApplication<EngineType>::Builder
    {
        const char* Name;

        auto SetName(
            const char* CurName) -> Builder&
        {
            Name = CurName;
            return *this;
        }

        template<typename... ArgsTy>
        HeadlessApplication Build(ArgsTy&&... Args)
        {
            return HeadlessApplication(std::forward<ArgsTy>(Args)...);
        }
    };
} // namespace Ame::Framework
