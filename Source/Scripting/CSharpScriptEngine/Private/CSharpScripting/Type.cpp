#include <CSharpScripting/Type.hpp>
#include <CSharpScripting/Instance.hpp>

namespace Ame::Scripting
{
    IType* CSType::GetBaseType() const
    {
        return {};
    }

    bool CSType::CastAs(IType* type) const
    {
        (void)type;
        return {};
    }

    size_t CSType::GetSize() const
    {
        return {};
    }

    Ptr<IInstance> CSType::CreateInstance(const InstanceCreateDesc& createDesc)
    {
        (void)createDesc;
        return {};
    }

    IField* CSType::GetField(const NativeString& name)
    {
        (void)name;
        return {};
    }

    IMethod* CSType::GetMethod(const NativeString& name)
    {
        (void)name;
        return {};
    }

    IProperty* CSType::GetProperty(const NativeString& name)
    {
        (void)name;
        return {};
    }
} // namespace Ame::Scripting
