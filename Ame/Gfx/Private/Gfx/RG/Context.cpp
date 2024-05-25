#include <Gfx/RG/Context.hpp>

namespace Ame::Gfx::RG
{
    Context::Context(
        Rhi::Device&   rhiDevice,
        Ecs::Universe& universe) :
        m_Device(rhiDevice),
        m_Resources(rhiDevice, universe)
    {
    }

    //

    const ResourceStorage& Context::GetStorage() const
    {
        return m_Resources;
    }

    ResourceStorage& Context::GetStorage()
    {
        return m_Resources;
    }

    //

    void Context::Reset()
    {
        auto& resourceStorage = GetStorage();
        resourceStorage.ResetCameraStorage();
    }

    void Context::Update()
    {
        auto& resourceStorage = GetStorage();
        resourceStorage.UpdateCoreResources();
    }

    void Context::UpdateFrameStorage(
        float                        engineTime,
        float                        gameTime,
        float                        deltaTime,
        const Ecs::Entity&           cameraEntity,
        const Math::TransformMatrix& transform,
        const Math::Matrix4x4&       projection,
        const Math::Vector2&         viewport)
    {
        auto& resourceStorage = GetStorage();
        resourceStorage.UpdateFrameResource(
            engineTime,
            gameTime,
            deltaTime,
            cameraEntity,
            transform,
            projection,
            viewport);
    }

    void Context::Execute()
    {
        GetStorage().UpdateResources();

        Rhi::CommandList commandList(m_Device.get());
        for (auto& level : m_Levels)
        {
            level.Execute(*this, commandList);
        }
    }

    void Context::Build(
        DependencyLevelListType&& levels)
    {
        m_Levels = std::move(levels);
    }
} // namespace Ame::Gfx::RG