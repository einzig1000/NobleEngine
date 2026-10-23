#include "MiningPointGaugeUI.h"
#include <GameObjects/Character/MiningPointGauge/MiningPointGauge.h>
#include <algorithm>

MiningPointGaugeUI::MiningPointGaugeUI()
{
	// sprites_生成、parts_作成
	BuildLayout();

	// 動的スプライト
	gauge_.render = std::make_unique<RenderObject>();
	gauge_.render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	gauge_.render->psoConfig_.ps = "assets/shaders/UI/MiningPointGauge.PS.hlsl";
	gauge_.render->psoConfig_.depthStencilID = DepthStencilID::Disable;
	gauge_.render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	gauge_.render->SetupFromShaders();
	gaugeParams_.textureIndex = Game::Asset::Texture::Load("assets/application/texture/UI/MiningPointGauge/MiningPointGauge.png");
}

MiningPointGaugeUI::~MiningPointGaugeUI()
{}

void MiningPointGaugeUI::Initialize()
{}

void MiningPointGaugeUI::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);

	if (!miningGauge_) return;

	const int32_t level = miningGauge_->GetLevel();
	const float ratio = miningGauge_->GetGaugeRatio();

	gaugeParams_.fillRatio = std::clamp(ratio, 0.0f, 1.0f);

	// ptが増えた
	const float levelProgress = static_cast<float>(level) + ratio;
	if (levelProgress > prevLevelProgress_)
	{
		Wave& wave = waves_[nextWaveIndex_];
		wave.distance = gaugeParams_.fillRatio + gaugeParams_.waveWidth;
		wave.counter.Initialize(wave.distance / waveSpeed_);
		wave.isActive = true;
		nextWaveIndex_ = (nextWaveIndex_ + 1) % kMaxWaveCount;
	}
	prevLevelProgress_ = levelProgress;

	// 波の先頭の位置
	for (int32_t i = 0; i < kMaxWaveCount; ++i)
	{
		Wave& wave = waves_[i];
		gaugeParams_.waveHeads[i] = -1.0f;
		if (!wave.isActive) continue;

		const float progress = wave.counter.GetProgress();
		if (progress >= 1.0f)
		{
			wave.isActive = false;
			continue;
		}
		gaugeParams_.waveHeads[i] = progress * wave.distance;
	}

	// レベルの文字(LevelTextの位置を数字の中心にする)
	levelText_ = std::to_string(level);
	if (ElementInstanceData* marker = GetPart(Part::LevelText))
	{
		levelTextPos_ = GetDigitTextPosition(levelText_, levelCharSize_, GetCenter(marker->data));
	}

	ElementInstanceData* slotSprite = GetSprite(parts_[static_cast<size_t>(Part::Gauge)], 0);

	// スロットの中心に置く(スロットのscale・回転に合わせる)
	const Vector2 iconSize = Vector2{ slotSprite->data.textureSize.x * slotSprite->data.scale.x, slotSprite->data.textureSize.y * slotSprite->data.scale.y };
	UpdateQuad(gauge_, GetCenter(slotSprite->data), iconSize, slotSprite->data.rotate, orthographic_);

	// 静的スプライトの更新
	UpdateStaticRenderObject(orthographic_);
}

void MiningPointGaugeUI::Draw(int32_t rt_ID)
{
	// 静的スプライトの描画
	DrawStaticRenderObject(rt_ID);

	// 動的スプライトの描画
	gauge_.render->SetBRegisterData(0, ShaderType::VertexShader, &gauge_.wvp);
	gauge_.render->SetBRegisterData(1, ShaderType::VertexShader, &gauge_.world);
	gauge_.render->SetBRegisterData(0, ShaderType::PixelShader, &gaugeParams_);
	gauge_.render->Draw(rt_ID);

	// レベル
	if (!miningGauge_ || levelText_.empty())
	{
		DrawOutlinedString(rt_ID, levelText_, levelCharSize_, levelTextPos_, gaugeParams_.fillColor, 2.0f);
	}
}

void MiningPointGaugeUI::DrawImGui()
{
	ImGui::Begin("MiningPointGaugeUI");

	// UIEditorで保存したあとなどに、データを読み直す
	if (ImGui::Button("UIデータを読み直す"))
	{
		BuildLayout();
	}

	ImGui::ColorEdit4("Fill Color", &gaugeParams_.fillColor.x);
	ImGui::ColorEdit4("Wave Color", &gaugeParams_.waveColor.x);
	ImGui::DragFloat("Wave Width", &gaugeParams_.waveWidth, 0.01f, 0.01f, 1.0f);
	ImGui::DragFloat("Wave Speed", &waveSpeed_, 0.05f, 0.1f, 10.0f);
	ImGui::DragInt("Level Char Size", &levelCharSize_, 1.0f, 8, 128);

	ImGui::End();
}

void MiningPointGaugeUI::BuildLayout()
{
	BuildSprites(UIElementType::MiningPointGauge);
	parts_ = FindParts<Part>();
}

ElementInstanceData* MiningPointGaugeUI::GetPart(Part part, int32_t labelIndex)
{
	return GetSprite(parts_[static_cast<size_t>(part)], labelIndex);
}