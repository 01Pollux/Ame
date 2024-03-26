#pragma once

#include <Engine/Engine.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    /// <summary>
    /// The entry point of the engine
    /// </summary>
    extern void Main(
        int    Argc,
        char** Argv);
} // namespace Ame

#define AME_MAIN_EXTERN(Argc, Argv)          \
    int main(                                \
        int    Argc,                         \
        char** Argv)                         \
    {                                        \
        Ame::Main(Argc, Argv);               \
        Ame::Log::Logger::CloseAllLoggers(); \
        return 0;                            \
    }

#define AME_MAIN(Argc, Argv)    \
    AME_MAIN_EXTERN(Argc, Argv) \
    void Ame::Main(             \
        int    Argc,            \
        char** Argv)
