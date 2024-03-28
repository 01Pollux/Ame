
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Log/Wrapper.hpp>

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::Engine, "Engine.log");
    Log::Logger::Register(Log::Names::Client, "Client.log");
    Log::Logger::Register(Log::Names::Client, "Ignore.log");

    Log::Client().Info("Hello, World!");
    Log::Editor().Warning("Ignore");
    Log::Client().Error("Goodbye, World!");

    auto App = WindowApplication<BaseEngine>::Builder()
        .Build();

    //WindowApplication<BaseEngine>::Builder()
    //    .Name("Simple Window")
    //    .Build()
    //    .Close()
    //    .Run();
}
