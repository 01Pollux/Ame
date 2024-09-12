#include <CSharpScripting/Library.hpp>

namespace Ame::Scripting
{
    ILibraryContext* CSLibrary::GetContext() const
    {
        return nullptr;
    }
    void CSLibrary::RegisterFunction(const String& functionName, void* callback)
    {
        (void)functionName;
        (void)callback;
    }
    IType* CSLibrary::GetType(const String& name)
    {
        (void)name;
        return {};
    }
    Co::generator<IType*> CSLibrary::GetTypes()
    {
        co_return;
    }
} // namespace Ame::Scripting