#pragma once

#include <map>
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
            return std::tie(m_Resource, m_ViewId) <=> std::tie(other.m_Resource, other.m_ViewId);
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

    class ResourceHandle
    {
        friend class DependencyLevel;
        friend class ResourceStorage;

        struct ViewDesc
        {
            ResourceViewDesc  Desc;
            Rhi::ResourceView View;
        };

    public:
        using ResourceViewMapType = std::map<size_t, ViewDesc>;
        using ResourceType        = std::variant<
            std::monostate,
            Rhi::Texture,
            Rhi::Buffer>;

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
        /// Get resource desc
        /// </summary>
        [[nodiscard]] const ResourceDesc& GetDesc() const noexcept;

        /// <summary>
        /// Import resource
        /// </summary>
        void Import(
            Rhi::Texture texture);

        /// <summary>
        /// Import resource
        /// </summary>
        void Import(
            Rhi::Buffer buffer);

        /// <summary>
        /// Set resource desc and change resource to be dynamic (not imported)
        /// </summary>
        void SetDynamic(
            const ResourceId&   id,
            const ResourceDesc& desc);

    public:
        /// <summary>
        /// Get the underlying resource as a texture
        /// </summary>
        [[nodiscard]] const Rhi::Texture* AsTexture() const;

        /// <summary>
        /// Get the underlying resource as a buffer
        /// </summary>
        [[nodiscard]] const Rhi::Buffer* AsBuffer() const;

    public:
        /// <summary>
        /// Create a resource view with name
        /// </summary>
        Rhi::BufferDesc& CreateBufferView(
            const ResourceViewId& viewId,
            ResourceViewDesc&&    desc);

        /// <summary>
        /// Create a resource view with name
        /// </summary>
        Rhi::TextureDesc& CreateTextureView(
            const ResourceViewId& viewId,
            ResourceViewDesc&&    desc);

    public:
        /// <summary>
        /// Get resource view desc
        /// </summary>
        [[nodiscard]] ResourceViewDesc& GetViewDescMut(
            const ResourceViewId& viewId);

        /// <summary>
        /// Get resource view desc
        /// </summary>
        [[nodiscard]] const ResourceViewDesc& GetViewDesc(
            const ResourceViewId& viewId) const;

        /// <summary>
        /// Get resource view descriptor handle
        /// </summary>
        [[nodiscard]] const Rhi::ResourceView& GetViewHandle(
            const ResourceViewId& viewId) const;

    public:
        /// <summary>
        /// Check if resource has a view
        /// </summary>
        [[nodiscard]] bool ContainsView(
            const ResourceViewId& viewId) const;

        /// <summary>
        /// check if resource is imported
        /// </summary>
        [[nodiscard]] bool IsImported() const noexcept;

    private:
        /// <summary>
        /// Reallocate resource if the desc has changed and recreate views
        /// </summary>
        void Reallocate(
            Rhi::Device& rhiDevice);

        /// <summary>
        /// Recreate resource views
        /// </summary>
        void RecreateViews();

    private:
        /// <summary>
        /// Release resource
        /// </summary>
        void Release();

    private:
        ResourceType m_Resource;

        ResourceDesc        m_Desc;
        ResourceViewMapType m_Views;
        size_t              m_DescHash = 0;

#ifndef AME_DIST
        String m_Name;
#endif

        bool m_ImportViewsChanged : 1 = false;
        bool m_IsImported         : 1 = false;
    };
} // namespace Ame::RG