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
            const PropertyDescriptor& Descriptor);

        PropertyMap(
            const PropertyMap* Other);

    public:
        void WriteTexture(
            const String&            Property,
            const Ptr<Rhi::Texture>& Texture,
            Rhi::TextureViewDesc     ViewDesc);

        void WriteBuffer(
            const String&           Property,
            const Ptr<Rhi::Buffer>& Buffer,
            Rhi::BufferViewDesc     ViewDesc);

        void WriteSampler(
            const String&            Property,
            Rhi::SamplerResourceView Sampler,
            Rhi::SamplerDesc         SamplerDesc);

        [[nodiscard]] const TextureResource& ReadTexture(
            const String& Property) const;

        [[nodiscard]] const BufferResource& ReadBuffer(
            const String& Property) const;

        [[nodiscard]] const SamplerResource& ReadSampler(
            const String& Property) const;

    public:
        /// <summary>
        /// Write a scalar value to the property map
        /// </summary>
        void WriteUserData(
            const String& Name,
            const void*   Data,
            size_t        Size);

        /// <summary>
        /// Read a scalar value from the property map
        /// </summary>
        void ReadUserData(
            const String& Name,
            void*         Data,
            size_t        Size) const;

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