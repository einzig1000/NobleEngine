#pragma once
#include <Game.h>
#include <GameObjects/UI/UIElement/IUIElement.h>
#include <array>
#include <string>

class MiningPointGaugeUI : public IUIElement
{
public:
	enum class Part
	{
		Gauge,
		LevelText,
	};

public:
	MiningPointGaugeUI();
	~MiningPointGaugeUI() override;
	void Initialize() override;
	void Update(int32_t cameraID) override;
	void Draw(int32_t rt_ID) override;
	void DrawImGui() override;

private:
	// 同時に出せる波の数。MiningPointGauge.PS.hlslのGaugeParams参照
	static constexpr int32_t kMaxWaveCount = 8;

	// MiningPointGauge.PS.hlslのGaugeParams参照
	struct GaugeParams
	{
		// 溜まった部分の色
		Vector4 fillColor = Vector4{ 0.216f, 1.0f, 0.015f, 1.0f };
		// 波の色(aが強さ)
		Vector4 waveColor = Vector4{ 1.0f, 1.0f, 1.0f, 0.8f };
		// ゲージのテクスチャ
		int32_t textureIndex = -1;
		// 溜まっている割合(0〜1)
		float fillRatio = 0.0f;
		// 波の尾の長さ(0〜1)
		float waveWidth = 0.15f;
		// 16バイトにそろえるための詰め物
		float padding = 0.0f;
		// 波の先頭の位置(0〜1)。負なら波なし。HLSL側では float4 waveHeads[2]
		float waveHeads[kMaxWaveCount] = { -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f };
	};

	// 1本の波
	struct Wave
	{
		// 出てからの経過(1で流れきる)
		CounterSec counter;
		// 流れきるまでの距離(出たときのfillRatio + waveWidth)
		float distance = 0.0f;
		bool isActive = false;
	};

private:
	Matrix4x4 orthographic_;

	// Jsonのデータからsprites_を作る
	void BuildLayout();
	// PartのlabelIndex番目のスプライト
	ElementInstanceData* GetPart(Part part, int32_t labelIndex = 0);

	// parts_[パーツ][labelIndex] = sprites_のインデックス
	std::array<std::vector<int32_t>, magic_enum::enum_count<Part>()> parts_;



	ElementInstanceData gauge_;
	GaugeParams gaugeParams_;

	// 波
	std::array<Wave, kMaxWaveCount> waves_;
	int32_t nextWaveIndex_ = 0;
	// 波の速さ(1秒に進む距離。ゲージ全体が1)
	float waveSpeed_ = 2.0f;
	// 前のフレームの「レベル+割合」(増えたら波を出す)
	float prevLevelProgress_ = 0.0f;

	// レベルの文字
	std::string levelText_;
	Vector2 levelTextPos_;
	int32_t levelCharSize_ = 46;
};