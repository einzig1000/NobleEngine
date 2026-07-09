#pragma once

#include <externals/imgui/imgui.h>
#include <externals/imgui/imgui_impl_dx12.h>
#include <externals/imgui/imgui_impl_win32.h>
#include <externals/imgui/imgui_stdlib.h>
#include <externals/imgui/ImGuizmo.h>

class DirectXManager;
class WindowManager;

/// <summary>
/// ImGui管理クラス
/// </summary>
class ImGuiManager
{
public:
	ImGuiManager(DirectXManager* dxManager, WindowManager* windowManager);
	~ImGuiManager();
	void BeginFrame();
	void EndFrame();
	void Draw();
	void Finalize();

	void ToggleDraw() { isDraw_ = !isDraw_; }

private:
	DirectXManager* dxManager_ = nullptr;
	WindowManager* windowManager_ = nullptr;
	bool isDraw_ = true;
};

