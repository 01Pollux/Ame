#pragma once

#include <Window/WindowFactory.hpp>

namespace Ame::Window
{
    AME_MT_OBJECT(WindowFactoryImpl)
    AME_INTERFACE_OF(IWindowFactory)
    {
    public:
        AME_BEGIN_INTERFACE_MAP(WindowFactoryImpl, CLSID_WindowFactory)
        AME_INTERFACE_ENTRY(IWindowFactory)
        AME_END_INTERFACE_MAP()

    public:
        Ptr<IWindow> CreateWindow(const WindowCreateDesc& createDesc) override;
    };

    AME_CLASS_REGISTER(WindowFactoryImpl);
} // namespace Ame::Window
