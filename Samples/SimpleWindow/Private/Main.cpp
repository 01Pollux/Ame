
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Log/Wrapper.hpp>

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::Engine, "Engine.log");
    Log::Logger::Register(Log::Names::Rhi, "Rhi.log");

    WindowApplication<BaseEngine>::Builder()
        .Title("Simple Window")
        .RendererBackend(Rhi::DeviceType::DirectX12)
        .Build()
        .Run();
}
