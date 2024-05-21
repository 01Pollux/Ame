#include <FlappyRocket/Engine.hpp>

#include <Gfx/Renderer.hpp>
#include <Gfx/Shading/Material.Compiler.hpp>
#include <Gfx/Cache/ShaderCache.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>
#include <Ecs/Component/Renderable/2D/Sprite.hpp>

#include <FlappyRocket/Game.Shader.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketGame::FlappyRocketGame(
        Rhi::Device&                      Device,
        Ecs::Universe&                    EcsUniverse,
        Gfx::Renderer&                    Renderer,
        Gfx::Cache::PipelineStateCache&   PipelineStateCache,
        Gfx::Cache::ShaderCache&          ShaderCache,
        Gfx::Cache::MaterialBindingCache& MaterialCache) :
        m_Device(&Device),
        m_EcsUniverse(&EcsUniverse),
        m_ShaderCache(&ShaderCache)
    {
        SetupRenderGraph(Renderer.GetRenderGraph(), PipelineStateCache, MaterialCache);
    }

    //

    void FlappyRocketGame::ResetWorld()
    {
        m_EcsUniverse->RemoveWorld(WorldName);
        auto& World = m_EcsUniverse->CreateWorld(WorldName);
        m_EcsUniverse->SetActiveWorld(&World);
    }

    //

    [[nodiscard]] static Ptr<Gfx::Shading::Material> CreateMaterial(
        Rhi::Device&             Device,
        Gfx::Cache::ShaderCache& ShaderCache)
    {
        using namespace EnumBitOperators;
        namespace GS = Gfx::Shading;

        GS::MaterialPipelineState PipelineState;
        GS::PropertyDescriptor    Descriptor;

        Rhi::ShaderCompileDesc CompileDesc;

        CompileDesc.SetStage(Rhi::ShaderType::VERTEX_SHADER);
        PipelineState.Shaders.emplace_back(ShaderCache.Load(s_ShaderSource, CompileDesc).get());

        CompileDesc.Flags |= Rhi::ShaderCompileFlags::LibraryShader;
        CompileDesc.SetStage(Rhi::ShaderType::FRAGMENT_SHADER);
        PipelineState.Shaders.emplace_back(ShaderCache.Load(s_ShaderSource, CompileDesc).get());

        return GS::MaterialCompiler::Compile(
                   Device,
                   std::move(PipelineState),
                   Descriptor)
            .get();
    }

    void FlappyRocketGame::AddAllEntities()
    {
        auto& World = *m_EcsUniverse->GetActiveWorld();

        auto Player       = World.CreateEntity(PlayerName);
        auto PlayerSprite = Ecs::Component::Sprite::Quad();

        PlayerSprite.Material = CreateMaterial(*m_Device, *m_ShaderCache);

        Player.AddComponent(std::move(PlayerSprite));
        Player.AddComponent<Ecs::Component::Transform>();

        auto Camera = World.CreateEntity(CameraName);
        Camera.AddComponent<Ecs::Component::Camera>();
        Camera.AddComponent<Ecs::Component::Transform>(Math::Mat::Identity<Math::Matrix3x3>, Math::Vec::Backward<Math::Vector3> * 2.f);
    }
} // namespace Ame::FlappyRocket
