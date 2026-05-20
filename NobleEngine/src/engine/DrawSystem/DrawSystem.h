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

	void AddSceneDrawList(const RenderObject* renderObject);
	void AddScreenDrawList(const RenderObject* renderObject);

	void AddDebugLineList(const Vector3& start, const Vector3& end, uint32_t color);
	void AddSphere(const Sphere& sphere, uint32_t color);
	void AddSphereXYZ(const SphereXYZ& sphere, uint32_t color);
	void AddCylinder(const Cylinder& cylinder, uint32_t color);
	void AddAABB(const AABB& aabb, uint32_t color);

private:
	std::unordered_map<uint32_t, std::vector<Vector3>> debugLineList_{};

	std::vector<const RenderObject*> sceneRenderObjects_{};
	std::vector<const RenderObject*> screenRenderObjects_{};

	// DirectXマネージャー
	DirectXManager* dxManager_ = nullptr;
	ResourceManager* resourceManager_ = nullptr;
};

