#include <RG/ResourceCacheStorage.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Hash/View.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
{
    Rhi::ResourceView ResourceCacheStorage::CreateView(
        const Rhi::Buffer&         buffer,
        const Rhi::BufferViewDesc& desc)
    {
        uint64_t timeStamp = m_RhiDevice.get().GetFrameCount();

        size_t hash = std::hash<Rhi::BufferViewDesc>{}(desc);
        auto   it   = m_ResourceViews.find(hash);

        if (it != m_ResourceViews.end())
        {
            // Update the time stamp
            m_ResourceViewTimeStamps.erase({ it->second.Hash, it->second.TimeStamp });
            m_ResourceViewTimeStamps.insert({ it->second.Hash, timeStamp });
        }
        else
        {
            // Create a new resource view
            it = m_ResourceViews.emplace(hash, ResourceViewTimeStamp{ buffer.CreateView(desc), hash, timeStamp }).first;
            m_ResourceViewTimeStamps.insert({ hash, timeStamp });
        }

        return it->second.View;
    }

    Rhi::ResourceView ResourceCacheStorage::CreateView(
        const Rhi::Texture&         texture,
        const Rhi::TextureViewDesc& textureDesc)
    {
        uint64_t timeStamp = m_RhiDevice.get().GetFrameCount();

		size_t hash = std::hash<Rhi::TextureViewDesc>{}(textureDesc);
		auto   it   = m_ResourceViews.find(hash);

		if (it != m_ResourceViews.end())
		{
			// Update the time stamp
			m_ResourceViewTimeStamps.erase({ it->second.Hash, it->second.TimeStamp });
			m_ResourceViewTimeStamps.insert({ it->second.Hash, timeStamp });
		}
		else
		{
			// Create a new resource view
			it = m_ResourceViews.emplace(hash, ResourceViewTimeStamp{ texture.CreateView(textureDesc), hash, timeStamp }).first;
			m_ResourceViewTimeStamps.insert({ hash, timeStamp });
		}

		return it->second.View;
    }

    void ResourceCacheStorage::ReleaseTimestamps()
    {
        uint64_t timeStamp  = m_RhiDevice.get().GetFrameCount();
        uint8_t  frameCount = m_RhiDevice.get().GetFrameCountInFlight();
        if (timeStamp < frameCount) [[unlikely]]
        {
            return;
        }

        timeStamp -= frameCount;
        m_ResourceViewTimeStamps.erase(m_ResourceViewTimeStamps.begin(), m_ResourceViewTimeStamps.lower_bound({ 0, timeStamp }));
    }
} // namespace Ame::RG