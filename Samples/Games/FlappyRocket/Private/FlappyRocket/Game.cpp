#include <FlappyRocket/Engine.hpp>

#include <Gfx/Renderer.hpp>
#include <Gfx/Shading/Material.Compiler.hpp>
#include <Gfx/Shading/Material.hpp>
#include <Gfx/Cache/ShaderCache.hpp>

#include <Ecs/Component/Renderable/2D/Sprite.hpp>
#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>
#include <Ecs/Component/Viewport/CameraOutput.hpp>

#include <Asset/Types/Gfx/TextureAsset.hpp>
#include <Asset/Storage.hpp>

#include <Gfx/RG/Passes/GBufferPass.hpp>
#include <FlappyRocket/Game.Shader.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketGame::FlappyRocketGame(
        Rhi::Device&             device,
        Ecs::Universe&           ecsUniverse,
        Asset::Storage&          assetStorage,
        Gfx::Cache::ShaderCache& shaderCache) :
        m_Device(&device),
        m_EcsUniverse(&ecsUniverse),
        m_AssetStorage(&assetStorage),
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
            .Sampler("_Sampler")
            .Resource("_Texture", Gfx::Shading::ResourceType::Texture2D, Gfx::Shading::ResourceDataType::Float4);

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

    static void SetMaterialProperties(
        Asset::Storage&             assetStorage,
        const String&               textureGuid,
        Ptr<Gfx::Shading::Material> material)
    {
        auto& assetManager = assetStorage.GetManager();
        auto  asset        = assetManager.Load(Guid::FromString(textureGuid));
        auto  textureAsset = std::dynamic_pointer_cast<Asset::Gfx::TextureAsset>(asset);

        if (!textureAsset)
        {
            throw std::runtime_error("Texture asset not found");
            return;
        }

        material->Set("_Color", Colors::c_White);
        material->Set("_Texture", textureAsset->GetTexture(), { Rhi::TextureViewType::ShaderResource2D });
        material->Set("_Sampler",
                      Rhi::SamplerDesc{
                          .filters{ nri::Filter::LINEAR, nri::Filter::LINEAR, nri::Filter::LINEAR },
                          .anisotropy = 8,
                          .mipMax     = 16.0f,
                          .addressModes{ nri::AddressMode::REPEAT, nri::AddressMode::REPEAT } });
    }

    void FlappyRocketGame::AddAllEntities()
    {
        auto& world = *m_EcsUniverse->GetActiveWorld();

        auto material = CreateMaterial(*m_Device, *m_ShaderCache);
        SetMaterialProperties(*m_AssetStorage, c_TextureGuid, material);
        for (float y = -50.f; y < 100.f; y += 1.f)
        {
            for (float x = -50.f; x < 100.f; x += 1.f)
            {
                auto name = "Player" + std::to_string(static_cast<int>(x)) + "_" + std::to_string(static_cast<int>(y));

                auto player       = world.CreateEntity(name);
                auto playerSprite = Ecs::Component::Sprite::Quad();

                playerSprite.Material = material;

                player.AddComponent(std::move(playerSprite));

                Ecs::Component::Transform transform;
                transform.SetPosition({ x, y, 0.f });

                player.AddComponent(std::move(transform));
            }
        }

        auto camera = world.CreateEntity(c_CameraName);
        camera.AddComponent<Ecs::Component::Camera>();
        camera.AddComponent<Ecs::Component::Transform>(
            Math::Mat::c_Identity<Math::Matrix3x3>,
            Math::Vec::c_Backward<Math::Vector3> * 5.f);

        camera.AddComponent<Ecs::Component::CameraOutput>(
            Gfx::RG::Std::GBufferPass::c_BaseColor_Roughness);
    }
} // namespace Ame::FlappyRocket
