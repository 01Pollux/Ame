#pragma once

#include <Rg/Core.hpp>

namespace Ame::Rg
{
    class ResourceId
    {
        static constexpr size_t c_InvalidId = std::numeric_limits<size_t>::max();

    public:
        ResourceId() = default;

        explicit ResourceId(size_t id) : m_Id(id)
        {
        }

        explicit ResourceId(StringView name) :
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
        [[nodiscard]] const char* GetName() const noexcept
        {
#ifndef AME_DIST
            return m_Name.c_str();
#else
            return "";
#endif
        }

        auto operator<=>(const ResourceId& other) const noexcept
        {
            return GetId() <=> other.GetId();
        }

        explicit operator bool() const noexcept
        {
            return GetId() != c_InvalidId;
        }

    private:
        StringHash m_Id = c_InvalidId;
#ifndef AME_DIST
        String m_Name;
#endif
    };

    //

    struct ResourceViewId
    {
        static constexpr size_t c_InvalidId = std::numeric_limits<size_t>::max();

    public:
        ResourceViewId() = default;

        ResourceViewId(const ResourceId& resource, size_t id) : m_ResourceId(resource.GetId()), m_ViewId(id)
        {
        }

        /// <summary>
        /// Get resource id
        /// </summary>
        [[nodiscard]] ResourceId GetResourceId() const noexcept
        {
            return ResourceId(m_ResourceId);
        }

        /// <summary>
        /// Get resource view id
        /// </summary>
        [[nodiscard]] size_t GetId() const noexcept
        {
            return m_ViewId;
        }

        explicit operator bool() const noexcept
        {
            return m_ResourceId != c_InvalidId && GetId() != c_InvalidId;
        }

    private:
        size_t m_ResourceId = c_InvalidId;
        size_t m_ViewId     = c_InvalidId;
    };

    //

    struct BufferResource
    {
        Ptr<Dg::IBuffer>         Resource;
        Ptr<Rhi::BufferInitData> InitData;
        Dg::BufferDesc           Desc;
    };

    struct TextureResource
    {
        Ptr<Dg::ITexture>         Resource;
        Ptr<Rhi::TextureInitData> InitData;
        Dg::TextureDesc           Desc;
    };

    struct BufferResourceView
    {
        Ptr<Dg::IBufferView>   View;
        BufferResourceViewDesc Desc;
    };

    struct TextureResourceView
    {
        Ptr<Dg::ITextureView>   View;
        TextureResourceViewDesc Desc;
    };

    //

    using RhiResourceView = std::variant<Dg::ITextureView*, Dg::IBufferView*>;

    using BufferViewMap  = std::map<size_t, BufferResourceView>;
    using TextureViewMap = std::map<size_t, TextureResourceView>;

    //

    struct BufferHandle : BufferResource
    {
        BufferViewMap Views;
    };

    struct TextureHandle : TextureResource
    {
        TextureViewMap Views;
    };

    //

    class AME_ENGINE_API ResourceHandle
    {
        friend class DependencyLevel;
        friend class ResourceStorage;

    public:
        ResourceHandle() = default;

    public:
        [[nodiscard]] bool IsImported() const noexcept;

    public:
        void Import(Dg::IBuffer* resource);
        void Import(Dg::ITexture* resource);
        void SetLocal(Rhi::BufferInitData* initData, const Dg::BufferDesc& desc);
        void SetLocal(Rhi::TextureInitData* initData, const Dg::TextureDesc& desc);

    public:
        [[nodiscard]] auto&& GetBufferDesc(this auto&& self)
        {
            return std::get<BufferHandle>(self.m_Handle).Desc;
        }

        [[nodiscard]] auto&& GetTextureDesc(this auto&& self)
        {
            return std::get<TextureHandle>(self.m_Handle).Desc;
        }

    public:
        [[nodiscard]] Dg::ITexture* AsTexture() const noexcept
        {
            Dg::ITexture* texture = nullptr;
            if (auto resource = std::get_if<TextureHandle>(&m_Handle))
            {
                texture = resource->Resource;
            }
            return texture;
        }

        [[nodiscard]] Dg::IBuffer* AsBuffer() const noexcept
        {
            Dg::IBuffer* buffer = nullptr;
            if (auto resource = std::get_if<BufferHandle>(&m_Handle))
            {
                buffer = resource->Resource;
            }
            return buffer;
        }

    public:
        [[nodiscard]] Dg::IBufferView* GetBufferView(size_t viewId) const noexcept
        {
            Dg::IBufferView* view = nullptr;
            if (auto resource = std::get_if<BufferHandle>(&m_Handle))
            {
                auto iter = resource->Views.find(viewId);
                if (iter != resource->Views.end())
                {
                    view = iter->second.View;
                }
            }
            return view;
        }

        [[nodiscard]] Dg::ITextureView* GetTextureView(size_t viewId) const noexcept
        {
            Dg::ITextureView* view = nullptr;
            if (auto resource = std::get_if<TextureHandle>(&m_Handle))
            {
                auto iter = resource->Views.find(viewId);
                if (iter != resource->Views.end())
                {
                    view = iter->second.View;
                }
            }
            return view;
        }

    public:
        [[nodiscard]] size_t CreateView(const BufferResourceViewDesc& viewDesc);
        [[nodiscard]] size_t CreateView(const TextureResourceViewDesc& viewDesc);

    public:
        void Reallocate(Dg::IRenderDevice* renderDevice);

    private:
        void RecreateViewsIfNeeded(BufferHandle& handle);
        void RecreateViewsIfNeeded(TextureHandle& handle);

    private:
        std::variant<BufferHandle, TextureHandle> m_Handle;

#ifndef AME_DIST
        String m_Name;
#endif

        bool m_IsImported : 1 = false;
    };
} // namespace Ame::Rg