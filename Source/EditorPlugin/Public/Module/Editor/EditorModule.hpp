#pragma once

#include <Module/Module.hpp>

#include <Module/Editor/Config.hpp>
#include <Module/Editor/EditorWindowSubmodule.hpp>

namespace Ame
{
    class EditorModule final : public BaseObject<IModule>
    {
    public:
        using Base = BaseObject<IModule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_EditorModule, IID_BaseModule, Base,
            m_EditorWindowSubmodule);

    public:
        EditorModule(
            IReferenceCounters*       counters,
            const EditorModuleConfig& config);

    private:
        Ptr<EditorWindowSubmodule> m_EditorWindowSubmodule;
    }; // namespace Ame
} // namespace Ame