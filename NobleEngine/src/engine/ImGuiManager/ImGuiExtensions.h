#pragma once

#include <externals/imgui/imgui.h>
#include <externals/imgui/imgui_impl_dx12.h>
#include <externals/imgui/imgui_impl_win32.h>
#include <externals/imgui/imgui_stdlib.h>
#include <externals/imgui/ImGuizmo.h>

namespace ImGui
{
	inline bool SelectableWithCopy(const std::string& text, bool selected)
	{
		const ImGuiStyle& style = ImGui::GetStyle();
		const float copyButtonWidth = ImGui::CalcTextSize("Copy").x + style.FramePadding.x * 2.0f;
		const float selectableWidth = std::max(ImGui::GetContentRegionAvail().x - copyButtonWidth - style.ItemSpacing.x, 1.0f);

		const bool clicked = ImGui::Selectable(text.c_str(), selected, 0, ImVec2(selectableWidth, 0.0f));
		ImGui::SetItemTooltip("%s", text.c_str());

		ImGui::SameLine();
		if (ImGui::SmallButton("Copy"))
		{
			ImGui::SetClipboardText(text.c_str());
		}
		return clicked;
	}
}