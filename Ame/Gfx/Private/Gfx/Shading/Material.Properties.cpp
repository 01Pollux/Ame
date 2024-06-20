#include <ranges>
#include <boost/functional/hash.hpp>

#include <Gfx/Shading/Material.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>

#include <Rhi/Hash/Resource.hpp>
#include <Rhi/Hash/View.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    void Material::MakeLocal(
        const String& propertyName,
        bool          local)
    {
        // using empty string for user data properties
        bool isUserData = propertyName.contains('.');

        m_PropertyLocalMap[isUserData ? UserDataPropertyTag : propertyName] = local;
    }

    bool Material::IsLocal(
        const String& propertyName) const
    {
        // using empty string for user data properties
        bool isUserData = propertyName.contains('.');

        auto iter = m_PropertyLocalMap.find(isUserData ? UserDataPropertyTag : propertyName);
        return iter != m_PropertyLocalMap.end() ? iter->second : false;
    }

    //

    uint32_t Material::GetSizeOfUserData() const
    {
        return m_SharedData->Properties.GetSizeOfUserData();
    }

    const void* Material::GetUserData() const
    {
        return IsLocal(UserDataPropertyTag) ? m_LocalData.Properties.GetUserData() : m_SharedData->Properties.GetUserData();
    }

    auto Material::GetResources() const -> Co::generator<ResourceIterator>
    {
        auto localResources  = m_LocalData.Properties.GetResources();
        auto sharedResources = m_SharedData->Properties.GetResources();

        for (auto [Local, Shared] : std::views::zip(localResources, sharedResources))
        {
            co_yield (IsLocal(Shared->first) ? Local : Shared);
        }
    }

    //

    void Material::Set(
        const String&                  propertyName,
        const Ptr<Rhi::ScopedTexture>& texture,
        Rhi::TextureViewDesc           viewDesc)
    {
        using namespace EnumBitOperators;

#ifdef AME_DEBUG
        Log::Gfx().Assert(
            (viewDesc.Type & (Rhi::TextureViewType::AnyShaderResource | Rhi::TextureViewType::AnyUnorderedAccess)) != Rhi::TextureViewType::None,
            "Texture view type is not shader resource nor unordered access");
#endif

        TextureResource textureResource{
            .Texture  = texture,
            .ViewDesc = std::move(viewDesc),
            .View     = std::make_shared<Rhi::ScopedResourceView>(texture->CreateView(viewDesc))
        };
        Set(propertyName, std::move(textureResource));
    }

    void Material::Set(
        const String&   propertyName,
        TextureResource textureResource)
    {
        InvalidatePropertyHash();
        auto& properties = IsLocal(propertyName) ? m_LocalData.Properties : m_SharedData->Properties;
        properties.WriteTexture(propertyName, std::move(textureResource));
    }

    void Material::Set(
        const String&                 propertyName,
        const Ptr<Rhi::ScopedBuffer>& buffer,
        Rhi::BufferViewDesc           viewDesc)
    {
        BufferResource bufferResource{
            .Buffer   = buffer,
            .ViewDesc = std::move(viewDesc),
            .View     = std::make_shared<Rhi::ScopedResourceView>(buffer->CreateView(viewDesc))
        };
        Set(propertyName, std::move(bufferResource));
    }

    void Material::Set(
        const String&  propertyName,
        BufferResource bufferResource)
    {
        InvalidatePropertyHash();
        auto& properties = IsLocal(propertyName) ? m_LocalData.Properties : m_SharedData->Properties;
        properties.WriteBuffer(propertyName, std::move(bufferResource));
    }

    void Material::Set(
        const String&    propertyName,
        Rhi::SamplerDesc samplerDesc)
    {
        auto&           allocator = m_SharedData->CommonState.GetAllocator();
        SamplerResource samplerResource{
            .ViewDesc = std::move(samplerDesc),
            .View     = std::make_shared<Rhi::ScopedResourceView>(allocator.CreateSampler(samplerDesc))
        };
        Set(propertyName, std::move(samplerResource));
    }

    void Material::Set(
        const String&   propertyName,
        SamplerResource samplerResource)
    {
        InvalidatePropertyHash();
        auto& properties = IsLocal(propertyName) ? m_LocalData.Properties : m_SharedData->Properties;
        properties.WriteSampler(propertyName, std::move(samplerResource));
    }

    //

    const TextureResource& Material::GetTexture(
        const String& propertyName) const
    {
        auto& properties = IsLocal(propertyName) ? m_LocalData.Properties : m_SharedData->Properties;
        return properties.ReadTexture(propertyName);
    }

    const BufferResource& Material::GetBuffer(
        const String& propertyName) const
    {
        auto& properties = IsLocal(propertyName) ? m_LocalData.Properties : m_SharedData->Properties;
        return properties.ReadBuffer(propertyName);
    }

    const SamplerResource& Material::GetSampler(
        const String& propertyName) const
    {
        auto& properties = IsLocal(propertyName) ? m_LocalData.Properties : m_SharedData->Properties;
        return properties.ReadSampler(propertyName);
    }

    //

    void Material::SetScalar(
        const String&    propertyName,
        const std::byte* data,
        size_t           size)
    {
        InvalidatePropertyHash();
        auto& properties = IsLocal(propertyName) ? m_LocalData.Properties : m_SharedData->Properties;
        properties.WriteUserData(propertyName, data, size);
    }

    //

    void Material::GetScalar(
        const String& Property,
        std::byte*    data,
        size_t        size) const
    {
        auto& properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        properties.ReadUserData(Property, data, size);
    }

    //

    void Material::InvalidatePropertyHash()
    {
        m_PropertiesHash.reset();
    }

    void Material::UpdatePropertyHash() const
    {
        auto userData     = IsLocal(UserDataPropertyTag) ? m_LocalData.Properties.GetUserData() : m_SharedData->Properties.GetUserData();
        auto userDataSize = m_SharedData->Properties.GetSizeOfUserData();

        uint64_t hash = boost::hash_range(userData, userData + userDataSize);

        auto localResources  = m_LocalData.Properties.GetResources();
        auto sharedResources = m_SharedData->Properties.GetResources();

        for (auto resourceIter : GetResources())
        {
            std::visit(
                VariantVisitor{
                    [&](const BufferResource& bufferResource)
                    {
                        boost::hash_combine(hash, bufferResource.Buffer.get());
                        boost::hash_combine(hash, std::hash<Rhi::BufferViewDesc>{}(bufferResource.ViewDesc));
                    },
                    [&](const TextureResource& textureResource)
                    {
                        boost::hash_combine(hash, textureResource.Texture.get());
                        boost::hash_combine(hash, std::hash<Rhi::TextureViewDesc>{}(textureResource.ViewDesc));
                    },
                    [&](const SamplerResource& samplerResource)
                    {
                        boost::hash_combine(hash, std::hash<Rhi::SamplerDesc>{}(samplerResource.ViewDesc));
                    } },
                resourceIter->second);
        }

        m_PropertiesHash = hash;
    }
} // namespace Ame::Gfx::Shading