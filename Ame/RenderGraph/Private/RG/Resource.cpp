#include <RG/Resource.hpp>
#include <Rhi/Hash/Resource.hpp>

#include <Rhi/Device/ResourceAllocator.hpp>
#include <RG/StateTracker.hpp>
#include <RG/ResourceCacheStorage.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
{
    ResourceHandle::~ResourceHandle()
    {
        CheckResourceState(true, true);
    }

    ResourceHandle::operator bool() const noexcept
    {
        return !std::holds_alternative<std::monostate>(m_Resource);
    }

    //

    void ResourceHandle::Release(
        ResourceStateTracker& stateTracker)
    {
        std::visit(
            VariantVisitor{
                [&](std::monostate) {},
                [&](auto& data)
                {
                    if (data.Resource)
                    {
                        stateTracker.EndTracking(data.Resource);
                        if (!m_IsImported)
                        {
                            data.Resource.Release();
                        }
                    }
                } },
            m_Resource);
        m_Resource = std::monostate{};
    }

    void ResourceHandle::Release()
    {
        if (!m_IsImported)
        {
            std::visit(
                VariantVisitor{
                    [&](std::monostate) {},
                    [&](auto& data)
                    {
                        if (data.Resource)
                        {
                            data.Resource.Release();
                        }
                    } },
                m_Resource);
        }
        m_Resource = std::monostate{};
    }

    //

    void ResourceHandle::BeginTracking(
        ResourceStateTracker& stateTracker)
    {
        CheckResourceState(false, true);
        std::visit(
            VariantVisitor{
                [](std::monostate) {},
                [&](auto& data)
                {
                    stateTracker.BeginTracking(data.Resource);
                } },
            m_Resource);
    }

    void ResourceHandle::BeginTrackingBuffer(
        ResourceStateTracker& stateTracker,
        Rhi::AccessStage      initialState)
    {
        auto buffer = AsBuffer();
        AME_LOG_ASSERT(Log::Gfx(), buffer != nullptr, "Resource is not a buffer");

        stateTracker.BeginTracking(buffer->Resource, initialState);
    }

    void ResourceHandle::BeginTrackingTexture(
        ResourceStateTracker&  stateTracker,
        Rhi::AccessLayoutStage initialState)
    {
        auto texture = AsTexture();
        AME_LOG_ASSERT(Log::Gfx(), texture != nullptr, "Resource is not a texture");

        stateTracker.BeginTracking(texture->Resource, initialState);
    }

    //

    void ResourceHandle::Import(
        RhiResource&& resource)
    {
        CheckResourceState(true, true);
        m_Resource = std::move(resource);

        std::visit(
            VariantVisitor{
                [&](std::monostate) {},
                [&](auto& data)
                {
                    if (data.Resource)
                    {
                        data.Desc = data.Resource.GetDesc();
                    }
                } },
            m_Resource);

        m_ImportViewsChanged = true;
        m_IsImported         = true;

#ifndef AME_DIST
        m_Name.clear();
#endif
    }

    void ResourceHandle::SetDynamic(
        const ResourceId& id,
        RhiResource&&     resource)
    {
        CheckResourceState(true, true);
        m_Resource = std::move(resource);

        m_ImportViewsChanged = false;
        m_IsImported         = false;

#ifndef AME_DIST
        m_Name = id.GetName();
#endif
    }

    //

    const RhiResource& ResourceHandle::Get() const noexcept
    {
        return m_Resource;
    }

    const TextureResource* ResourceHandle::AsTexture() const noexcept
    {
        return std::get_if<TextureResource>(&m_Resource);
    }

    TextureResource* ResourceHandle::AsTexture() noexcept
    {
        return std::get_if<TextureResource>(&m_Resource);
    }

    const BufferResource* ResourceHandle::AsBuffer() const noexcept
    {
        return std::get_if<BufferResource>(&m_Resource);
    }

    BufferResource* ResourceHandle::AsBuffer() noexcept
    {
        return std::get_if<BufferResource>(&m_Resource);
    }

    //

    BufferResource& ResourceHandle::CreateBufferView(
        const ResourceViewId&         viewId,
        const BufferResourceViewDesc& desc)
    {
        auto buffer = AsBuffer();
        AME_LOG_ASSERT(Log::Gfx(), buffer != nullptr, "Resource is not a buffer");

        buffer->Views.emplace(viewId, desc);
        return *buffer;
    }

    TextureResource& ResourceHandle::CreateTextureView(
        const ResourceViewId&          viewId,
        const TextureResourceViewDesc& desc)
    {
        auto texture = AsTexture();
        AME_LOG_ASSERT(Log::Gfx(), texture != nullptr, "Resource is not a texture");

        texture->Views.emplace(viewId, desc);
        return *texture;
    }

    //

    const BufferResourceView* ResourceHandle::GetBufferView(
        const ResourceViewId& viewId) const noexcept
    {
        const BufferResourceView* view = nullptr;
        if (auto buffer = AsBuffer())
        {
            auto viewIter = buffer->Views.find(viewId);
            view          = viewIter != buffer->Views.end() ? &viewIter->second : nullptr;
        }
        return view;
    }

    const TextureResourceView* ResourceHandle::GetTextureView(
        const ResourceViewId& viewId) const noexcept
    {
        const TextureResourceView* view = nullptr;
        if (auto texture = AsTexture())
        {
            auto viewIter = texture->Views.find(viewId);
            view          = viewIter != texture->Views.end() ? &viewIter->second : nullptr;
        }
        return view;
    }

    //

    bool ResourceHandle::IsImported() const noexcept
    {
        return m_IsImported;
    }

    //

    void ResourceHandle::Reallocate(
        ResourceStateTracker&         stateTracker,
        ResourceCacheStorage&         cacheStorage,
        Rhi::DeviceResourceAllocator& allocator)
    {
        std::visit(
            VariantVisitor{
                [](std::monostate) {},
                [&](auto& data)
                {
                    if (!IsImported())
                    {
                        using descType = std::decay_t<decltype(data.Desc)>;
                        size_t hash    = std::hash<descType>{}(data.Desc);
                        if (hash != m_DescHash)
                        {
                            m_DescHash = hash;

                            Release(stateTracker);

                            if constexpr (std::is_same_v<descType, Rhi::TextureDesc>)
                            {
                                data.Resource = allocator.CreateTexture(data.Desc);
                            }
                            else
                            {
                                data.Resource = allocator.CreateBuffer(data.Desc, data.Location);
                            }

#ifndef AME_DIST
                            data.Resource.SetName(m_Name.c_str());
#endif
                            BeginTracking(stateTracker);
                        }
                    }

                    RecreateViews(cacheStorage, data);
                } },
            m_Resource);
    }

    //

    void ResourceHandle::RecreateViews(
        ResourceCacheStorage& cacheStorage,
        BufferResource&       bufferResource)
    {
        for (auto& [id, view] : bufferResource.Views)
        {
            view.View = cacheStorage.CreateView(bufferResource.Resource, view.Desc);
        }
    }

    void ResourceHandle::RecreateViews(
        ResourceCacheStorage& cacheStorage,
        TextureResource&      textureResource)
    {
        for (auto& [id, view] : textureResource.Views)
        {
            std::visit(
                VariantVisitor{
                    [&](const auto& desc)
                    {
                        view.View = cacheStorage.CreateView(textureResource.Resource, desc);
                    } },
                view.Desc);
        }
    }

    //

    void ResourceHandle::CheckResourceState(
        bool wasReleased,
        bool doAssert) const
    {
#ifndef AME_DIST
        if (std::holds_alternative<std::monostate>(m_Resource) != wasReleased)
        {
            if (doAssert)
            {
                Log::Gfx().Error("Resource was released");
                DebugBreak();
            }
            else
            {
                Log::Gfx().Warning("Resource was released");
            }
        }
#endif
    }
} // namespace Ame::RG