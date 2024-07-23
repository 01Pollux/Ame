#pragma once

#include <Application/Application.hpp>
#include <Editor/Config.hpp>

namespace Ame
{
    class EditorApplication : public BaseApplication
    {
    public:
        using Base = BaseApplication;

    public:
        explicit EditorApplication(
            const EditorApplicationConfig& config);

    protected:
        void OnLoad() override;
        void OnInitialize() override;
        void OnShutdown() override;
        void OnUnload() override;
    };
} // namespace Ame
