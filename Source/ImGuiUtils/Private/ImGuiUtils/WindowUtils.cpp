#include <ImGuiUtils/Utils.hpp>
#include <imgui_internal.h>

namespace Ame::ImGuiUtils
{
    // From imgui.cpp
    void RenderWindowOuterBorders(ImGuiWindow* window)
    {
        // Data for resizing from borders
        struct ImGuiResizeBorderDef
        {
            ImVec2 InnerDir;
            ImVec2 SegmentN1, SegmentN2;
            float  OuterAngle;
        };

        constexpr ImGuiResizeBorderDef resizeBorderDef[4] = {
            { ImVec2(+1, 0), ImVec2(0, 1), ImVec2(0, 0), IM_PI * 1.00f }, // Left
            { ImVec2(-1, 0), ImVec2(1, 0), ImVec2(1, 1), IM_PI * 0.00f }, // Right
            { ImVec2(0, +1), ImVec2(0, 0), ImVec2(1, 0), IM_PI * 1.50f }, // Up
            { ImVec2(0, -1), ImVec2(1, 1), ImVec2(0, 1), IM_PI * 0.50f }  // Down
        };

        auto GetResizeBorderRect = [](ImGuiWindow* window, ImGuiDir borderN, float perpPadding,
                                      float thickness) -> ImRect
        {
            ImRect rect = window->Rect();
            if (thickness == 0.0f)
            {
                rect.Max -= ImVec2(1, 1);
            }
            switch (borderN)
            {
            case ImGuiDir_Left:
                return ImRect(
                    rect.Min.x - thickness, rect.Min.y + perpPadding, rect.Min.x + thickness, rect.Max.y - perpPadding);
            case ImGuiDir_Right:
                return ImRect(
                    rect.Max.x - thickness, rect.Min.y + perpPadding, rect.Max.x + thickness, rect.Max.y - perpPadding);
            case ImGuiDir_Up:
                return ImRect(
                    rect.Min.x + perpPadding, rect.Min.y - thickness, rect.Max.x - perpPadding, rect.Min.y + thickness);
            case ImGuiDir_Down:
                return ImRect(
                    rect.Min.x + perpPadding, rect.Max.y - thickness, rect.Max.x - perpPadding, rect.Max.y + thickness);
            default:
                std::unreachable();
            }
        };

        //

        float rounding   = window->WindowRounding;
        float borderSize = window->WindowBorderSize;
        if (borderSize > 0.0f && !(window->Flags & ImGuiWindowFlags_NoBackground))
            window->DrawList->AddRect(
                window->Pos, window->Pos + window->Size, ImGui::GetColorU32(ImGuiCol_Border), rounding, 0, borderSize);

        ImGuiDir borderHeld = static_cast<ImGuiDir>(window->ResizeBorderHeld);
        if (borderHeld != -1)
        {
            const auto& def     = resizeBorderDef[borderHeld];
            ImRect      borderR = GetResizeBorderRect(window, borderHeld, rounding, 0.0f);
            window->DrawList->PathArcTo(
                ImLerp(borderR.Min, borderR.Max, def.SegmentN1) + ImVec2(0.5f, 0.5f) + def.InnerDir * rounding,
                rounding, def.OuterAngle - IM_PI * 0.25f, def.OuterAngle);
            window->DrawList->PathArcTo(
                ImLerp(borderR.Min, borderR.Max, def.SegmentN2) + ImVec2(0.5f, 0.5f) + def.InnerDir * rounding,
                rounding, def.OuterAngle, def.OuterAngle + IM_PI * 0.25f);
            window->DrawList->PathStroke(
                ImGui::GetColorU32(ImGuiCol_SeparatorActive), 0, ImMax(2.0f, borderSize)); // Thicker than usual
        }

        float frameBorderSize = ImGui::GetStyle().FrameBorderSize;
        if (frameBorderSize > 0 && !(window->Flags & ImGuiWindowFlags_NoTitleBar) && !window->DockIsActive)
        {
            float Y = window->Pos.y + window->TitleBarHeight - 1.f;
            window->DrawList->AddLine(ImVec2(window->Pos.x + borderSize, Y),
                                      ImVec2(window->Pos.x + window->Size.x - borderSize, Y),
                                      ImGui::GetColorU32(ImGuiCol_Border), frameBorderSize);
        }
    }
} // namespace Ame::ImGuiUtils