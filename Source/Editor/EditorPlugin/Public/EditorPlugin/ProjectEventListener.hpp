#pragma once

#include <Core/Interface.hpp>
#include <Core/Signal.hpp>

namespace Ame::Signals
{
    AME_SIGNAL_DECL(OnProjectSave, void());
    AME_SIGNAL_DECL(OnProjectReload, void());
    AME_SIGNAL_DECL(OnProjectClose, void());
} // namespace Ame::Signals

namespace Ame::Editor
{
    struct ProjectEventListener
    {
        Signals::OnProjectSave_Signal   OnSave;
        Signals::OnProjectReload_Signal OnReload;
        Signals::OnProjectClose_Signal  OnClose;
    };
} // namespace Ame::Editor
