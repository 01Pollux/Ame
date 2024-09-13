#pragma once

#include <CSharpScripting/Core.hpp>
#include <CSharpScripting/EngineConfig.hpp>

namespace Ame::Scripting
{
    class CLRRuntime
    {
    public:
        CLRRuntime(const CSScriptEngineConfig& config);

    public:
        template<typename Ty> [[nodiscard]] Ty GetFunction(const String& className, const String& functionName) const
        {
            return std::bit_cast<Ty>(GetFunctionPtr(className, functionName));
        }

        [[nodiscard]] void* GetFunctionPtr(const String& className, const String& functionName) const;

    private:
        void LoadHostFxrLibrary();
        void LoadHostFxrRuntime();
        void Initialize(const String& runtimePath);

    private:
        DllLibrary    m_HostFxrLibrary;
        NetHostString m_RuntimePath;

        hostfxr_handle                            m_HostHandle  = nullptr;
        load_assembly_and_get_function_pointer_fn m_GetFunction = nullptr;
    };
} // namespace Ame::Scripting