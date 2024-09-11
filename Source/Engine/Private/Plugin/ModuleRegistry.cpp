#include <Plugin/ModuleRegistryImpl.hpp>

namespace Ame
{
    UniquePtr<IModuleRegistry> CreateModuleRegistry()
    {
        return std::make_unique<ModuleRegistryImpl>();
    }
} // namespace Ame