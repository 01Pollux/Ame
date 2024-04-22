#include <FlappyRocket/Engine.hpp>

#include <Engine/Subsystem/Timer.hpp>
#include <Rhi/Subsystem/Device.hpp>
#include <Ecs/Subsystem/Entity.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketEngine::FlappyRocketEngine()
    {
        Log::Logger::Register(Log::Names::Engine, "Logs/Engine.log");
        Log::Logger::Register(Log::Names::Ecs, "Logs/Engine.log");
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

        m_Game = FlappyRocketGame(GetSubsystem<Ecs::EntitySubsystem>());
        CreateWorld();
    }

    void FlappyRocketEngine::SetClearColor(
        Rhi::Device& Device) const
    {
        Device.SetClearColor(Colors::Gray);
    }

    void FlappyRocketEngine::CreateWorld()
    {
        m_Game.ResetWorld();
    }

    //

    void FlappyRocketEngine::LoopUpdate(
        const EngineTimer& Timer)
    {
        m_Game.Update(Timer);
    }
} // namespace Ame::FlappyRocket