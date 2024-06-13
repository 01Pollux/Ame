#include <RG/Resource.hpp>
#include <Rhi/Hash/Resource.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
{
    ResourceHandle::ResourceHandle(
        ResourceHandle&& other) noexcept :
        m_Resource(std::exchange(other.m_Resource, std::monostate{})),
        m_Desc(std::move(other.m_Desc)),
        m_DescHash(other.m_DescHash),
        m_Views(std::move(other.m_Views)),
        m_ImportViewsChanged(other.m_ImportViewsChanged),
        m_IsImported(other.m_IsImported)
    {
    }

    ResourceHandle& ResourceHandle::operator=(
        ResourceHandle&& other) noexcept
    {
        if (this != &other)
        {
            Release();

            other.m_Resource     = {};
            m_Resource           = std::exchange(other.m_Resource, std::monostate{});
            m_Desc               = std::move(other.m_Desc);
            m_DescHash           = other.m_DescHash;
            m_Views              = std::move(other.m_Views);
            m_ImportViewsChanged = other.m_ImportViewsChanged;
            m_IsImported         = other.m_IsImported;
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
        Rhi::Texture texture)
    {
        if (texture)
        {
            m_Desc     = texture.GetDesc();
            m_Resource = std::move(texture);
        }
        else
        {
            m_Resource = std::monostate();
        }

        m_ImportViewsChanged = true;
        m_IsImported         = true;

#ifndef AME_DIST
        m_Name.clear();
#endif
    }

    void ResourceHandle::Import(
        Rhi::Buffer buffer)
    {
        if (buffer)
        {
            m_Desc     = buffer.GetDesc();
            m_Resource = std::move(buffer);
        }
        else
        {
            m_Resource = std::monostate();
        }

        m_ImportViewsChanged = true;
        m_IsImported         = true;

#ifndef AME_DIST
        m_Name.clear();
#endif
    }

    void ResourceHandle::SetDynamic(
        const ResourceId&   id,
        const ResourceDesc& desc)
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
        m_Desc = desc;

#ifndef AME_DIST
        m_Name = id.GetName();
#endif
    }

    //

    const Rhi::Texture* ResourceHandle::AsTexture() const
    {
        return std::get_if<Rhi::Texture>(&m_Resource);
    }

    const Rhi::Buffer* ResourceHandle::AsBuffer() const
    {
        return std::get_if<Rhi::Buffer>(&m_Resource);
    }

    //

    nri::BufferDesc& ResourceHandle::CreateBufferView(
        const ResourceViewId& viewId,
        ResourceViewDesc&&    desc)
    {
        if (IsImported())
        {
            m_ImportViewsChanged = true;
        }

        auto& view = m_Views[viewId.Get()];
        view.Desc  = std::move(desc);
        return std::get<nri::BufferDesc>(m_Desc);
    }

    nri::TextureDesc& ResourceHandle::CreateTextureView(
        const ResourceViewId& viewId,
        ResourceViewDesc&&    desc)
    {
        if (IsImported())
        {
            m_ImportViewsChanged = true;
        }

        auto& view = m_Views[viewId.Get()];
        view.Desc  = std::move(desc);
        return std::get<nri::TextureDesc>(m_Desc);
    }

    //

    ResourceViewDesc& ResourceHandle::GetViewDescMut(
        const ResourceViewId& viewId)
    {
        return m_Views.at(viewId.Get()).Desc;
    }

    const ResourceViewDesc& ResourceHandle::GetViewDesc(
        const ResourceViewId& viewId) const
    {
        return m_Views.at(viewId.Get()).Desc;
    }

    const Rhi::ResourceView& ResourceHandle::GetViewHandle(
        const ResourceViewId& viewId) const
    {
        return m_Views.at(viewId.Get()).View;
    }

    //

    bool ResourceHandle::ContainsView(
        const ResourceViewId& viewId) const
    {
        return m_Views.contains(viewId.Get());
    }

    bool ResourceHandle::IsImported() const noexcept
    {
        return m_IsImported;
    }

    //

    void ResourceHandle::Reallocate(
        Rhi::Device& rhiDevice)
    {
        bool changedResource = false;

        if (IsImported())
        {
            changedResource = m_ImportViewsChanged &&
                              !std::holds_alternative<std::monostate>(m_Resource);
        }
        else
        {
            std::visit(
                VariantVisitor{
                    [&](const Rhi::TextureDesc& desc)
                    {
                        size_t hash = std::hash<nri::TextureDesc>{}(desc);
                        if (hash == m_DescHash)
                        {
                            return;
                        }
                        m_DescHash      = hash;
                        changedResource = true;

                        m_Resource = std::monostate{};
                        Rhi::Texture texture(rhiDevice, Rhi::MemoryLocation::DEVICE, desc);
#ifndef AME_DIST
                        texture.SetName(m_Name.c_str());
#endif
                        m_Resource.emplace<Rhi::Texture>(std::move(texture));
                    },
                    [&](const Rhi::BufferDesc& desc)
                    {
                        size_t hash = std::hash<nri::BufferDesc>{}(desc);
                        if (hash == m_DescHash)
                        {
                            return;
                        }
                        m_DescHash      = hash;
                        changedResource = true;

                        m_Resource = std::monostate{};
                        Rhi::Buffer buffer(rhiDevice, Rhi::MemoryLocation::DEVICE, desc);
#ifndef AME_DIST
                        buffer.SetName(m_Name.c_str());
#endif
                        m_Resource.emplace<Rhi::Buffer>(std::move(buffer));
                    },
                },
                m_Desc);
        }

        if (changedResource)
        {
            RecreateViews();
        }
    }

    void ResourceHandle::RecreateViews()
    {
        Rhi::ResourceView viewHandle;
        VariantVisitor    visitor{
            [&](const Rhi::BufferViewDesc& ViewDesc)
            {
                const auto& buffer = std::get<Rhi::Buffer>(m_Resource);
                viewHandle         = buffer.CreateView(ViewDesc);
            },
            [&](const Rhi::TextureViewDesc& ViewDesc)
            {
                const auto& texture = std::get<Rhi::Texture>(m_Resource);
                viewHandle          = texture.CreateView(ViewDesc);
            }
        };

        for (auto& [id, view] : m_Views)
        {
            std::visit(visitor, view.Desc);
            view.View = std::move(viewHandle);
        }
    }

    void ResourceHandle::Release()
    {
        m_Views.clear();
        m_Resource = std::monostate{};
    }
} // namespace Ame::RG