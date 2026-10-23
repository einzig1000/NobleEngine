#pragma once

#include <externals/imgui/imgui.h>
#include <externals/imgui/imgui_impl_dx12.h>
#include <externals/imgui/imgui_impl_win32.h>
#include <externals/imgui/imgui_stdlib.h>
#include <externals/imgui/ImGuizmo.h>

#include <span>
#include <numbers>
#include <string>
#include <algorithm>

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

	// values を折れ線で表示する。offset は左端に来る要素の位置(リングバッファなら一番古い値の位置)
	// 右端の値(= 最新)をグラフの上に重ねて出す
	inline void PlotLineGraph(const char* label, std::span<const float> values, float minValue, float maxValue, int offset = 0, const ImVec2& size = ImVec2(0.0f, 80.0f))
	{
		IM_ASSERT(offset >= 0);
		const int count = static_cast<int>(values.size());

		char overlay[32] = "";
		if (count > 0)
		{
			std::snprintf(overlay, sizeof(overlay), "%.2f", values[(offset + count - 1) % count]);
		}
		ImGui::PlotLines(label, values.data(), count, offset, overlay, minValue, maxValue, size);
	}

	// value を minValue〜maxValue の割合にして横棒で表示する
	inline void GaugeBar(float value, float minValue, float maxValue, const ImVec2& size = ImVec2(0.0f, 0.0f))
	{
		IM_ASSERT(maxValue > minValue);
		const float fraction = std::clamp((value - minValue) / (maxValue - minValue), 0.0f, 1.0f);

		char overlay[32];
		std::snprintf(overlay, sizeof(overlay), "%.2f / %.2f", value, maxValue);
		ImGui::ProgressBar(fraction, size, overlay);
	}

	// value を minValue〜maxValue の割合にして円弧で表示する。12時の位置から時計回りに伸びる
	inline void GaugeCircle(float value, float minValue, float maxValue, float radius = 40.0f, float thickness = 6.0f, ImU32 color = IM_COL32(0, 255, 0, 255))
	{
		IM_ASSERT(maxValue > minValue);
		const float fraction = std::clamp((value - minValue) / (maxValue - minValue), 0.0f, 1.0f);

		// 線は太さの半分だけ円の外にはみ出すので、そのぶんも含めて場所を取る
		const float extent = radius + thickness * 0.5f;
		const ImVec2 topLeft = ImGui::GetCursorScreenPos();
		const ImVec2 center(topLeft.x + extent, topLeft.y + extent);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddCircle(center, radius, ImGui::GetColorU32(ImGuiCol_FrameBg), 0, thickness);
		if (fraction > 0.0f)
		{
			constexpr float kStartAngle = -std::numbers::pi_v<float> *0.5f;
			drawList->PathArcTo(center, radius, kStartAngle, kStartAngle + 2.0f * std::numbers::pi_v<float> *fraction);
			drawList->PathStroke(color, ImDrawFlags_None, thickness);
		}

		// 直接描いた図形はカーソルを進めないので、ここで場所を確保する
		ImGui::Dummy(ImVec2(extent * 2.0f, extent * 2.0f));
	}
}
