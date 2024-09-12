#pragma once

#include <Scripting/Library.hpp>
#include <CSharpScripting/Type.hpp>

namespace Ame::Scripting
{
    // {0046865B-F4FE-4F70-A1B0-D681153A7DC3}
    inline constexpr UId IID_CSLibrary{ 0x46865b, 0xf4fe, 0x4f70, { 0xa1, 0xb0, 0xd6, 0x81, 0x15, 0x3a, 0x7d, 0xc3 } };

    class CSLibrary : public BaseObject<ILibrary>
    {
    public:
        using Base = BaseObject<ILibrary>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSLibrary, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSLibrary) : Base(counters)
        {
        }

    public:
        auto GetContext() const -> ILibraryContext* override;
        void RegisterFunction(const String& functionName, void* callback) override;

        auto GetType(const String& name) -> IType* override;
        auto GetTypes() -> Co::generator<IType*> override;
    };
} // namespace Ame::Scripting
