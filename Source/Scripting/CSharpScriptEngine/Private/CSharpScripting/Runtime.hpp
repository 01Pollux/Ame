#pragma once

#include <CSharpScripting/Core.hpp>
#include <CSharpScripting/EngineConfig.hpp>

namespace Ame::Scripting
{
    class CLRRuntime
    {
    public:
        CLRRuntime(const CSScriptEngineConfig& config);

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