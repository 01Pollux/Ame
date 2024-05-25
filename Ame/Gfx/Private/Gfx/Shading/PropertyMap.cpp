#include <Gfx/Shading/PropertyMap.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    PropertyMap::PropertyMap(
        const PropertyDescriptor& descriptor) :
        m_UserData(descriptor)
    {
        if (m_UserData.GetStructSize() > 0)
        {
            m_UserDataBuffer = std::make_unique<uint8_t[]>(m_UserData.GetStructSize());
        }

        for (auto& resource : descriptor.GetResources())
        {
            switch (resource.get().Type)
            {
            case ResourceType::Buffer:
            case ResourceType::RWBuffer:
            case ResourceType::StructuredBuffer:
            case ResourceType::RWStructuredBuffer:
                m_Resources[resource.get().PropName] = BufferResource();
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
                m_Resources[resource.get().PropName] = TextureResource();
                break;

            case ResourceType::Sampler:
                m_Resources[resource.get().PropName] = SamplerResource();
                break;

            default:
                std::unreachable();
            }
        }
    }

    PropertyMap::PropertyMap(
        const PropertyMap* other) :
        m_UserData(other->m_UserData),
        m_Resources(other->m_Resources)
    {
        if (m_UserData.GetStructSize() > 0)
        {
            m_UserDataBuffer = std::make_unique<uint8_t[]>(m_UserData.GetStructSize());
            std::copy(other->GetUserData(), other->GetUserData() + m_UserData.GetStructSize(), m_UserDataBuffer.get());
        }
    }

    //

    void PropertyMap::WriteTexture(
        const String&   propertyName,
        TextureResource textureResource)
    {
        auto iter = m_Resources.find(propertyName);

#ifdef AME_DEBUG
        Log::Gfx().Assert(iter != m_Resources.end(), "Property not found in property map");
        Log::Gfx().Assert(std::holds_alternative<TextureResource>(iter->second), "Property is not a texture resource");
#endif

        iter->second = std::move(textureResource);
    }

    void PropertyMap::WriteBuffer(
        const String&  propertyName,
        BufferResource bufferResource)
    {
        auto iter = m_Resources.find(propertyName);

#ifdef AME_DEBUG
        Log::Gfx().Assert(iter != m_Resources.end(), "Property not found in property map");
        Log::Gfx().Assert(std::holds_alternative<BufferResource>(iter->second), "Property is not a buffer resource");
#endif

        iter->second = std::move(bufferResource);
    }

    void PropertyMap::WriteSampler(
        const String&   propertyName,
        SamplerResource samplerResource)
    {
        auto iter = m_Resources.find(propertyName);

#ifdef AME_DEBUG
        Log::Gfx().Assert(iter != m_Resources.end(), "Property not found in property map");
        Log::Gfx().Assert(std::holds_alternative<SamplerResource>(iter->second), "Property is not a sampler resource");
#endif

        iter->second = std::move(samplerResource);
    }

    //

    const TextureResource& PropertyMap::ReadTexture(
        const String& propertyName) const
    {
        auto iter = m_Resources.find(propertyName);

#ifdef AME_DEBUG
        Log::Gfx().Assert(iter != m_Resources.end(), "Property not found in property map");
        Log::Gfx().Assert(std::holds_alternative<TextureResource>(iter->second), "Property is not a texture resource");
#endif

        return std::get<TextureResource>(iter->second);
    }

    const BufferResource& PropertyMap::ReadBuffer(
        const String& propertyName) const
    {
        auto iter = m_Resources.find(propertyName);

#ifdef AME_DEBUG
        Log::Gfx().Assert(iter != m_Resources.end(), "Property not found in property map");
        Log::Gfx().Assert(std::holds_alternative<BufferResource>(iter->second), "Property is not a buffer resource");
#endif

        return std::get<BufferResource>(iter->second);
    }

    const SamplerResource& PropertyMap::ReadSampler(
        const String& propertyName) const
    {
        auto iter = m_Resources.find(propertyName);

#ifdef AME_DEBUG
        Log::Gfx().Assert(iter != m_Resources.end(), "Property not found in property map");
        Log::Gfx().Assert(std::holds_alternative<SamplerResource>(iter->second), "Property is not a sampler resource");
#endif

        return std::get<SamplerResource>(iter->second);
    }

    auto PropertyMap::GetResources() const -> Co::generator<ResourceMap::const_iterator>
    {
        for (auto iter = m_Resources.begin(); iter != m_Resources.end(); iter++)
        {
            co_yield iter;
        }
    }

    //

    void PropertyMap::WriteUserData(
        const String& propertyName,
        const void*   data,
        size_t        size)
    {
        uint32_t offset = m_UserData.GetOffset(propertyName);

#ifdef AME_DEBUG
        Log::Gfx().Assert(offset + size <= GetSizeOfUserData(), "User data buffer overflow");
#endif

        std::memcpy(m_UserDataBuffer.get() + offset, data, size);
    }

    void PropertyMap::ReadUserData(
        const String& propertyName,
        void*         data,
        size_t        size) const
    {
        uint32_t offset = m_UserData.GetOffset(propertyName);

#ifdef AME_DEBUG
        Log::Gfx().Assert(offset + size <= m_UserData.GetStructSize(), "User data buffer overflow");
#endif

        std::memcpy(data, m_UserDataBuffer.get() + offset, size);
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
