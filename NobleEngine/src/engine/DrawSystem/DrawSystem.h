#pragma once
#include "definition/definition.h"

#include "DrawSystem/RenderData/RenderObject.h"
#include "DirectX/FrameCbAllocator/FrameCbAllocator.h"

class DirectXManager;
class ResourceManager;

/// <summary>
/// 描画管理クラス
/// </summary>
class DrawSystem
{
private:
	static constexpr uint32_t kFramesInFlight_ = 2;
	FrameCbAllocator cbAllocators_[kFramesInFlight_]{};

	uint32_t GetFrameIndex() const;

public:
	DrawSystem(DirectXManager* dxManager, ResourceManager* resourceManager);
	~DrawSystem();
	void Reset();
	void SceneDraw();
	void ScreenDraw();

	// テスト用の動作確認関数。
	void DrawRenderObject();

	void AddDrawList(const RenderObject* renderObject);

	void AddDebugLineList(const Vector3& start, const Vector3& end, uint32_t color);
	void AddSphere(const Sphere& sphere, uint32_t color);
	void AddSphereXYZ(const SphereXYZ& sphere, uint32_t color);
	void AddCylinder(const Cylinder& cylinder, uint32_t color);
	void AddAABB(const AABB& aabb, uint32_t color);

private:
	std::unordered_map<uint32_t, std::vector<Vector3>> debugLineList_{};

	std::vector<const RenderObject*> renderObjects_{};

	// DirectXマネージャー
	DirectXManager* dxManager_ = nullptr;
	ResourceManager* resourceManager_ = nullptr;

	// 現フレームで描画されている頂点数(モデルは除く)
	size_t vertexDataUsed_ = 0;

	// 描画コールカウント
	size_t drawCallIndex_ = 0;

	// 1フレームに呼び出せる描画コールの最大数
	size_t kMaxDrawCallPerFrame_ = 1024;

	// スクリーン描画用の
};

