#pragma once
#include <GameManager/Phase/IPhase.h>

class MapManager;
class SkyBox;
class CameraController;
class ScreenDrawer;

enum class Stage
{
	None,
	// タイトル画面
	Title,
	// ワールドセレクトとクレジット
	Menu,
	// ワールドセレクト
	WorldSelect,
	// 新規作成
	CreateNewWorld,
};

enum class Button
{
	TiTleLogo,
	WorldSelect,
	Credit,
	Back,
	CreateNewWorld,
};

struct ButtonInfo
{
	// ボタンラベル
	std::string label;
	// ボタンに表示するテキスト
	std::string text;

	// テキストが書かれたテクスチャのID
	int32_t textureID = -1;
	// 文字サイズ
	int32_t charSize = 0;
	// 文字開始位置
	Vector2 textPos;
	// 文字間隔
	float textSpace = 0.0f;
	// テキストの色
	Vector4 textColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);


	// テクスチャの色
	Vector4 textureColor = Vector4(0.11f, 0.11f, 0.11f, 1.0f);
	
	EulerTransforms transforms;
	Matrix4x4 worldMatrix;
	std::unique_ptr<RenderObject> render_;

	std::function<void()> onClick;
};

class TitlePhase :
	public IPhase
{
public:
	TitlePhase();
	~TitlePhase() override;

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }


private:
	// ボタンデータ読み込み
	void LoadButtonData();


	void OnClickButton(std::string buttonLabel);

	void MoveCamera(Stage stage, float duration);

	// カメラ
	std::unique_ptr<CameraController> cameraController_;
	int32_t c_title_ = -1;
	// 描画マネージャ
	std::unique_ptr<ScreenDrawer> screenDrawer_;

	// マップ
	std::unique_ptr<MapManager> map_;
	std::unique_ptr<SkyBox> skyBox_;

	std::vector<ButtonInfo> buttons_;
	std::string buttonJsonPath_ = "assets/application/json/TitleButtons.json";
	const ModelData* buttonColliderModel_ = nullptr;
	int32_t buttonColliderModelID_ = -1;
	int32_t buttonModelID_ = -1;

	int32_t selectedButtonIndex_ = -1;
};