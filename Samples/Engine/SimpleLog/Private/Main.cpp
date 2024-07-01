#include <Framework/EntryPoint.hpp>
#include <Framework/Framework.hpp>

#include <Log/Wrapper.hpp>

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
    Log::Logger::Register(Log::Names::c_Client, "Client.log");
    Log::Logger::Register(Log::Names::c_Client, "Ignore.log");

    auto application =
        Bootstrap::Application()
            .Build();

    Log::Client().Info("Hello, World!");
    Log::Editor().Warning("Ignored");

    application.Run();

    Log::Client().Error("Goodbye, World!");
}
