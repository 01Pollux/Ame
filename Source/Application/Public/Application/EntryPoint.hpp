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

#define AME_MAIN_ENTRY_POINT \
    int main(                \
        int    argc,         \
        char** argv)

#endif

#define AME_MAIN_APPLICATION(ApplicationType) \
    AME_MAIN_ENTRY_POINT                      \
    {                                         \
        ApplicationType app(argc, argv);      \
        return app.Run();                     \
    }
