#include <Gfx/RG/Context.hpp>

namespace Ame::Gfx::RG
{
    Context::Context(
        Rhi::Device&   Device,
        Ecs::Universe& Universe) :
        m_Device(Device),
        m_Resources(Device, Universe)
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
        auto& RgStorage = GetStorage();
        RgStorage.ResetCameraStorage();
    }

    void Context::Update()
    {
        auto& RgStorage = GetStorage();
        RgStorage.UpdateCoreResources();
    }

    void Context::UpdateFrameStorage(
        float                        EngineTime,
        float                        GameTime,
        float                        DeltaTime,
        const Ecs::Entity&           CameraEntity,
        const Math::TransformMatrix& Transform,
        const Math::Matrix4x4&       Projection,
        const Math::Vector2&         Viewport)
    {
        auto& RgStorage = GetStorage();
        RgStorage.UpdateFrameResource(EngineTime, GameTime, DeltaTime, CameraEntity, Transform, Projection, Viewport);
    }

    void Context::Execute()
    {
        GetStorage().UpdateResources();

        Rhi::CommandList CommandList(m_Device.get());
        for (auto& Level : m_Levels)
        {
            Level.Execute(*this, CommandList);
        }
    }

    void Context::Build(
        DependencyLevelListType&& Levels)
    {
        m_Levels = std::move(Levels);
    }
} // namespace Ame::Gfx::RG