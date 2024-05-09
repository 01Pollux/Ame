#include <FlappyRocket/Engine.hpp>
#include <Rhi/Subsystem/Device.hpp>
#include <FlappyRocket/Subsystem/Game.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketEngine::FlappyRocketEngine()
    {
        Log::Logger::Register(Log::Names::Engine, "Logs/Engine.log");
        Log::Logger::Register(Log::Names::Rhi, "Logs/Engine.log");
        Log::Logger::Register(Log::Names::Renderer, "Logs/Engine.log");
        Log::Logger::Register(Log::Names::Ecs, "Logs/Engine.log");
        Log::Logger::Register(Log::Names::Client, "Logs/FlappyRocket.log");

        Log::Engine().SetLevel(Log::LogLevel::Warning);
        Log::Client().SetLevel(Log::LogLevel::Trace);
    }

    void FlappyRocketEngine::Initialize()
    {
        BaseEngine::Initialize();

        SetClearColor(GetSubsystem<Rhi::DeviceSubsystem>());

        m_Game = GetSubsystem<FlappyRocketGameSubsystem>();
        CreateWorld();
    }

    void FlappyRocketEngine::SetClearColor(
        Rhi::Device& Device) const
    {
        Device.SetClearColor(Colors::Gray);
    }

    void FlappyRocketEngine::CreateWorld()
    {
        m_Game->ResetWorld();
        m_Game->AddAllEntities();
    }
} // namespace Ame::FlappyRocket