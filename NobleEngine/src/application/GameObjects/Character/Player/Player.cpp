#include "Player.h"
#include <GameObjects/Map/MapManager.h>

Player::Player()
{
	// プレイヤーデータ初期化
	render_.modelID_ = Game::Asset::Model::Load("resources/prototypes/model/cube/cube.obj");
	render_.psoConfig_.vs = "resources/shaders/SimpleModel/SimpleModel.VS.hlsl";
	render_.psoConfig_.ps = "resources/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_.SetupFromShaders();

	c_viewCameraID_ = Game::Camera::AddCamera("PlayerView");
	Game::Camera::Setter::SetDistance(0.1f, 0, EaseType::IN_BACK, c_viewCameraID_);

	//SetHaveItem();

	//AddItem(ItemID::作業台ブロック);
	//for (int i = 0; i < 64; ++i)
	//{
	//	AddItem(ItemID::ダイヤモンド);
	//	AddItem(ItemID::鉄インゴット);
	//	AddItem(ItemID::棒);
	//}
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

	mapManager_->RegisterCharacter(this);

	haveItem_.SetCharacterPosition(translate_.value);
}

void Player::Update(int32_t cameraID)
{
	if (Game::IO::Key::IsJustPressed('Q'))
	{
		inventory_.AddItem(ItemID::Tool_Sword_of_Gold);
	}

	// 移動更新
	UpdateDash();
	UpdateMove(cameraID);
	UpdateJump();

	// 移動後の視線レイ更新
	UpdateViewRay(cameraID);

	// ターゲットブロック取得
	SetTargetBlock();

	// 左クリック処理
	if (Game::IO::Mouse::IsHeld(0))
	{
		UpdateLeftClick();
	}

	// 右クリック処理
	if (Game::IO::Mouse::IsJustPressed(1))
	{
		UpdateRightClick();
	}

	haveItem_.SetItem(inventory_.GetCurrentSelectedItemID());
	haveItem_.Update(cameraID);

	// AABB更新
	aabb_.max = translate_.value + Vector3(0.5f, 1.0f, 0.5f);
	aabb_.min = translate_.value - Vector3(0.5f, 0.0f, 0.5f);

	// 実際に移動
	translate_.velocity += translate_.acceleration;
	mapManager_->SweepAABB(aabb_, translate_.velocity);
	translate_.value += translate_.velocity;

	// 接地判定更新
	if (translate_.velocity.y == 0.0f)isGrounded_ = true;
	else isGrounded_ = false;

	Game::Camera::Setter::SetCenter(translate_.value + Vector3(0.0f, 5.0f, 0.0f), 0, EaseType::IN_BACK, c_viewCameraID_);

	Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(scale_.value, rotate_.value, translate_.value);
	Matrix4x4 wvpMatrix = worldMatrix * Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	int32_t texID = 2;

	render_.SetCBufferData(0, ShaderType::VertexShader, &wvpMatrix);
	render_.SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
	render_.SetCBufferData(0, ShaderType::PixelShader, &color);
	render_.SetCBufferData(1, ShaderType::PixelShader, &texID);
}

void Player::Draw(int32_t renderTextureID)
{
	// プレイヤー描画
	render_.Draw(renderTextureID);

	// 所持アイテム描画
	haveItem_.Draw(renderTextureID);
}

void Player::DrawImGui()
{
	ImGui::Begin("Player Info");
	ImGui::DragFloat3("Position", &translate_.value.x, 1.0f);
	ImGui::DragFloat3("Scale", &scale_.value.x, 0.1f);
	ImGui::End();
}


void Player::UpdateViewRay(int32_t cameraID)
{
	Vector3 cameraRot = Game::Camera::Getter::GetRotate(cameraID);
	Vector3 direction = Game::Math::DirectionFromYawPitch(cameraRot.y, cameraRot.x);

	viewRay_.origin = translate_.value;
	//viewRay_.origin.y += translate_.value.y + 1.6f; // プレイヤーの目線の高さに調整
	viewRay_.diff = direction.Normalized();

	SetViewRay(viewRay_);
}
void Player::UpdateMove(int32_t cameraID)
{
	Vector3 cameraRot = Game::Camera::Getter::GetRotate(cameraID);
	Vector3 forward = Game::Math::DirectionFromYawPitch(cameraRot.y, 0.0f);
	forward.Normalize();

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
		// 正規化
		input.Normalize();

		// 入力ベクトルの角度（ラジアン）
		float angle = std::atan2(input.x, input.y); // XZ平面での回転

		// forward を angle だけ回転
		float cosA = std::cos(angle);
		float sinA = std::sin(angle);

		// 進む方向ベクトル
		moveDir = Vector3(
			forward.x * cosA - forward.z * sinA,
			forward.y,
			forward.x * sinA + forward.z * cosA
		);

		moveDir.Normalize();
	}
	Move(moveDir, speed_);
}
void Player::UpdateDash()
{
	if (Game::IO::Key::IsJustReleased('W'))
	{
		speed_ = normalSpeed_;
		if (wHeldFrames_ < 20)dashBufferTimer_ = 20;
	}
	if (dashBufferTimer_ > 0)
	{
		dashBufferTimer_--;
		if (Game::IO::Key::IsJustPressed('W'))
		{
			speed_ = dashSpeed_;
		}
	}

	wHeldFrames_ = Game::IO::Key::HoldFrames('W');
}
void Player::UpdateJump()
{
	// ジャンプ処置
	if (Game::IO::Key::IsJustPressed(VK_SPACE))
	{
		Jump();
	}
}

void Player::AddItemToItemslot(ItemID itemID)
{
	//haveItem_.AddItem(itemID);
}
