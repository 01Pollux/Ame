#include <Engine/Logic.hpp>

namespace Ame
{
    void EngineLogic::Tick(
        EngineTimer& Timer,
        Rhi::Device* RhiDevice)
    {
    }

    void EngineLogic::Stop()
    {
        m_IsRunning = false;
    }

    //

    void EngineLogic::Initialize()
    {
    }

    void EngineLogic::Shutdown()
    {
    }
} // namespace Ame
