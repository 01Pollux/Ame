#include <Framework/EntryPoint.hpp>
#include <Framework/Framework.hpp>

#include <Log/Wrapper.hpp>

AME_MAIN(Argc, Argv)
{
    namespace BF = Bootstrap::Features;

    auto application =
        Bootstrap::Application()
            .Feature<BF::Logger>(Log::Names::c_Engine, "Engine.log")
            .Feature<BF::Logger>(Log::Names::c_Client, "Client.log")
            .Feature<BF::Logger>(Log::Names::c_Client, "Ignore.log")
            .Feature<BF::Logger>(Log::Names::c_Asset, nullptr)
            .Build();

    Log::Client().Info("Hello, World!");
    Log::Editor().Warning("Ignored");

    application.Run();

    Log::Client().Error("Goodbye, World!");
}
