#include <FlappyRocket/Engine.hpp>

#include <Rhi/Subsystem/Device.hpp>
#include <Asset/Subsystem/Storage.hpp>
#include <Gfx/Subsystem/Pipelines/DeferredPlusPipeline.hpp>
#include <FlappyRocket/Subsystem/Game.hpp>

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

    void FlappyRocketEngine::Initialize()
    {
        BaseEngine::Initialize();

        SetClearColor(GetSubsystem<Rhi::DeviceSubsystem>());

        auto& assetStorage = GetSubsystem<Asset::StorageSubsystem>();
        assetStorage.Mount<Asset::DirectoryAssetPackage>("Shared/Assets");

        GetSubsystem<Gfx::RG::GraphRendererSubsystem>();

        m_Game = GetSubsystem<FlappyRocketGameSubsystem>();
        CreateWorld();
    }

    void FlappyRocketEngine::SetClearColor(
        Rhi::Device& rhiDevice) const
    {
        rhiDevice.SetClearColor(Colors::c_Gray);
    }

    void FlappyRocketEngine::CreateWorld()
    {
        m_Game->ResetWorld();
        m_Game->AddAllEntities();
    }
} // namespace Ame::FlappyRocket