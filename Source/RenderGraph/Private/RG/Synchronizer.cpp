#include <Rg/Synchronizer.hpp>
#include <Rg/ResourceStorage.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rg
{
    ResourceSynchronizer::ResourceSynchronizer(
        ResourceStorage& storage) :
        m_Storage(storage)
    {
    }

    //

    Co::shared_result<void> ResourceSynchronizer::WaitFor(
        const ResourceId& id)
    {
        ResourceId::Validate(id);
        auto& producer = m_WaitMap[id];

        if (m_Storage.get().ContainsResource(id) ||
            m_Storage.get().ContainsUserData(id))
        {
            producer.Signal();
        }
        else
        {
#ifndef AME_DIST
            Log::Gfx().Trace("RenderGraph Waiting for: '{}'", id.GetName());
#endif
        }
        return producer.Result;
    }

    void ResourceSynchronizer::Signal(
        const ResourceId& id)
    {
        ResourceId::Validate(id);
#ifndef AME_DIST
        Log::Gfx().Trace("RenderGraph Signaling : '{}'", id.GetName());
#endif
        m_WaitMap[id].Signal();
    }
} // namespace Ame::Rg