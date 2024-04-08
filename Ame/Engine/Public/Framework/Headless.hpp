#pragma once

#include <Framework/Framework.hpp>

namespace Ame::Framework
{
    template<typename EngineType>
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
            Ptr<Co::runtime> Runtime,
            ArgsTy&&... Args) :
            m_Engine(std::forward<ArgsTy>(Args)...)
        {
            m_Engine.RegisterSubsystem<CoroutineSubsystem>(std::move(Runtime));
        }

    private:
        EngineType m_Engine;
    };

    template<typename EngineType>
    struct HeadlessApplication<EngineType>::Builder
    {
    public:
        auto& Runtime(
            Ptr<Co::runtime> Runtime)
        {
            m_Runtime = std::move(Runtime);
            return *this;
        }

    public:
        template<typename... ArgsTy>
        [[nodiscard]] HeadlessApplication Build(
            ArgsTy&&... Args)
        {
            if (!m_Runtime)
            {
                m_Runtime = std::make_shared<Co::runtime>();
            }

            return { std::move(m_Runtime), std::forward<ArgsTy>(Args)... };
        }

    private:
        Ptr<Co::runtime> m_Runtime;
    };
} // namespace Ame::Framework
