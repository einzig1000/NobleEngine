#pragma once
#include <GameManager/Phase/IPhase.h>
#include <TestAnimation/TestAnimation.h>
#include <TestParticle/TestParticle.h>
#include <memory>

struct RenderTargetData
{
	// 名前
	std::string name;
	// サイズ
	uint32_t width = 1280;
	uint32_t height = 720;
	// 参照するレンダーテクスチャ
	std::vector<std::string> in;
	// 書きだすレンダーテクスチャ
	std::vector<std::string> out;
};

struct CameraData
{
	// 名前
	std::string name;
	// サイズ
	Vector2 size;
	// 位置
	Vector3 center;
	// 回転
	Vector3 rotation;
	// 距離
	float distance = 20.0f;
	// 画角
	float fov = 0.45f;
};

struct RenderObjectData
{
	// 名前
	std::string name;
	// psoConfig
	GraphicsPSOConfig config;
	// 書きだすレンダーテクスチャ
	std::vector<std::string> out;
	// モデルパス
	std::string modelPath;
};

class TestPhase :
	public IPhase
{
public:
	TestPhase();
	~TestPhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }

	void Load_renderTarget();
	void Save_renderTarget();

	void Load_camera();
	void Save_camera();

	void Load_renderObject();
	void Save_renderObject();


	void DrawDebugInfo();

private:
	std::unique_ptr<RenderObject> cbvOnly_;
	EulerTransforms transform1_;
	Vector4 color1_;
	std::unique_ptr<RenderObject> cbvAndSrv_;
	EulerTransforms transform2_[10];
	Vector4 color2_[10];
	int32_t tex2_[10];
	std::unique_ptr<RenderObject> line_;
	std::unique_ptr<RenderObject> skybox_;
	std::unique_ptr<RenderObject> PunctualLight_;
	LightDataForGPU lightData_;
	Material materialData_;
	std::unique_ptr<RenderObject> environmentMap_;



	std::unique_ptr<RenderObject> screenDrawObjectMain1_;
	EulerTransforms main1ScreenTransform_;
	std::unique_ptr<RenderObject> screenDrawObjectMain2_;
	EulerTransforms main2ScreenTransform_;
	std::unique_ptr<RenderObject> screenDrawObjectMiniMap1_;
	EulerTransforms miniMap1ScreenTransform_;
	std::unique_ptr<RenderObject> screenDrawObjectMiniMap2_;
	EulerTransforms miniMap2ScreenTransform_;


	std::unique_ptr<TestAnimation> testAnimation_;
	std::unique_ptr<TestParticle> testParticle_;

	std::unordered_map<int32_t, RenderTargetData> renderTargetData_;
	std::unordered_map<int32_t, CameraData> cameraData_;
	std::vector<std::unique_ptr<RenderObject>> remderObjects_;
	std::vector<RenderObjectData> renderObjectData_;

	int32_t rt_main1_depth_;
	int32_t rt_main2_depth_;
	int32_t rt_miniMap1_depth_;
	int32_t rt_miniMap2_depth_;

	int32_t t_uvChecker;
	int32_t t_monsterBall_;
	int32_t t_white1x1_;
	int32_t t_dds_;

	uint32_t audio1;
	uint32_t audio2;
};