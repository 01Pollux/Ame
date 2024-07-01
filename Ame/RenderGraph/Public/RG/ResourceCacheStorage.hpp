#pragma once

#include <boost/container/flat_set.hpp>
#include <RG/Core.hpp>

namespace Ame::RG
{
    class ResourceCacheStorage
    {
    private:
        struct ResoureIndexTimeStamp
        {
            size_t Hash;
            size_t TimeStamp;

            auto operator<=>(const ResoureIndexTimeStamp& other) const
            {
                return std::tie(TimeStamp, Hash) <=> std::tie(other.TimeStamp, other.Hash);
            }
        };

        struct ResourceViewTimeStamp
        {
            Rhi::ScopedResourceView View;
            size_t                  Hash;
            size_t                  TimeStamp;
        };

        using ResourceViewMap          = std::map<size_t, ResourceViewTimeStamp>; // Key is the hash of the resource view name
        using ResourceViewTimeStampSet = boost::container::flat_set<ResoureIndexTimeStamp>;

    public:
        ResourceCacheStorage(
            Rhi::Device& device) :
            m_RhiDevice(device)
        {
        }

    public:
        /// <summary>
        /// Create a resource view with name and update the time stamp
        /// </summary>
        [[nodiscard]] Rhi::ResourceView CreateView(
            const Rhi::Buffer&         buffer,
            const Rhi::BufferViewDesc& desc);

        /// <summary>
        /// Create a resource view with name and update the time stamp
        /// </summary>
        [[nodiscard]] Rhi::ResourceView CreateView(
            const Rhi::Texture&         texture,
            const Rhi::TextureViewDesc& textureDesc);

    public:
        /// <summary>
        /// Release all resource views with time stamps less than the current time stamp minus frame count
        /// </summary>
        void ReleaseTimestamps();

    private:
        Ref<Rhi::Device> m_RhiDevice;

        ResourceViewMap          m_ResourceViews;
        ResourceViewTimeStampSet m_ResourceViewTimeStamps;
    };
} // namespace Ame::RG