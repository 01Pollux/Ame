#pragma once

#include <Graphics/EntityCompositor/Core.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Gfx
{
    struct EntityDrawCommandIterator
    {
        CRef<EntityDrawCommand> Command;
        uint32_t                CounterOffset;
    };

    class EntityDrawCommandListIterator
    {
    public:
        EntityDrawCommandListIterator(
            const EntityDrawCommandList& list,
            uint32_t                     commandOffset) :
            m_DrawCommandList(list),
            m_CommandOffset(commandOffset)
        {
        }

        [[nodiscard]] Co::generator<EntityDrawCommandIterator> GetCommands() const
        {
            uint32_t offset = m_CommandOffset;
            for (auto& command : m_DrawCommandList.get())
            {
                co_yield EntityDrawCommandIterator(command, offset++);
            }
        }

    private:
        CRef<EntityDrawCommandList> m_DrawCommandList;
        uint32_t                    m_CommandOffset;
    };

    class EntityDrawCommandsCategoryIterator
    {
    public:
        EntityDrawCommandsCategoryIterator(
            const EntityDrawCommandsCategory& categories) noexcept :
            m_DrawCategories(categories)
        {
        }

        [[nodiscard]] Co::generator<EntityDrawCommandListIterator> GetGroups() const
        {
            uint32_t commandOffset = 0;
            for (auto& list : m_DrawCategories.get())
            {
                co_yield EntityDrawCommandListIterator(list, commandOffset);
                commandOffset += static_cast<uint32_t>(list.size());
            }
        }

    private:
        CRef<EntityDrawCommandsCategory> m_DrawCategories;
    };
} // namespace Ame::Gfx