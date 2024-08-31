#pragma once

#include <Core/Signal.hpp>
#include <Math/Colors.hpp>

namespace Ame::Signals
{
    AME_SIGNAL_DECL(OnRenderBegin, void());
    AME_SIGNAL_DECL(OnRenderUpdate, void());
    AME_SIGNAL_DECL(OnRenderPostUpdate, void());
    AME_SIGNAL_DECL(OnRenderEnd, void());

    AME_SIGNAL_DECL(OnImGuiRender, void());
    AME_SIGNAL_DECL(OnImGuiPostRender, void());
} // namespace Ame::Signals
