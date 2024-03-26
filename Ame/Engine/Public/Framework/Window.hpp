#pragma once

#include <Engine/Engine.hpp>

#define AME_GETSET_IMPL(Func, Name) \
    const auto& Func() const        \
    {                               \
        return Name;                \
    }                               \
    auto& Func(Type Value)          \
    {                               \
        Name = Value;               \
        return *this;               \
    }

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
        AME_GETSET_IMPL(Name, m_Name);

        template<typename... ArgsTy>
        HeadlessApplication Build(ArgsTy&&... Args)
        {
            return HeadlessApplication(std::forward<ArgsTy>(Args)...);
        }

    private:
        const char* m_Name = "Ame";
    };
} // namespace Ame::Framework

#undef AME_GETSET_IMPL
