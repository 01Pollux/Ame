#include <Gfx/RG/Resource.hpp>
#include <Rhi/Hash/Resource.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::RG
{
    ResourceHandle::ResourceHandle(
        ResourceHandle&& Other) noexcept :
        m_Resource(std::exchange(Other.m_Resource, std::monostate{})),
        m_Desc(std::move(Other.m_Desc)),
        m_DescHash(Other.m_DescHash),
        m_Views(std::move(Other.m_Views)),
        m_ImportViewsChanged(Other.m_ImportViewsChanged),
        m_IsImported(Other.m_IsImported)
    {
    }

    ResourceHandle& ResourceHandle::operator=(
        ResourceHandle&& Other) noexcept
    {
        if (this != &Other)
        {
            Release();

            Other.m_Resource     = {};
            m_Resource           = std::exchange(Other.m_Resource, std::monostate{});
            m_Desc               = std::move(Other.m_Desc);
            m_DescHash           = Other.m_DescHash;
            m_Views              = std::move(Other.m_Views);
            m_ImportViewsChanged = Other.m_ImportViewsChanged;
            m_IsImported         = Other.m_IsImported;
        }
        return *this;
    }

    ResourceHandle::~ResourceHandle()
    {
        Release();
    }

    ResourceHandle::operator bool() const noexcept
    {
        return !std::holds_alternative<std::monostate>(m_Resource);
    }

    //

    const ResourceDesc& ResourceHandle::GetDesc() const noexcept
    {
        return m_Desc;
    }

    void ResourceHandle::Import(
        Rhi::Texture Texture)
    {
        m_Desc     = Texture ? Texture.GetDesc() : Rhi::TextureDesc{};
        m_Resource = std::move(Texture);

        m_ImportViewsChanged = true;
        m_IsImported         = true;
    }

    void ResourceHandle::Import(
        Rhi::Buffer Buffer)
    {
        m_Desc     = Buffer ? Buffer.GetDesc() : Rhi::BufferDesc{};
        m_Resource = std::move(Buffer);

        m_ImportViewsChanged = true;
        m_IsImported         = true;
    }

    void ResourceHandle::SetDynamic(
        const ResourceDesc& Desc)
    {
        std::visit(
            VariantVisitor{
                [&](nri::Buffer*)
                {
                    m_Resource = std::monostate{};
                },
                [&](nri::Texture*)
                {
                    m_Resource = std::monostate{};
                },
                [](const auto&) {} },
            m_Resource);
        m_Desc = Desc;
    }

    //

    Opt<Rhi::Texture> ResourceHandle::AsTexture() const
    {
        auto Texture = std::get_if<Rhi::Texture>(&m_Resource);
        return Texture ? Opt{ *Texture } : std::nullopt;
    }

    Opt<Rhi::Buffer> ResourceHandle::AsBuffer() const
    {
        auto Buffer = std::get_if<Rhi::Buffer>(&m_Resource);
        return Buffer ? Opt{ *Buffer } : std::nullopt;
    }

    //

    nri::BufferDesc& ResourceHandle::CreateBufferView(
        const ResourceViewId& ViewId,
        ResourceViewDesc&&    ViewDesc)
    {
        if (IsImported())
        {
            m_ImportViewsChanged = true;
        }

        auto& View = m_Views[ViewId.Get()];
        View.Desc  = std::move(ViewDesc);
        return std::get<nri::BufferDesc>(m_Desc);
    }

    nri::TextureDesc& ResourceHandle::CreateTextureView(
        const ResourceViewId& ViewId,
        ResourceViewDesc&&    ViewDesc)
    {
        if (IsImported())
        {
            m_ImportViewsChanged = true;
        }

        auto& View = m_Views[ViewId.Get()];
        View.Desc  = std::move(ViewDesc);
        return std::get<nri::TextureDesc>(m_Desc);
    }

    //

    ResourceViewDesc& ResourceHandle::GetViewDescMut(
        const ResourceViewId& ViewId)
    {
        return m_Views.at(ViewId.Get()).Desc;
    }

    const ResourceViewDesc& ResourceHandle::GetViewDesc(
        const ResourceViewId& ViewId) const
    {
        return m_Views.at(ViewId.Get()).Desc;
    }

    const Rhi::ResourceView& ResourceHandle::GetViewHandle(
        const ResourceViewId& ViewId) const
    {
        return m_Views.at(ViewId.Get()).View;
    }

    //

    bool ResourceHandle::ContainsView(
        const ResourceViewId& ViewId) const
    {
        return m_Views.contains(ViewId.Get());
    }

    bool ResourceHandle::IsImported() const noexcept
    {
        return m_IsImported;
    }

    //

    void ResourceHandle::Reallocate(
        Rhi::Device& RhiDevice)
    {
        if (IsImported())
        {
            if (m_ImportViewsChanged)
            {
                RecreateViews();
                m_ImportViewsChanged = false;
            }
            return;
        }

        bool ChangedResource = false;

        std::visit(
            VariantVisitor{
                [&](const Rhi::TextureDesc& Desc)
                {
                    std::hash<nri::TextureDesc> Hasher;

                    size_t Hash = Hasher(Desc);
                    if (Hash == m_DescHash)
                    {
                        return;
                    }

                    m_DescHash = Hash;
                    m_Resource.emplace<Rhi::Texture>(RhiDevice, Rhi::MemoryLocation::DEVICE, Desc);

                    ChangedResource = true;
                },
                [&](const Rhi::BufferDesc& Desc)
                {
                    std::hash<nri::BufferDesc> Hasher;

                    size_t Hash = Hasher(Desc);
                    if (Hash == m_DescHash)
                    {
                        return;
                    }

                    m_DescHash = Hash;
                    m_Resource.emplace<Rhi::Buffer>(RhiDevice, Rhi::MemoryLocation::DEVICE, Desc);

                    ChangedResource = true;
                },
            },
            m_Desc);

        if (ChangedResource)
        {
            RecreateViews();
        }
    }

    void ResourceHandle::RecreateViews()
    {
        Rhi::ResourceView RhiView;
        VariantVisitor    Visitor{
            [&](const Rhi::BufferViewDesc& ViewDesc)
            {
                const auto& Buf = std::get<Rhi::Buffer>(m_Resource);
                RhiView         = Buf.CreateView(ViewDesc);
            },
            [&](const Rhi::TextureViewDesc& ViewDesc)
            {
                const auto& Tex     = std::get<Rhi::Texture>(m_Resource);
                auto&       TexDesc = Tex.GetDesc();
                RhiView             = Tex.CreateView(ViewDesc);
            }
        };

        for (auto& [Id, View] : m_Views)
        {
            std::visit(Visitor, View.Desc);
            View.View = std::move(RhiView);
        }
    }

    void ResourceHandle::Release()
    {
        m_Views.clear();
        m_Resource = std::monostate{};
    }
} // namespace Ame::Gfx::RG