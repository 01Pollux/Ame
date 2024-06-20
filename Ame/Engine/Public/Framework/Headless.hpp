#pragma once

#include <Framework/Framework.hpp>

namespace Ame::Framework
{
    template<typename EngineType>
    class HeadlessApplication
    {
    public:
        struct Builder;

        void Run()
        {
            m_Engine.Run().get();
        }

        auto& Close()
        {
            m_Engine.Close();
            return *this;
        }

    private:
        template<typename... ArgsTy>
        HeadlessApplication(
            const Co::runtime_options& runtimeOptions,
            ArgsTy&&... args) :
            m_Engine(std::forward<ArgsTy>(args)...)
        {
            m_Engine.RegisterSubsystem<CoroutineSubsystem>(runtimeOptions);
        }

    private:
        EngineType m_Engine;
    };

    template<typename EngineType>
    struct HeadlessApplication<EngineType>::Builder
    {
    public:
        auto& RuntimeOptions()
        {
            return m_RuntimeOptions;
        }

        auto& RuntimeOptions(
            const Co::runtime_options& runtimeOptions)
        {
            m_RuntimeOptions = runtimeOptions;
            return *this;
        }

    public:
        template<typename... ArgsTy>
        [[nodiscard]] HeadlessApplication Build(
            ArgsTy&&... args)
        {
            return { m_RuntimeOptions, std::forward<ArgsTy>(args)... };
        }

    private:
        Co::runtime_options m_RuntimeOptions;
    };
} // namespace Ame::Framework
