#pragma once

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

class DirectXManager;
class WindowManager;

/// <summary>
/// ImGui管理クラス
/// </summary>
class ImGuiManager
{
public:
	void Initialize(DirectXManager* dxManager, WindowManager* windowManager);
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

