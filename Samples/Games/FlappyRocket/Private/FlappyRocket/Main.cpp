#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <FlappyRocket/Engine.hpp>

AME_MAIN(Argc, Argv)
{
    using EngineBuilder = Framework::WindowApplication<FlappyRocket::FlappyRocketEngine>::Builder;
    EngineBuilder()
        .Title("Flappy Rocket")
        .Build()
        .Run();
}
