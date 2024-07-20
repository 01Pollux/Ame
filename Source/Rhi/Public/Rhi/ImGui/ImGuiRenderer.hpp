#pragma once

#include <Core/Interface.hpp>
#include <Core/String.hpp>
#include <Rhi/Core.hpp>

struct ImFont;
struct ImFontConfig;

namespace Ame::Rhi
{
    struct ImGuiRendererCreateDesc;

    // {9389313E-D479-45C0-A7E5-61D619EBE567}
    static const UId IID_ImGuiRenderer = { 0x9389313e, 0xd479, 0x45c0, { 0xa7, 0xe5, 0x61, 0xd6, 0x19, 0xeb, 0xe5, 0x67 } };
    // {DDFF902A-8F6B-4402-81AF-1BF94C7CE17A}
    static const UId IID_ImGuiNullRenderer = { 0xddff902a, 0x8f6b, 0x4402, { 0x81, 0xaf, 0x1b, 0xf9, 0x4c, 0x7c, 0xe1, 0x7a } };

    class IImGuiRenderer : public IObject
    {
    public:
        virtual void AME_METHOD(BeginFrame)() = 0;
        virtual void AME_METHOD(EndFrame)()   = 0;

        virtual void AME_METHOD(SubmitDrawData)() = 0;
        virtual void AME_METHOD(Reset)()          = 0;

        [[nodiscard]] virtual ImFont* AME_METHOD(FindFont)(const String& fontName) const                                   = 0;
        [[nodiscard]] virtual ImFont* AME_METHOD(LoadFont)(const String& fontName, const ImFontConfig& fontConfig)         = 0;
        [[nodiscard]] virtual ImFont* AME_METHOD(LoadCompressedFont)(const char* fontName, const ImFontConfig& fontConfig) = 0;
    };

    /// <summary>
    /// Creates an ImGui renderer, on release builds, this returns a dummy renderer.
    /// </summary>
    [[nodiscard]] Ptr<IImGuiRenderer> CreateImGuiRenderer(
        const ImGuiRendererCreateDesc& desc);

    [[nodiscard]] Ptr<IImGuiRenderer> CreateImGuiNullRenderer();
} // namespace Ame::Rhi