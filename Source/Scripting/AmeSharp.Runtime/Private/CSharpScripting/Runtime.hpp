#pragma once

#include <CSharpScripting/Core.hpp>
#include <CSharpScripting/EngineConfig.hpp>

namespace Ame::Scripting
{
    class CLRRuntime
    {
    public:
        enum class Functions : uint8_t
        {
            AssemblyBridge_GetType,
            AssemblyBridge_GetTypes,

            TypeBridge_Free,
            TypeBridge_GetName,
            TypeBridge_GetBaseType,
            TypeBridge_CastAs,
            TypeBridge_GetSize,
            TypeBridge_CreateInstance,
            TypeBridge_GetMethod,
            TypeBridge_GetMethods,
            TypeBridge_GetAttribute,
            TypeBridge_GetAttributes,
            TypeBridge_GetProperty,
            TypeBridge_GetProperties,
            TypeBridge_GetField,
            TypeBridge_GetFields,

            MethodBridge_Free,
            MethodBridge_IsStatic,
            MethodBridge_GetName,
            MethodBridge_GetParamTypes,
            MethodBridge_GetReturnType,
            MethodBridge_Invoke,

            AttributeBridge_Free,
            AttributeBridge_GetValue,

            InstanceBridge_Free,
            InstanceBridge_InvokeMethod,
            InstanceBridge_GetField,
            InstanceBridge_SetField,
            InstanceBridge_GetProperty,
            InstanceBridge_SetProperty,

            PropertyBridge_Free,
            PropertyBridge_GetName,
            PropertyBridge_IsStatic,
            PropertyBridge_GetValue,
            PropertyBridge_SetValue,

            FieldBridge_Free,
            FieldBridge_GetName,
            FieldBridge_IsStatic,
            FieldBridge_GetValue,
            FieldBridge_SetValue,

            Count
        };

    public:
        CLRRuntime(const CSScriptEngineConfig& config);

    public:
        template<typename Ty> [[nodiscard]] Ty GetFunction(const String& className, const String& functionName) const
        {
            return std::bit_cast<Ty>(GetFunctionPtr(className, functionName));
        }
        template<typename Ty> [[nodiscard]] Ty GetCommonFunction(Functions type) const
        {
            return std::bit_cast<Ty>(GetCommonFunctionPtr(type));
        }

        [[nodiscard]] void* GetFunctionPtr(const String& className, const String& functionName) const;
        [[nodiscard]] void* GetCommonFunctionPtr(Functions type) const;

    private:
        void LoadHostFxrLibrary();
        void LoadHostFxrRuntime();
        void Initialize(const String& runtimePath);

    private:
        void RegisterCommonFunctions();
        void RegisterCommonFunction(Functions type, void* function);
        void RegisterCommonFunctions_AssemblyBridge();
        void RegisterCommonFunctions_TypeBridge();
        void RegisterCommonFunctions_MethodBridge();
        void RegisterCommonFunctions_AttributeBridge();
        void RegisterCommonFunctions_InstanceBridge();
        void RegisterCommonFunctions_PropertyBridge();
        void RegisterCommonFunctions_FieldBridge();

    private:
        DllLibrary    m_HostFxrLibrary;
        NetHostString m_RuntimePath;

        hostfxr_handle                            m_HostHandle  = nullptr;
        load_assembly_and_get_function_pointer_fn m_GetFunction = nullptr;

        void* m_CommonFunctions[static_cast<size_t>(Functions::Count)]{};
    };
} // namespace Ame::Scripting