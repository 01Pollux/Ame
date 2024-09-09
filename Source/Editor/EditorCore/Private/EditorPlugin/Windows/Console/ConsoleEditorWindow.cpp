#include <EditorPlugin/Windows/Console/ConsoleEditorWindow.hpp>

#include <Core/Enum.hpp>
#include <ImGuiUtils/Utils.hpp>

#include <Log/Logger.hpp>

namespace Ame::Editor
{
    ConsoleEditorWindow::ConsoleEditorWindow(IReferenceCounters* counters) : Base(counters, ConsoleEditorWindowPath)
    {
        if (Log::s_Logger)
        {
            m_LogStream = std::make_unique<ConsoleLogStream>();
            Log::s_Logger->AddStream(m_LogStream.get());
        }
    }

    ConsoleEditorWindow::~ConsoleEditorWindow()
    {
        if (m_LogStream)
        {
            Log::s_Logger->RemoveStream(m_LogStream.get());
        }
    }

    //

    void ConsoleEditorWindow::OnDrawVisible()
    {
        imcxx::window window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
        if (!window)
        {
            return;
        }

        const ImVec2 windowRegion  = ImGui::GetContentRegionAvail();
        const float  logTypesWidth = 300.f;

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Show output from: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(logTypesWidth);
        if (imcxx::combo_box logTypes{ "##LogTypes", m_LogStream->GetLevelMaskString(), ImGuiComboFlags_HeightSmall })
        {
            auto levelMask = m_LogStream->GetLevelMask();
            for (auto level : Enum::enum_values<Log::LogLevel>())
            {
                if (level == Log::LogLevel::Disabled)
                {
                    continue;
                }

                auto name = Enum::enum_name(level);
                if (ImGui::CheckboxFlags(
                        name.data(), &levelMask, static_cast<uint64_t>(1ull << std::to_underlying(level))))
                {
                    m_LogStream->SetLevelMask(levelMask);
                }
            }
        }

        ImGui::SameLine();
        if (imcxx::button{ ICON_FA_TRASH_ALT })
        {
            m_LogStream->Clear();
        }

        // Reserve enough left-over height for 1 separator + 1 input text
        const float contentFooter = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

        imcxx::window_child content{ "##Content",
                                     { windowRegion.x, -contentFooter },
                                     ImGuiChildFlags_NavFlattened,
                                     ImGuiWindowFlags_HorizontalScrollbar };
        if (!content)
        {
            return;
        }

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear"))
            {
                m_LogStream->Clear();
            }
            ImGui::EndPopup();
        }

        m_LogStream->UpdateActiveLogs();

        ImGuiListClipper clipper;
        clipper.Begin(m_LogStream->GetLogCount());
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                const auto& log   = m_LogStream->GetLog(static_cast<uint16_t>(i));
                const auto  color = ImGuiUtils::GetImColor(m_LogStream->GetLevelColor(log.Level));

                imcxx::shared_color textColor(ImGuiCol_Text, color);
                imcxx::text         logText(log.Message);
            }
        }
    }
} // namespace Ame::Editor