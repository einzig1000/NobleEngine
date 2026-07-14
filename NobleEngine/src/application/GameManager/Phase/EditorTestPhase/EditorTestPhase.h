#pragma once
#include <GameManager/Phase/IPhase.h>
#include <memory>

class EditorTestPhase : public IPhase
{
public:
	EditorTestPhase();
	~EditorTestPhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }

	void DrawDebugInfo();

private:
	std::vector<std::unique_ptr<RenderObject>> renderObjects_;
	std::vector<EulerTransforms> transforms_;
	std::vector<Vector4> colors_;

	std::unique_ptr<RenderObject> postEffect1_;

	std::unique_ptr<RenderObject> screenDrawObjectMain1_;
	EulerTransforms main1ScreenTransform_;

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

	int32_t c_worldView_;
	int32_t c_main1_;
	int32_t c_main2_;
	int32_t c_miniMap1_;
	int32_t c_miniMap2_;

	uint32_t audio1;
	uint32_t audio2;
};