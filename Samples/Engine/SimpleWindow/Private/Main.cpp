
#include <Framework/EntryPoint.hpp>
#include <Framework/Framework.hpp>

#include <Subsystem/Rhi/Device.hpp>
#include <Subsystem/Gfx/Renderer.hpp>

#include <Rhi/Device/CreateDesc.hpp>

#include <Log/Wrapper.hpp>

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
    Log::Logger::Register(Log::Names::c_Rhi, "Engine.log");
    Log::Logger::Register(Log::Names::c_Client, "Engine.log");
    Log::Logger::Register(Log::Names::c_Window, "Window.log");

    Bootstrap::Application()
        .With<Rhi::DeviceSubsystem>(Rhi::DeviceCreateDesc{
            .Window = Rhi::WindowDesc{ .Window{ .Title = "Simple Window" } },
            .Type   = Rhi::DeviceType::DirectX12 })
        .With<Gfx::RendererSubsystem>()
        .Build()
        .Run()
        .wait();
}
