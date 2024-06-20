#pragma once

#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/Resource/Fence.hpp>

namespace Ame::Rhi
{
    struct SubmissionContext
    {
        CommandList      CommandListRef;
        Fence            FenceRef;
        CommandQueueType QueueType;

        auto operator->() noexcept
		{
			return &CommandListRef;
		}
    };
} // namespace Ame::Rhi