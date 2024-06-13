#include <RG/Context.hpp>

namespace Ame::RG
{
    Context::Context(
        Rhi::Device& rhiDevice) :
        m_Device(rhiDevice),
        m_Resources(rhiDevice)
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

    void Context::Update()
    {
        auto& resourceStorage = GetStorage();
        resourceStorage.ImportCoreResources();
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
} // namespace Ame::RG