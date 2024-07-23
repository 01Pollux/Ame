#pragma once

#include <Module/Submodule.hpp>

#include <EditorPlugin/EditorWindowManager.hpp>

namespace Ame
{
    // {BE08421C-CE71-4D12-8CDF-7BE552A8C033}
    static const UId IID_EditorWindowSubmodule = { 0xbe08421c, 0xce71, 0x4d12, { 0x8c, 0xdf, 0x7b, 0xe5, 0x52, 0xa8, 0xc0, 0x33 } };

    class EditorWindowSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_EditorWindowSubmodule, IID_BaseSubmodule, Base,
            m_EditorWindowManager);

    public:
        EditorWindowSubmodule(
            IReferenceCounters*                          counters,
            const Editor::EditorWindowManagerCreateDesc& editorManagerCreateDesc);

    private:
        Ptr<Editor::IEditorWindowManager> m_EditorWindowManager;
    };
} // namespace Ame