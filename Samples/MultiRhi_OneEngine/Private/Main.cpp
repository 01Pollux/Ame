
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>
#include <Window/Window.hpp>

#include <Log/Wrapper.hpp>

using namespace Ame;

class MutliEngineSample : public BaseEngine
{
public:
    using BaseEngine::BaseEngine;

protected:
    void Initialize() override
    {
        BaseEngine::Initialize();
        Log::Engine().Trace("Initializing Sample...");

        CreateSecondaryDevice();

        m_Title = m_RhiDevice.GetWindow().GetTitle();

        auto TimerQueue  = m_Runtime->timer_queue();
        m_SecondaryTimer = TimerQueue->make_timer(
            std::chrono::seconds(1),
            std::chrono::seconds(1),
            m_Runtime->thread_pool_executor(),
            [this]()
            {
                if (m_SecondaryDevice.ProcessEvents())
                {
                    m_SecondaryDevice.BeginFrame();
                    m_SecondaryDevice.EndFrame();
                }
                else
                {
                    m_SecondaryTimer.cancel();
                    m_SecondaryDevice = {};
                }
            });
    }

private:
    void CreateSecondaryDevice()
    {
        Rhi::DeviceCreateDesc Desc{
            .Window = Rhi::WindowDesc{
                .Window{
                    .Title = "Secondary Window",
                    .Size  = { 800, 600 },
                } },
            .Type = Rhi::DeviceType::Vulkan,
        };
        Desc.SetFirstAdapter();

        m_SecondaryDevice = Rhi::Device(Desc);
    }

private:
    StringU8    m_Title;
    Co::timer   m_SecondaryTimer;
    Rhi::Device m_SecondaryDevice;
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::Engine, "Engine.log");
    Log::Logger::Register(Log::Names::Rhi, "Engine.log");

    auto Runtime(std::make_shared<Co::runtime>());

    WindowApplication<MutliEngineSample>::Builder()
        .Title("MultiRhi OneEngine")
        .Runtime(std::move(Runtime))
        .RendererBackend(Rhi::DeviceType::DirectX12)
        .Build()
        .Run();
}
