#pragma once

#include <Graphics/EntityCompositor/Instance.hpp>

namespace Ame::Gfx
{
    class EntityDispatcher
    {
    private:
        using EntityDrawCommandList      = std::vector<EntityDrawCommand>;
        using EntityDrawCommandsCategory = std::array<EntityDrawCommandList, static_cast<size_t>(DrawInstanceType::Count)>;

    public:
    private:
    };
} // namespace Ame::Gfx