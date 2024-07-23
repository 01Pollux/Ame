#pragma once

#include <span>
#include <imgui.h>

#include <ImGuiUtils/imcxx/all_in_one.hpp>
#include <ImGuiUtils/WindowUtils.hpp>
#include <ImGuiUtils/Fonts/FontAwesome5.hpp>

#include <Core/Enum.hpp>
#include <Core/String.hpp>
#include <Math/Vector.hpp>
#include <Math/Colors.hpp>

namespace Ame::ImGuiUtils
{
    /// <summary>
    /// Draw a text with a background color of ImGuiCol_FrameBg.
    /// </summary>
    void DrawTextBG(
        const char* text,
        const char* textEnd = nullptr);

    /// <summary>
    /// Draw a text with a background color of ImGuiCol_FrameBg.
    /// </summary>
    inline void DrawTextBG(
        const String& text)
    {
        DrawTextBG(text.c_str(), text.c_str() + text.size());
    }

    //

    /// <summary>
    /// Increment the cursor position on the X axis.
    /// </summary>
    void IncrementCursorPosX(
        float pos);

    /// <summary>
    /// Increment the cursor position on the Y axis.
    /// </summary>
    void IncrementCursorPosY(
        float pos);

    /// <summary>
    /// Increment the cursor position.
    /// </summary>
    void IncrementCursorPos(
        const ImVec2& pos);

    /// <summary>
    /// Align the cursor position on the X axis.
    /// </summary>
    void TableAlignCenter(
        float width);

    /// <summary>
    /// Center the cursor position on the X axis.
    /// </summary>
    void CenterCursorX();

    /// <summary>
    /// Center the cursor position on the Y axis.
    /// </summary>
    void CenterCursorY();

    /// <summary>
    /// Center the cursor position.
    /// </summary>
    void CenterCursor(
        bool   region,
        ImVec2 offset  = {},
        bool   centerX = true,
        bool   centerY = false);

    /// <summary>
    /// Center the text position.
    /// </summary>
    void CenterText(
        const char* text,
        bool        region,
        bool        centerX = true,
        bool        centerY = false);

    //

    /// <summary>
    /// Push a font scale.
    /// </summary>
    [[nodiscard]] float PushFontScale(
        float scale);

    /// <summary>
    /// Push a font scale while multiplying with old value.
    /// </summary>
    [[nodiscard]] float PushFontScaleMul(
        float scale);

    /// <summary>
    /// Push a font scale while adding with old value.
    /// </summary>
    [[nodiscard]] float PushFontScaleMul(
        float scale);

    /// <summary>
    /// Pop a font scale.
    /// </summary>
    void PopFontScale(
        float oldScale);

    //

    /// <summary>
    /// Draw label with the given color.
    /// </summary>
    void DrawLabel(
        const String&  label,
        const ImColor& color,
        const ImVec2&  maxSize = { -FLT_MIN, -FLT_MIN });

    /// <summary>
    /// Draw label with the given color.
    /// </summary>
    void DrawLabel(
        const char*    label,
        const ImColor& color,
        const ImVec2&  maxSize = { -FLT_MIN, -FLT_MIN });

    //

    // Properties
    enum class ComponentActionType : uint8_t
    {
        Remove,
        Copy,
        Paste,
        Reset,

        _Last_Enum
    };

    struct ComponentHeaderInfo
    {
        ComponentActionType AllowedActions;
        ComponentActionType FinalAction : 2 {};
        bool                IsOpen      : 1 = false;

        operator bool() const noexcept
        {
            return IsOpen;
        }
    };

    /// <summary>
    /// Begins a component header with the given label and togglable state.
    /// </summary>
    [[nodiscard]] static ComponentHeaderInfo BeginComponentHeader(
        const char* label,
        bool*       togglable = nullptr)
    {
        ComponentHeaderInfo ret;
        const bool          isActive = togglable ? *togglable : true;

        {
            // Check if the component is disabled
            imcxx::disabled Uisabled{ !isActive };
            ret.IsOpen = ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_Framed);
        }

        // Draw the settings button
        float frameHeight = ImGui::GetFrameHeight();
        ImGui::SameLine(ImGui::GetWindowWidth() - frameHeight);

        imcxx::shared_item_id id{ label };

        // Draw the settings button
        if (imcxx::button{ ICON_FA_COG, ImVec2{ frameHeight, frameHeight } })
        {
            ImGui::OpenPopup("");
        }

        // Draw the settings popup
        if (imcxx::popup componentOptions{ "" })
        {
            using namespace EnumBitOperators;
            for (auto& [buttonName, action] : {
                     std::pair{ "Copy", ComponentActionType::Copy },
                     std::pair{ "Paste", ComponentActionType::Paste },
                     std::pair{ "Reset", ComponentActionType::Reset },
                     std::pair{ "Remove", ComponentActionType::Remove } })
            {
                bool actionAllowed = (ret.AllowedActions & action) == action;

                imcxx::disabled disabled(actionAllowed);
                if (imcxx::menuitem_entry{ buttonName })
                {
                    if (actionAllowed)
                    {
                        ret.FinalAction = action;
                    }
                }
            }
        }

        return ret;
    }

    /// <summary>
    /// Ends a component header.
    /// </summary>
    static void EndComponentHeader()
    {
        ImGui::TreePop();
    }

    /// <summary>
    /// Insert a dummy spacing for next property
    /// </summary>
    static void PropertySpacing()
    {
        ImGui::Dummy({ 0.f, 5.f });
    }

    /// <summary>
    /// Draws a component label with the given tooltip.
    /// </summary>
    void DrawComponentLabel(
        const char* label,
        bool        sameLine          = true,
        float       propertyNameWidth = 0.25f);

    enum class DrawVectorType : uint8_t
    {
        Drag,
        Input,
        Slider
    };

    static constexpr const char* c_DrawVectorPositionNames[] = {
        "X",
        "Y",
        "Z",
        "W"
    };

    template<Concepts::VectorType _Ty>
    struct DrawVectorData
    {
        _Ty& Value = nullptr;

        typename _Ty::value_type Min = _Ty::value_type(0);
        typename _Ty::value_type Max = _Ty::value_type(0);

        /// <summary>
        /// Flags are either ImGuiSliderFlags for slider+drag or ImGuiInputTextFlags for input.
        /// </summary>
        uint32_t Flags = 0;

        /// <summary>
        /// Names must be an array of _Ty::length strings.
        /// </summary>
        std::span<const char* const> Names         = c_DrawVectorPositionNames;
        std::optional<ImVec4>        PropertyColor = ImVec4{ 0.47f, 0.19f, 0.19f, 1.f };
    };

    /// <summary>
    /// Draw a vector component.
    ///
    /// If Format is nullptr, the default format is used.
    /// If Flags is 0, the default flags are used.
    /// Flags are either ImGuiSliderFlags for slider+drag or ImGuiInputTextFlags for input.
    /// </summary>
    template<DrawVectorType DrawType, Concepts::VectorType _Ty>
    static bool DrawVectorComponent(
        const DrawVectorData<_Ty>& drawData)
    {
        ImGui::PushID(std::addressof(drawData.Value));
        bool changed = false;

        float textSize = 0.f;
        for (typename _Ty::length_type i = 0; i < _Ty::length(); i++)
        {
            textSize += ImGui::CalcTextSize(drawData.Names[i]).x;
        }

        float regionWidth = ((ImGui::GetContentRegionAvail().x - textSize) / _Ty::length()) -
                            (ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x + 1.5f);

        for (typename _Ty::length_type i = 0; i < _Ty::length(); i++)
        {
            if (drawData.PropertyColor) [[likely]]
            {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, *drawData.PropertyColor);
            }
            DrawTextBG(drawData.Names[i]);
            if (drawData.PropertyColor) [[likely]]
            {
                ImGui::PopStyleColor();
            }

            ImGui::SetNextItemWidth(regionWidth);
            ImGui::SameLine();
            ImGui::PushID(i);

            if constexpr (DrawType == DrawVectorType::Drag)
            {
                if constexpr (std::is_same_v<typename _Ty::value_type, float>)
                {
                    changed |= ImGui::DragFloat("", &drawData.Value[i], 0.1f, drawData.Min, drawData.Max);
                }
                else if constexpr (std::is_same_v<typename _Ty::value_type, int32_t> ||
                                   std::is_same_v<typename _Ty::value_type, uint32_t>)
                {
                    changed |= ImGui::DragInt("", std::bit_cast<int*>(&drawData.Value[i]), 1, drawData.Min, drawData.Max);
                }
            }
            else if constexpr (DrawType == DrawVectorType::Input)
            {
                if constexpr (std::is_same_v<typename _Ty::value_type, float>)
                {
                    changed |= ImGui::InputFloat("", &drawData.Value[i]);
                    if (drawData.Min < drawData.Max)
                    {
                        drawData.Value[i] = std::clamp(drawData.Value[i], drawData.Min, drawData.Max);
                    }
                }
                else if constexpr (std::is_same_v<typename _Ty::value_type, int32_t> ||
                                   std::is_same_v<typename _Ty::value_type, uint32_t>)
                {
                    changed |= ImGui::InputInt("", std::bit_cast<int*>(&drawData.Value[i]));
                    if (drawData.Min < drawData.Max)
                    {
                        drawData.Value[i] = std::clamp(drawData.Value[i], drawData.Min, drawData.Max);
                    }
                }
            }
            else if constexpr (DrawType == DrawVectorType::Slider)
            {
                if constexpr (std::is_same_v<typename _Ty::value_type, float>)
                {
                    changed |= ImGui::SliderFloat("", &drawData.Value[i], drawData.Min, drawData.Max);
                }
                else if constexpr (std::is_same_v<typename _Ty::value_type, int32_t> ||
                                   std::is_same_v<typename _Ty::value_type, uint32_t>)
                {
                    changed |= ImGui::SliderInt("", std::bit_cast<int*>(&drawData.Value[i]), drawData.Min, drawData.Max);
                }
            }

            ImGui::PopID();
            if (i != (_Ty::length() - 1))
            {
                ImGui::SameLine();
            }
        }
        ImGui::PopID();
        return changed;
    }

    /// <summary>
    /// Draw a vector component.
    ///
    /// If Format is nullptr, the default format is used.
    /// If Flags is 0, the default flags are used.
    /// Flags are either ImGuiSliderFlags for slider+drag or ImGuiInputTextFlags for input.
    /// </summary>
    template<Concepts::VectorType _Ty>
    static bool DragVectorComponent(
        DrawVectorData<_Ty> drawData)
    {
        return DrawVectorComponent<DrawVectorType::Drag>(std::move(drawData));
    }

    /// <summary>
    /// Draw a vector component.
    ///
    /// If Format is nullptr, the default format is used.
    /// If Flags is 0, the default flags are used.
    /// Flags are either ImGuiSliderFlags for slider+drag or ImGuiInputTextFlags for input.
    /// </summary>
    template<Concepts::VectorType _Ty>
    static bool DragVectorComponent(
        _Ty& value)
    {
        return DrawVectorComponent<DrawVectorType::Drag>(DrawVectorData(value));
    }

    /// <summary>
    /// Draw a vector component.
    ///
    /// If Format is nullptr, the default format is used.
    /// If Flags is 0, the default flags are used.
    /// Flags are either ImGuiSliderFlags for slider+drag or ImGuiInputTextFlags for input.
    /// </summary>
    template<Concepts::VectorType _Ty>
    static bool InputVectorComponent(
        DrawVectorData<_Ty> drawData)
    {
        return DrawVectorComponent<DrawVectorType::Input>(std::move(drawData));
    }

    /// <summary>
    /// Draw a vector component.
    ///
    /// If Format is nullptr, the default format is used.
    /// If Flags is 0, the default flags are used.
    /// Flags are either ImGuiSliderFlags for slider+drag or ImGuiInputTextFlags for input.
    /// </summary>
    template<Concepts::VectorType _Ty>
    static bool InputVectorComponent(
        _Ty& value)
    {
        return DrawVectorComponent<DrawVectorType::Input>(DrawVectorData(value));
    }

    /// <summary>
    /// Draw a vector component.
    ///
    /// If Format is nullptr, the default format is used.
    /// If Flags is 0, the default flags are used.
    /// Flags are either ImGuiSliderFlags for slider+drag or ImGuiInputTextFlags for input.
    /// </summary>
    template<Concepts::VectorType _Ty>
    static bool SliderVectorComponent(
        DrawVectorData<_Ty> drawData)
    {
        return DrawVectorComponent<DrawVectorType::Slider>(std::move(drawData));
    }

    /// <summary>
    /// Draw a vector component.
    ///
    /// If Format is nullptr, the default format is used.
    /// If Flags is 0, the default flags are used.
    /// Flags are either ImGuiSliderFlags for slider+drag or ImGuiInputTextFlags for input.
    /// </summary>
    template<Concepts::VectorType _Ty>
    static bool SliderVectorComponent(
        _Ty& value)
    {
        return DrawVectorComponent<DrawVectorType::Slider>(DrawVectorData(value));
    }

    /// <summary>
    /// Draw a color picker.
    /// </summary>
    bool DrawColorPicker(
        const char*         name,
        Math::Color4&       color,
        ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoSidePreview |
                                    ImGuiColorEditFlags_NoSmallPreview |
                                    ImGuiColorEditFlags_AlphaBar |
                                    ImGuiColorEditFlags_HDR);

    /// <summary>
    /// Draw a color picker.
    /// </summary>
    bool DrawColorPicker(
        const char*         Name,
        Math::Color3&       Color,
        ImGuiColorEditFlags Flags = ImGuiColorEditFlags_NoSidePreview |
                                    ImGuiColorEditFlags_NoSmallPreview |
                                    ImGuiColorEditFlags_AlphaBar |
                                    ImGuiColorEditFlags_HDR);

    //

    enum class BasicIconType : uint8_t
    {
        Flow,
        Circle,
        Square,
        Grid,
        RoundSquare,
        Diamond
    };

    /// <summary>
    /// Draw basic shape icon to draw list
    /// </summary>
    void DrawIcon(
        ImDrawList*   drawList,
        const ImVec2& size,
        BasicIconType type,
        bool          filled,
        ImU32         color,
        ImU32         fillColor);

    /// <summary>
    /// Draw basic shape icon to current window's draw list
    /// </summary>
    inline void DrawIcon(
        const ImVec2& size,
        BasicIconType type,
        bool          filled,
        ImU32         color,
        ImU32         fillColor)
    {
        DrawIcon(ImGui::GetWindowDrawList(), size, type, filled, color, fillColor);
    }
} // namespace Ame::ImGuiUtils