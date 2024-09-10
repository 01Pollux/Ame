#pragma once

#include <Core/Coroutine.hpp>
#include <Log/Logger.hpp>

#include <Log/Streams/FileStream.hpp>
#include <Log/Streams/ConsoleStream.hpp>
#include <Log/Streams/MsvcDebugStream.hpp>

#if defined(AME_PLATFORM_WINDOWS) && defined(AME_DIST)

#define AME_MAIN_ENTRY_POINT                                                                                           \
    int WinMainWrapper(int argc, char** argv);                                                                         \
    int __stdcall wWinMain(void*, void*, const wchar_t*, int)                                                          \
    {                                                                                                                  \
        return WinMainWrapper(__argc, __argv);                                                                         \
    }                                                                                                                  \
    int WinMainWrapper([[maybe_unused]] int argc, [[maybe_unused]] char** argv)

#else
#define AME_MAIN_ENTRY_POINT int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
#endif

#define AME_MAIN_APPLICATION_CONFIG(ApplicationType, Config)                                                           \
    AME_MAIN_ENTRY_POINT                                                                                               \
    {                                                                                                                  \
        int ret = 0;                                                                                                   \
        Ame::Coroutine::Initialize();                                                                                  \
        {                                                                                                              \
            ApplicationType app(Config);                                                                               \
            ret = app.Run();                                                                                           \
        }                                                                                                              \
        Ame::Coroutine::Shutdown();                                                                                    \
        Ame::Log::s_Logger = {};                                                                                       \
        return ret;                                                                                                    \
    }

#define AME_MAIN_APPLICATION_DEFAULT(ApplicationType, LogPath)                                                         \
    AME_MAIN_APPLICATION_CONFIG(ApplicationType,                                                                       \
                                []()                                                                                   \
                                {                                                                                      \
                                    Ame::ApplicationConfig config;                                                     \
                                                                                                                       \
                                    auto& loggerStreams = config.Engine.CoreConfig.LoggerDesc.Streams;                 \
                                    loggerStreams.emplace_back(std::make_unique<Ame::Log::FileStream>(LogPath));       \
                                    loggerStreams.emplace_back(std::make_unique<Ame::Log::MsvcDebugStream>());         \
                                    loggerStreams.emplace_back(std::make_unique<Ame::Log::ConsoleStream>());           \
                                                                                                                       \
                                    return config;                                                                     \
                                }())

#define AME_MAIN_APPLICATION_NOLOG(ApplicationType) AME_MAIN_APPLICATION_CONFIG(ApplicationType, {})
