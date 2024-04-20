#include <FlappyRocket/Engine.hpp>

#include <Engine/Subsystem/Timer.hpp>
#include <Rhi/Subsystem/Device.hpp>
#include <Ecs/Subsystem/Entity.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketEngine::FlappyRocketEngine() :
        m_Game(GetSubsystem<Ecs::EntitySubsystem>())
    {
        Log::Logger::Register(Log::Names::Engine, "Logs/Engine.log");
        Log::Logger::Register(Log::Names::Client, "Logs/FlappyRocket.log");

        Log::Engine().SetLevel(Log::LogLevel::Warning);
        Log::Client().SetLevel(Log::LogLevel::Trace);
    }

    void FlappyRocketEngine::Initialize()
    {
        BaseEngine::Initialize();

        OnUpdate().ObjectSignal().Listen(
            [](BaseEngine& Engine)
            {
                auto& Self = static_cast<FlappyRocketEngine&>(Engine);
                Self.LoopUpdate(
                    Self.GetSubsystem<TimerSubsystem>());
            });

        SetClearColor(GetSubsystem<Rhi::DeviceSubsystem>());
    }

    void FlappyRocketEngine::SetClearColor(
        Rhi::Device& Device) const
    {
        Device.SetClearColor(Colors::Gray);
    }

    //

    void FlappyRocketEngine::LoopUpdate(
        const EngineTimer& Timer)
    {
    }
} // namespace Ame::FlappyRocket