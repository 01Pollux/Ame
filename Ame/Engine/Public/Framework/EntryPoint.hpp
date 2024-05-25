#pragma once

#include <Framework/Framework.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Framework
{
    /// <summary>
    /// The entry point of the engine
    /// </summary>
    extern void Main(
        int    argc,
        char** argv);
} // namespace Ame::Framework

#define AME_MAIN_EXTERN(Argc, Argv)          \
    int main(                                \
        int    argc,                         \
        char** argv)                         \
    {                                        \
        Ame::Framework::Main(argc, argv);    \
        Ame::Log::Logger::CloseAllLoggers(); \
        Ame::Rhi::Device::CleanupCheck();    \
        return 0;                            \
    }

#define AME_MAIN(argc, argv)    \
    AME_MAIN_EXTERN(argc, argv) \
    void Ame::Framework::Main(  \
        int    argc,            \
        char** argv)
