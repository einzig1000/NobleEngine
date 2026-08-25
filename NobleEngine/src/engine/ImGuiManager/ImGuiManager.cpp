#include <ImGuiManager/ImGuiManager.h>
#include <DirectX/DirectXManager.h>
#include <Window/WindowManager.h>

ImGuiManager::ImGuiManager(DirectXManager* dxManager, WindowManager* windowManager)
	: dxManager_(dxManager), windowManager_(windowManager)
{
	// imguiのコンテキストを作成
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// imguiのスタイルを設定
	ImGui::StyleColorsDark();

	// imguiのWin32初期化
	ImGui_ImplWin32_Init(windowManager_->GetHwnd());

	// imguiのDirectX12初期化
	uint32_t slot = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->Allocate();
	ImGui_ImplDX12_Init(
		dxManager_->GetDevice(),
		dxManager_->GetSwapChain()->GetSwapChainDesc().BufferCount,
		dxManager_->GetSwapChain()->GetRtvDesc().Format,
		dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->GetSRVDescriptorHeap(),
		dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->GetCPUHandleAt(slot),                    // ImGuiフォントSRV用のCPUハンドル
		dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager()->GetGPUHandleAt(slot)                     // ImGuiフォントSRV用のGPUハンドル
	);


#ifdef _RELEASE
	io.IniFilename = nullptr;

	ImGui::LoadIniSettingsFromDisk("imgui.ini");
#endif 

}

ImGuiManager::~ImGuiManager()
{}

void ImGuiManager::BeginFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	
	// 画面全体をドッキング可能なスペースにする
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImGuiManager::EndFrame()
{
	ImGui::Render();
}

void ImGuiManager::Draw()
{
	if (!isDraw_) return;

//#ifdef _DEBUG

	ImDrawData* draw_data = ImGui::GetDrawData();
	if (draw_data != nullptr && draw_data->CmdListsCount > 0)
	{
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxManager_->GetCommandContextManager()->GetCommandList(dxManager_->GetSwapChain()->GetCurrentBackBufferIndex()));
	}

//#endif
}

void ImGuiManager::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}