#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <RootBinding/FrameCbAllocator/FrameCbAllocator.h>
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

/// <summary>
/// 描画管理クラス
/// </summary>
class DrawSystem
{
public:
	DrawSystem(DirectXManager* dxManager, AssetManager* assetManager);
	~DrawSystem();
	void Reset();
	// 最終レンダーテクスチャをバックバッファに書き込むフェーズ
	void ScreenDraw();

	void AddDrawList(const RenderObject* renderObject, int32_t RenderTargetID, const std::vector<int32_t>& deps);
	void Execute(); // 旧SceneDraw/PostEffectDraw/PreScreenDrawをこれ1つに統合

private:
	std::vector<int32_t> SortNodes(); // トポロジカルソート＋循環検出
	// <書き込み先RenderTextureID, DrawNode>マップ
	std::unordered_map<int32_t, DrawNode> drawNodes_{};

private:
	DirectXManager* dxManager_ = nullptr;
	AssetManager* assetManager_ = nullptr;

	// 描画オブジェクトをRenderTextureに描画する。
	void DrawObject(const RenderObject* renderObject);

	// <書き込み先RenderTextureID & 描画オブジェクト配列> マップ
	//std::unordered_map<int32_t, std::vector<const RenderObject*>> sceneRenderObjects_{};
	//std::unordered_map<int32_t, std::vector<const RenderObject*>> postEffectRenderObjects_{};
	//std::vector<const RenderObject*> screenRenderObjects_{};

	// NobleScreen用のRenderObject
	std::unique_ptr<RenderObject> screenRenderObject_ = nullptr;
	int32_t rt_nobleScreenID_ = -1;

	FrameCbAllocator cbAllocators_[Constexprs::kFrameCount]{};
};

