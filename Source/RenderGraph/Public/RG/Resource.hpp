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

    using BufferResourceViewDesc = std::variant<
        Dg::BUFFER_VIEW_TYPE,
        Dg::BufferViewDesc>;

    using TextureResourceViewDesc = std::variant<
        Dg::TEXTURE_VIEW_TYPE,
        Dg::TextureViewDesc,
        RenderTargetViewDesc,
        DepthStencilViewDesc>;

    struct BufferResourceView
    {
        Ptr<Dg::IBufferView>   View;
        BufferResourceViewDesc Desc;
        size_t                 Hash = 0;
    };

    struct TextureResourceView
    {
        Ptr<Dg::ITextureView>   View;
        TextureResourceViewDesc Desc;
        size_t                  Hash = 0;
    };

    struct BufferResource
    {
        Ptr<Dg::IBuffer> Resource;
        Dg::BufferDesc   Desc;
    };

    struct TextureResource
    {
        Ptr<Dg::ITexture> Resource;
        Dg::TextureDesc   Desc;
    };

    using RhiBufferViewMap  = std::map<ResourceViewId, BufferResourceView>;
    using RhiTextureViewMap = std::map<ResourceViewId, TextureResourceView>;

    using RhiTextureViewRef = CRef<TextureResourceView>;
    using RhiBufferViewRef  = CRef<BufferResourceView>;

    //

    // Both RhiResource and RhiResourceViewMap, must have same variant index for ResourceHandle::InitializeViewMap to work
    using RhiResource        = std::variant<std::monostate, TextureResource, BufferResource>;
    using RhiResourceViewMap = std::variant<std::monostate, RhiTextureViewMap, RhiBufferViewMap>;

    using RhiResourceView = std::variant<std::monostate, RhiTextureViewRef, RhiBufferViewRef>;

    //

    class ResourceHandle
    {
        friend class DependencyLevel;
        friend class ResourceStorage;

    public:
        ResourceHandle() = default;

        explicit operator bool() const noexcept;

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
        /// Get texture resource's view or nullptr if view is not found or not for buffer
        /// </summary>
        [[nodiscard]] const BufferResourceView* GetBufferView(
            const ResourceViewId& viewId) const noexcept;

        /// <summary>
        /// Get texture resource's view or nullptr if view is not found or not for texture
        /// </summary>
        [[nodiscard]] const TextureResourceView* GetTextureView(
            const ResourceViewId& viewId) const noexcept;

        /// <summary>
        /// Get texture resource's view or std::monostate if view is not found
        /// </summary>
        [[nodiscard]] RhiResourceView GetView(
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
            Dg::IRenderDevice* renderDevice);

        /// <summary>
        /// Initialize views to the view map
        /// </summary>
        void InitializeViewMap();

    private:
        /// <summary>
        /// Recreate resource views
        /// </summary>
        void RecreateViewsIfNeeded(
            BufferResource& bufferResource);

        /// <summary>
        /// Recreate resource views
        /// </summary>
        void RecreateViewsIfNeeded(
            TextureResource& textureResource);

    private:
        RhiResource        m_Resource;
        RhiResourceViewMap m_Views;

        size_t m_DescHash = 0;

#ifndef AME_DIST
        String m_Name;
#endif

        bool m_IsImported : 1 = false;
    };
} // namespace Ame::RG