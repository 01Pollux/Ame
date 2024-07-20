#pragma once

#include <Application/Application.hpp>

namespace Ame
{
    struct EditorApplicationConfig
    {
        ApplicationConfig Application;
    };

    class EditorApplication : public BaseApplication
    {
    public:
        using Base = BaseApplication;

    public:
        explicit EditorApplication(
            const EditorApplicationConfig& editorConfig);

    protected:
        void OnLoad() override;
        void OnInitialize() override;
        void OnShutdown() override;
        void OnUnload() override;
    };
} // namespace Ame
