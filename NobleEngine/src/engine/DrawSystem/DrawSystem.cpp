#include "DrawSystem.h"
#include <AssetManager/AssetManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <DirectX/DirectXManager.h>
#include <DirectX/RootBinding/RootBindingManager.h>
#include <DirectX/CommandContextManager/CommandContextManager.h>
#include <DirectX/RenderTarget/SwapChainManager/SwapChainManager.h>
#include <DirectX/Pipeline/PipelineStateManager/PipelineStateManager.h>
#include <DirectX/RenderTarget/RenderTextureManager/RenderTextureManager.h>
#include <Utilities/Logger/Logger.h>
#include <queue>

DrawSystem::DrawSystem(DirectXManager* dxManager, AssetManager* assetManager, RootBindingManager* rootBindingManager)
	:dxManager_(dxManager), assetManager_(assetManager), rootBindingManager_(rootBindingManager)
{
	rt_nobleScreenID_ = dxManager_->GetRenderTextureManager()->CreateRenderTarget(
		Constexprs::kInitialWindowWidth,
		Constexprs::kInitialWindowHeight,
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
	backBufferIndex_ = dxManager_->GetSwapChainManager()->GetCurrentBackBufferIndex();

	drawNodes_.clear();
	rootParamValuePool_.clear();
}


// 描画リストに追加
void DrawSystem::AddDrawList(const RenderObject* renderObject, int32_t RenderTargetID, const std::vector<int32_t>& deps)
{
	// SetupFromShadersを呼び忘れたオブジェクトは描画しない
	if (!renderObject->GetRootLayout())
	{
		Logger::Error("%s : SetupFromShaders()が呼ばれていません", renderObject->name_);
#ifdef _DEBUG
		DebugBreak();
#endif
		return;
	}

	int32_t targetID = RenderTargetID;

	if (targetID == -1)
	{
		targetID = rt_nobleScreenID_;
	}

	DrawNode& node = drawNodes_[targetID];
	// RenderTargetIDに書き込む描画パケットを保存
	node.packets.push_back(BuildDrawPacket(renderObject));
	// 自身を書き込む前に書き込み終了していてほしいRenderTargetIDを保存
	for (int32_t dep : deps)
	{
		node.deps.insert(dep);
	}
}

// 描画パケットを作成
DrawSystem::DrawPacket DrawSystem::BuildDrawPacket(const RenderObject* renderObject)
{
	const RootLayout* layout = renderObject->GetRootLayout();
	assert(layout);
	const auto& values = renderObject->GetRootValues();

	DrawPacket packet{};
	packet.layout = layout;
	packet.pso = dxManager_->GetPipelineStateManager()->GetGraphicsPipelineState(renderObject->psoConfig_, *layout);
	packet.topology = renderObject->psoConfig_.topology;
	packet.useMeshShader = (renderObject->psoConfig_.ms != "unknown");
	packet.modelID = renderObject->modelID_;
	packet.instanceNum = renderObject->instanceNum_;
	packet.rootValuePoolOffset = static_cast<uint32_t>(rootParamValuePool_.size());
	packet.rootValuePoolSize = static_cast<uint32_t>(values.size());

	if (!values.empty())
	{
		rootParamValuePool_.resize(rootParamValuePool_.size() + values.size());
		std::memcpy(rootParamValuePool_.data() + packet.rootValuePoolOffset, values.data(), values.size() * sizeof(RootParamValue));
	}

	return packet;
}


// 描画
void DrawSystem::DrawObject(const DrawPacket* packet)
{
	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex_);
	auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

	// 1) RootSignatureセット
	cmdList->SetGraphicsRootSignature(packet->layout->rootSignature);
	// 2) PSOセット
	cmdList->SetPipelineState(packet->pso);
	// 3) CBV・SRVセット
	const RootParamValue* values = rootParamValuePool_.data() + packet->rootValuePoolOffset;
	for (uint32_t i = 0; i < packet->rootValuePoolSize; ++i)
	{
		const auto& param = packet->layout->params[i];
		const auto& value = values[i];

		if (param.paramType == ParamType::CBV)
		{
			assert(value.gpuAddress != 0);
			cmdList->SetGraphicsRootConstantBufferView(static_cast<UINT>(i), value.gpuAddress);
		}
		else if (param.paramType == ParamType::SRV)
		{
			assert(value.allocIndex != UINT32_MAX);
			cmdList->SetGraphicsRootDescriptorTable(static_cast<UINT>(i), srvManager->GetGPUHandleAt(value.allocIndex));
		}
	}

	// メッシュシェーダ使用かで分岐
	if (packet->useMeshShader)
	{
		cmdList->DispatchMesh(packet->instanceNum, 1, 1);
	}
	else
	{
		// 4) トポロジーセット
		cmdList->IASetPrimitiveTopology(packet->topology);

		// モデルの検索
		const ModelData* obj = assetManager_->GetModelManager()->GetModelBank()->GetModelData(packet->modelID);
		if (!obj)return;
		cmdList->IASetVertexBuffers(0, 1, &obj->vertexBufferView);

		const uint32_t indexCount = static_cast<uint32_t>(obj->indices.size());
		if (obj->indexBufferView.BufferLocation != 0 && indexCount > 0)
		{
			cmdList->IASetIndexBuffer(&obj->indexBufferView);
			cmdList->DrawIndexedInstanced(indexCount, packet->instanceNum, 0, 0, 0);
		}
		else
		{
			const uint32_t vertexCount = static_cast<uint32_t>(obj->vertices.size());
			cmdList->DrawInstanced(vertexCount, packet->instanceNum, 0, 0);
		}
	}
}

// 描画順をソート
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
		Logger::Log("DrawSystem: 依存グラフに循環があります。描画順序を解決できません。");
		assert(false);
	}
	return order;
}

// 一斉描画
void DrawSystem::Execute()
{
	// 依存関係を満たす安全な描画順を取得
	const auto order = SortNodes();

	// その順番通りに、RTごとにBeginRenderPass→描画→EndRenderPassを繰り返す
	for (int32_t rtID : order)
	{
		dxManager_->BeginRenderPass(dxManager_->GetRenderTextureManager()->Get(rtID), true);
		for (const auto& packet : drawNodes_[rtID].packets)
		{
			DrawObject(&packet);
		}
		dxManager_->EndRenderPass(dxManager_->GetRenderTextureManager()->Get(rtID), true);
	}
}

// NobleScreenに描画
void DrawSystem::ScreenDraw()
{
	screenRenderObject_->SetBRegisterData(0, ShaderType::PixelShader, &rt_nobleScreenID_);
	DrawPacket packet = BuildDrawPacket(screenRenderObject_.get());
	DrawObject(&packet);
}

