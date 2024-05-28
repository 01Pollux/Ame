#include <FlappyRocket/Engine.hpp>

#include <Gfx/Renderer.hpp>
#include <Gfx/Shading/Material.Compiler.hpp>
#include <Gfx/Cache/ShaderCache.hpp>

#include <Ecs/Component/Renderable/2D/Sprite.hpp>
#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>
#include <Ecs/Component/Viewport/CameraOutput.hpp>

#include <Gfx/RG/Passes/GBufferPass.hpp>
#include <FlappyRocket/Game.Shader.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketGame::FlappyRocketGame(
        Rhi::Device&             device,
        Ecs::Universe&           ecsUniverse,
        Gfx::Cache::ShaderCache& shaderCache) :
        m_Device(&device),
        m_EcsUniverse(&ecsUniverse),
        m_ShaderCache(&shaderCache)
    {
    }

    //

    void FlappyRocketGame::ResetWorld()
    {
        m_EcsUniverse->RemoveWorld(c_WorldName);
        auto& world = m_EcsUniverse->CreateWorld(c_WorldName);
        m_EcsUniverse->SetActiveWorld(&world);
    }

    //

    [[nodiscard]] static Ptr<Gfx::Shading::Material> CreateMaterial(
        Rhi::Device&             device,
        Gfx::Cache::ShaderCache& shaderCache)
    {
        using namespace EnumBitOperators;
        namespace GS = Gfx::Shading;

        GS::MaterialPipelineState pipelineState;
        pipelineState.Rasterizer.Cull = Rhi::CullMode::NONE;

        GS::PropertyDescriptor descriptor;
        descriptor
            .Float4("_Color")
            .Resource("_Texture", Gfx::Shading::ResourceType::Texture2D, Gfx::Shading::ResourceDataType::Float4)
            .Sampler("_Sampler");

        Rhi::ShaderCompileDesc compileDesc;

        compileDesc.SetStage(Rhi::ShaderType::VERTEX_SHADER);
        pipelineState.Shaders.emplace_back(shaderCache.Load(c_ShaderSource, compileDesc).get());

        compileDesc.Flags |= Rhi::ShaderCompileFlags::LibraryShader;
        compileDesc.SetStage(Rhi::ShaderType::FRAGMENT_SHADER);
        pipelineState.Shaders.emplace_back(shaderCache.Load(c_ShaderSource, compileDesc).get());

        return GS::MaterialCompiler::Compile(
                   device,
                   shaderCache,
                   std::move(pipelineState),
                   descriptor)
            .get();
    }

    void FlappyRocketGame::AddAllEntities()
    {
        auto& world = *m_EcsUniverse->GetActiveWorld();

        auto player       = world.CreateEntity(c_PlayerName);
        auto playerSprite = Ecs::Component::Sprite::Quad();

        playerSprite.Material = CreateMaterial(*m_Device, *m_ShaderCache);

        player.AddComponent(std::move(playerSprite));
        player.AddComponent<Ecs::Component::Transform>();

        auto camera = world.CreateEntity(c_CameraName);
        camera.AddComponent<Ecs::Component::Camera>();
        camera.AddComponent<Ecs::Component::Transform>(
            Math::Mat::c_Identity<Math::Matrix3x3>,
            Math::Vec::c_Backward<Math::Vector3> * 10.f);

        camera.AddComponent<Ecs::Component::CameraOutput>(
            Gfx::RG::Std::GBufferPass::c_BaseColor_Roughness);
    }
} // namespace Ame::FlappyRocket
