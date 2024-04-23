#pragma once

#include <Frame/Frame.hpp>

namespace Ame
{
    /// <summary>
    /// Execute all function synchronously
    /// </summary>
    class SyncFrame final : public IFrame
    {
    public:
        using IFrame::IFrame;

    protected:
        void DoTick() override;
    };
} // namespace Ame