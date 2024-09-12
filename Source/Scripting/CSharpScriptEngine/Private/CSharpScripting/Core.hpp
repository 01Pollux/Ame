#pragma once

#include <nethost.h>
#include <hostfxr.h>
#include <boost/dll.hpp>

namespace Ame::Scripting
{
    using DllLibrary = boost::dll::shared_library;

    struct CoreCLRFunctions
    {
        hostfxr_set_error_writer_fn              SetErrorWriter;
        hostfxr_initialize_for_runtime_config_fn InitializeRuntimeConfig;
        hostfxr_get_runtime_delegate_fn          GetRuntimeDelegate;
        hostfxr_close_fn                         Close;

        void Load(const DllLibrary& library)
        {
            SetErrorWriter = library.get<hostfxr_set_error_writer_fn>("hostfxr_set_error_writer");
            InitializeRuntimeConfig =
                library.get<hostfxr_initialize_for_runtime_config_fn>("hostfxr_initialize_for_runtime_config");
            GetRuntimeDelegate = library.get<hostfxr_get_runtime_delegate_fn>("hostfxr_get_runtime_delegate");
            Close              = library.get<hostfxr_close_fn>("hostfxr_close");
        }
    };
} // namespace Ame::CSharp