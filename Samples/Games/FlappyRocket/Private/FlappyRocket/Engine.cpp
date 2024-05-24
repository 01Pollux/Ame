#include <FlappyRocket/Engine.hpp>

#include <Rhi/Subsystem/Device.hpp>
#include <Asset/Subsystem/Storage.hpp>
#include <FlappyRocket/Subsystem/Game.hpp>

#include <Asset/Packs/Directory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketEngine::FlappyRocketEngine()
    {
        Log::Logger::Register(Log::Names::Engine, "Engine.log");
        Log::Logger::Register(Log::Names::Rhi, "Engine.log");
        Log::Logger::Register(Log::Names::Gfx, "Engine.log");
        Log::Logger::Register(Log::Names::Ecs, "Engine.log");
        Log::Logger::Register(Log::Names::Asset, "Engine.log");
        Log::Logger::Register(Log::Names::Client, "FlappyRocket.log");

        Log::Engine().SetLevel(Log::LogLevel::Warning);
        Log::Client().SetLevel(Log::LogLevel::Trace);
    }

    void FlappyRocketEngine::Initialize()
    {
        BaseEngine::Initialize();

        SetClearColor(GetSubsystem<Rhi::DeviceSubsystem>());

        auto& AssetStorage = GetSubsystem<Asset::StorageSubsystem>();
        AssetStorage.Mount<Asset::DirectoryAssetPackage>("Shared/Assets");

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