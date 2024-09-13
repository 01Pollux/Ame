#include <filesystem>
#include <CSharpScripting/Runtime.hpp>

#include <Log/Logger.hpp>

struct NativeString
{
    const char* Data;
    size_t      Length;
};

extern "C" __declspec(dllexport) void __cdecl TestingThis()
{
    printf("Test\n");
}

extern "C" __declspec(dllexport) void __cdecl AmeCSharp_LogMessage(const NativeString& str, char lvl)
{
    printf("%s\n", str.Data);
    printf("Level: %c\n", lvl);
}

extern "C" __declspec(dllexport) void __cdecl Ame_BaseObject_QueryInterface(void* thisPtr, const Ame::UId& iid,
                                                                            void** ppObject)
{
    using namespace Ame;
    printf("QueryInterface\n");

    auto baseObject = static_cast<IObject*>(thisPtr);
    baseObject->QueryInterface(iid, std::bit_cast<IObject**>(ppObject));
}

extern "C" __declspec(dllexport) void __cdecl Ame_BaseObject_AddRef(void* thisPtr)
{
    using namespace Ame;
    printf("AddRef\n");
    auto baseObject = static_cast<IObject*>(thisPtr);
    baseObject->AddRef();
}

extern "C" __declspec(dllexport) void __cdecl Ame_BaseObject_Release(void* thisPtr)
{
    using namespace Ame;
    printf("Release\n");
    auto baseObject = static_cast<IObject*>(thisPtr);
    baseObject->Release();
}

namespace Ame::Scripting
{
    CLRRuntime::CLRRuntime(const CSScriptEngineConfig& config) :
        m_RuntimePath(Strings::To<NetHostString>(config.GetRuntimePath()))
    {
        auto runtimePath = config.GetRuntimeConfigPath();
        if (!std::filesystem::exists(runtimePath))
        {
            throw std::runtime_error("Runtime config not found");
        }

        LoadHostFxrLibrary();
        LoadHostFxrRuntime();
        Initialize(runtimePath);
    }

    //

    void* CLRRuntime::GetFunctionPtr(const String& className, const String& functionName) const
    {
        void* function = nullptr;
        m_GetFunction(m_RuntimePath.c_str(), Strings::To<NetHostString>(className).c_str(),
                      Strings::To<NetHostString>(functionName).c_str(), NETHOST_UNMANAGED_CALLER_DELEGATE, nullptr,
                      &function);
        AME_LOG_ASSERT(function != nullptr, "Failed to get function pointer");
        return function;
    }

    void CLRRuntime::LoadHostFxrLibrary()
    {
        size_t bufferSize = 0;
        get_hostfxr_path(nullptr, &bufferSize, nullptr);
        std::basic_string<char_t> hostFxrPath(bufferSize, '\0');
        if (get_hostfxr_path(hostFxrPath.data(), &bufferSize, nullptr))
        {
            throw std::runtime_error("Failed to locate hostfxr");
        }

        AME_LOG_TRACE(std::format("Found hostfxr at: {}", Strings::To<String>(hostFxrPath)));
        m_HostFxrLibrary = DllLibrary(hostFxrPath.c_str());
    }

    void CLRRuntime::LoadHostFxrRuntime()
    {
        auto setErrorWriter =
            m_HostFxrLibrary.get<std::remove_pointer_t<hostfxr_set_error_writer_fn>>("hostfxr_set_error_writer");
        setErrorWriter([](const char_t* message) { AME_LOG_ERROR(Strings::To<String>(message)); });
    }

    void CLRRuntime::Initialize(const String& runtimePath)
    {
        auto initializeRuntime = m_HostFxrLibrary.get<std::remove_pointer_t<hostfxr_initialize_for_runtime_config_fn>>(
            "hostfxr_initialize_for_runtime_config");

        auto getRuntimeDelegate = m_HostFxrLibrary.get<std::remove_pointer_t<hostfxr_get_runtime_delegate_fn>>(
            "hostfxr_get_runtime_delegate");

        auto status = static_cast<HostFxError>(
            initializeRuntime(Strings::To<NetHostString>(runtimePath).c_str(), nullptr, &m_HostHandle));
        AME_LOG_VALIDATE(status == HostFxError::Success || status == HostFxError::Success_HostAlreadyInitialized,
                         "Failed to initialize runtime config");
        AME_LOG_VALIDATE(m_HostHandle != nullptr, "Host handle is null");

        getRuntimeDelegate(
            m_HostHandle, hdt_load_assembly_and_get_function_pointer, std::bit_cast<void**>(&m_GetFunction));
    }
} // namespace Ame::Scripting