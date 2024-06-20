#include <FlappyRocket/Engine.hpp>

#include <Subsystem/Rhi/Device.hpp>
#include <Subsystem/Asset/Storage.hpp>
#include <Subsystem/Gfx/Pipelines/ForwardPlusPipeline.hpp>
#include <Subsystem/FlappyRocket/Game.hpp>

#include <Asset/Packs/Directory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketEngine::FlappyRocketEngine()
    {
        Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
        Log::Logger::Register(Log::Names::c_Rhi, "Engine.log");
        Log::Logger::Register(Log::Names::c_Gfx, "Engine.log");
        Log::Logger::Register(Log::Names::c_Ecs, "Engine.log");
        Log::Logger::Register(Log::Names::c_Asset, "Engine.log");
        Log::Logger::Register(Log::Names::c_Client, "FlappyRocket.log");

        Log::Engine().SetLevel(Log::LogLevel::Warning);
        Log::Client().SetLevel(Log::LogLevel::Trace);
    }

    Co::result<void> FlappyRocketEngine::Run()
    {
        auto& engineFrame = GetSubsystem<EngineFrameSubsystem>();

        Initialize();

        while (engineFrame.IsRunning())
        {
            engineFrame.Tick();
        }

        co_return;
    }

    Co::null_result FlappyRocketEngine::Initialize()
    {
        auto& assetStorage = GetSubsystem<Asset::StorageSubsystem>();
        assetStorage.Mount<Asset::DirectoryAssetPackage>("Shared/Assets");

        InstallSubsystem<Gfx::GraphRendererSubsystem>();
        m_Game = GetSubsystem<FlappyRocketGameSubsystem>();

        CreateWorld();
        co_return;
    }

    void FlappyRocketEngine::CreateWorld()
    {
        m_Game->ResetWorld();
        m_Game->AddAllEntities();
    }
} // namespace Ame::FlappyRocket