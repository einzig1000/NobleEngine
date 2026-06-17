#pragma once
#include <GameManager/Phase/IPhase/IPhase.h>
#include <TestAnimation/TestAnimation.h>
#include <TestParticle/TestParticle.h>
#include <TestMeshShader/TestMeshShader.h>
#include <memory>

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

	void DrawDebugInfo();

private:
	std::unique_ptr<RenderObject> cbvOnly_;
	EulerTransform transform1_;
	Vector4 color1_;
	std::unique_ptr<RenderObject> cbvAndSrv_;
	EulerTransform transform2_[10];
	Vector4 color2_[10];
	int32_t tex2_[10];
	std::unique_ptr<RenderObject> line_;
	std::unique_ptr<RenderObject> skybox_;
	std::unique_ptr<RenderObject> PunctualLight_;
	LightDataForGPU lightData_;
	Material materialData_;
	std::unique_ptr<RenderObject> environmentMap_;

	std::unique_ptr<RenderObject> postEffect1_;
	std::unique_ptr<RenderObject> postEffect2_;
	std::unique_ptr<RenderObject> postEffect3_;
	std::unique_ptr<RenderObject> postEffect4_;


	std::unique_ptr<RenderObject> screenDrawObjectMain1_;
	EulerTransform main1ScreenTransform_;
	std::unique_ptr<RenderObject> screenDrawObjectMain2_;
	EulerTransform main2ScreenTransform_;
	std::unique_ptr<RenderObject> screenDrawObjectMiniMap1_;
	EulerTransform miniMap1ScreenTransform_;
	std::unique_ptr<RenderObject> screenDrawObjectMiniMap2_;
	EulerTransform miniMap2ScreenTransform_;


	std::unique_ptr<TestAnimation> testAnimation_;
	std::unique_ptr<TestParticle> testParticle_;
	std::unique_ptr<TestMeshShader> testMeshShader_;

	int32_t rt_main1_;
	int32_t rt_main1_depth_;
	int32_t rt_main2_;
	int32_t rt_main2_depth_;
	int32_t rt_miniMap1_;
	int32_t rt_miniMap1_depth_;
	int32_t rt_miniMap2_;
	int32_t rt_miniMap2_depth_;

	int32_t rt_Vignette_;
	int32_t rt_GrayScale_;
	int32_t rt_luminanceBasedOutline_;
	int32_t rt_depthBasedOutline_;

	int32_t t_uvChecker;
	int32_t t_monsterBall_;
	int32_t t_white1x1_;
	int32_t t_dds_;

	uint32_t audio1;
	uint32_t audio2;
};