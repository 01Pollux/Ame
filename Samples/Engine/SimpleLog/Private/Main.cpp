#include <Framework/EntryPoint.hpp>
#include <Framework/Headless.hpp>

#include <Log/Wrapper.hpp>

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
    Log::Logger::Register(Log::Names::c_Client, "Client.log");
    Log::Logger::Register(Log::Names::c_Client, "Ignore.log");

    Log::Client().Info("Hello, World!");
    Log::Editor().Warning("Ignore");

    HeadlessApplication<BaseEngine>::Builder()
        .Build()
        .Close()
        .Run();

    Log::Client().Error("Goodbye, World!");
}
