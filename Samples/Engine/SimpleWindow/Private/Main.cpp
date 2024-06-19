
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Log/Wrapper.hpp>

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
    Log::Logger::Register(Log::Names::c_Rhi, "Engine.log");

    WindowApplication<BaseEngine>::Builder()
        .Title("Simple Window")
        .Build()
        .Run();
}
