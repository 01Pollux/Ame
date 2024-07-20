#pragma once

#include <Application/Application.hpp>

namespace Ame
{
    class EditorApplication : public BareApplication
    {
    public:
        using Base = BareApplication;

    public:
        EditorApplication(int argc, char** argv) :
            Base(argc, argv)
        {
        }

    protected:
        void OnLoad() override;
        void OnInitialize() override;
        void OnShutdown() override;
        void OnUnload() override;
    };
} // namespace Ame
