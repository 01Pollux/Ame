#pragma once

#include <Frame/Frame.hpp>
#include <Core/Coroutine.hpp>

namespace Ame
{
    /// <summary>
    /// Execute all function assynchronously
    /// </summary>
    class AsyncFrame final : public IFrame
    {
    public:
        AsyncFrame(
            FrameTimer&  Timer,
            Co::runtime& Runtime);

    protected:
        void DoTick() override;

    private:
        Ref<Co::runtime> m_Runtime;
    };
} // namespace Ame