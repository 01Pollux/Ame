#pragma once

#include <Rg/Core.hpp>

namespace Ame::Rg
{
    class ResourceId
    {
        static constexpr size_t c_InvalidId = std::numeric_limits<size_t>::max();

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
        [[nodiscard]] const char* GetName() const noexcept
        {
#ifndef AME_DIST
            return m_Name.c_str();
#else
            return "";
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

    public:
        /// <summary>
        /// Check if resource id is valid, and asserts (debug only) if it's not
        /// </summary>
        static void Validate(const ResourceId& id);

    private:
        StringHash m_Id = c_InvalidId;
#ifndef AME_DIST
        String m_Name;
#endif
    };

    //

    using RhiBufferViewMap  = std::map<size_t, Ptr<Dg::IBufferView>>;
    using RhiTextureViewMap = std::map<size_t, Ptr<Dg::ITextureView>>;

    //

    using RhiResource        = std::variant<Ptr<Dg::IBuffer>, Ptr<Dg::ITexture>>;
    using RhiResourceViewMap = std::variant<RhiBufferViewMap, RhiTextureViewMap>;
    using RhiResourceView    = std::variant<Dg::ITextureView*, Dg::IBufferView*>;

    //

    class ResourceHandle
    {
        friend class DependencyLevel;
        friend class ResourceStorage;

    public:
        ResourceHandle() = default;

    public:
        void Import(const ResourceId& id, RhiResource resource);
        void SetLocal(const ResourceId& id, RhiResource resource);

    public:
        [[nodiscard]] const RhiResource& Get() const noexcept;
        [[nodiscard]] Dg::ITexture*      AsTexture() const noexcept;
        [[nodiscard]] Dg::IBuffer*       AsBuffer() const noexcept;

    public:
        [[nodiscard]] Dg::IBufferView*  LoadView(const BufferResourceViewDesc& viewDesc);
        [[nodiscard]] Dg::ITextureView* LoadView(const TextureResourceViewDesc& viewDesc);

    public:
        [[nodiscard]] bool IsImported() const noexcept;

    private:
        void InitializeViewMap();

    private:
        RhiResource        m_Resource;
        RhiResourceViewMap m_Views;

#ifndef AME_DIST
        String m_Name;
#endif

        bool m_IsImported : 1 = false;
    };
} // namespace Ame::Rg