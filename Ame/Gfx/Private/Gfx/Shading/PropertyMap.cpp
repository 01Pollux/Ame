#include <Gfx/Shading/PropertyMap.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    PropertyMap::PropertyMap(
        const PropertyDescriptor& Descriptor) :
        m_UserData(Descriptor)
    {
        if (m_UserData.GetStructSize() > 0)
        {
            m_UserDataBuffer = std::make_unique<uint8_t[]>(m_UserData.GetStructSize());
        }

        for (auto& Resource : Descriptor.GetResources())
        {
            switch (Resource.get().Type)
            {
            case ResourceType::Buffer:
            case ResourceType::RWBuffer:
            case ResourceType::StructuredBuffer:
            case ResourceType::RWStructuredBuffer:
                m_Resources[Resource.get().PropName] = BufferResource();
                break;

            case ResourceType::Texture1D:
            case ResourceType::Texture1DArray:
            case ResourceType::Texture2D:
            case ResourceType::Texture2DArray:
            case ResourceType::Texture2DMS:
            case ResourceType::Texture2DMSArray:
            case ResourceType::Texture3D:
            case ResourceType::TextureCube:
            case ResourceType::TextureCubeArray:
            case ResourceType::RWTexture1D:
            case ResourceType::RWTexture1DArray:
            case ResourceType::RWTexture2D:
            case ResourceType::RWTexture2DArray:
            case ResourceType::RWTexture3D:
                m_Resources[Resource.get().PropName] = TextureResource();
                break;

            case ResourceType::Sampler:
                m_Resources[Resource.get().PropName] = SamplerResource();
                break;

            default:
                std::unreachable();
            }
        }
    }

    PropertyMap::PropertyMap(
        const PropertyMap* Other) :
        m_UserData(Other->m_UserData),
        m_Resources(Other->m_Resources)
    {
        if (m_UserData.GetStructSize() > 0)
        {
            m_UserDataBuffer = std::make_unique<uint8_t[]>(m_UserData.GetStructSize());
            std::copy(Other->GetUserData(), Other->GetUserData() + m_UserData.GetStructSize(), m_UserDataBuffer.get());
        }
    }

    //

    void PropertyMap::WriteTexture(
        const String&   Property,
        TextureResource Texture)
    {
        auto Iter = m_Resources.find(Property);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Iter != m_Resources.end(), "Property not found in property map");
        Log::Renderer().Assert(std::holds_alternative<TextureResource>(Iter->second), "Property is not a texture resource");
#endif

        Iter->second = std::move(Texture);
    }

    void PropertyMap::WriteBuffer(
        const String&  Property,
        BufferResource Buffer)
    {
        auto Iter = m_Resources.find(Property);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Iter != m_Resources.end(), "Property not found in property map");
        Log::Renderer().Assert(std::holds_alternative<BufferResource>(Iter->second), "Property is not a buffer resource");
#endif

        Iter->second = std::move(Buffer);
    }

    void PropertyMap::WriteSampler(
        const String&   Property,
        SamplerResource Sampler)
    {
        auto Iter = m_Resources.find(Property);

#ifdef AME_DEBUG
        Log::Renderer().Assert(Iter != m_Resources.end(), "Property not found in property map");
        Log::Renderer().Assert(std::holds_alternative<SamplerResource>(Iter->second), "Property is not a sampler resource");
#endif

        Iter->second = std::move(Sampler);
    }

    //

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
        for (auto Iter = m_Resources.begin(); Iter != m_Resources.end(); Iter++)
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
