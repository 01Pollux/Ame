#pragma once

#include <Core/Coroutine.hpp>
#include <Graphics/EntityCompositor/Core.hpp>
#include <Rhi/Utils/IndirectCommandStructs.hpp>

namespace Ame::Gfx
{
    struct EntityDrawRowIterator
    {
        CRef<EntityDrawCommand> Command;
        uint32_t                InstanceOffset;
    };

    class EntityDrawCommandListIterator
    {
    public:
        EntityDrawCommandListIterator(const EntityDrawCommandList& list, DrawInstanceType instancesType,
                                      uint32_t commandOffset) :
            m_DrawCommandList(list),
            m_CommandOffset(commandOffset), m_InstancesType(instancesType)
        {
        }

        [[nodiscard]] Co::generator<EntityDrawRowIterator> GetRows() const
        {
            uint32_t offset = m_CommandOffset;
            for (auto& command : m_DrawCommandList.get())
            {
                co_yield EntityDrawRowIterator(command, offset);
                offset++;
            }
        }

        [[nodiscard]] DrawInstanceType GetType() const noexcept
        {
            return m_InstancesType;
        }

    private:
        CRef<EntityDrawCommandList> m_DrawCommandList;
        uint32_t                    m_CommandOffset;
        DrawInstanceType            m_InstancesType;
    };

    class EntityDrawCommandsCategoryIterator
    {
    public:
        EntityDrawCommandsCategoryIterator(const EntityDrawCommandsCategory& categories) noexcept :
            m_DrawCategories(categories)
        {
        }

        [[nodiscard]] Co::generator<EntityDrawCommandListIterator> GetGroups() const
        {
            uint32_t commandOffset = 0;

            int i = 0;
            for (auto& list : m_DrawCategories.get())
            {
                co_yield EntityDrawCommandListIterator(list, static_cast<DrawInstanceType>(i), commandOffset);
                commandOffset += static_cast<uint32_t>(list.size());
            }
        }

    private:
        CRef<EntityDrawCommandsCategory> m_DrawCategories;
    };
} // namespace Ame::Gfx