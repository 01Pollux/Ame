
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Log/Wrapper.hpp>

using namespace Ame;

class TriangleSampleEngine : public BaseEngine
{
protected:
    void Initialize() override
    {
        BaseEngine::Initialize();
    }

private:
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::Engine, "Engine.log");
    Log::Logger::Register(Log::Names::Rhi, "Engine.log");

    WindowApplication<TriangleSampleEngine>::Builder()
        .Title("Simple Window")
        .Build()
        .Run();
}
