#pragma once

#include <RG/Core.hpp>

namespace Ame::RG
{
    class ResourceId
    {
        static constexpr size_t c_InvalidId = std::numeric_limits<uint32_t>::max();

    public:
        ResourceId() = default;

        explicit ResourceId(
            StringView name) :
            m_Id(name)
#ifndef AME_DIST
            ,
            m_Name(name)
#endif
        {
        }

        /// <summary>
        /// Get resource id
        /// </summary>
        [[nodiscard]] size_t GetId() const noexcept
        {
            return m_Id;
        }

        /// <summary>
        /// Get resource name (non-dist only)
        /// </summary>
        [[nodiscard]] StringView GetName() const noexcept
        {
#ifndef AME_DIST
            return m_Name;
#else
            return Strings::c_Empty<String>;
#endif
        }

        auto operator<=>(
            const ResourceId& other) const noexcept
        {
            return m_Id <=> other.m_Id;
        }

        explicit operator bool() const noexcept
        {
            return static_cast<size_t>(m_Id) != c_InvalidId;
        }

        /// <summary>
        /// Create a resource view id from this resource id
        /// </summary>
        [[nodiscard]] ResourceViewId operator()(
            StringView viewName) const;

    private:
        StringHash m_Id = c_InvalidId;
#ifndef AME_DIST
        String m_Name;
#endif
    };

    //

    class ResourceViewId
    {
        static constexpr size_t InvalidId = std::numeric_limits<uint32_t>::max();

    public:
        ResourceViewId() = default;

        explicit ResourceViewId(
            StringView name,
            StringView viewName) :
            ResourceViewId(ResourceId(name), viewName)
        {
        }

        explicit ResourceViewId(
            ResourceId id,
            StringView viewName) :
            m_Resource(id),
            m_ViewId(viewName)
        {
        }

        /// <summary>
        /// Get resource id
        /// </summary>
        [[nodiscard]] const ResourceId& GetResource() const noexcept
        {
            return m_Resource;
        }

        /// <summary>
        /// Get resource view id
        /// </summary>
        [[nodiscard]] size_t Get() const noexcept
        {
            return m_ViewId;
        }

        explicit operator bool() const noexcept
        {
            return m_Resource && static_cast<size_t>(m_ViewId) != InvalidId;
        }

        auto operator<=>(
            const ResourceViewId& other) const noexcept
        {
            return m_ViewId <=> other.m_ViewId;
        }

    private:
        ResourceId m_Resource;
        StringHash m_ViewId = InvalidId;
    };

    //

    inline ResourceViewId ResourceId::operator()(
        StringView viewName) const
    {
        return ResourceViewId(*this, viewName);
    }

    //

    using BufferResourceViewDesc = Rhi::BufferViewDesc;

    using TextureResourceViewDesc = std::variant<
        Rhi::TextureViewDesc,
        RenderTargetViewDesc,
        DepthStencilViewDesc>;

    struct BufferResourceView
    {
        BufferResourceViewDesc Desc;
        Rhi::ResourceView      View;
    };

    struct TextureResourceView
    {
        TextureResourceViewDesc Desc;
        Rhi::ResourceView       View;
    };

    using BufferResourceViewMap  = std::map<ResourceViewId, BufferResourceView>;
    using TextureResourceViewMap = std::map<ResourceViewId, TextureResourceView>;

    struct TextureResource
    {
        Rhi::Texture           Resource;
        Rhi::TextureDesc       Desc;
        TextureResourceViewMap Views;
    };

    struct BufferResource
    {
        Rhi::Buffer           Resource;
        Rhi::BufferDesc       Desc;
        BufferResourceViewMap Views;
        Rhi::MemoryLocation   Location;
    };

    using RhiResource = std::variant<std::monostate, TextureResource, BufferResource>;

    //

    class ResourceHandle
    {
        friend class DependencyLevel;
        friend class ResourceStorage;

    public:
        ResourceHandle() = default;

        ResourceHandle(const ResourceHandle&) = delete;
        ResourceHandle(ResourceHandle&&) noexcept;

        ResourceHandle& operator=(const ResourceHandle&) = delete;
        ResourceHandle& operator=(ResourceHandle&&) noexcept;

        ~ResourceHandle();

        explicit operator bool() const noexcept;

    public:
        /// <summary>
        /// Release resource and remove from state tracker
        /// </summary>
        void Release(
            ResourceStateTracker& stateTracker);

        /// <summary>
        /// Release resource
        /// </summary>
        void Release();

    public:
        /// <summary>
        /// Begin tracking resource
        /// </summary>
        void BeginTracking(
            ResourceStateTracker& stateTracker);

        /// <summary>
        /// Begin tracking buffer resource
        /// </summary>
        void BeginTrackingBuffer(
            ResourceStateTracker& stateTracker,
            Rhi::AccessStage      initialState = { Rhi::AccessBits::UNKNOWN, Rhi::StageBits::ALL });

        /// <summary>
        /// Begin tracking texture resource
        /// </summary>
        void BeginTrackingTexture(
            ResourceStateTracker&  stateTracker,
            Rhi::AccessLayoutStage initialState = { Rhi::AccessBits::UNKNOWN, Rhi::LayoutType::UNKNOWN, Rhi::StageBits::ALL });

    public:
        /// <summary>
        /// Import resource
        /// </summary>
        void Import(
            RhiResource&& resource);

        /// <summary>
        /// Set resource desc and change resource to be dynamic (not imported)
        /// </summary>
        void SetDynamic(
            const ResourceId& id,
            RhiResource&&     resource);

    public:
        /// <summary>
        /// Get resource data
        /// </summary>
        [[nodiscard]] const RhiResource& Get() const noexcept;

        /// <summary>
        /// Get the underlying resource as a texture
        /// </summary>
        [[nodiscard]] const TextureResource* AsTexture() const noexcept;

        /// <summary>
        /// Get the underlying resource as a texture
        /// </summary>
        [[nodiscard]] TextureResource* AsTexture() noexcept;

        /// <summary>
        /// Get the underlying resource as a buffer
        /// </summary>
        [[nodiscard]] const BufferResource* AsBuffer() const noexcept;

        /// <summary>
        /// Get the underlying resource as a buffer
        /// </summary>
        [[nodiscard]] BufferResource* AsBuffer() noexcept;

    public:
        /// <summary>
        /// Create a resource view with name
        /// </summary>
        BufferResource& CreateBufferView(
            const ResourceViewId&         viewId,
            const BufferResourceViewDesc& desc);

        /// <summary>
        /// Create a resource view with name
        /// </summary>
        TextureResource& CreateTextureView(
            const ResourceViewId&          viewId,
            const TextureResourceViewDesc& desc);

    public:
        /// <summary>
        /// Get buffer resource's view
        /// </summary>
        [[nodiscard]] const BufferResourceView* GetBufferView(
            const ResourceViewId& viewId) const noexcept;

        /// <summary>
        /// Get texture resource's view
        /// </summary>
        [[nodiscard]] const TextureResourceView* GetTextureView(
            const ResourceViewId& viewId) const noexcept;

    public:
        /// <summary>
        /// check if resource is imported
        /// </summary>
        [[nodiscard]] bool IsImported() const noexcept;

    private:
        /// <summary>
        /// Reallocate resource if the desc has changed and recreate views
        /// </summary>
        void Reallocate(
            ResourceStateTracker&         stateTracker,
            ResourceCacheStorage&         cacheStorage,
            Rhi::DeviceResourceAllocator& allocator);

    private:
        /// <summary>
        /// Recreate resource views
        /// </summary>
        void RecreateViews(
            ResourceCacheStorage&         cacheStorage,
            BufferResource&               bufferResource);

        /// <summary>
        /// Recreate resource views
        /// </summary>
        void RecreateViews(
            ResourceCacheStorage&         cacheStorage,
            TextureResource&              textureResource);

    private:
        /// <summary>
        /// Check if resource was released
        /// </summary>
        void CheckResourceState(
            bool wasReleased,
            bool doAssert) const;

    private:
        RhiResource m_Resource;

        size_t m_DescHash = 0;

#ifndef AME_DIST
        String m_Name;
#endif

        bool m_ImportViewsChanged : 1 = false;
        bool m_IsImported         : 1 = false;
    };
} // namespace Ame::RG