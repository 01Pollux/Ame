#pragma once

#include <Core/Ame.hpp>
#include <Object/Signal.hpp>

namespace Ame::Signals
{
    AME_SIGNAL_DECL(OnStartFrame, void());

    AME_SIGNAL_DECL(OnUpdate, void());
    AME_SIGNAL_DECL(OnPostUpdate, void());

    AME_SIGNAL_DECL(OnRender, void());
    AME_SIGNAL_DECL(OnPostRender, void());

    AME_SIGNAL_DECL(OnEndFrame, void());
} // namespace Ame::Signals