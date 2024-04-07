
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>
#include <Window/Window.hpp>

#include <Log/Wrapper.hpp>

static constexpr uint32_t NumberOfApps = 5;

using namespace Ame;

constexpr bool is_format()
{
    if constexpr (requires { std::formattable<std::string, char>; })
    {
        return true;
    }
    else
    {
        return false;
    }
}

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
                constexpr bool p = is_format();

                double FPS = 1.0 / m_Timer.GetDeltaTime();
                m_RhiDevice.GetWindow().SetTitle(StringU8::formatted("{} - FPS: {:.2f}", m_Title, FPS));
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
    Tasks.reserve(NumberOfApps);

    for (size_t i = 0; i < NumberOfApps; i++)
    {
        StringU8 Name = StringU8::formatted("Sample {}", i);
        Tasks.emplace_back(Executor->submit(
            [Runtime, Name]
            {
                WindowApplication<MutliEngineSample>::Builder()
                    .Title(Name.c_str())
                    .Runtime(std::move(Runtime))
                    .RendererBackend(Rhi::DeviceType::DirectX12)
                    .Build()
                    .Run();
            }));
    }

    Co::when_all(Executor, Tasks.begin(), Tasks.end()).run().get();
}
