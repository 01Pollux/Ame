#include <Engine/Engine.hpp>

#include <Frame/Subsystem/Frame.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    void BaseEngine::Run()
    {
        EngineFrame& engineFrame = GetSubsystem<EngineFrameSubsystem>();

        DoInitialize();
        engineFrame.Run();
        DoShutdown();
    }

    void BaseEngine::Close()
    {
        EngineFrame& engineFrame = GetSubsystem<EngineFrameSubsystem>();
        engineFrame.Stop();
    }

    //

    void BaseEngine::DoInitialize()
    {
        Log::Engine().Trace("Initializing Engine...");
        Initialize();
        Log::Engine().Trace("Engine Initialized");
    }

    void BaseEngine::DoShutdown()
    {
        Log::Engine().Trace("Shutting down Engine...");
        Shutdown();
        Log::Engine().Trace("Engine Shutdown");
    }
} // namespace Ame
