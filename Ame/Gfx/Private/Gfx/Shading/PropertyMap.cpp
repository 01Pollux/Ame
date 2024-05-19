#include <Gfx/Shading/PropertyMap.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    PropertyMap::PropertyMap(
        const PropertyDescriptor& Descriptor)
    {
    }

    PropertyMap::PropertyMap(
        const PropertyMap* Other)
    {
    }

    //

    void PropertyMap::WriteTexture(
        const String&            Property,
        const Ptr<Rhi::Texture>& Texture,
        Rhi::TextureViewDesc     ViewDesc)
    {
        auto Iter = m_Resources.find(Property);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Iter != m_Resources.end(), "Property not found in property map");
        Log::Renderer().Assert(std::holds_alternative<TextureResource>(Iter->second), "Property is not a texture resource");
        Log::Renderer().Assert(ViewDesc.Type == Rhi::TextureViewType::AnyShaderResource ||
                                   ViewDesc.Type == Rhi::TextureViewType::AnyUnorderedAccess,
                               "Texture view type is not shader resource nor unordered access");

#endif

        auto& Resource = std::get<TextureResource>(Iter->second);

        Resource.Texture  = Texture;
        Resource.ViewDesc = std::move(ViewDesc);
        Resource.View     = Texture->CreateView(Resource.ViewDesc);
    }

    void PropertyMap::WriteBuffer(
        const String&           Property,
        const Ptr<Rhi::Buffer>& Buffer,
        Rhi::BufferViewDesc     ViewDesc)
    {
        auto Iter = m_Resources.find(Property);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Iter != m_Resources.end(), "Property not found in property map");
        Log::Renderer().Assert(std::holds_alternative<BufferResource>(Iter->second), "Property is not a buffer resource");
#endif

        auto& Resource = std::get<BufferResource>(Iter->second);

        Resource.Buffer   = Buffer;
        Resource.ViewDesc = std::move(ViewDesc);
        Resource.View     = Buffer->CreateView(Resource.ViewDesc);
    }

    void PropertyMap::WriteSampler(
        const String&            Property,
        Rhi::SamplerResourceView Sampler,
        Rhi::SamplerDesc         SamplerDesc)
    {
        auto Iter = m_Resources.find(Property);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Iter != m_Resources.end(), "Property not found in property map");
        Log::Renderer().Assert(std::holds_alternative<SamplerResource>(Iter->second), "Property is not a sampler resource");
        Log::Renderer().Assert(Sampler.IsOwning(), "Sampler resource is not owning");
#endif

        auto& Resource = std::get<SamplerResource>(Iter->second);

        Resource.ViewDesc = std::move(SamplerDesc);
        Resource.View     = std::move(Sampler);
    }

    const TextureResource& PropertyMap::ReadTexture(
        const String& Property) const
    {
        auto Iter = m_Resources.find(Property);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Iter != m_Resources.end(), "Property not found in property map");
        Log::Renderer().Assert(std::holds_alternative<TextureResource>(Iter->second), "Property is not a texture resource");
#endif

        return std::get<TextureResource>(Iter->second);
    }

    const BufferResource& PropertyMap::ReadBuffer(
        const String& Property) const
    {
        auto Iter = m_Resources.find(Property);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Iter != m_Resources.end(), "Property not found in property map");
        Log::Renderer().Assert(std::holds_alternative<BufferResource>(Iter->second), "Property is not a buffer resource");
#endif

        return std::get<BufferResource>(Iter->second);
    }

    const SamplerResource& PropertyMap::ReadSampler(
        const String& Property) const
    {
        auto Iter = m_Resources.find(Property);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Iter != m_Resources.end(), "Property not found in property map");
        Log::Renderer().Assert(std::holds_alternative<SamplerResource>(Iter->second), "Property is not a sampler resource");
#endif

        return std::get<SamplerResource>(Iter->second);
    }

    auto PropertyMap::GetResources() const -> Co::generator<ResourceMap::const_iterator>
    {
        for (auto Iter = m_Resources.begin(); Iter != m_Resources.end(); ++Iter)
        {
            co_yield Iter;
        }
    }

    //

    void PropertyMap::WriteUserData(
        const String& Name,
        const void*   Data,
        size_t        Size)
    {
        uint32_t Offset = m_UserData.GetOffset(Name);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Offset + Size <= GetSizeOfUserData(), "User data buffer overflow");
#endif

        std::memcpy(m_UserDataBuffer.get() + Offset, Data, Size);
    }

    void PropertyMap::ReadUserData(
        const String& Name,
        void*         Data,
        size_t        Size) const
    {
        uint32_t Offset = m_UserData.GetOffset(Name);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Offset + Size <= m_UserData.GetStructSize(), "User data buffer overflow");
#endif

        std::memcpy(Data, m_UserDataBuffer.get() + Offset, Size);
    }

    const uint8_t* PropertyMap::GetUserData() const
    {
        return m_UserDataBuffer.get();
    }

    uint32_t PropertyMap::GetSizeOfUserData() const
    {
        return m_UserData.GetStructSize();
    }
} // namespace Ame::Gfx::Shading
