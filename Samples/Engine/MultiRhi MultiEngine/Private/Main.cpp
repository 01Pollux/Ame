
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

        m_Title = m_RhiDevice.GetWindow().GetTitle();

        auto TimerQueue = m_Runtime->timer_queue();
        m_TitleTimer    = TimerQueue->make_timer(
            std::chrono::seconds(1),
            std::chrono::seconds(1),
            m_Runtime->thread_pool_executor(),
            [this]()
            {
                double   FPS   = 1.0 / m_Timer.GetDeltaTime();
                StringU8 Title = StringU8::formatted("{} - FPS: {:.2f}", m_Title, FPS);
                m_RhiDevice.GetWindow().SetTitle(Title);
            });
    }

private:
    StringU8  m_Title;
    Co::timer m_TitleTimer;
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::Engine, "Engine.log");
    Log::Logger::Register(Log::Names::Rhi, "Engine.log");

    auto Runtime(std::make_shared<Co::runtime>());
    auto Executor = Runtime->thread_pool_executor();

    std::vector<Co::result<void>> Tasks;

    for (auto [Backend, Name] : {
             std::pair{ Rhi::DeviceType ::DirectX12, "DirectX 12" },
             std::pair{ Rhi::DeviceType::Vulkan, "Vulkan" },
         })
    {
        Tasks.emplace_back(Executor->submit(
            [Runtime, Backend, Name]
            {
                WindowApplication<MutliEngineSample>::Builder()
                    .Title(Name)
                    .Runtime(std::move(Runtime))
                    .RendererBackend(Backend)
                    .Build()
                    .Run();
            }));
    }

    Co::when_all(Executor, Tasks.begin(), Tasks.end()).run().get();
}
