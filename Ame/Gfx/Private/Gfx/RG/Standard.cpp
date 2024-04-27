#include <Gfx/RG/PassStorage.hpp>

namespace Ame::Gfx::RG
{
    void PassStorage::SetStandard(
        StdGraph Type)
    {
        Clear();
        switch (Type)
        {
        case StdGraph::Null:
        {
            return;
        }
        case StdGraph::DeferredPlus:
        {

            break;
        }
        }
    }
} // namespace Ame::Gfx::RG