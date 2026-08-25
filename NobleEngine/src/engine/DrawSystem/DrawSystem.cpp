#include "DrawSystem.h"
#include <AssetManager/AssetManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <DirectX/DirectXManager.h>
#include <Window/WindowManager.h>
#include <Utilities/Logger/Logger.h>
#include <Engine.h>
#include <queue>

DrawSystem::DrawSystem(DirectXManager* dxManager, AssetManager* assetManager)
	:dxManager_(dxManager), assetManager_(assetManager)
{
	for (uint32_t i = 0; i < Constexprs::kFrameCount; ++i)
	{
		cbAllocators_[i].Initialize(dxManager_->GetDevice(), 8 * 1024 * 1024, L"FrameCBAllocator");
	}

	rt_nobleScreenID_ = dxManager_->GetRenderTextureManager()->CreateRenderTarget(
		Engine::Instance().GetWindowManager()->winWidth_,
		Engine::Instance().GetWindowManager()->winHeight_,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, "NobleScreen", Vector4{ 0.11f, 0.11f, 0.11f, 1.0f });

	screenRenderObject_ = std::make_unique<RenderObject>();
	screenRenderObject_->psoConfig_.blendID = BlendStateID::Normal2;
	screenRenderObject_->psoConfig_.vs = "assets/shaders/FullScreen/FullScreen.VS.hlsl";
	screenRenderObject_->psoConfig_.ps = "assets/shaders/FullScreen/CopyImage.PS.hlsl";
	screenRenderObject_->modelID_ = assetManager_->GetModelManager()->GetModelLoader()->LoadModel("assets/engine/model/plane/plane.obj");
	screenRenderObject_->SetupFromShaders();
}

DrawSystem::~DrawSystem()
{}

void DrawSystem::Reset()
{
	// CBアロケータをリセット
	auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	cbAllocators_[backBufferIndex].Reset();

	drawNodes_.clear();
}

///// いつかRenderObjectリストをソートしたい
//① PSO
//② トポロジ
//③ ルートシグネチャ

void DrawSystem::AddDrawList(const RenderObject* renderObject, int32_t RenderTargetID, const std::vector<int32_t>& deps)
{
	int32_t targetID = RenderTargetID;

	if (targetID == -1)
	{
		targetID = rt_nobleScreenID_;
	}

	// RenderTargetIDに書き込むRenderObjectを保存
	drawNodes_[targetID].objects.push_back(renderObject);
	// 自身を書き込む前に書き込み終了していてほしいRenderTargetIDを保存
	for (int32_t dep : deps)
	{
		drawNodes_[targetID].deps.insert(dep);
	}
}

std::vector<int32_t> DrawSystem::SortNodes()
{
	// inDegree[rtID] = このrtIDのRTがあと何個のRTの完了を待っているか
	std::unordered_map<int32_t, int32_t> inDegree;
	// dependents[rtID] = このrtIDのRTの書き込み終了を待っていたRTのid一覧
	std::unordered_map<int32_t, std::vector<int32_t>> dependents;
	// 全ノードの入次数を0で初期化
	for (const auto& [rtID, node] : drawNodes_)
	{
		inDegree[rtID] = 0;
	}

	// 実際に依存関係を数え上げ、逆引きテーブル(dependents)も同時に構築する
	for (const auto& [rtID, node] : drawNodes_)
	{
		for (int32_t dep : node.deps)
		{
			// depが 今フレーム誰も書き込んでいないRT を指しているなら無視
			if (!drawNodes_.contains(dep)) continue;

			// depの書き込み終わったらrtIDをチェックしにいく という逆引きを登録
			dependents[dep].push_back(rtID);
			// rtIDは依存が1つ増えたので入次数を+1
			inDegree[rtID]++;
		}
	}


	std::queue<int32_t> ready;
	for (const auto& [rtID, deg] : inDegree)
	{
		// 他RT参照が0のRTは無条件描画
		if (deg == 0) ready.push(rtID);
	}

	// 実際の描画順
	std::vector<int32_t> order;
	while (!ready.empty())
	{
		int32_t rtID = ready.front();
		ready.pop();
		order.push_back(rtID);

		// rtIDの描画予約が済んだため、rtIDに依存していたRTの入次数を-1する
		for (int32_t next : dependents[rtID])
		{
			// その時依存が0になったRTは描画可能になるのでreadyに追加
			if (--inDegree[next] == 0) ready.push(next);
		}
	}

	if (order.size() != drawNodes_.size())
	{
		Log("DrawSystem: 依存グラフに循環があります。描画順序を解決できません。");
		assert(false);
	}
	return order;
}

void DrawSystem::DrawObject(const RenderObject* renderObject)
{
	auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
	auto& cb = cbAllocators_[backBufferIndex];
	auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

	// 1) RootSignatureセット
	cmdList->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature(renderObject->GetRootParams()).Get());
	// 2) PSOセット
	cmdList->SetPipelineState(dxManager_->GetPipelineStateManager()->GetGraphicsPipelineState(renderObject->psoConfig_, renderObject->GetRootParams()).Get());
	// 3) CBV・SRVセット
	const auto& cpuStrage = renderObject->GetCpuStorage();
	const auto& rootParams = renderObject->GetRootParams();
	for (size_t i = 0; i < rootParams.size(); ++i)
	{
		const auto& param = rootParams[i];

		if (param.paramType == ParamType::CBV)
		{
			const auto alloc = cb.Allocate(param.sizeBytes);
			std::memcpy(alloc.cpu, cpuStrage.data() + param.offsetBytes, param.sizeBytes);
			cmdList->SetGraphicsRootConstantBufferView(static_cast<UINT>(i), alloc.gpu);
		}
		else if (param.paramType == ParamType::SRV)
		{
			assert(param.srvAllocIndex != UINT32_MAX);
			cmdList->SetGraphicsRootDescriptorTable(static_cast<UINT>(i), srvManager->GetGPUHandleAt(param.srvAllocIndex));
		}
	}

	// メッシュシェーダ使用かで分岐
	if (renderObject->psoConfig_.ms != "unknown")
	{
		cmdList->DispatchMesh(renderObject->instanceNum_, 1, 1);
	}
	else
	{
		// 4) トポロジーセット
		cmdList->IASetPrimitiveTopology(renderObject->psoConfig_.topology);

		// モデルの検索
		const ModelData* obj = assetManager_->GetModelManager()->GetModelBank()->GetModelData(renderObject->modelID_);
		if (!obj)return;
		cmdList->IASetVertexBuffers(0, 1, &obj->vertexBufferView);

		const uint32_t indexCount = static_cast<uint32_t>(obj->indices.size());
		if (obj->indexBufferView.BufferLocation != 0 && indexCount > 0)
		{
			cmdList->IASetIndexBuffer(&obj->indexBufferView);
			cmdList->DrawIndexedInstanced(indexCount, renderObject->instanceNum_, 0, 0, 0);
		}
		else
		{
			const uint32_t vertexCount = static_cast<uint32_t>(obj->vertices.size());
			cmdList->DrawInstanced(vertexCount, renderObject->instanceNum_, 0, 0);
		}
	}
}

void DrawSystem::Execute()
{
	// 依存関係を満たす安全な描画順を取得
	const auto order = SortNodes();

	// その順番通りに、RTごとにBeginRenderPass→描画→EndRenderPassを繰り返す
	for (int32_t rtID : order)
	{
		dxManager_->BeginRenderPass(dxManager_->GetRenderTextureManager()->Get(rtID), true);
		for (const auto* renderObject : drawNodes_[rtID].objects)
		{
			DrawObject(renderObject);
		}
		dxManager_->EndRenderPass(dxManager_->GetRenderTextureManager()->Get(rtID), true);
	}
}

void DrawSystem::ScreenDraw()
{
	screenRenderObject_->SetCBufferData(0, ShaderType::PixelShader, &rt_nobleScreenID_);
	DrawObject(screenRenderObject_.get());

#ifdef _DEBUG

	ImGui::Begin("mainDisplay");
	ImGui::Image(ImTextureID(dxManager_->GetRenderTextureManager()->Get(rt_nobleScreenID_)->colorsrvAlloc.gpu.ptr), ImVec2(800, 450));
	ImGui::End();

#endif // DEBUG
}
