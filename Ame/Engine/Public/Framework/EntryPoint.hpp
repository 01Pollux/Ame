#pragma once

#include <Engine/Engine.hpp>

#include <Log/Logger.hpp>

namespace Ame::Framework
{
    /// <summary>
    /// The entry point of the engine
    /// </summary>
    extern void Main(
        int    Argc,
        char** Argv);
} // namespace Ame::Framework

#define AME_MAIN_EXTERN(Argc, Argv)          \
    int main(                                \
        int    Argc,                         \
        char** Argv)                         \
    {                                        \
        Ame::Framework::Main(Argc, Argv);    \
        Ame::Log::Logger::CloseAllLoggers(); \
        Ame::Rhi::Device::CleanupCheck();    \
        return 0;                            \
    }

#define AME_MAIN(Argc, Argv)    \
    AME_MAIN_EXTERN(Argc, Argv) \
    void Ame::Framework::Main(  \
        int    Argc,            \
        char** Argv)
