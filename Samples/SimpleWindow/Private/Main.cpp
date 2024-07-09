#include <Framework/EntryPoint.hpp>
#include <Framework/Framework.hpp>

#include <Subsystem/Core/EngineTick.hpp>
#include <Subsystem/Core/EngineFrame.hpp>
#include <Subsystem/Rhi/Device.hpp>

#include <Rhi/Device/CreateDesc.hpp>
#include <Window/Window.hpp>

using namespace Ame;

struct WindowProcessor : public IEngineTick
{
public:
    struct Subsystem;

public:
    WindowProcessor(
        EngineFrame&    engineFrame,
        Rhi::RhiDevice& rhiDevice) :
        m_EngineFrame(engineFrame),
        m_RhiDevice(rhiDevice)
    {
    }

protected:
    Co::result<void> Tick(
        Co::runtime& runtime)
    {
        bool isRunning = m_RhiDevice.get().ProcessTasks();
        m_RhiDevice.get().AdvanceFrame();
        if (!isRunning)
        {
            m_EngineFrame.get().Stop();
        }
        co_return;
    }

private:
    Ref<EngineFrame>    m_EngineFrame;
    Ref<Rhi::RhiDevice> m_RhiDevice;
};

struct WindowProcessor::Subsystem : public SingleSubsystem<
                                        WindowProcessor,
                                        Dependency<
                                            EngineFrameSubsystem,
                                            Rhi::RhiDeviceSubsystem>>,
                                    kgr::overrides<EngineTickSubsystem>
{
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
    Log::Logger::Register(Log::Names::c_Rhi, "Engine.log");
    Log::Logger::Register(Log::Names::c_Client, "Engine.log");
    Log::Logger::Register(Log::Names::c_Window, "Window.log");

    auto getDeviceCreateDesc = []() -> Rhi::DeviceCreateDesc
    {
        Windowing::WindowDesc windowDesc{
            .Title = "Simple Window"
        };

        return Rhi::DeviceCreateDesc{
            .Types{
                Rhi::DeviceCreateDescD3D12{},
                Rhi::DeviceCreateDescVulkan{},
                Rhi::DeviceCreateDescD3D11{},
                Rhi::DeviceCreateDescGL{} },
            .Surface = Rhi::RenderSurfaceDesc{
                std::make_shared<Windowing::Window>(windowDesc) }
        };
    };

    Bootstrap::Application()
        .With<Rhi::RhiDeviceSubsystem>(getDeviceCreateDesc())
        .With<WindowProcessor::Subsystem>()
        .Build()
        .Run()
        .wait();
}
