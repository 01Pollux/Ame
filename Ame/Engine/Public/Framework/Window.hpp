#pragma once

#include <Engine/Engine.hpp>

namespace Ame::Framework
{
    template<typename EngineType>
        requires std::is_base_of_v<Ame::BaseEngine, EngineType>
    class WindowApplication
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
    struct WindowApplication<EngineType>::Builder
    {
    public:
        auto& Name(
            const char* Name)
        {
            m_Name = Name;
            return *this;
        }

        template<typename... ArgsTy>
        auto Build(
            ArgsTy&&... Args)
        {
            return WindowApplication<EngineType>(std::forward<ArgsTy>(Args)...);
        }

    private:
        const char* m_Name = "Ame";
    };
} // namespace Ame::Framework
