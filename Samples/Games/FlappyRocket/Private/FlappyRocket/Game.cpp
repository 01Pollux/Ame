#include <FlappyRocket/Engine.hpp>

#include <Gfx/Renderer.hpp>
#include <Gfx/Shading/Material.Compiler.hpp>
#include <Rhi/Resource/Shader.Compiler.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>
#include <Ecs/Component/Renderable/2D/Sprite.hpp>

#include <FlappyRocket/Game.Shader.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketGame::FlappyRocketGame(
        Rhi::Device&                    Device,
        Ecs::Universe&                  EcsUniverse,
        Gfx::Renderer&                  Renderer,
        Gfx::Cache::PipelineStateCache& PipelineStateCache) :
        m_Device(&Device),
        m_EcsUniverse(&EcsUniverse)
    {
        SetupRenderGraph(Renderer.GetRenderGraph(), PipelineStateCache);
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
        Rhi::Device& Device)
    {
        namespace GS = Gfx::Shading;

        GS::MaterialPipelineState PipelineState;
        GS::PropertyDescriptor    Descriptor;

        Rhi::ShaderCompileDesc CompileDesc;

        CompileDesc.Stage = Rhi::ShaderType::VERTEX_SHADER;
        PipelineState.Shaders.emplace_back(
            Rhi::ShaderCompiler::Compile(
                Device,
                s_ShaderSource,
                CompileDesc));

        CompileDesc.Stage = Rhi::ShaderType::FRAGMENT_SHADER;
        PipelineState.Shaders.emplace_back(
            Rhi::ShaderCompiler::Compile(
                Device,
                s_ShaderSource,
                CompileDesc));

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

        PlayerSprite.Material = CreateMaterial(*m_Device);

        Player.AddComponent(std::move(PlayerSprite));
        Player.AddComponent<Ecs::Component::Transform>();

        auto Camera = World.CreateEntity(CameraName);
        Camera.AddComponent<Ecs::Component::Camera>();
        Camera.AddComponent<Ecs::Component::Transform>(Math::Mat::Identity<Math::Matrix3x3>, Math::Vec::Backward<Math::Vector3> * 2.f);
    }
} // namespace Ame::FlappyRocket
