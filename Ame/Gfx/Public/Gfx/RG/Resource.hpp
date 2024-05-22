#pragma once

#include <map>
#include <Gfx/RG/Core.hpp>

namespace Ame::Gfx::RG
{
    class ResourceId
    {
        static constexpr size_t InvalidId = std::numeric_limits<uint32_t>::max();

    public:
        ResourceId() = default;

        explicit ResourceId(
            StringView Name) :
            m_Id(Name)
#ifndef AME_DIST
            ,
            m_Name(Name)
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
            return Strings::Empty<String>;
#endif
        }

        auto operator<=>(
            const ResourceId& Other) const noexcept
        {
            return m_Id <=> Other.m_Id;
        }

        operator bool() const noexcept
        {
            return static_cast<size_t>(m_Id) != InvalidId;
        }

        /// <summary>
        /// Create a resource view id from this resource id
        /// </summary>
        [[nodiscard]] ResourceViewId operator()(
            StringView ViewName) const;

    private:
        StringHash m_Id = InvalidId;
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
            StringView Name,
            StringView ViewName) :
            ResourceViewId(ResourceId(Name), ViewName)
        {
        }

        explicit ResourceViewId(
            ResourceId ResId,
            StringView ViewName) :
            m_Resource(ResId),
            m_ViewId(ViewName)
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

        operator bool() const noexcept
        {
            return m_Resource && static_cast<size_t>(m_ViewId) != InvalidId;
        }

        auto operator<=>(
            const ResourceViewId& Other) const noexcept
        {
            auto Cmp = m_Resource <=> Other.m_Resource;
            if (Cmp == std::strong_ordering::equal)
            {
                Cmp = m_ViewId <=> Other.m_ViewId;
            }
            return Cmp;
        }

    private:
        ResourceId m_Resource;
        StringHash m_ViewId = InvalidId;
    };

    //

    inline ResourceViewId ResourceId::operator()(
        StringView ViewName) const
    {
        return ResourceViewId(*this, ViewName);
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

        operator bool() const noexcept;

    public:
        /// <summary>
        /// Get resource desc
        /// </summary>
        [[nodiscard]] const ResourceDesc& GetDesc() const noexcept;

        /// <summary>
        /// Import resource
        /// </summary>
        void Import(
            Rhi::Texture Texture);

        /// <summary>
        /// Import resource
        /// </summary>
        void Import(
            Rhi::Buffer Buffer);

        /// <summary>
        /// Set resource desc and change resource to be dynamic (not imported)
        /// </summary>
        void SetDynamic(
            const ResourceId&   Id,
            const ResourceDesc& Desc);

    public:
        /// <summary>
        /// Get the underlying resource as a texture
        /// </summary>
        [[nodiscard]] Opt<Rhi::Texture> AsTexture() const;

        /// <summary>
        /// Get the underlying resource as a buffer
        /// </summary>
        [[nodiscard]] Opt<Rhi::Buffer> AsBuffer() const;

    public:
        /// <summary>
        /// Create a resource view with name
        /// </summary>
        Rhi::BufferDesc& CreateBufferView(
            const ResourceViewId& ViewId,
            ResourceViewDesc&&    ViewDesc);

        /// <summary>
        /// Create a resource view with name
        /// </summary>
        Rhi::TextureDesc& CreateTextureView(
            const ResourceViewId& ViewId,
            ResourceViewDesc&&    ViewDesc);

    public:
        /// <summary>
        /// Get resource view desc
        /// </summary>
        [[nodiscard]] ResourceViewDesc& GetViewDescMut(
            const ResourceViewId& ViewId);

        /// <summary>
        /// Get resource view desc
        /// </summary>
        [[nodiscard]] const ResourceViewDesc& GetViewDesc(
            const ResourceViewId& ViewId) const;

        /// <summary>
        /// Get resource view descriptor handle
        /// </summary>
        [[nodiscard]] const Rhi::ResourceView& GetViewHandle(
            const ResourceViewId& ViewId) const;

    public:
        /// <summary>
        /// Check if resource has a view
        /// </summary>
        [[nodiscard]] bool ContainsView(
            const ResourceViewId& ViewId) const;

        /// <summary>
        /// check if resource is imported
        /// </summary>
        [[nodiscard]] bool IsImported() const noexcept;

    private:
        /// <summary>
        /// Reallocate resource
        /// </summary>
        void Reallocate(
            Rhi::Device& RhiDevice);

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
#ifndef AME_DIST
        String m_Name;
#endif

        ResourceType m_Resource;

        ResourceDesc m_Desc;
        size_t       m_DescHash = 0;

        ResourceViewMapType m_Views;

        bool m_ImportViewsChanged : 1 = false;
        bool m_IsImported         : 1 = false;
    };
} // namespace Ame::Gfx::RG