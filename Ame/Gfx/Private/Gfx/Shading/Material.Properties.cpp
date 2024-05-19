#include <ranges>
#include <boost/functional/hash.hpp>

#include <Gfx/Shading/Material.hpp>
#include <Rhi/Device/Device.hpp>
#include <Rhi/Hash/Resource.hpp>
#include <Rhi/Hash/View.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    void Material::MakeLocal(
        const String& PropertyName,
        bool          Local)
    {
        // using empty string for user data properties
        bool IsUserData = PropertyName.contains('.');

        m_PropertyLocalMap[IsUserData ? UserDataPropertyTag : PropertyName] = Local;
    }

    bool Material::IsLocal(
        const String& PropertyName) const
    {
        // using empty string for user data properties
        bool IsUserData = PropertyName.contains('.');

        auto Iter = m_PropertyLocalMap.find(IsUserData ? UserDataPropertyTag : PropertyName);
        return Iter != m_PropertyLocalMap.end() ? Iter->second : false;
    }

    //

    void Material::Set(
        const String&            Property,
        const Ptr<Rhi::Texture>& Texture,
        Rhi::TextureViewDesc     ViewDesc)
    {
#ifdef AME_DEBUG
        Log::Renderer().Assert(
            ViewDesc.Type == Rhi::TextureViewType::AnyShaderResource ||
                ViewDesc.Type == Rhi::TextureViewType::AnyUnorderedAccess,
            "Texture view type is not shader resource nor unordered access");
#endif

        TextureResource Resource{
            .Texture  = Texture,
            .ViewDesc = std::move(ViewDesc),
            .View     = std::make_shared<Rhi::ResourceView>(Texture->CreateView(Resource.ViewDesc))
        };
        Set(Property, std::move(Resource));
    }

    void Material::Set(
        const String&   Property,
        TextureResource Texture)
    {
        InvalidateHash();
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        Properties.WriteTexture(Property, std::move(Texture));
    }

    void Material::Set(
        const String&           Property,
        const Ptr<Rhi::Buffer>& Buffer,
        Rhi::BufferViewDesc     ViewDesc)
    {
        BufferResource Resource{
            .Buffer   = Buffer,
            .ViewDesc = std::move(ViewDesc),
            .View     = std::make_shared<Rhi::ResourceView>(Buffer->CreateView(Resource.ViewDesc))
        };
        Set(Property, std::move(Resource));
    }

    void Material::Set(
        const String&  Property,
        BufferResource Buffer)
    {
        InvalidateHash();
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        Properties.WriteBuffer(Property, std::move(Buffer));
    }

    void Material::Set(
        const String&    Property,
        Rhi::SamplerDesc SamplerDesc)
    {
        SamplerResource Resource{
            .ViewDesc = std::move(SamplerDesc),
            .View     = std::make_shared<Rhi::SamplerResourceView>(m_SharedData->CommonState.GetDevice(), SamplerDesc)
        };
        Set(Property, std::move(Resource));
    }

    void Material::Set(
        const String&   Property,
        SamplerResource Sampler)
    {
        InvalidateHash();
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        Properties.WriteSampler(Property, std::move(Sampler));
    }

    //

    const TextureResource& Material::GetTexture(
        const String& Property) const
    {
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        return Properties.ReadTexture(Property);
    }

    const BufferResource& Material::GetBuffer(
        const String& Property) const
    {
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        return Properties.ReadBuffer(Property);
    }

    const SamplerResource& Material::GetSampler(
        const String& Property) const
    {
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        return Properties.ReadSampler(Property);
    }

    //

    void Material::SetScalar(
        const String& Property,
        const void*   Value,
        size_t        Size)
    {
        InvalidateHash();
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        Properties.WriteUserData(Property, Value, Size);
    }

    //

    void Material::GetScalar(
        const String& Property,
        void*         Value,
        size_t        Size) const
    {
        auto& Properties = IsLocal(Property) ? m_LocalData.Properties : m_SharedData->Properties;
        Properties.ReadUserData(Property, Value, Size);
    }

    //

    void Material::InvalidateHash()
    {
        m_PropertiesHash.reset();
    }

    void Material::UpdateHash() const
    {
        auto UserData     = IsLocal(UserDataPropertyTag) ? m_LocalData.Properties.GetUserData() : m_SharedData->Properties.GetUserData();
        auto UserDataSize = m_SharedData->Properties.GetSizeOfUserData();

        uint64_t Hash = boost::hash_range(UserData, UserData + UserDataSize);

        auto LocalResources  = m_LocalData.Properties.GetResources();
        auto SharedResources = m_SharedData->Properties.GetResources();

        for (auto [Local, Shared] : std::views::zip(LocalResources, SharedResources))
        {
            const RhiResourceType& Resource = IsLocal(Shared->first) ? Local->second : Shared->second;

            std::visit(
                VariantVisitor{
                    [&](const BufferResource& Buffer)
                    {
                        boost::hash_combine(Hash, Buffer.Buffer.get());
                        boost::hash_combine(Hash, std::hash<Rhi::BufferViewDesc>{}(Buffer.ViewDesc));
                    },
                    [&](const TextureResource& Texture)
                    {
                        boost::hash_combine(Hash, Texture.Texture.get());
                        boost::hash_combine(Hash, std::hash<Rhi::TextureViewDesc>{}(Texture.ViewDesc));
                    },
                    [&](const SamplerResource& Sampler)
                    {
                        boost::hash_combine(Hash, std::hash<Rhi::SamplerDesc>{}(Sampler.ViewDesc));
                    } },
                Resource);
        }

        m_PropertiesHash = Hash;
    }
} // namespace Ame::Gfx::Shading