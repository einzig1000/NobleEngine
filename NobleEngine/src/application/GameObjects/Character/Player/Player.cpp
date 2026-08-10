#include "Player.h"
#include <GameObjects/Character/SwingMining/SwingMining.h>
#include <GameObjects/Character/RangeMining/RangeMining.h>

Player::Player()
{
	// プレイヤーデータ初期化
	render_.modelID_ = Game::Asset::Model::Load("resources/prototypes/model/cube/cube.obj");
	ModelData* modelData = Game::Asset::Model::GetData(render_.modelID_);
	SetBoundingBox(modelData->aabb[0]);
	render_.psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
	render_.psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_.SetupFromShaders();

	c_viewCameraID_ = Game::Camera::AddCamera("PlayerView");
	Game::Camera::Setter::SetDistance(0.1f, 0, EaseType::IN_BACK, c_viewCameraID_);

	swingMining_ = std::make_unique<SwingMining>(this);
	rangeMining_ = std::make_unique<RangeMining>(this);
}

Player::~Player()
{}

void Player::Initialize()
{
	translate_.value = Vector3(0.0f, 20.0f, 0.0f);
	translate_.velocity = Vector3(0.0f, -0.0f, 0.0f);
	translate_.acceleration = Vector3(0.0f, Constexprs::GRAVITY, 0.0f);

	scale_.value = Vector3(1.0f, 1.0f, 1.0f);
	scale_.velocity = Vector3(0.0f, 0.0f, 0.0f);
	scale_.acceleration = Vector3(0.0f, 0.0f, 0.0f);

	rotate_.value = Vector3(0.0f, 0.0f, 0.0f);
	rotate_.velocity = Vector3(0.0f, 0.0f, 0.0f);
	rotate_.acceleration = Vector3(0.0f, 0.0f, 0.0f);

	RegisterToMap();

	AddItem(ItemID::Tool_Pickel_of_Iron);
}

void Player::Update(int32_t cameraID)
{
	previousHP_ = static_cast<float>(HP_);

	// 移動系入力処理
	UpdateInput(cameraID);
	// マップ情報を見て適切に移動を適用する
	ApplyMove();

	// 移動後の視線レイ更新
	ComputeViewRay();

	// ターゲットブロック取得
	SetTargetBlock();

	// 左クリックで起きるイベント更新(ブロック破壊とか)
	UpdateLeftClick();
	// 右クリックで起きるイベント更新(ブロック設置とか)
	UpdateRightClick();

	// 持ってるアイテムの更新
	UpdateHaveItem(cameraID);

	// 採掘モードに応じて処理を切り替え
	switch (miningMode_)
	{
	case MiningPattern::Swing:
		swingMining_->Update();
		break;
	case MiningPattern::Range:
		rangeMining_->Update();
		break;
	default:
		break;
	}

	Game::Camera::Setter::SetCenter(translate_.value, 0, EaseType::IN_BACK, c_viewCameraID_);

	Matrix4x4 wvpMatrix = worldMatrix_ * Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	int32_t texID = 2;

	render_.SetCBufferData(0, ShaderType::VertexShader, &wvpMatrix);
	render_.SetCBufferData(1, ShaderType::VertexShader, &worldMatrix_);
	render_.SetCBufferData(0, ShaderType::PixelShader, &color);
	render_.SetCBufferData(1, ShaderType::PixelShader, &texID);
}

void Player::UpdateLeftClick()
{
	if (!Game::IO::Mouse::IsHeld(0)) return;

	//DestroyBlockInAABB(aabb);
}

void Player::UpdateRightClick()
{
	if (!Game::IO::Mouse::IsJustPressed(1)) return;
}




void Player::Draw(int32_t renderTextureID)
{
	// プレイヤー描画
	//render_.Draw(renderTextureID);

	DrawHaveItem(renderTextureID);
}

void Player::DrawImGui()
{
	ImGui::Begin("Player Info");
	ImGui::DragFloat3("Position", &translate_.value.x, 1.0f);
	ImGui::DragFloat3("Scale", &scale_.value.x, 0.1f);
	ImGui::DragFloat3("Velocity", &translate_.velocity.x, 1.0f);

	// モード選択UIがまだ無いので暫定でImGuiから切り替える
	if (ImGui::RadioButton("Swing", miningMode_ == MiningPattern::Swing))
	{
		SetMiningPattern(MiningPattern::Swing);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Range", miningMode_ == MiningPattern::Range))
	{
		SetMiningPattern(MiningPattern::Range);
	}
	if (miningMode_ == MiningPattern::Range)
	{
		ImGui::Text(rangeMining_->HasStartPoint() ? "Range: waiting for 2nd point" : "Range: waiting for 1st point");
	}

	ImGui::End();
}

void Player::SetMiningPattern(MiningPattern pattern)
{
	// Range切り替え時は選択途中の状態を持ち越さない
	if (pattern == MiningPattern::Range && miningMode_ != MiningPattern::Range)
	{
		rangeMining_->Reset();
	}
	miningMode_ = pattern;
}


void Player::UpdateInputSpeed()
{
	if (dash_ && Game::IO::Key::IsJustReleased('W'))
	{
		dash_ = false;
		speed_ = normalSpeed_;
	}

	// 10フレーム以内の単タップを検知したら
	if (Game::IO::Key::TestTapLong('W', 10))
	{
		dashBufferTimer_ = 30;
	}
	else if (dashBufferTimer_ > 0)
	{
		dashBufferTimer_--;

		if (Game::IO::Key::IsJustPressed('W'))
		{
			dash_ = true;
			speed_ = dashSpeed_;
			dashBufferTimer_ = 0;
		}
	}
}
void Player::UpdateInputMove(int32_t cameraID)
{
	// 移動処理
	Vector2 input(0.0f, 0.0f);

	if (Game::IO::Key::IsHeld('W')) input.y += 1.0f;
	if (Game::IO::Key::IsHeld('S')) input.y -= 1.0f;
	if (Game::IO::Key::IsHeld('A')) input.x += 1.0f;
	if (Game::IO::Key::IsHeld('D')) input.x -= 1.0f;

	// 移動方向ベクトル
	Vector3 moveDir = Vector3(0.0f, 0.0f, 0.0f);

	if (input.x != 0.0f || input.y != 0.0f)
	{
		Vector3 cameraPos = Game::Camera::Getter::GetTranslate(cameraID);
		Vector3 cameraCenter = Game::Camera::Getter::GetCenter(cameraID);
		Vector3 cameraDir = cameraCenter - cameraPos;
		cameraDir.y = 0.0f;
		cameraDir.Normalize();

		// 正規化
		input.Normalize();

		// 入力ベクトルの角度（ラジアン）
		float angle = std::atan2(input.x, input.y); // XZ平面での回転

		// forward を angle だけ回転
		float cosA = std::cos(angle);
		float sinA = std::sin(angle);

		// 進む方向ベクトル
		moveDir = Vector3(
			cameraDir.x * cosA - cameraDir.z * sinA,
			cameraDir.y,
			cameraDir.x * sinA + cameraDir.z * cosA
		);

		moveDir.Normalize();
	}
	Move(moveDir, speed_);
}
void Player::UpdateInputJump()
{
	// ジャンプ処置
	if (Game::IO::Key::IsJustPressed(VK_SPACE))
	{
		Jump();
	}
}

void Player::UpdateInput(int32_t cameraID)
{
	UpdateInputSpeed();
	UpdateInputJump();
	UpdateInputMove(cameraID);
}
