#pragma once

#include <Core/Ame.hpp>
#include <Object/Signal.hpp>

namespace Ame
{
    class BaseEngine;

    namespace Rhi
    {
        class Device;
    } // namespace Rhi
} // namespace Ame

AME_SIGNAL_DECL(Ame::BaseEngine, OnStartFrame);

AME_SIGNAL_DECL(Ame::BaseEngine, OnUpdate);
AME_SIGNAL_DECL(Ame::BaseEngine, OnPostUpdate);

AME_SIGNAL_DECL(Ame::BaseEngine, OnRender);
AME_SIGNAL_DECL(Ame::BaseEngine, OnPostRender);

AME_SIGNAL_DECL(Ame::BaseEngine, OnEndFrame);
