#include <ImGuiUtils/Utils.hpp>

namespace Ame::ImGuiUtils
{
    void DrawTextBG(
        const char* text,
        const char* textEnd)
    {
        auto   drawList  = ImGui::GetWindowDrawList();
        ImVec2 pos       = ImGui::GetCursorScreenPos();
        ImVec2 size      = ImGui::CalcTextSize(text, textEnd);
        float  frameSize = ImGui::GetStyle().FramePadding.y * 2.f;
        ImVec2 endPos    = { pos.x + size.x + frameSize, pos.y + size.y + frameSize };

        drawList->AddRectFilled(pos, endPos, ImGui::GetColorU32(ImGuiCol_FrameBg));
        drawList->AddText({ pos.x + frameSize / 2.f, pos.y + frameSize / 2.f }, ImGui::GetColorU32(ImGuiCol_Text), text, textEnd);

        ImGui::Dummy(size);
    }

    void IncrementCursorPosX(
        float pos)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + pos);
    }

    void IncrementCursorPosY(
        float pos)
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + pos);
    }

    void IncrementCursorPos(
        const ImVec2& pos)
    {
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + pos.x, ImGui::GetCursorPosY() + pos.y));
    }

    void TableAlignCenter(
        float width)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - width / 2.0f);
    }

    void CursorMiddleScreen(
        const char* label)
    {
        float textW = ImGui::CalcTextSize(label).x;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - textW / 2.0f);
        ImGui::Text(label);
    }

    void CenterCursorX()
    {
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f);
    }

    void CenterCursorY()
    {
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2.0f);
    }

    void CenterCursor(
        bool   region,
        ImVec2 offset,
        bool   centerX,
        bool   centerY)
    {
        ImVec2 middle    = (region ? ImGui::GetContentRegionAvail() : ImGui::GetWindowSize()) / 2.f;
        ImVec2 cursorPos = ImGui::GetCursorPos() + offset;
        if (centerX)
        {
            cursorPos.x += middle.x;
        }
        if (centerY)
        {
            cursorPos.y += middle.y;
        }
        ImGui::SetCursorPos(cursorPos);
    }

    void CenterText(
        const char* text,
        bool        region,
        bool        centerX,
        bool        centerY)
    {
        ImVec2 textSize = ImGui::CalcTextSize(text);
        CenterCursor(region, -textSize / 2.f, centerX, centerY);
        ImGui::Text(text);
    }

    //

    float PushFontScale(
        float scale)
    {
        ImFont* Font = ImGui::GetFont();
        return std::exchange(Font->Scale, scale);
    }

    float PushFontScaleMul(
        float scale)
    {
        ImFont* Font = ImGui::GetFont();
        return std::exchange(Font->Scale, Font->Scale * scale);
    }

    float PushFontScaleAccumulate(
        float scale)
    {
        ImFont* Font = ImGui::GetFont();
        return std::exchange(Font->Scale, Font->Scale + scale);
    }

    void PopFontScale(
        float oldScale)
    {
        ImFont* Font = ImGui::GetFont();
        std::exchange(Font->Scale, oldScale);
    }

    //

    void DrawLabel(
        const String&  label,
        const ImColor& color,
        const ImVec2&  maxSize)
    {
        auto drawList = ImGui::GetWindowDrawList();
        auto pos      = ImGui::GetCursorScreenPos();
        auto size     = ImGui::CalcTextSize(label.c_str(), label.c_str() + label.size());
        auto endPos   = pos + size;

        if (maxSize.x != -FLT_MIN && size.x > maxSize.x)
        {
            size.x   = maxSize.x;
            endPos.x = pos.x + size.x;
        }
        if (maxSize.y != -FLT_MIN && size.y > maxSize.y)
        {
            size.y   = maxSize.y;
            endPos.y = pos.y + size.y;
        }

        drawList->AddRectFilled(pos, endPos, color);
        drawList->AddText(pos, ImGui::GetColorU32(ImGuiCol_Text), label.c_str(), label.c_str() + label.size());
        ImGui::Dummy(size);
    }

    void DrawLabel(
        const char*    label,
        const ImColor& color,
        const ImVec2&  maxSize)
    {
        auto drawList = ImGui::GetWindowDrawList();
        auto pos      = ImGui::GetCursorScreenPos();
        auto size     = ImGui::CalcTextSize(label);
        auto endPos   = pos + size;

        if (maxSize.x != -FLT_MIN && size.x > maxSize.x)
        {
            size.x   = maxSize.x;
            endPos.x = pos.x + size.x;
        }
        if (maxSize.y != -FLT_MIN && size.y > maxSize.y)
        {
            size.y   = maxSize.y;
            endPos.y = pos.y + size.y;
        }

        drawList->AddRectFilled(pos, endPos, color);
        drawList->AddText(pos, ImGui::GetColorU32(ImGuiCol_Text), label);
        ImGui::Dummy(size);
    }

    //

    void DrawComponentLabel(
        const char* label,
        bool        sameLine,
        float       propertyNameWidth)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);

        if (sameLine)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowSize().x * propertyNameWidth);
        }
    }

    bool DrawColorPicker(
        const char*         name,
        Math::Color4&       color,
        ImGuiColorEditFlags flags)
    {
        imcxx::shared_item_id id(name);

        ImVec4 imColor{ color.r(), color.g(), color.b(), color.a() };
        if (imcxx::button{ imcxx::button::color{}, "", imColor, flags, ImVec2{ ImGui::GetContentRegionAvail().x, 0.f } })
        {
            ImGui::OpenPopup("ColorPalette");
        }
        if (imcxx::popup ColorPopup{ "ColorPalette" })
        {
            return ImGui::ColorPicker4("", color.data(), flags);
        }
        return false;
    }

    bool DrawColorPicker(
        const char*         name,
        Math::Color3&       color,
        ImGuiColorEditFlags flags)
    {
        Math::Color4 tmpColor{ color };
        if (DrawColorPicker(name, tmpColor, flags | ImGuiColorEditFlags_NoAlpha))
        {
            color = tmpColor;
            return true;
        }
        return false;
    }

    //

    void DrawIcon(
        ImDrawList*   drawList,
        const ImVec2& size,
        BasicIconType type,
        bool          filled,
        ImU32         color,
        ImU32         fillColor)
    {
        if (!ImGui::IsRectVisible(size))
        {
            ImGui::Dummy(size);
            return;
        }

        auto rectMin    = ImGui::GetCursorScreenPos();
        auto rectMax    = rectMin + size;
        auto rectCenter = (rectMax + rectMin) * 0.5f;

        float outlineScale  = size.x / 24.0f;
        int   extraSegments = static_cast<int>(2 * outlineScale); // for full circle

        if (type == BasicIconType::Flow)
        {
            auto offset = -static_cast<int>(size.x * 0.25f * 0.25f);

            rectMin.x += offset;
            rectMax.x += offset;
            rectCenter.x += offset * 0.5f;
        }

        switch (type)
        {
        case BasicIconType::Flow:
        {
            float margin   = (filled ? 2.0f : 2.0f) * outlineScale;
            float rounding = 0.1f * outlineScale;
            float tipRound = 0.7f; // percentage of triangle edge (for tip)

            ImVec2 canvasMin(rectMin.x + margin + outlineScale, rectMin.y + margin);
            ImVec2 canvasMax(rectMax.x - margin + outlineScale, rectMax.y - margin);

            float  canvasX    = canvasMin.x;
            float  canvasY    = canvasMin.y;
            ImVec2 canvasSize = canvasMax - canvasMin;

            float left     = canvasX + canvasSize.x * 0.5f * 0.3f;
            float right    = canvasX + canvasSize.x - canvasSize.x * 0.5f * 0.3f;
            float top      = canvasY + canvasSize.y * 0.5f * 0.2f;
            float bottom   = canvasY + canvasSize.y - canvasSize.y * 0.5f * 0.2f;
            float center_y = (top + bottom) * 0.5f;

            ImVec2 tip_top(canvasX + canvasSize.x * 0.5f, top);
            ImVec2 tip_right(right, center_y);
            ImVec2 tip_bottom(canvasX + canvasSize.x * 0.5f, bottom);

            drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
            drawList->PathBezierCubicCurveTo(
                ImVec2(left, top),
                ImVec2(left, top),
                ImVec2(left, top) + ImVec2(rounding, 0));

            drawList->PathLineTo(tip_top);
            drawList->PathLineTo(tip_top + (tip_right - tip_top) * tipRound);
            drawList->PathBezierCubicCurveTo(
                tip_right,
                tip_right,
                tip_bottom + (tip_right - tip_bottom) * tipRound);

            drawList->PathLineTo(tip_bottom);
            drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
            drawList->PathBezierCubicCurveTo(
                ImVec2(left, bottom),
                ImVec2(left, bottom),
                ImVec2(left, bottom) - ImVec2(0, rounding));

            if (!filled)
            {
                // if it has alpha
                if (fillColor & 0xFF000000)
                {
                    drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.size(), fillColor);
                }
                drawList->PathStroke(color, true, 2.0f * outlineScale);
            }
            else
            {
                drawList->PathFillConvex(color);
            }
            break;
        }

        case BasicIconType::Circle:
        {
            if (!filled)
            {
                float radius = 0.5f * size.x / 2.0f - 0.5f;

                if (fillColor & 0xFF000000)
                {
                    drawList->AddCircleFilled(rectCenter, radius, fillColor, 12 + extraSegments);
                }
                drawList->AddCircle(rectCenter, radius, color, 12 + extraSegments, 2.0f * outlineScale);
            }
            else
            {
                drawList->AddCircleFilled(rectCenter, 0.5f * size.x / 2.0f, color, 12 + extraSegments);
            }
            break;
        }

        case BasicIconType::Square:
        {
            if (filled)
            {
                auto radius = 0.5f * size.x / 2.0f;
                auto p0     = rectCenter - ImVec2(radius, radius);
                auto p1     = rectCenter + ImVec2(radius, radius);

                drawList->AddRectFilled(p0, p1, color, 0, ImDrawFlags_RoundCornersAll);
            }
            else
            {
                auto radius = 0.5f * size.x / 2.0f - 0.5f;
                auto p0     = rectCenter - ImVec2(radius, radius);
                auto p1     = rectCenter + ImVec2(radius, radius);

                if (fillColor & 0xFF000000)
                {
                    drawList->AddRectFilled(p0, p1, fillColor, 0, ImDrawFlags_RoundCornersAll);
                }
                drawList->AddRect(p0, p1, color, 0, ImDrawFlags_RoundCornersAll, 2.0f * outlineScale);
            }
            break;
        }

        case BasicIconType::Grid:
        {
            auto radius = 0.5f * size.x / 2.0f;
            auto w      = ceilf(radius / 3.0f);

            auto BaseTl = ImVec2(floorf(rectCenter.x - w * 2.5f), floorf(rectCenter.y - w * 2.5f));
            auto BaseBr = ImVec2(floorf(BaseTl.x + w), floorf(BaseTl.y + w));

            auto tl = BaseTl;
            auto br = BaseBr;
            for (int i = 0; i < 3; ++i)
            {
                tl.x = BaseTl.x;
                br.x = BaseBr.x;
                drawList->AddRectFilled(tl, br, color);
                tl.x += w * 2;
                br.x += w * 2;
                if (i != 1 || filled)
                    drawList->AddRectFilled(tl, br, color);
                tl.x += w * 2;
                br.x += w * 2;
                drawList->AddRectFilled(tl, br, color);

                tl.y += w * 2;
                br.y += w * 2;
            }
            break;
        }

        case BasicIconType::RoundSquare:
        {
            if (filled)
            {
                auto radius = 0.5f * size.x / 2.0f;
                auto p0     = rectCenter - ImVec2(radius, radius);
                auto p1     = rectCenter + ImVec2(radius, radius);

                drawList->AddRectFilled(p0, p1, color, radius * 0.5f, ImDrawFlags_RoundCornersAll);
            }
            else
            {
                auto radius = 0.5f * size.x / 2.0f - 0.5f;
                auto p0     = rectCenter - ImVec2(radius, radius);
                auto p1     = rectCenter + ImVec2(radius, radius);

                if (fillColor & 0xFF000000)
                {
                    drawList->AddRectFilled(p0, p1, fillColor, radius * 0.5f, ImDrawFlags_RoundCornersAll);
                }
                drawList->AddRect(p0, p1, color, radius * 0.5f, ImDrawFlags_RoundCornersAll, 2.0f * outlineScale);
            }
            break;
        }

        case BasicIconType::Diamond:
        {
            if (filled)
            {
                auto radius = 0.607f * size.x / 2.0f;
                drawList->PathLineTo(rectCenter + ImVec2(0, -radius));
                drawList->PathLineTo(rectCenter + ImVec2(radius, 0));
                drawList->PathLineTo(rectCenter + ImVec2(0, radius));
                drawList->PathLineTo(rectCenter + ImVec2(-radius, 0));
                drawList->PathFillConvex(color);
            }
            else
            {
                auto radius = 0.607f * size.x / 2.0f - 0.5f;
                drawList->PathLineTo(rectCenter + ImVec2(0, -radius));
                drawList->PathLineTo(rectCenter + ImVec2(radius, 0));
                drawList->PathLineTo(rectCenter + ImVec2(0, radius));
                drawList->PathLineTo(rectCenter + ImVec2(-radius, 0));

                if (fillColor & 0xFF000000)
                {
                    drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.size(), fillColor);
                }
                drawList->PathStroke(color, true, 2.0f * outlineScale);
            }
            break;
        }
        default:
            std::unreachable();
        }

        ImGui::Dummy(size);
    }
} // namespace Ame::ImGuiUtils
