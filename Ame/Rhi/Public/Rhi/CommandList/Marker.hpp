#pragma once

#include <Rhi/CommandList/CommandList.hpp>

namespace Ame::Rhi
{
    class CommandList;

    class CommandListMarker
    {
    public:
        CommandListMarker(
            CommandList& RhiCommandList,
            const char*  Tag) :
            m_CommandList(RhiCommandList)
        {
#ifndef AME_DIST
            m_CommandList.get().BeginMarker(Tag);
#endif
        }

        CommandListMarker(const CommandListMarker&)            = delete;
        CommandListMarker& operator=(const CommandListMarker&) = delete;

        CommandListMarker(CommandListMarker&&)            = delete;
        CommandListMarker& operator=(CommandListMarker&&) = delete;

        ~CommandListMarker()
        {
#ifndef AME_DIST
            m_CommandList.get().EndMarker();
#endif
        }

    private:
        Ref<CommandList> m_CommandList;
    };
} // namespace Ame::Rhi