#pragma once

#include <Rg/Resource.hpp>

namespace Ame::Rg
{
    class ResourceSynchronizer
    {
        struct WaitState
        {
            Co::shared_result<void>  Result;
            Co::result_promise<void> Promise;
            WaitState()
            {
                Result = Promise.get_result();
            }

            void Signal()
            {
                if (Promise)
                {
                    Promise.set_result();
                }
            }
        };

        using WaitMap = std::map<ResourceId, WaitState>;

    public:
        ResourceSynchronizer(
            ResourceStorage& storage);

    public:
        [[nodiscard]] Co::shared_result<void> WaitFor(const ResourceId& id);
        void                                  Signal(const ResourceId& id);

    private:
        Ref<ResourceStorage> m_Storage;
        WaitMap              m_WaitMap;
    };
} // namespace Ame::Rg