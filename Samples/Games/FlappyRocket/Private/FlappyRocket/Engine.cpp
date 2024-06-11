#include <FlappyRocket/Engine.hpp>

#include <Rhi/Subsystem/Device.hpp>
#include <Asset/Subsystem/Storage.hpp>
#include <Gfx/Subsystem/Pipelines/DeferredPlusPipeline.hpp>
#include <FlappyRocket/Subsystem/Game.hpp>

#include <Asset/Packs/Directory.hpp>

#include <Log/Wrapper.hpp>

#include <WorldNTree/Subsystem.hpp>

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

        InstallSubsystem<Gfx::RG::GraphRendererSubsystem>();
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

        auto& worldTree = GetSubsystem<Extensions::WorldOctTreeBoxSubsystem>();

        std::vector<Ecs::Entity> entities;

        Geometry::Frustum frustum;
        frustum.Origin      = Math::Vector3(0.0f, 0.0f, 0.0f);
        frustum.Orientation = Math::Quaternion::Identity;
        frustum.Near        = 0.1f;
        frustum.Far         = 100.0f;
        frustum.LeftSlope   = 1.0f;
        frustum.RightSlope  = 1.0f;
        frustum.TopSlope    = 1.0f;
        frustum.BottomSlope = 1.0f;
        worldTree.FrustumCull(entities, frustum);
    }
} // namespace Ame::FlappyRocket