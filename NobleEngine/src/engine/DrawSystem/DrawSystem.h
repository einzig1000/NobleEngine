#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <DrawSystem/RenderObject/RenderObject.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <set>

struct DrawNode
{
	std::vector<const RenderObject*> objects;	// このRTに書き込むオブジェクト群
	std::set<int32_t> deps;						// このRTに書き込む前に描画完了していてほしいRT群
};

class DirectXManager;
class AssetManager;
class RootBindingManager;

/// <summary>
/// 描画管理クラス
/// </summary>
class DrawSystem
{
public:
	DrawSystem(DirectXManager* dxManager, AssetManager* assetManager, RootBindingManager* rootBindingManager);
	~DrawSystem();
	void Reset();
	// 最終レンダーテクスチャをバックバッファに書き込むフェーズ
	void ScreenDraw();

	void AddDrawList(const RenderObject* renderObject, int32_t RenderTargetID, const std::vector<int32_t>& deps);
	void Execute();

	D3D12_GPU_VIRTUAL_ADDRESS GetCurrentFrameCbGpuAddress(size_t sizeBytes, const void* data);

private:
	std::vector<int32_t> SortNodes(); // トポロジカルソート＋循環検出
	// <書き込み先RenderTextureID, DrawNode>マップ
	std::unordered_map<int32_t, DrawNode> drawNodes_{};

private:
	DirectXManager* dxManager_ = nullptr;
	AssetManager* assetManager_ = nullptr;
	RootBindingManager* rootBindingManager_ = nullptr;
	UINT backBufferIndex_ = 0;

	// 描画オブジェクトをRenderTextureに描画する。
	void DrawObject(const RenderObject* renderObject);

	// NobleScreen用のRenderObject
	std::unique_ptr<RenderObject> screenRenderObject_ = nullptr;
	int32_t rt_nobleScreenID_ = -1;
};

