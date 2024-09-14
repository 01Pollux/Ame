#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>
#include <Scripting/Types/NativeString.hpp>

namespace Ame::Scripting
{
    class IInstance;
    class IField;
    class IMethod;
    class IProperty;

    struct InstanceCreateDesc
    {
        /// <summary>
        ///  Used if the constructor has multiple overloads.
        /// </summary>
        const char*  Constructor   = nullptr;
        const void** Arguments     = nullptr;
        size_t       ArgumentCount = 0;
    };

    class IType : public IObject
    {
    public:
        [[nodiscard]] virtual auto GetName() const -> NativeString   = 0;
        [[nodiscard]] virtual auto GetBaseType() const -> Ptr<IType> = 0;
        [[nodiscard]] virtual auto CastAs(IType* type) const -> bool = 0;
        [[nodiscard]] virtual auto GetSize() const -> size_t         = 0;

        [[nodiscard]] virtual auto CreateInstance(const InstanceCreateDesc& createDesc) -> Ptr<IInstance> = 0;

        [[nodiscard]] virtual auto GetField(const NativeString& name) -> IField*       = 0;
        [[nodiscard]] virtual auto GetMethod(const NativeString& name) -> IMethod*     = 0;
        [[nodiscard]] virtual auto GetProperty(const NativeString& name) -> IProperty* = 0;
    };
} // namespace Ame::Scripting
