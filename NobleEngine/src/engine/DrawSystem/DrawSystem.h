#pragma once
#include <definition/definition.h>
#include <definition/constexprs.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <DirectX/FrameCbAllocator/FrameCbAllocator.h>
#include <memory>

class DirectXManager;
class AssetManager;


/// <summary>
/// 描画管理クラス
/// </summary>
class DrawSystem
{
private:
	FrameCbAllocator cbAllocators_[Constexprs::kFrameCount]{};

public:
	DrawSystem(DirectXManager* dxManager, AssetManager* resourceManager);
	~DrawSystem();
	void Reset();
	// 指定したレンダーテクスチャに書き込むフェーズ
	void SceneDraw();
	// 書き込み終わったレンダーテクスチャにポストエフェクトをかけるフェーズ
	void PostEffectDraw();
	// 加工が終わったレンダーテクスチャを利用して最終レンダーテクスチャに書き込むフェーズ
	void PreScreenDraw();
	// 最終レンダーテクスチャをバックバッファに書き込むフェーズ
	void ScreenDraw();

	void AddSceneDrawList(const RenderObject* renderObject, int32_t renderTextureID);
	void AddPostEffectDrawList(const RenderObject* renderObject, int32_t renderTextureID);
	void AddScreenDrawList(const RenderObject* renderObject);


	void AddDebugLineList(const Vector3& start, const Vector3& end, uint32_t color);
	void AddSphere(const Sphere& sphere, uint32_t color);
	void AddSphereXYZ(const SphereXYZ& sphere, uint32_t color);
	void AddCylinder(const Cylinder& cylinder, uint32_t color);
	void AddAABB(const AABB& aabb, uint32_t color);

private:
	// 描画オブジェクトをRenderTextureに描画する。
	void DrawObject(const RenderObject* renderObject);

	// <書き込み先RenderTextureID & 描画オブジェクト配列> マップ
	std::unordered_map<int32_t, std::vector<const RenderObject*>> sceneRenderObjects_{};
	std::unordered_map<int32_t, std::vector<const RenderObject*>> postEffectRenderObjects_{};
	std::vector<const RenderObject*> screenRenderObjects_{};

	std::unordered_map<uint32_t, std::vector<Vector3>> debugLineList_{};

	int32_t rt_nobleScreenID_ = -1;

	std::unique_ptr<RenderObject> screenRenderObject_ = nullptr;

	PSOConfig ScreenDrawPSOConfig_;
	std::vector<RootParam> ScreenDrawRootParams_;

	// DirectXマネージャー
	DirectXManager* dxManager_ = nullptr;
	AssetManager* resourceManager_ = nullptr;
};

