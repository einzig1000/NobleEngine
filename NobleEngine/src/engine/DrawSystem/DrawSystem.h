#pragma once
#include <definition/definition.h>

#include <DrawSystem/RenderData/RenderObject.h>
#include <DirectX/FrameCbAllocator/FrameCbAllocator.h>
#include <DirectX/RenderTextureManager/RenderTextureID.h>

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

	enum class RenderPassType
	{
		Scene,
		PostEffect
	};

public:
	DrawSystem(DirectXManager* dxManager, ResourceManager* resourceManager);
	~DrawSystem();
	void Reset();
	// RenderTextureにオブジェクトを描画する
	void SceneDraw();
	// 完成したRenderTextureを加工して新たなRenderTextureに描画する。
	void PostEffectDraw();
	// preBackBufferのレンダーテクスチャをバックバッファに描画する。
	void ScreenDraw();

	void AddSceneDrawList(const RenderObject* renderObject, RenderTextureID renderTextureID);
	void AddScreenDrawList(const RenderObject* renderObject, RenderTextureID renderTextureID);

	void AddDebugLineList(const Vector3& start, const Vector3& end, uint32_t color);
	void AddSphere(const Sphere& sphere, uint32_t color);
	void AddSphereXYZ(const SphereXYZ& sphere, uint32_t color);
	void AddCylinder(const Cylinder& cylinder, uint32_t color);
	void AddAABB(const AABB& aabb, uint32_t color);

private:
	// 書き込みたいRenderTextureを指定してResourceのStateをD3D12_RESOURCE_STATE_RENDER_TARGETに遷移させる。
	void BeginRenderPass(RenderTextureID renderTextureID, RenderPassType passType);
	// SetRenderTargetで変えたResouruceのStateをD3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEに遷移させる。
	void EndRenderPass(RenderTextureID renderTextureID, RenderPassType passType);
	// 描画オブジェクトをRenderTextureに描画する。
	void DrawObject(const RenderObject* renderObject);

	// <書き込み先RenderTextureID & 描画オブジェクト配列> マップ
	std::unordered_map<RenderTextureID, std::vector<const RenderObject*>> sceneRenderObjects_{};
	std::unordered_map<RenderTextureID, std::vector<const RenderObject*>> screenRenderObjects_{};

	std::unordered_map<uint32_t, std::vector<Vector3>> debugLineList_{};

	PSOConfig ScreenDrawPSOConfig_;
	std::vector<RootParam> ScreenDrawRootParams_;

	// DirectXマネージャー
	DirectXManager* dxManager_ = nullptr;
	ResourceManager* resourceManager_ = nullptr;
};

