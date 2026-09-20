#pragma once
#include <GameManager/Phase/IPhase.h>
#include <Test/TestParticle/Particle.h>

// VS b0 : 192 byte
struct WaterTransformCB
{
	Matrix4x4 world;
	Matrix4x4 worldInverseTranspose;
	Matrix4x4 viewProjection;
};

// VS b1 : 112 byte
struct WaterWaveCB
{
	// 大きくするとその軸の波が細かく多くなる
	Vector4 relativeScale{ 1.0f, 1.0f, 1.0f, 0.0f };
	// 波専用の座標軸。いじった人間から殺していく
	Vector4 waveAxisX{ 1.0f, 0.0f, 0.0f, 0.0f };
	Vector4 waveAxisY{ 0.0f, 1.0f, 0.0f, 0.0f };
	Vector4 waveAxisZ{ 0.0f, 0.0f, 1.0f, 0.0f };
	// 水面の高さ。手動でいじるものではない。
	float surfaceY = 0.0f;
	// 実質波の高さ。
	float commonWorldScale = 1.0f;
	// 
	float sideWaveDepth = 0.02f;
	// 真・波の高さ。大きくするとその軸の波が多くなる
	float waveHeight = 0.04f;
	// 波の周波数。
	float waveFrequency = 10.000f;
	// 波のスピード。実質timeScale
	float waveSpeed = 2.0f;
	// ↓の倍率。
	float motionHeightBoost = 2.0f;
	// グラスが今どれだけ激しく動いているか
	float motionIntensity = 0.0f;
	// 時間。motionWaveTime += deltaTimeMs * 0.001f;
	float motionWaveTime = 0.0f;
	float pad0 = 0.0f, pad1 = 0.0f, pad2 = 0.0f;
};

// PS b0 : 16 byte
struct WaterCameraCB
{
	Vector3 cameraPositionWS{};
	float   pad = 0.0f;
};

// PS b1 : 96 byte
struct WaterColorCB
{
	Vector4 colorA{ 1.0f, 0.0f, 0.0f, 0.65f };
	Vector4 colorB{ 0.0f, 0.0f, 1.0f, 0.65f };
	Vector4 baseColor{ 1.0f, 1.0f, 1.0f, 0.65f };

	float colorBalance = 0.5f;
	float colorBlendWidth = 0.25f;
	float colorDistortion = 0.30f;
	float colorPatternScale = 1.0f;

	float convectionSpeed = 1.0f;
	float convectionStrength = 0.4f;
	float convectionScale = 8.0f;
	float mixProgress = 0.0f;

	float motionIntensity = 0.0f;
	float motionWaveTime = 0.0f;
	float smoothness = 0.8f;
	float fresnelStrength = 0.3f;   // 0で元Unity版と同じ
};

// PS b2 : 64 byte
struct WaterLightingCB
{
	Vector4 mainLightDirection{ 0.0f, 1.0f, 0.0f, 0.0f };
	Vector4 mainLightColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 ambientSky{ 0.25f, 0.30f, 0.38f, 1.0f };
	Vector4 ambientGround{ 0.06f, 0.05f, 0.05f, 1.0f };
};

static_assert(sizeof(WaterTransformCB) == 192);
static_assert(sizeof(WaterWaveCB) == 112);
static_assert(sizeof(WaterCameraCB) == 16);
static_assert(sizeof(WaterColorCB) == 96);
static_assert(sizeof(WaterLightingCB) == 64);

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
	void ChangePhase(Phase phase) override { nextPhase_ = phase; }

private:
	// カメラID
	int32_t c_main_ = -1;
	// モデルID
	int32_t m_cube_ = -1;
	int32_t m_plane_ = -1;
	int32_t m_human_ = -1;
	int32_t m_water_ = -1;
	std::vector<int32_t> m_alcohol_;
	// テクスチャID
	int32_t t_uvChecker_ = -1;
	int32_t t_monsterBall_ = -1;
	std::vector<int32_t> t_alcohol_;
	// アニメーションID
	int32_t a_sneakWalk_ = -1;
	// サウンドID
	int32_t s_aura_ = -1;

	void Initialize_SimpleModels();
	void Update_SimpleModels();
	void Draw_SimpleModels();
	void DrawImGui_SimpleModels();
	std::unique_ptr<RenderObject> simpleModels_;
	int32_t instanceCount_ = 10;
	// ディスクリプタヒープスロット
	int32_t worldMatrixHeapSlot_ = -1;
	int32_t colorHeapSlot_ = -1;
	int32_t textureIndexHeapSlot_ = -1;
	std::vector<EulerTransforms> transforms_;
	std::vector<Matrix4x4> worldMatrices_;
	std::vector<Vector4> colors_;
	std::vector<int32_t> textureIndices_;


	void Initialize_Sprite();
	void Update_Sprite();
	void Draw_Sprite();
	void DrawImGui_Sprite();
	std::unique_ptr<RenderObject> sprite_;
	EulerTransforms spriteTransform_;
	Vector4 spriteColor_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	int32_t spriteTexture_ = -1;


	void Initialize_Animation();
	void Update_Animation();
	void Draw_Animation();
	void DrawImGui_Animation();
	std::unique_ptr<RenderObject> animation_;
	std::unique_ptr<ComputeObject> animationCompute_;
	float animationTime_ = 0.0f;
	int32_t resultHeapSlot_ = -1;
	uint32_t numVertices_ = 0;
	SkinInstance skinInstance_;
	const ModelData* modelData_ = nullptr;


	void Initialize_Water();
	void Update_Water();
	void Draw_Water();
	void DrawImGui_Water();
	std::unique_ptr<RenderObject> water_;
	EulerTransforms waterTransform_;
	WaterTransformCB waterTransformCB_;
	WaterWaveCB waterWaveCB_;
	WaterCameraCB waterCameraCB_;
	WaterColorCB waterColorCB_;
	WaterLightingCB waterLightingCB_;


	Particle particle_;
};