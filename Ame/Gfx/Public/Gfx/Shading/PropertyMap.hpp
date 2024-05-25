#pragma once

#include <Gfx/Shading/PropertyDescriptor.hpp>
#include <boost/container/flat_map.hpp>

#include <variant>

namespace Ame::Gfx::Shading
{
    class PropertyMap
    {
    public:
        using ResourceMap = boost::container::flat_map<String, RhiResourceType>;

    public:
        PropertyMap(
            const PropertyDescriptor& descriptor);

        PropertyMap(
            const PropertyMap* other);

    public:
        void WriteTexture(
            const String&   propertyName,
            TextureResource textureResource);

        void WriteBuffer(
            const String&  propertyName,
            BufferResource bufferResource);

        void WriteSampler(
            const String&   propertyName,
            SamplerResource samplerResource);

        [[nodiscard]] const TextureResource& ReadTexture(
            const String& propertyName) const;

        [[nodiscard]] const BufferResource& ReadBuffer(
            const String& propertyName) const;

        [[nodiscard]] const SamplerResource& ReadSampler(
            const String& propertyName) const;

        /// <summary>
        /// Get the resources of this material
        /// </summary>
        [[nodiscard]] Co::generator<ResourceMap::const_iterator> GetResources() const;

    public:
        /// <summary>
        /// Write a scalar value to the property map
        /// </summary>
        void WriteUserData(
            const String& propertyName,
            const void*   data,
            size_t        size);

        /// <summary>
        /// Read a scalar value from the property map
        /// </summary>
        void ReadUserData(
            const String& propertyName,
            void*         data,
            size_t        size) const;

        /// <summary>
        /// Get the user data of this material
        /// </summary>
        [[nodiscard]] const uint8_t* GetUserData() const;

        /// <summary>
        /// Get the size of the user data of this material
        /// </summary>
        [[nodiscard]] uint32_t GetSizeOfUserData() const;

    private:
        UPtr<uint8_t[]>    m_UserDataBuffer;
        PropertyDescriptor m_UserData;
        ResourceMap        m_Resources;
    };
} // namespace Ame::Gfx::Shading