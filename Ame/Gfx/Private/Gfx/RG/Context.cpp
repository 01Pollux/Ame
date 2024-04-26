#include <Gfx/RG/Context.hpp>

namespace Ame::Gfx::RG
{
    Context::Context(
        Rhi::Device& Device) :
        m_Device(Device),
        m_Storage(Device)
    {
    }

    //

    const Storage& Context::GetStorage() const
    {
        return m_Storage;
    }

    Storage& Context::GetStorage()
    {
        return m_Storage;
    }

    //

    void Context::UpdateFrameStorage(
        float                        EngineTime,
        float                        GameTime,
        float                        DeltaTime,
        const Math::TransformMatrix& Transform,
        const Math::Matrix4x4&       Projection,
        const Math::Vector2&         Viewport)
    {
        auto& RgStorage = GetStorage();
        RgStorage.UpdateFrameResource(EngineTime, GameTime, DeltaTime, Transform, Projection, Viewport);
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

        for (size_t i = 0; i < m_Levels.size(); i++)
        {
            auto ThisLevel = &m_Levels[i];
            auto NextLevel = (i + 1 < m_Levels.size()) ? &m_Levels[i + 1] : nullptr;
            auto PrevLevel = (i > 0) ? &m_Levels[i - 1] : nullptr;

            ResolveTransitions(ThisLevel, NextLevel, PrevLevel);
        }
    }

    //

    void Context::ResolveTransitions(
        DependencyLevel* ThisLevel,
        DependencyLevel*,
        DependencyLevel* PrevLevel)
    {
        if (PrevLevel)
        {
            auto& BufferTransitions  = ThisLevel->m_BufferStatesToTransitions;
            auto& TextureTransitions = ThisLevel->m_TextureStatesToTransitions;

            BufferTransitions.merge(PrevLevel->m_BufferStatesToTransitions);
            TextureTransitions.merge(PrevLevel->m_TextureStatesToTransitions);
        }
    }
} // namespace Ame::Gfx::RG