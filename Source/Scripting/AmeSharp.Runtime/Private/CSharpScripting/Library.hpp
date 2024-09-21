#pragma once

#include <Scripting/Library.hpp>
#include <CSharpScripting/Type.hpp>
#include <CSharpScripting/Runtime.hpp>

namespace Ame::Scripting
{
    // {0046865B-F4FE-4F70-A1B0-D681153A7DC3}
    inline constexpr UId IID_CSLibrary{ 0x46865b, 0xf4fe, 0x4f70, { 0xa1, 0xb0, 0xd6, 0x81, 0x15, 0x3a, 0x7d, 0xc3 } };

    class CSLibraryContext;

    class CSLibrary : public ILibrary
    {
        using GetNameFn  = NativeString (*)(void* library);
        using GetTypeFn  = void* (*)(void* library, const NativeString& name);
        using GetTypesFn = NativeArray<void*> (*)(void* library);

    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSLibrary, ILibrary);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSLibrary, CSLibraryContext* context, void* library);

    public:
        auto GetName() const -> NativeString override;

        auto GetContext() const -> ILibraryContext* override;
        auto GetType(const NativeString& name) -> Ptr<IType> override;
        auto GetTypes() -> Co::generator<Ptr<IType>> override;

    public:
        [[nodiscard]] auto GetRuntime() const -> const CLRRuntime&;

    private:
        CSLibraryContext* m_Context;
        void*             m_Library = 0;
    };
} // namespace Ame::Scripting
