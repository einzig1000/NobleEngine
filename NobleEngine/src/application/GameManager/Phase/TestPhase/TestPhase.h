#pragma once
#include "GameManager/Phase/PhaseParent/PhaseParent.h"
#include <memory>

class TestPhase :
	public PhaseParent
{
public:
	TestPhase();
	~TestPhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }



private:
	std::unique_ptr<RenderObject> cbvOnly_;
	Transforms transform1_;
	Vector4 color1_;
	std::unique_ptr<RenderObject> cbvAndSrv_;
	Transforms transform2_[10];
	Vector4 color2_[10];
	int32_t tex2_[10];
	std::unique_ptr<RenderObject> line_;
	std::unique_ptr<RenderObject> skybox_;
	std::unique_ptr<RenderObject> PunctualLight_;
	LightDataForGPU lightData_;
	Material materialData_;
	std::unique_ptr<RenderObject> environmentMap_;
	std::unique_ptr<RenderObject> postEffect_;
	Transforms postEffectTransform_;

	uint32_t audio1;
	uint32_t audio2;
};