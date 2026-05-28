#include <DrawSystem/DrawSystem.h>
#include <DirectX/DirectXManager.h>
#include <ResourceManager/ResourceManager.h>
#include <Window/WindowManager.h>
#include <DirectX/Pipeline/ShaderReflectionHelper/ShaderReflectionHelper.h>
#include <Utilities/Converter/StringConverter/StringConverter.h>
#include <numbers>

DrawSystem::DrawSystem(DirectXManager* dxManager, ResourceManager* resourceManager)
	:dxManager_(dxManager), resourceManager_(resourceManager)
{
	for (uint32_t i = 0; i < kFrameCount; ++i)
	{
		cbAllocators_[i].Initialize(dxManager_->GetDevice(), 8 * 1024 * 1024, L"FrameCBAllocator");
	}

	ScreenDrawPSOConfig_.vs = "resources/shaders/FullScreen/FullScreen.VS.hlsl";
	ScreenDrawPSOConfig_.ps = "resources/shaders/FullScreen/CopyImage.PS.hlsl";
	ScreenDrawPSOConfig_.dsvFormatID = DSVFormatID::Unknown;

	RootParam rootParam{};
	rootParam.paramType = ParamType::CBV;
	rootParam.shaderType = ShaderType::PixelShader;
	rootParam.ComputeHash();
	ScreenDrawRootParams_.push_back(rootParam);
	rootParam.paramType = ParamType::SRV;
	rootParam.shaderType = ShaderType::PixelShader;
	rootParam.ComputeHash(); 
	rootParam.srvAllocIndex = 0;
	ScreenDrawRootParams_.push_back(rootParam);
}

DrawSystem::~DrawSystem()
{}

void DrawSystem::Reset()
{
	// CBアロケータをリセット
	//for (uint32_t i = 0; i < kFrameCount; ++i)
	//{
	//	cbAllocators_[i].Reset();
	//}
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
	cmdList->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetOrCreateRootSignature(renderObject->GetRootParams()).Get());
	// 2) PSOセット
	cmdList->SetPipelineState(dxManager_->GetPipelineStateManager()->GetOrCreateGraphicsPipelineState(renderObject->psoConfig_, renderObject->GetRootParams()).Get());
	// 3) トポロジーセット
	cmdList->IASetPrimitiveTopology(renderObject->psoConfig_.topology);
	// 4) CBV・SRVセット
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
	// モデルの検索
	const ModelData* obj = resourceManager_->GetModelManager()->GetModelData(renderObject->modelID_);
	if (!obj) return;
	// 頂点数の取得
	const uint32_t kSumVertex = static_cast<uint32_t>(obj->vertices.size());
	// 5)頂点バッファをバインド
	cmdList->IASetVertexBuffers(0, 1, &obj->vertexBufferView);

	// 6)描画
	cmdList->DrawInstanced(kSumVertex, renderObject->instanceNum_, 0, 0);
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

void DrawSystem::ScreenDraw()
{
	for (const auto* renderObject : screenRenderObjects_)
	{
		DrawObject(renderObject);
	}
}


//auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();
//auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList();
//auto& cb = cbAllocators_[GetFrameIndex()];
//
//// 1) RootSignatureセット
//cmdList->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetOrCreateRootSignature(ScreenDrawRootParams_).Get());
//// 2) PSOセット
//cmdList->SetPipelineState(dxManager_->GetPipelineStateManager()->GetOrCreateGraphicsPipelineState(ScreenDrawPSOConfig_, ScreenDrawRootParams_).Get());
//// 3) トポロジーセット
//cmdList->IASetPrimitiveTopology(ScreenDrawPSOConfig_.topology);
//// 4) CBV・SRVセット
//
//// dxManager_->GetRenderTextureManager()->Get(RenderTextureID::PreBackBuffer)->srvAlloc.indexを送る
//const auto alloc = cb.Allocate(sizeof(uint32_t));
////std::memcpy(alloc.cpu, cpuStrage.data() + param.offsetBytes, param.sizeBytes);
//std::memcpy(alloc.cpu, &dxManager_->GetRenderTextureManager()->Get(RenderTextureID::PreBackBuffer)->srvAlloc.index, sizeof(uint32_t));
//cmdList->SetGraphicsRootConstantBufferView(0, alloc.gpu);
//cmdList->SetGraphicsRootDescriptorTable(1, srvManager->GetGPUHandleAt(0));
//// 6)描画
//cmdList->DrawInstanced(3, 1, 0, 0);

/// 既にいろいろいじってしまった過去のScreenDraw()。
//void DrawSystem::ScreenDraw()
//{
//	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList();
//	auto& cb = cbAllocators_[GetFrameIndex()];
//	auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();
//
//	// トポロジーセット
//	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	// RootSignatureセット
//	cmdList->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetOrCreateRootSignature(renderObject->GetRootParams()).Get());
//	// PSOセット
//	cmdList->SetPipelineState(dxManager_->GetPipelineStateManager()->GetOrCreateGraphicsPipelineState(renderObject->psoConfig_, renderObject->GetRootParams()).Get());
//	// CBV・SRVセット
//	const auto& cpuStrage = renderObject->GetCpuStorage();
//	const auto& rootParams = renderObject->GetRootParams();
//	for (size_t i = 0; i < rootParams.size(); ++i)
//	{
//		const auto& param = rootParams[i];
//
//		if (param.paramType == ParamType::CBV)
//		{
//			const auto alloc = cb.Allocate(param.sizeBytes);
//			std::memcpy(alloc.cpu, cpuStrage.data() + param.offsetBytes, param.sizeBytes);
//			cmdList->SetGraphicsRootConstantBufferView(static_cast<UINT>(i), alloc.gpu);
//		}
//		else if (param.paramType == ParamType::SRV)
//		{
//			assert(param.srvAllocIndex != UINT32_MAX);
//			cmdList->SetGraphicsRootDescriptorTable(static_cast<UINT>(i), srvManager->GetGPUHandleAt(param.srvAllocIndex));
//		}
//	}
//
//	// モデルの検索
//	const ModelData* obj = resourceManager_->GetModelManager()->GetModelData(renderObject->modelID);
//	if (!obj) return;
//	// 頂点数の取得
//	const uint32_t kSumVertex = static_cast<uint32_t>(obj->vertices.size());
//	// 頂点バッファをバインド
//	cmdList->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
//
//	// 描画
//	cmdList->DrawInstanced(kSumVertex, renderObject->instanceNum_, 0, 0);
//}

/// 過去のSceneDraw()。RenderTexture指定がない
//void DrawSystem::SceneDraw()
//{
//	auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList();
//	auto& cb = cbAllocators_[GetFrameIndex()];
//	auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();
//
//	for (auto* renderObject : sceneRenderObjects_)
//	{
//		// 1) RootSignatureセット
//		cmdList->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetOrCreateRootSignature(renderObject->GetRootParams()).Get());
//		// 2) PSOセット
//		cmdList->SetPipelineState(dxManager_->GetPipelineStateManager()->GetOrCreateGraphicsPipelineState(renderObject->psoConfig_, renderObject->GetRootParams()).Get());
//		// 3) トポロジーセット
//		cmdList->IASetPrimitiveTopology(renderObject->psoConfig_.topology);
//		// 4) CBV・SRVセット
//		const auto& cpuStrage = renderObject->GetCpuStorage();
//		const auto& rootParams = renderObject->GetRootParams();
//		for (size_t i = 0; i < rootParams.size(); ++i)
//		{
//			const auto& param = rootParams[i];
//
//			if (param.paramType == ParamType::CBV)
//			{
//				const auto alloc = cb.Allocate(param.sizeBytes);
//				std::memcpy(alloc.cpu, cpuStrage.data() + param.offsetBytes, param.sizeBytes);
//				cmdList->SetGraphicsRootConstantBufferView(static_cast<UINT>(i), alloc.gpu);
//			}
//			else if (param.paramType == ParamType::SRV)
//			{
//				assert(param.srvAllocIndex != UINT32_MAX);
//				cmdList->SetGraphicsRootDescriptorTable(static_cast<UINT>(i), srvManager->GetGPUHandleAt(param.srvAllocIndex));
//			}
//		}
//		// モデルの検索
//		const ModelData* obj = resourceManager_->GetModelManager()->GetModelData(renderObject->modelID);
//		if (!obj) continue;
//		// 頂点数の取得
//		const uint32_t kSumVertex = static_cast<uint32_t>(obj->vertices.size());
//		// 5)頂点バッファをバインド
//		cmdList->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
//
//		// 6)描画
//		cmdList->DrawInstanced(kSumVertex, renderObject->instanceNum_, 0, 0);
//	}
//}



void DrawSystem::AddDebugLineList(const Vector3& start, const Vector3& end, uint32_t color)
{
	debugLineList_[color].push_back(start);
	debugLineList_[color].push_back(end);
}
void DrawSystem::AddSphere(const Sphere& sphere, uint32_t color)
{
	SphereXYZ sphereXYZ;
	sphereXYZ.center = sphere.center;
	sphereXYZ.radius = { sphere.radius, sphere.radius, sphere.radius };
	AddSphereXYZ(sphereXYZ, color);
}
void DrawSystem::AddSphereXYZ(const SphereXYZ& sphere, uint32_t color)
{
	uint32_t kSubdivision = 12;

	const float kLonStep = float((2.0f * std::numbers::pi_v<float>) / kSubdivision);
	const float kLatStep = float(std::numbers::pi_v<float> / kSubdivision);

	// 経度方向のリング（縦の輪）
	for (uint32_t lon = 0; lon < kSubdivision; ++lon)
	{
		float lonAngle = lon * kLonStep;

		for (uint32_t lat = 0; lat < kSubdivision; ++lat)
		{
			float lat0 = -std::numbers::pi_v<float> / 2.0f + lat * kLatStep;
			float lat1 = lat0 + kLatStep;

			Vector3 p0{
				sphere.radius.x * std::cos(lat0) * std::cos(lonAngle),
				sphere.radius.y * std::sin(lat0),
				sphere.radius.z * std::cos(lat0) * std::sin(lonAngle)
			};

			Vector3 p1{
				sphere.radius.x * std::cos(lat1) * std::cos(lonAngle),
				sphere.radius.y * std::sin(lat1),
				sphere.radius.z * std::cos(lat1) * std::sin(lonAngle)
			};

			AddDebugLineList(p0 + sphere.center, p1 + sphere.center, color);
		}
	}

	// 緯度方向のリング（横の輪）
	for (uint32_t lat = 1; lat < kSubdivision; ++lat)
	{
		float latAngle = -std::numbers::pi_v<float> / 2.0f + lat * kLatStep;

		for (uint32_t lon = 0; lon < kSubdivision; ++lon)
		{
			float lon0 = lon * kLonStep;
			float lon1 = lon0 + kLonStep;

			Vector3 p0{
				sphere.radius.x * std::cos(latAngle) * std::cos(lon0),
				sphere.radius.y * std::sin(latAngle),
				sphere.radius.z * std::cos(latAngle) * std::sin(lon0)
			};

			Vector3 p1{
				sphere.radius.x * std::cos(latAngle) * std::cos(lon1),
				sphere.radius.y * std::sin(latAngle),
				sphere.radius.z * std::cos(latAngle) * std::sin(lon1)
			};

			AddDebugLineList(p0 + sphere.center, p1 + sphere.center, color);
		}
	}
}
void DrawSystem::AddCylinder(const Cylinder& cylinder, uint32_t color)
{
	Vector3 axis = cylinder.topCenter - cylinder.bottomCenter;
	const float height = axis.Length();
	if (height <= 1e-6f)
	{
		// 高さゼロなら円として扱う
		SphereXYZ s{};
		s.center = cylinder.bottomCenter;
		s.radius = { cylinder.radius, cylinder.radius, cylinder.radius };
		AddSphereXYZ(s, color);
		return;
	}
	axis = axis / height;

	// 軸と平行でない適当なベクトル
	Vector3 tmp = (std::abs(axis.y) < 0.99f) ? Vector3{ 0.0f, 1.0f, 0.0f } : Vector3{ 1.0f, 0.0f, 0.0f };
	Vector3 u = axis.Cross(tmp).Normalized();
	Vector3 v = axis.Cross(u).Normalized();

	const uint32_t kSubdivision = 32;
	const float kStep = float((2.0f * std::numbers::pi_v<float>) / kSubdivision);

	// 上下の円周
	auto addCircle = [&](const Vector3& center)
		{
			for (uint32_t i = 0; i < kSubdivision; ++i)
			{
				const float a0 = kStep * i;
				const float a1 = kStep * (i + 1);

				Vector3 p0 = center + (u * (cylinder.radius * std::cos(a0))) + (v * (cylinder.radius * std::sin(a0)));
				Vector3 p1 = center + (u * (cylinder.radius * std::cos(a1))) + (v * (cylinder.radius * std::sin(a1)));
				AddDebugLineList(p0, p1, color);
			}
		};

	addCircle(cylinder.bottomCenter);
	addCircle(cylinder.topCenter);

	// 側面の縦線
	const uint32_t kSideLines = 12;
	for (uint32_t i = 0; i < kSideLines; ++i)
	{
		const float a = float((2.0f * std::numbers::pi_v<float>) * (float(i) / float(kSideLines)));
		Vector3 rim = (u * (cylinder.radius * std::cos(a))) + (v * (cylinder.radius * std::sin(a)));

		AddDebugLineList(cylinder.bottomCenter + rim, cylinder.topCenter + rim, color);
	}
}
void DrawSystem::AddAABB(const AABB& aabb, uint32_t color)
{
	Vector3 p[8] = {
	{ aabb.min.x, aabb.min.y, aabb.min.z }, // 0
	{ aabb.max.x, aabb.min.y, aabb.min.z }, // 1
	{ aabb.max.x, aabb.max.y, aabb.min.z }, // 2
	{ aabb.min.x, aabb.max.y, aabb.min.z }, // 3
	{ aabb.min.x, aabb.min.y, aabb.max.z }, // 4
	{ aabb.max.x, aabb.min.y, aabb.max.z }, // 5
	{ aabb.max.x, aabb.max.y, aabb.max.z }, // 6
	{ aabb.min.x, aabb.max.y, aabb.max.z }, // 7
	};

	// 底面
	AddDebugLineList(p[0], p[1], color);
	AddDebugLineList(p[1], p[2], color);
	AddDebugLineList(p[2], p[3], color);
	AddDebugLineList(p[3], p[0], color);

	// 上面
	AddDebugLineList(p[4], p[5], color);
	AddDebugLineList(p[5], p[6], color);
	AddDebugLineList(p[6], p[7], color);
	AddDebugLineList(p[7], p[4], color);

	// 縦
	AddDebugLineList(p[0], p[4], color);
	AddDebugLineList(p[1], p[5], color);
	AddDebugLineList(p[2], p[6], color);
	AddDebugLineList(p[3], p[7], color);
}

//void DrawSystem::DrawAllModel()
//{
//	/// 描画順をソート
//	/// 不透過オブジェクト→
//	/// カメラから遠い透過オブジェクト→
//	/// カメラから近い透過オブジェクト
//	/// なおテクスチャによる透過はこの世に存在しないものとする
//	Vector3 cameraPos = Game::Camera::Getter::GetCurrentTranslate();
//
//	std::sort(modelDrawList_.begin(), modelDrawList_.end(),
//		[cameraPos](RenderData_Model* a, RenderData_Model* b)
//		{
//			bool aTransparent = (a->color.w < 1.0f);
//			bool bTransparent = (b->color.w < 1.0f);
//
//			// 不透明 vs 半透明
//			if (aTransparent != bTransparent)
//			{
//				// 不透明を先に
//				return !aTransparent && bTransparent;
//			}
//
//			// 両方不透明なら順序は気にしない（falseで安定ソート）
//			if (!aTransparent && !bTransparent)
//			{
//				return false;
//			}
//
//			// 両方半透明ならカメラ距離で遠い順に
//			Vector3 aWorldPos = a->GetWorldPosition();
//			Vector3 bWorldPos = b->GetWorldPosition();
//
//			float distA = (aWorldPos - cameraPos).LengthSq();
//			float distB = (bWorldPos - cameraPos).LengthSq();
//
//			return distA > distB; // 遠いものを先に
//		});
//
//
//	for (auto& renderData : modelDrawList_)
//	{
//		// 画面内か判定
//		if (!renderData->inPicture)continue;
//
//		// 描画回数上限
//		if (drawCallIndex_ >= kMaxDrawCallPerFrame_)continue;
//
//		// モデルの検索
//		const ModelData* obj = resourceManager_->GetModelManager()->GetModelData(renderData->GetModel());
//		if (!obj)continue;
//
//		// テクスチャの検索
//		const TextureData* tex = resourceManager_->GetTextureManager()->GetTextureData(renderData->GetTexture());
//		if (!tex)continue;
//
//		// ルートシグネチャを設定
//		if (renderData->options.wireframe || wireframeMode_)
//			// ワイヤーフレーム用PSOを設定
//			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetTrianglePipelineState(BlendMode::Wireframe));
//		else
//			// Triangle用PSOを設定
//			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetTrianglePipelineState(renderData->options.blendMode));
//
//		// 頂点数の取得
//		const uint32_t kSumVertex = static_cast<uint32_t>(obj->modelData.vertices.size());
//
//		// WVP行列
//		wvpData_[drawCallIndex_]->World = renderData->GetWorldMatrix();
//		wvpData_[drawCallIndex_]->WVP = renderData->GetWorldMatrix() * viewProjectionMatrix_;
//
//		// マテリアル
//		Vector4 color = { renderData->color.x / 255.0f, renderData->color.y / 255.0f, renderData->color.z / 255.0f, renderData->color.w / 255.0f };
//		materialData_[drawCallIndex_]->color = color;
//		materialData_[drawCallIndex_]->shininess = 1.0f;
//		Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData->uvTransform.scale));
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData->uvTransform.rotate.z));
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData->uvTransform.translate));
//		materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;
//
//		// ライト
//		if (!renderData->options.useOwnLight)
//		{
//			*lightData_[drawCallIndex_] = *directionalLightData_;
//		}
//		else
//		{
//			lightData_[drawCallIndex_]->color = renderData->options.dirLight.color;
//			lightData_[drawCallIndex_]->direction = renderData->options.dirLight.direction.Normalized();
//			lightData_[drawCallIndex_]->intensity = renderData->options.dirLight.intensity;
//			lightData_[drawCallIndex_]->mode = renderData->options.dirLight.mode;
//			lightData_[drawCallIndex_]->phong = renderData->options.dirLight.phong;
//		}
//
//		// カメラデータ
//		cameraData_->worldPosition = Game::Camera::Getter::GetCurrentTranslate();
//
//		// 頂点バッファをバインド（描画に使う頂点データを指定）
//		dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
//		// ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
//		// ルートパラメータ1にWVP（ワールド・ビュー・プロジェクション）用定数バッファをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
//		// ルートパラメータ2にテクスチャのSRV（シェーダリソースビュー）をバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
//		// ルートパラメータ3にライト用定数バッファをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, lightResources_[drawCallIndex_]->GetGPUVirtualAddress());
//		// ルートパラメータ4にスペキュラライト用定数バッファをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
//		// 頂点数分のインスタンス描画を実行（実際に描画コマンドを発行）
//		dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);
//
//		drawCallIndex_++;
//	}
//}

//void DrawSystem::DrawAllTriangle()
//{
//	// B方式：フレームCBアロケータ（Upload線形）から確保して RootCBV に直刺し
//	auto& cb = cbAllocators_[GetFrameIndex()];
//
//	for (auto& renderData : triangleDrawList_)
//	{
//		// 描画回数上限
//		if (drawCallIndex_ >= kMaxDrawCallPerFrame_) continue;
//
//		// テクスチャの検索
//		const TextureData* tex = resourceManager_->GetTextureManager()->GetTextureData(renderData->texture);
//		if (!tex) continue;
//
//		// PSOを設定
//		if (renderData->options.wireframe || wireframeMode_)
//		{
//			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(
//				dxManager_->GetPipelineStateManager()->GetTrianglePipelineState(BlendMode::Wireframe));
//		}
//		else
//		{
//			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(
//				dxManager_->GetPipelineStateManager()->GetTrianglePipelineState(renderData->options.blendMode));
//		}
//
//		// 頂点数の取得
//		const uint32_t kSumVertex = 3;
//		// 必要な頂点数分配列を拡張
//		if (vertexDataUsed_ + kSumVertex > vertexData_.size())
//		{
//			vertexData_.resize(vertexDataUsed_ + kSumVertex);
//		}
//
//		// 上
//		vertexData_[vertexDataUsed_ + 0].position = { renderData->pos1.x, renderData->pos1.y, renderData->pos1.z, 1.0f };
//		vertexData_[vertexDataUsed_ + 0].texcoord = { 0.5f, 0.0f };
//		vertexData_[vertexDataUsed_ + 0].normal = { 0.0f, 0.0f, -1.0f };
//
//		// 右下
//		vertexData_[vertexDataUsed_ + 1].position = { renderData->pos2.x, renderData->pos2.y, renderData->pos2.z, 1.0f };
//		vertexData_[vertexDataUsed_ + 1].texcoord = { 1.0f, 1.0f };
//		vertexData_[vertexDataUsed_ + 1].normal = { 0.0f, 0.0f, -1.0f };
//
//		// 左下
//		vertexData_[vertexDataUsed_ + 2].position = { renderData->pos3.x,renderData->pos3.y, renderData->pos3.z, 1.0f };
//		vertexData_[vertexDataUsed_ + 2].texcoord = { 0.0f, 1.0f };
//		vertexData_[vertexDataUsed_ + 2].normal = { 0.0f, 0.0f, -1.0f };
//
//		// WVP行列
//		TransformationMatrix wvp{};
//		wvp.World = Matrix4x4::MakeAffineMatrix(renderData->transforms.scale, renderData->transforms.rotate, renderData->transforms.translate);
//		wvp.WVP = wvp.World * viewProjectionMatrix_;
//
//		// ライト（共有ライトをコピー）
//		DirectionalLight light{};
//		light = *directionalLightData_;
//
//		// マテリアル
//		Material material{};
//		material.color = ConvertUintToVector4(renderData->color);
//		material.shininess = 1.0f;
//
//		Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData->uvTransform.scale));
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData->uvTransform.rotate.z));
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData->uvTransform.translate));
//		material.uvTransform = uvTransformMatrix;
//
//		// 動的頂点バッファを確保
//		if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) continue;
//		std::memcpy(vertexMappedPtr_ + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);
//
//		// 頂点バッファビュー
//		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
//		vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress() + sizeof(VertexData) * static_cast<UINT>(vertexDataUsed_);
//		vertexBufferView.SizeInBytes = sizeof(VertexData) * kSumVertex;
//		vertexBufferView.StrideInBytes = sizeof(VertexData);
//
//		// ===== フレームCBアロケータから確保して RootCBV に刺す =====
//		const auto materialAlloc = cb.Allocate(sizeof(Material));
//		std::memcpy(materialAlloc.cpu, &material, sizeof(Material));
//
//		const auto wvpAlloc = cb.Allocate(sizeof(TransformationMatrix));
//		std::memcpy(wvpAlloc.cpu, &wvp, sizeof(TransformationMatrix));
//
//		const auto lightAlloc = cb.Allocate(sizeof(DirectionalLight));
//		std::memcpy(lightAlloc.cpu, &light, sizeof(DirectionalLight));
//
//
//		// 頂点バッファ
//		dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
//		// マテリアル
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialAlloc.gpu);
//		// wvp
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpAlloc.gpu);
//		// テクスチャ
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
//		// ライト
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, lightAlloc.gpu);
//
//		// 描画
//		dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(3, 1, 0, 0);
//
//
//		drawCallIndex_++;
//		vertexDataUsed_ += kSumVertex;
//	}
//}

//void DrawSystem::DrawAllRect()
//{
//	for (auto& renderData : rectDrawList_)
//	{
//		// 描画回数上限
//		if (drawCallIndex_ >= kMaxDrawCallPerFrame_) continue;
//
//		// テクスチャの検索
//		const TextureData* tex = resourceManager_->GetTextureManager()->GetTextureData(renderData->texture);
//		if (!tex) continue;
//
//		// PSOを設定
//		if (renderData->options.wireframe || wireframeMode_)
//		{	// ワイヤーフレーム用PSOを設定
//			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetTrianglePipelineState(BlendMode::Wireframe));
//		}
//		else
//		{	// Triangle用PSOを設定
//			dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetTrianglePipelineState(renderData->options.blendMode));
//		}
//
//		// 頂点数の取得
//		const uint32_t kSumVertex = 6;
//		// 必要な頂点数分配列を拡張
//		if (vertexDataUsed_ + kSumVertex > vertexData_.size())
//		{
//			vertexData_.resize(vertexDataUsed_ + kSumVertex);
//		}
//
//
//		/// 三角形１つめ
//		// 左上
//		vertexData_[vertexDataUsed_ + 0].position = { renderData->pos3.x, renderData->pos3.y, renderData->pos3.z, 1.0f };
//		vertexData_[vertexDataUsed_ + 0].texcoord = { 0.0f, 0.0f };
//		vertexData_[vertexDataUsed_ + 0].normal = { 0.0f, 0.0f, -1.0f };
//
//		// 右上
//		vertexData_[vertexDataUsed_ + 1].position = { renderData->pos1.x, renderData->pos1.y, renderData->pos1.z, 1.0f };
//		vertexData_[vertexDataUsed_ + 1].texcoord = { 1.0f, 0.0f };
//		vertexData_[vertexDataUsed_ + 1].normal = { 0.0f, 0.0f, -1.0f };
//
//		// 右下
//		vertexData_[vertexDataUsed_ + 2].position = { renderData->pos2.x, renderData->pos2.y, renderData->pos2.z, 1.0f };
//		vertexData_[vertexDataUsed_ + 2].texcoord = { 1.0f, 1.0f };
//		vertexData_[vertexDataUsed_ + 2].normal = { 0.0f, 0.0f, -1.0f };
//
//		/// 三角形２つめ
//		// 左上
//		vertexData_[vertexDataUsed_ + 3].position = { renderData->pos3.x, renderData->pos3.y, renderData->pos3.z, 1.0f };
//		vertexData_[vertexDataUsed_ + 3].texcoord = { 0.0f, 0.0f };
//		vertexData_[vertexDataUsed_ + 3].normal = { 0.0f, 0.0f, -1.0f };
//
//		// 右下
//		vertexData_[vertexDataUsed_ + 4].position = { renderData->pos2.x, renderData->pos2.y, renderData->pos2.z, 1.0f };
//		vertexData_[vertexDataUsed_ + 4].texcoord = { 1.0f, 1.0f };
//		vertexData_[vertexDataUsed_ + 4].normal = { 0.0f, 0.0f, -1.0f };
//
//		// 左下
//		vertexData_[vertexDataUsed_ + 5].position = { renderData->pos4.x, renderData->pos4.y, renderData->pos4.z, 1.0f };
//		vertexData_[vertexDataUsed_ + 5].texcoord = { 0.0f, 1.0f };
//		vertexData_[vertexDataUsed_ + 5].normal = { 0.0f, 0.0f, -1.0f };
//
//
//		// WVP行列
//		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData->transforms.scale, renderData->transforms.rotate, renderData->transforms.translate);
//		Matrix4x4 wvpMatrix = world * viewProjectionMatrix_;
//
//		wvpData_[drawCallIndex_]->World = world;
//		wvpData_[drawCallIndex_]->WVP = wvpMatrix;
//
//		// ライトの設定
//		*lightData_[drawCallIndex_] = *directionalLightData_;
//
//		// マテリアル
//		Vector4 color = ConvertUintToVector4(renderData->color);
//		materialData_[drawCallIndex_]->color = color;
//		materialData_[drawCallIndex_]->shininess = 1.0f;
//		Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData->uvTransform.scale));
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData->uvTransform.rotate.z));
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData->uvTransform.translate));
//		materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;
//
//		// 動的頂点バッファを確保
//		if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) continue;        
//		std::memcpy(vertexMappedPtr_ + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);
//
//		// 頂点バッファビュー
//		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
//		vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress() + sizeof(VertexData) * static_cast<UINT>(vertexDataUsed_);
//		vertexBufferView.SizeInBytes = sizeof(VertexData) * kSumVertex;
//		vertexBufferView.StrideInBytes = sizeof(VertexData);
//
//		// RootSignatureを設定。
//		dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
//		// CBVを設定する マテリアル用のCBufferの場所を設定
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
//		// CBVを設定する wvp用のCBufferの場所を設定
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
//		// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
//		// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, lightResources_[drawCallIndex_]->GetGPUVirtualAddress());
//
//		// 描画
//		dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(6, 1, 0, 0);
//
//
//		drawCallIndex_++;
//		vertexDataUsed_ += kSumVertex;
//	}
//}

//void DrawSystem::DrawAllSprite()
//{
//	for (auto& renderData : spriteDrawList_)
//	{
//		// 描画回数上限
//		if (drawCallIndex_ >= kMaxDrawCallPerFrame_) continue;
//
//		// テクスチャの検索
//		const TextureData* tex = resourceManager_->GetTextureManager()->GetTextureData(renderData->texture);
//		if (!tex) continue;
//
//		// PSOを設定
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetTrianglePipelineState(renderData->options.blendMode)); // Triangle用PSOを設定
//
//		// 必要な頂点数
//		const uint32_t kSumVertex = 4;
//		// 必要な頂点数分配列を拡張
//		if (vertexDataUsed_ + kSumVertex > vertexData_.size())
//		{
//			vertexData_.resize(vertexDataUsed_ + kSumVertex);
//		}
//
//		// 座標頂点
//		float halfWidth = static_cast<float>(tex->metadata.width) * 0.5f;
//		float halfHeight = static_cast<float>(tex->metadata.height) * 0.5f;
//
//		// UV座標頂点
//		if (renderData->cutImageSize.x == 0 && renderData->cutImageSize.y == 0)
//		{
//			renderData->cutImageSize.x = static_cast<int>(tex->metadata.width);
//			renderData->cutImageSize.y = static_cast<int>(tex->metadata.height);
//		}
//		if (renderData->cutImageLeftTop.x < 0) renderData->cutImageLeftTop.x = 0;
//		if (renderData->cutImageLeftTop.y < 0) renderData->cutImageLeftTop.y = 0;
//
//		Vector2 point = { float(renderData->cutImageLeftTop.x), float(renderData->cutImageLeftTop.y) };
//		Vector2 size = { float(renderData->cutImageSize.x), float(renderData->cutImageSize.y) };
//
//		float L = point.x / tex->metadata.width;
//		float R = (point.x + size.x) / tex->metadata.width;
//		float T = point.y / tex->metadata.height;
//		float B = (point.y + size.y) / tex->metadata.height;
//
//		// 左下 (index 0)
//		vertexData_[vertexDataUsed_ + 0].position = { -halfWidth, -halfHeight, 0.0f, 1.0f };
//		vertexData_[vertexDataUsed_ + 0].texcoord = { L, T };
//		vertexData_[vertexDataUsed_ + 0].normal = { 0.0f, 0.0f, -1.0f };
//
//		// 左上 (index 1)
//		vertexData_[vertexDataUsed_ + 1].position = { halfWidth, -halfHeight, 0.0f, 1.0f };
//		vertexData_[vertexDataUsed_ + 1].texcoord = { R, T };
//		vertexData_[vertexDataUsed_ + 1].normal = { 0.0f, 0.0f, -1.0f };
//
//		// 右下 (index 2)
//		vertexData_[vertexDataUsed_ + 2].position = { -halfWidth, halfHeight, 0.0f, 1.0f };
//		vertexData_[vertexDataUsed_ + 2].texcoord = { L, B };
//		vertexData_[vertexDataUsed_ + 2].normal = { 0.0f, 0.0f, -1.0f };
//
//		// 右上 (index 3)
//		vertexData_[vertexDataUsed_ + 3].position = { halfWidth, halfHeight, 0.0f, 1.0f };
//		vertexData_[vertexDataUsed_ + 3].texcoord = { R, B };
//		vertexData_[vertexDataUsed_ + 3].normal = { 0.0f, 0.0f, -1.0f };
//
//		// アンカーによる位置調整
//		switch (renderData->anchor)
//		{
//		case Anchor::Center:
//		{
//			// 左下 (index 0)
//			vertexData_[vertexDataUsed_ + 0].position.x;
//			vertexData_[vertexDataUsed_ + 0].position.y;
//			// 左上 (index 1)
//			vertexData_[vertexDataUsed_ + 1].position.x;
//			vertexData_[vertexDataUsed_ + 1].position.y;
//			// 右下 (index 2)
//			vertexData_[vertexDataUsed_ + 2].position.x;
//			vertexData_[vertexDataUsed_ + 2].position.y;
//			// 右上 (index 3)
//			vertexData_[vertexDataUsed_ + 3].position.x;
//			vertexData_[vertexDataUsed_ + 3].position.y;
//			break;
//		}
//		case Anchor::CenterLeft:
//		{
//			// 左下 (index 0)
//			vertexData_[vertexDataUsed_ + 0].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 0].position.y;
//			// 左上 (index 1)
//			vertexData_[vertexDataUsed_ + 1].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 1].position.y;
//			// 右下 (index 2)
//			vertexData_[vertexDataUsed_ + 2].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 2].position.y;
//			// 右上 (index 3)
//			vertexData_[vertexDataUsed_ + 3].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 3].position.y;
//			break;
//		}
//		case Anchor::CenterRight:
//		{
//			// 左下 (index 0)
//			vertexData_[vertexDataUsed_ + 0].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 0].position.y;
//			// 左上 (index 1)
//			vertexData_[vertexDataUsed_ + 1].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 1].position.y;
//			// 右下 (index 2)
//			vertexData_[vertexDataUsed_ + 2].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 2].position.y;
//			// 右上 (index 3)
//			vertexData_[vertexDataUsed_ + 3].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 3].position.y;
//			break;
//		}
//		case Anchor::CenterTop:
//		{
//			// 左下 (index 0)
//			vertexData_[vertexDataUsed_ + 0].position.x;
//			vertexData_[vertexDataUsed_ + 0].position.y += halfHeight;
//			// 左上 (index 1)
//			vertexData_[vertexDataUsed_ + 1].position.x;
//			vertexData_[vertexDataUsed_ + 1].position.y += halfHeight;
//			// 右下 (index 2)
//			vertexData_[vertexDataUsed_ + 2].position.x;
//			vertexData_[vertexDataUsed_ + 2].position.y += halfHeight;
//			// 右上 (index 3)
//			vertexData_[vertexDataUsed_ + 3].position.x;
//			vertexData_[vertexDataUsed_ + 3].position.y += halfHeight;
//			break;
//		}
//		case Anchor::CenterDown:
//		{
//			// 左下 (index 0)
//			vertexData_[vertexDataUsed_ + 0].position.x;
//			vertexData_[vertexDataUsed_ + 0].position.y += -halfHeight;
//			// 左上 (index 1)
//			vertexData_[vertexDataUsed_ + 1].position.x;
//			vertexData_[vertexDataUsed_ + 1].position.y += -halfHeight;
//			// 右下 (index 2)
//			vertexData_[vertexDataUsed_ + 2].position.x;
//			vertexData_[vertexDataUsed_ + 2].position.y += -halfHeight;
//			// 右上 (index 3)
//			vertexData_[vertexDataUsed_ + 3].position.x;
//			vertexData_[vertexDataUsed_ + 3].position.y += -halfHeight;
//			break;
//		}
//		case Anchor::LeftTop:
//		{
//			// 左下 (index 0)
//			vertexData_[vertexDataUsed_ + 0].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 0].position.y += halfHeight;
//			// 左上 (index 1)
//			vertexData_[vertexDataUsed_ + 1].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 1].position.y += halfHeight;
//			// 右下 (index 2)
//			vertexData_[vertexDataUsed_ + 2].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 2].position.y += halfHeight;
//			// 右上 (index 3)
//			vertexData_[vertexDataUsed_ + 3].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 3].position.y += halfHeight;
//			break;
//		}
//		case Anchor::RightTop:
//		{
//			// 左下 (index 0)
//			vertexData_[vertexDataUsed_ + 0].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 0].position.y += halfHeight;
//			// 左上 (index 1)
//			vertexData_[vertexDataUsed_ + 1].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 1].position.y += halfHeight;
//			// 右下 (index 2)
//			vertexData_[vertexDataUsed_ + 2].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 2].position.y += halfHeight;
//			// 右上 (index 3)
//			vertexData_[vertexDataUsed_ + 3].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 3].position.y += halfHeight;
//			break;
//		}
//		case Anchor::LeftDown:
//		{
//			// 左下 (index 0)
//			vertexData_[vertexDataUsed_ + 0].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 0].position.y += -halfHeight;
//			// 左上 (index 1)
//			vertexData_[vertexDataUsed_ + 1].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 1].position.y += -halfHeight;
//			// 右下 (index 2)
//			vertexData_[vertexDataUsed_ + 2].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 2].position.y += -halfHeight;
//			// 右上 (index 3)
//			vertexData_[vertexDataUsed_ + 3].position.x += halfWidth;
//			vertexData_[vertexDataUsed_ + 3].position.y += -halfHeight;
//			break;
//		}
//		case Anchor::RightDown:
//		{
//			// 左下 (index 0)
//			vertexData_[vertexDataUsed_ + 0].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 0].position.y += -halfHeight;
//			// 左上 (index 1)
//			vertexData_[vertexDataUsed_ + 1].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 1].position.y += -halfHeight;
//			// 右下 (index 2)
//			vertexData_[vertexDataUsed_ + 2].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 2].position.y += -halfHeight;
//			// 右上 (index 3)
//			vertexData_[vertexDataUsed_ + 3].position.x += -halfWidth;
//			vertexData_[vertexDataUsed_ + 3].position.y += -halfHeight;
//			break;
//		}
//		default:
//			break;
//		}
//
//		// WVP行列
//		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(renderData->transforms.scale, renderData->transforms.rotate, renderData->transforms.translate);
//		Matrix4x4 wvpMatrix = (world * orthoProjectionMatrix_);
//
//		wvpData_[drawCallIndex_]->World = world;
//		wvpData_[drawCallIndex_]->WVP = wvpMatrix;
//
//		// ライトの設定
//		lightData_[drawCallIndex_]->color = { 1.0f, 1.0f, 1.0f, 1.0f };
//		lightData_[drawCallIndex_]->intensity = 1.0f;
//		lightData_[drawCallIndex_]->mode = LightMode::None;
//		lightData_[drawCallIndex_]->phong = false;
//
//		// マテリアル
//		float uvCenterX = (renderData->pivot.x) - (halfWidth);
//		float uvCenterY = (renderData->pivot.y) - (halfHeight);
//		uvCenterX = uvCenterX / halfWidth / 2;
//		uvCenterY = uvCenterY / halfHeight / 2;
//
//		Matrix4x4 toCenter = Matrix4x4::MakeTranslateMatrix({ -uvCenterX, -uvCenterY, 0.0f });
//		Matrix4x4 fromCenter = Matrix4x4::MakeTranslateMatrix({ uvCenterX, uvCenterY, 0.0f });
//		Matrix4x4 uvTransformMatrix = Matrix4x4::MakeIdentity4x4();
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeScaleMatrix(renderData->uvTransform.scale));
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeRotateZMatrix(renderData->uvTransform.rotate.z));
//
//		uvTransformMatrix = (fromCenter * (uvTransformMatrix * toCenter));
//
//		uvTransformMatrix = (uvTransformMatrix * Matrix4x4::MakeTranslateMatrix(renderData->uvTransform.translate));
//
//		// ===== UV マテリアルへ反映 =====
//		Vector4 color = ConvertUintToVector4(renderData->color);
//		materialData_[drawCallIndex_]->color = color;
//		materialData_[drawCallIndex_]->shininess = 1.0f;
//		materialData_[drawCallIndex_]->uvTransform = uvTransformMatrix;
//
//		// スプライトの中心座標
//		Vector2 center = { renderData->transforms.translate.x, renderData->transforms.translate.y };
//
//		halfWidth *= renderData->transforms.scale.x;
//		halfHeight *= renderData->transforms.scale.y;
//
//		// アンカーに応じて中心座標を補正
//		switch (renderData->anchor)
//		{
//		case Anchor::CenterLeft:
//			center.x += halfWidth;
//			break;
//		case Anchor::CenterRight:
//			center.x -= halfWidth;
//			break;
//		case Anchor::CenterTop:
//			center.y += halfHeight;
//			break;
//		case Anchor::CenterDown:
//			center.y -= halfHeight;
//			break;
//		case Anchor::LeftTop:
//			center.x += halfWidth;
//			center.y += halfHeight;
//			break;
//		case Anchor::RightTop:
//			center.x -= halfWidth;
//			center.y += halfHeight;
//			break;
//		case Anchor::LeftDown:
//			center.x += halfWidth;
//			center.y -= halfHeight;
//			break;
//		case Anchor::RightDown:
//			center.x -= halfWidth;
//			center.y -= halfHeight;
//			break;
//		default:
//			break;
//		}
//
//		// スプライトのAABB
//		float left = center.x - halfWidth;
//		float right = center.x + halfWidth;
//		float top = center.y - halfHeight;
//		float bottom = center.y + halfHeight;
//
//		left *= float(WindowManager::winWidth_) / 1280.0f;
//		right *= float(WindowManager::winWidth_) / 1280.0f;
//		top *= float(WindowManager::winHeight_) / 720.0f;
//		bottom *= float(WindowManager::winHeight_) / 720.0f;
//
//		// マウス座標取得
//		Vector2 mousePos = Game::IO::Mouse::GetPosition();
//
//		// マウス座標は仮想座標へ変換してから衝突判定に使う。
//		float windowWidth = float(WindowManager::winWidth_);
//		float windowHeight = float(WindowManager::winHeight_);
//
//		// 当たり判定
//		renderData->isCollisionMouseRay = (mousePos.x >= left && mousePos.x <= right && mousePos.y >= top && mousePos.y <= bottom);
//
//
//
//		// 動的頂点バッファを確保
//		if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) continue;
//		memcpy(vertexMappedPtr_ + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);
//
//		// 頂点バッファビュー
//		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
//		vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress() + sizeof(VertexData) * (vertexDataUsed_);
//		vertexBufferView.SizeInBytes = sizeof(VertexData) * static_cast<UINT>(kSumVertex);
//		vertexBufferView.StrideInBytes = sizeof(VertexData);
//
//		// Spriteの描画
//		dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
//		dxManager_->GetCommandContextManager()->GetCommandList()->IASetIndexBuffer(&indexBufferView);
//		// CBVを設定する マテリアル用のCBufferの場所を設定
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
//		// CBVを設定する wvp用のCBufferの場所を設定
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
//		// SRVのDescriptorTableの先頭を設定。２はrootParameters[2]。
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, tex->textureSrvHandleGPU);
//		// CBVを設定する ディレクショナルライト用のCBufferの場所を設定
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, lightResources_[drawCallIndex_]->GetGPUVirtualAddress());
//
//		// 描画
//		dxManager_->GetCommandContextManager()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
//
//		drawCallIndex_++;
//		vertexDataUsed_ += kSumVertex;
//	}
//}

//void DrawSystem::DrawAllLine()
//{
//	for (auto& renderData : lineDrawList_)
//	{
//		// 描画回数上限
//		if (drawCallIndex_ >= kMaxDrawCallPerFrame_) continue;
//
//		// 2点未満
//		if (renderData->points.size() < 2) continue;
//
//		// 指定された点
//		std::vector<Vector3> mainPoints;
//		for (size_t i = 1; i < renderData->points.size(); ++i)
//		{
//			mainPoints.push_back(renderData->points[i]);
//		}
//		// 曲線だった場合の補完点も含めた点リスト(points同士を直線で結ぶ)
//		std::vector<Vector3> points;
//
//		// 補完の分割数
//		const int subdivision = my_max(uint32_t(1), renderData->kSubdivision);
//
//		switch (renderData->lineType)
//		{
//		case LineType::Line:
//		{
//			points = mainPoints;
//			break;
//		}
//		case LineType::BezierCurve:
//		{
//			if (mainPoints.size() >= 4)
//			{
//				for (size_t i = 0; i + 3 < mainPoints.size(); i += 3)
//				{
//					const Vector3& p0 = mainPoints[i];
//					const Vector3& p1 = mainPoints[i + 1];
//					const Vector3& p2 = mainPoints[i + 2];
//					const Vector3& p3 = mainPoints[i + 3];
//
//					if (i == 0)
//					{
//						points.push_back(p0); // 最初だけ始点を追加
//					}
//					for (int j = 0; j <= subdivision; ++j)
//					{
//						float t = static_cast<float>(j) / static_cast<float>(subdivision);
//						Vector3 point =
//							(p0 * std::powf(1.0f - t, 3)) +
//							(p1 * (3.0f * std::powf(1.0f - t, 2) * t)) +
//							(p2 * (3.0f * (1.0f - t) * t * t)) +
//							(p3 * (t * t * t));
//						points.push_back(point);
//					}
//				}
//			}
//			else if (mainPoints.size() == 3)
//			{
//				const Vector3& p0 = mainPoints[0];
//				const Vector3& p1 = mainPoints[1];
//				const Vector3& p2 = mainPoints[2];
//
//				for (int j = 0; j <= subdivision; ++j)
//				{
//					float t = static_cast<float>(j) / static_cast<float>(subdivision);
//					Vector3 a01 = p0 + (p1 - p0) * t;
//					Vector3 a12 = p1 + (p2 - p1) * t;
//					Vector3 point = a01 + (a12 - a01) * t;
//
//					points.push_back(point);
//				}
//			}
//			else
//			{
//				points = mainPoints;
//				Log("DrawLine制御点不足  ID:%d  name:%s", renderData->ID, renderData->name);
//				continue;
//			}
//			break;
//		}
//		case LineType::SplineCurve:
//		{
//			if (mainPoints.size() >= 3)
//			{
//				for (size_t i = 0; i < mainPoints.size() - 1; ++i)
//				{
//					const Vector3& p1 = mainPoints[i];
//					const Vector3& p2 = mainPoints[i + 1];
//					const Vector3& p0 = (i == 0) ? p1 : mainPoints[i - 1];
//					const Vector3& p3 = (i + 2 < mainPoints.size()) ? mainPoints[i + 2] : p2;
//
//					if (i == 0)
//					{
//						points.push_back(p1); // 最初だけ始点を追加
//					}
//
//					for (int j = 1; j <= subdivision; ++j)
//					{
//						float t = static_cast<float>(j) / static_cast<float>(subdivision);
//						Vector3 point =
//							(((p1 * 2.0f) +
//								(-p0 + p2) * t +
//								((p0 * 2.0f) - (p1 * 5.0f) + (p2 * 4.0f) - p3) * t * t +
//								(-p0 + (p1 * 3.0f) - (p2 * 3.0f) + p3) * t * t * t)) * 0.5f;
//						points.push_back(point);
//					}
//				}
//			}
//			else
//			{
//				points = mainPoints;
//				Log("DrawLine制御点不足  ID:%d  name:%s", renderData->ID, renderData->name);
//				continue;
//			}
//			break;
//		}
//		default:
//		{
//			break;
//		}
//		}
//
//		// [p0,p1,p1,p2,p2,p3,...] の形に展開
//		std::vector<Vector3> out;
//
//		out.reserve((points.size() - 1) * 2);
//		for (size_t i = 0; i + 1 < points.size(); ++i)
//		{
//			out.push_back(points[i]);
//			out.push_back(points[i + 1]);
//		}
//
//		// 頂点数の取得
//		const uint32_t kSumVertex = static_cast<uint32_t>(out.size());
//		// 必要な頂点数分配列を拡張
//		if (vertexDataUsed_ + kSumVertex > vertexData_.size())
//		{
//			vertexData_.resize(vertexDataUsed_ + kSumVertex);
//		}
//
//		for (size_t i = 0; i < out.size(); i++)
//		{
//			vertexData_[vertexDataUsed_ + i].position = { out[i].x, out[i].y, out[i].z, 1.0f };
//		}
//
//		// WVP行列
//		wvpData_[drawCallIndex_]->World = Matrix4x4::MakeIdentity4x4();
//		wvpData_[drawCallIndex_]->WVP = viewProjectionMatrix_;
//
//		// マテリアル
//		Vector4 color = ConvertUintToVector4(renderData->color);
//		materialData_[drawCallIndex_]->color = color;
//		materialData_[drawCallIndex_]->shininess = 1.0f;
//		materialData_[drawCallIndex_]->uvTransform = Matrix4x4::MakeIdentity4x4(); // 線にUV変換いらない
//
//		// 動的頂点バッファを確保
//		if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) continue; 
//		// 永続Mapされた VB にコピー
//		std::memcpy(vertexMappedPtr_ + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData)* kSumVertex);
//
//		// 頂点バッファビュー
//		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
//		vertexBufferView.BufferLocation = vertexResource_->GetGPUVirtualAddress() + sizeof(VertexData) * static_cast<UINT>(vertexDataUsed_);
//		vertexBufferView.SizeInBytes = sizeof(VertexData) * kSumVertex;
//		vertexBufferView.StrideInBytes = sizeof(VertexData);
//		
//		// RootSignatureを設定
//		dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
//		// ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
//		// ルートパラメータ1にWVP用定数バッファをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
//		// 描画コマンドの発行
//		dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);
//
//
//		drawCallIndex_++;
//		vertexDataUsed_ += kSumVertex;
//	}
//}

//void DrawSystem::DrawAllParticle()
//{
//	for (auto& renderData : particleDrawList_)
//	{
//		if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;
//
//		// インスタンス数 0
//		if (renderData->GetCurrentSum() == 0) continue;
//
//		// モデルの検索
//		ModelData* obj = resourceManager_->GetModelManager()->GetModelData(renderData->model);
//		if (!obj) continue;
//
//		// テクスチャの検索
//		const TextureData* tex = resourceManager_->GetTextureManager()->GetTextureData(renderData->texture);
//		if (!tex) continue;
//
//		// RootSignatureとPSOを設定
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootSignature(dxManager_->GetPipelineStateManager()->GetRootSignature_particle());
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetParticlePipelineState(renderData->blendMode));
//
//		// 頂点数の取得
//		const uint32_t kSumVertex = static_cast<uint32_t>(obj->modelData.vertices.size());
//
//		// マテリアルデータ
//		Vector4 color = ConvertUintToVector4(renderData->color);
//		materialData_[drawCallIndex_]->color = color;
//		materialData_[drawCallIndex_]->shininess = 1.0f;
//		materialData_[drawCallIndex_]->uvTransform = Matrix4x4::MakeIdentity4x4();
//
//		// 頂点バッファをバインド（描画に使う頂点データを指定）
//		dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
//
//		// ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
//		// ルートパラメータ2にテクスチャのSRV（シェーダリソースビュー）をバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(1, tex->textureSrvHandleGPU);
//		// ルートパラメータ3にパーティクル情報用SRVをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, renderData->srvAllocation_.gpu);
//		// ルートパラメータ4にカメラViewProjection行列用定数バッファをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(3, viewProjectionResource_->GetGPUVirtualAddress());
//
// 
//		dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, renderData->GetCurrentSum() , 0, 0);
//		
//		drawCallIndex_++;
//	}
//}

//void DrawSystem::DrawAllBlock()
//{
//	// PSOを設定(一旦normal固定)
//	dxManager_->GetCommandContextManager()->GetCommandList()->SetPipelineState(dxManager_->GetPipelineStateManager()->GetBlockPipelineState(BlendMode::kBlendModeNormal));
//	for (auto& renderData : blockDrawList_)
//	{
//		// 描画数０
//		if (renderData->currentDrawSum == 0) continue;
//
//		// 描画数オーバー(capacityは事前に確保しているインスタンス数の上限)
//		if (renderData->currentDrawSum > renderData->capacity)
//		{
//			assert(false); 
//			continue;
//		}
//
//		// モデルの検索
//		ModelData* obj = resourceManager_->GetModelManager()->GetModelData(renderData->model);
//		if (!obj) continue;
//
//		// テクスチャの検索
//		const TextureData* tex = resourceManager_->GetTextureManager()->GetTextureData(renderData->texture);
//		if (!tex) continue;
//		const TextureData* tex2 = resourceManager_->GetTextureManager()->GetTextureData(renderData->breakTexture);
//		if (!tex2) continue;
//
//		if (baseAtlasInfoData_)
//		{
//			baseAtlasInfoData_->invAtlasSize = { 1.0f / float(tex->metadata.width), 1.0f / float(tex->metadata.height) };
//		}
//		if (breakAtlasInfoData_)
//		{
//			breakAtlasInfoData_->invAtlasSize = { 1.0f / float(tex2->metadata.width), 1.0f / float(tex2->metadata.height) };
//		}
//
//
//		// 頂点数の取得
//		const uint32_t kSumVertex = static_cast<uint32_t>(obj->modelData.vertices.size());
//
//		// 頂点バッファをバインド（描画に使う頂点データを指定）
//		dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &obj->vertexBufferView);
//
//
//		// ルートパラメータ0にワールド行列配列をバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(0, renderData->worldMatrixSrvAllocation_.gpu);
//		// ルートパラメータ1に破壊タイルのインデックス配列をバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(1, renderData->breakTileSrvAllocation_.gpu);
//		// ルートパラメータ2にベースタイルのインデックス配列をバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(2, renderData->baseTileSrvAllocation_.gpu);
//		// ルートパラメータ3にベーステクスチャをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(3, tex->textureSrvHandleGPU);
//		// ルートパラメータ4に破壊テクスチャをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(4, tex2->textureSrvHandleGPU);
//		// ルートパラメータ5にカラーをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootDescriptorTable(5, renderData->colorSrvAllocation_.gpu);
//		// ルートパラメータ6にカメラViewProjection行列用定数バッファをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(6, viewProjectionResource_->GetGPUVirtualAddress());
//		// ルートパラメータ7に破壊タイルのUV情報用定数バッファをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(7, breakAtlasInfoResource_->GetGPUVirtualAddress());
//		// ルートパラメータ8にベースタイルのUV情報用定数バッファをバインド
//		dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(8, baseAtlasInfoResource_->GetGPUVirtualAddress());
//
//		dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, renderData->currentDrawSum, 0, 0);
//	}
//}

//void DrawSystem::DrawAllDebugLine()
//{
//	// 色ごとに1回ずつ描画（＝色数がDrawCall数になる）
//	for (auto& [color, segments] : debugLineList_)
//	{
//		if (!segments.empty())
//		{
//			// 描画回数上限
//			if (drawCallIndex_ >= kMaxDrawCallPerFrame_) return;
//
//			// (start,end) のペア前提
//			if (segments.size() < 2 || (segments.size() % 2) != 0) return;
//
//			const uint32_t kSumVertex = static_cast<uint32_t>(segments.size());
//
//			// 必要頂点数分確保
//			if (vertexDataUsed_ + kSumVertex > vertexData_.size())
//			{
//				vertexData_.resize(vertexDataUsed_ + kSumVertex);
//			}
//
//			// 頂点詰め
//			for (uint32_t i = 0; i < kSumVertex; ++i)
//			{
//				vertexData_[vertexDataUsed_ + i].position = { segments[i].x, segments[i].y, segments[i].z, 1.0f };
//				vertexData_[vertexDataUsed_ + i].texcoord = { 0.0f, 0.0f };
//				vertexData_[vertexDataUsed_ + i].normal = { 0.0f, 0.0f, -1.0f };
//			}
//
//			// WVP
//			wvpData_[drawCallIndex_]->World = Matrix4x4::MakeIdentity4x4();
//			wvpData_[drawCallIndex_]->WVP = viewProjectionMatrix_;
//
//			// ライト
//			*lightData_[drawCallIndex_] = *directionalLightData_;
//
//			// マテリアル
//			materialData_[drawCallIndex_]->color = ConvertUintToVector4(color);
//			materialData_[drawCallIndex_]->shininess = 1.0f;
//			materialData_[drawCallIndex_]->uvTransform = Matrix4x4::MakeIdentity4x4();
//
//			// Upload（動的VB）
//			if (!EnsureDynamicVB(vertexDataUsed_ + kSumVertex)) return;
//			std::memcpy(vertexMappedPtr_ + vertexDataUsed_, &vertexData_[vertexDataUsed_], sizeof(VertexData) * kSumVertex);
//
//			// VBV
//			D3D12_VERTEX_BUFFER_VIEW vbv{};
//			vbv.BufferLocation = vertexResource_->GetGPUVirtualAddress() + sizeof(VertexData) * static_cast<UINT>(vertexDataUsed_);
//			vbv.SizeInBytes = sizeof(VertexData) * kSumVertex;
//			vbv.StrideInBytes = sizeof(VertexData);
//
//			// 頂点バッファをバインド
//			dxManager_->GetCommandContextManager()->GetCommandList()->IASetVertexBuffers(0, 1, &vbv);
//			// 形状を設定 (線)
//			dxManager_->GetCommandContextManager()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
//			// ルートパラメータ0にマテリアル用定数バッファ（色・ライティング情報など）をバインド
//			dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[drawCallIndex_]->GetGPUVirtualAddress());
//			// ルートパラメータ1にWVP（ワールド・ビュー・プロジェクション）用定数バッファをバインド
//			dxManager_->GetCommandContextManager()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResources_[drawCallIndex_]->GetGPUVirtualAddress());
//			// 描画コマンドの発行
//			dxManager_->GetCommandContextManager()->GetCommandList()->DrawInstanced(kSumVertex, 1, 0, 0);
//
//			// カウンタ更新
//			drawCallIndex_++;
//			vertexDataUsed_ += kSumVertex;
//		}
//	}
//}
