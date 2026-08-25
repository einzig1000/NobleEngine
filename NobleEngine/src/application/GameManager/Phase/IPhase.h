#pragma once
#include <Game.h>
#include <definition/definition.h>
#include <definition/constexprs.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <ImGuiManager/ImGuiManager.h>
#include <memory>

// フェーズ間
struct PhaseContext
{
	std::string mapName = "default";
	bool isNewGame = false;
	uint32_t seed = 123456;
};

class IPhase
{
public:
	virtual ~IPhase() = default;

	void SetContext(PhaseContext* context) { context_ = context; }

	virtual void Initialize() = 0;
	virtual	void Update() = 0;
	virtual	void Draw() = 0;
	virtual void DrawImGui() = 0;

	virtual void ChangePhase(PHASE phase);
	virtual PHASE GetNextPhase();

protected:
	PHASE nextPhase_ = PHASE::Phase_None;

	PhaseContext* context_ = nullptr;
};

