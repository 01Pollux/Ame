#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>

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

    class IType
    {
    public:
        virtual auto GetBaseType() const -> IType*     = 0;
        virtual auto CastAs(IType* type) const -> bool = 0;
        virtual auto GetSize() const -> size_t         = 0;

        virtual auto CreateInstance(const InstanceCreateDesc& createDesc) -> Ptr<IInstance> = 0;

        virtual auto GetField(const String& name) -> IField*       = 0;
        virtual auto GetMethod(const String& name) -> IMethod*     = 0;
        virtual auto GetProperty(const String& name) -> IProperty* = 0;
    };
} // namespace Ame::Scripting
