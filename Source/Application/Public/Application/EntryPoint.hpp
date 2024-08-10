#pragma once

#if defined(NEON_PLATFORM_WINDOWS) && defined(NEON_DIST)

#define AME_MAIN_ENTRY_POINT                   \
    int WinMainWrapper(int argc, char** argv); \
    int __stdcall wWinMain(                    \
        void*,                                 \
        void*,                                 \
        const wchar_t*,                        \
        int)                                   \
    {                                          \
        return WinMainWrapper(__argc, __argv); \
    }                                          \
    int WinMainWrapper(int argc, char** argv)

#else

#ifdef AME_DEBUG

#ifdef AME_PLATFORM_WINDOWS
#include <malloc.h>
#include <crtdbg.h>
#endif

static void EnableDebugInfo()
{
#ifdef AME_PLATFORM_WINDOWS
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetBreakAlloc(9554);
    _CrtSetBreakAlloc(9553);
    _CrtSetBreakAlloc(9552);
#endif
}

#define AME_MAIN_ENTRY_POINT          \
    int _dbg_main(                    \
        int    argc,                  \
        char** argv);                 \
    int main(                         \
        int    argc,                  \
        char** argv)                  \
    {                                 \
        EnableDebugInfo();            \
        return _dbg_main(argc, argv); \
    }                                 \
    int _dbg_main(                    \
        int    argc,                  \
        char** argv)
#else
#define AME_MAIN_ENTRY_POINT \
    int main(                \
        int    argc,         \
        char** argv)
#endif
#endif

#define AME_MAIN_APPLICATION_CONFIG(ApplicationType, Config) \
    AME_MAIN_ENTRY_POINT                                     \
    {                                                        \
        ApplicationType app(Config);                         \
        return app.Run();                                    \
    }

#define AME_MAIN_APPLICATION(ApplicationType) \
    AME_MAIN_ENTRY_POINT                      \
    {                                         \
        ApplicationType app({});              \
        return app.Run();                     \
    }
