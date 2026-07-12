#include "DrawSystem.h"
#include <AssetManager/AssetManager.h>
#include <ImGuiManager/ImGuiManager.h>
#include <DirectX/DirectXManager.h>
#include <Window/WindowManager.h>
#include <Engine.h>
#include <numbers>

DrawSystem::DrawSystem(DirectXManager* dxManager, AssetManager* resourceManager)
	:dxManager_(dxManager), resourceManager_(resourceManager)
{
	for (uint32_t i = 0; i < Constexprs::kFrameCount; ++i)
	{
		cbAllocators_[i].Initialize(dxManager_->GetDevice(), 8 * 1024 * 1024, L"FrameCBAllocator");
	}

	rt_nobleScreenID_ = dxManager_->GetRenderTextureManager()->CreateRenderTarget(
		//Engine::Instance().GetWindowManager()->winHeight_,
		//Engine::Instance().GetWindowManager()->winHeight_,
		1280, 720,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, "NobleScreen", 1.0f);

	screenRenderObject_ = std::make_unique<RenderObject>();
	screenRenderObject_->psoConfig_.blendID = BlendStateID::Normal2;
	screenRenderObject_->psoConfig_.vs = "resources/shaders/FullScreen/FullScreen.VS.hlsl";
	screenRenderObject_->psoConfig_.ps = "resources/shaders/FullScreen/CopyImage.PS.hlsl";
	screenRenderObject_->SetupFromShaders();
}

DrawSystem::~DrawSystem()
{}

void DrawSystem::Reset()
{
	// CBアロケータをリセット
	auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
	cbAllocators_[backBufferIndex].Reset();

	sceneRenderObjects_.clear();
	postEffectRenderObjects_.clear();
	screenRenderObjects_.clear();
}

///// いつかRenderObjectリストをソートしたい
//① PSO
//② トポロジ
//③ ルートシグネチャ
//// 形状を設定 (三角形)
//dxManager_->GetCommandContextManager()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//// ルートシグネチャを設定
//dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature());
//// ルートシグネチャを設定
//dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature_block());
//// 形状を設定 (線)
//dxManager_->GetCommandContextManager()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
//// PSOを設定 (線はNormal固定)
//dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetLinePipelineState(BlendMode::kBlendModeNormal));


void DrawSystem::AddSceneDrawList(const RenderObject* renderObject, int32_t renderTextureID)
{
	sceneRenderObjects_[renderTextureID].push_back(renderObject);
}

void DrawSystem::AddPostEffectDrawList(const RenderObject* renderObject, int32_t renderTextureID)
{
	postEffectRenderObjects_[renderTextureID].push_back(renderObject);
}

void DrawSystem::AddScreenDrawList(const RenderObject* renderObject)
{
	screenRenderObjects_.push_back(renderObject);
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
		const ModelData* obj = resourceManager_->GetModelManager()->GetModelBank()->GetModelData(renderObject->modelID_);
		//cmdList->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
		if (!obj) return;
		D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			obj->vertexBufferView,
			obj->skinCluster.influenceBufferView
		};

		// 5)頂点バッファをバインド
		cmdList->IASetVertexBuffers(0, 2, vbvs);

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

void DrawSystem::SceneDraw()
{
	for (const auto& [rtID, renderObjects] : sceneRenderObjects_)
	{
		dxManager_->BeginRenderPass(dxManager_->GetRenderTextureManager()->Get(rtID), true);
		for (const auto* renderObject : renderObjects)
		{
			DrawObject(renderObject);
		}
		dxManager_->EndRenderPass(dxManager_->GetRenderTextureManager()->Get(rtID), true);
	}
}

void DrawSystem::PostEffectDraw()
{
	for (const auto& [rtID, renderObjects] : postEffectRenderObjects_)
	{
		dxManager_->BeginRenderPass(dxManager_->GetRenderTextureManager()->Get(rtID), false);
		for (const auto* renderObject : renderObjects)
		{
			DrawObject(renderObject);
		}
		dxManager_->EndRenderPass(dxManager_->GetRenderTextureManager()->Get(rtID), false);
	}
}

void DrawSystem::PreScreenDraw()
{
	dxManager_->BeginRenderPass(dxManager_->GetRenderTextureManager()->Get(rt_nobleScreenID_), true);
	for (const auto* renderObject : screenRenderObjects_)
	{
		DrawObject(renderObject);
	}
	dxManager_->EndRenderPass(dxManager_->GetRenderTextureManager()->Get(rt_nobleScreenID_), true);
}

void DrawSystem::ScreenDraw()
{
	screenRenderObject_->modelID_ = resourceManager_->GetModelManager()->GetModelLoader()->LoadModel("resources/prototypes/model/plane/plane.obj");
	screenRenderObject_->SetCBufferData(0, ShaderType::PixelShader, &rt_nobleScreenID_);
	DrawObject(screenRenderObject_.get());

// デバッグモードの時は、PreScreenDrawで描画した内容をImGuiのウィンドウに表示する
#ifdef _DEBUG

	ImGui::Begin("mainDisplay");
	ImGui::Image(ImTextureID(dxManager_->GetRenderTextureManager()->Get(rt_nobleScreenID_)->colorsrvAlloc.gpu.ptr), ImVec2(800, 450));
	//if (ImGui::ImageButton("texture##nobleScreen", ImTextureID(dxManager_->GetRenderTextureManager()->Get(rt_nobleScreenID_)->colorsrvAlloc.gpu.ptr), ImVec2(800, 450)))
	//{
	//	ImGui::OpenPopup("nobleScreen");
	//}
	ImGui::End();


#endif // DEBUG
}
