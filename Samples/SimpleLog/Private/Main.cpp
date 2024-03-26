
#include <Framework/EntryPoint.hpp>
#include <Framework/Headless.hpp>

AME_MAIN(Argc, Argv)
{
    HeadlessApplication<BaseEngine>::Builder()
        .SetName("Simple Log")
        .Build()
        .Close()
        .Run();
}
