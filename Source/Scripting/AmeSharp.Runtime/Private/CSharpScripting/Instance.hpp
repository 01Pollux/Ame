#pragma once

#include <Scripting/Instance.hpp>
#include <CSharpScripting/Runtime.hpp>

namespace Ame::Scripting
{
    // {F53516D8-4B6B-4AEA-AADF-7892298F830A}
    inline constexpr UId IID_CSInstance{
        0xf53516d8, 0x4b6b, 0x4aea, { 0xaa, 0xdf, 0x78, 0x92, 0x29, 0x8f, 0x83, 0xa }
    };

    class IType;

    class CSInstance : public BaseObject<IInstance>
    {
    public:
        using Base = BaseObject<IInstance>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSInstance, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSInstance, const CLRRuntime& runtime, void* instance);

    public:
        ~CSInstance() override;

    public:
        auto GetType() const -> IType*
        {
            return {};
        }
        void InvokeMethod(const char* methodName, const void* arguments, void* returnPtr)
        {
            (void)methodName;
            (void)arguments;
            (void)returnPtr;
        }
        void GetField(const char* fieldName, void* valuePtr)
        {
            (void)fieldName;
            (void)valuePtr;
        }
        void SetField(const char* fieldName, const void* valuePtr)
        {
            fieldName;
            (void)valuePtr;
        }
        void GetProperty(const char* propertyName, void* valuePtr)
        {
            (void)propertyName;
            (void)valuePtr;
        }
        void SetProperty(const char* propertyName, const void* valuePtr)
        {
            (void)propertyName;
            (void)valuePtr;
        }

    private:
        const CLRRuntime* m_Runtime  = nullptr;
        void*             m_Instance = nullptr;
    };
} // namespace Ame::Scripting
