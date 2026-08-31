#include "Player.h"
#include <GameObjects/Character/SwingMining/SwingMining.h>
#include <GameObjects/Character/RangeMining/RangeMining.h>
#include <GameObjects/EventBus/EventBus.h>
#include <numbers>
#include <algorithm>

Player::Player()
{
	// プレイヤーデータ初期化
	render_.modelID_ = Game::Asset::Model::Load("assets/application/Minecraft/player/player.obj");
	const ModelData* modelData = Game::Asset::Model::GetData(render_.modelID_);
	SetBoundingBox(modelData->colliderShape.aabbs[0]);
	render_.psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	render_.psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render_.SetupFromShaders();

	swingMining_ = std::make_unique<SwingMining>(this);
	rangeMining_ = std::make_unique<RangeMining>(this);
}

Player::~Player()
{}

void Player::Initialize()
{
	translate_.value = Vector3{ 0.0f, 60.0f, 0.0f };
	translate_.velocity = Vector3{ 0.0f, -0.0f, 0.0f };
	translate_.acceleration = Vector3{ 0.0f, Constexprs::GRAVITY, 0.0f };

	scale_.value = Vector3{ 1.0f, 1.0f, 1.0f };
	scale_.velocity = Vector3{ 0.0f, 0.0f, 0.0f };
	scale_.acceleration = Vector3{ 0.0f, 0.0f, 0.0f };

	rotate_.value = Vector3{ 0.0f, 0.0f, 0.0f };
	rotate_.velocity = Vector3{ 0.0f, 0.0f, 0.0f };
	rotate_.acceleration = Vector3{ 0.0f, 0.0f, 0.0f };

	RegisterToMap();

	AddItem(ItemID::Tool_Pickel_of_Iron);
}

//void Player::Update(int32_t 俯瞰カメラID, int32_t 自身の視点カメラID)
// 俯瞰カメラID は WorldMatrixとか作るのに必要
// 自身の視点カメラID は ViewRayの計算とかに必要
void Player::Update(int32_t cameraID)
{
	// 外部イベント確認
	CheckExternalEvents();

	previousHP_ = static_cast<float>(HP_);

	// 入力に対する処理
	UpdateInput(cameraID);

	// マップ情報を見て適切に移動を適用する
	ApplyMove();

	// 移動後の視線レイ更新
	ComputeViewRay(cameraID);

	Game::Camera::Setter::SetCenter(translate_.value, 0, EaseType::IN_BACK, cameraID);

	// 視線レイからターゲットブロック取得
	SetTargetBlock();

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


	Matrix4x4 wvpMatrix = worldMatrix_ * Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	int32_t texID = Game::Asset::Texture::Load("assets/engine/texture/white1x1.png");

	render_.SetCBufferData(0, ShaderType::VertexShader, &wvpMatrix);
	render_.SetCBufferData(1, ShaderType::VertexShader, &worldMatrix_);
	render_.SetCBufferData(0, ShaderType::PixelShader, &color);
	render_.SetCBufferData(1, ShaderType::PixelShader, &texID);
}


void Player::CheckExternalEvents()
{
	if (eventBus_)
	{
		// 採掘モード切り替えイベント
		const std::vector<Event>& miningModeEvents = eventBus_->GetEvents(EventType::MiningModeChanged);
		if (!miningModeEvents.empty())
		{
			// 1Fに一回しか変更フラグはされない
			miningModeEvents[0].value[0] == 0 ? miningMode_ = MiningPattern::Swing : miningMode_ = MiningPattern::Range;

			eventBus_->Clear(EventType::MiningModeChanged);
		}

		// アイテム取得イベント
		const std::vector<Event>& itemPickupEvents = eventBus_->GetEvents(EventType::ItemPickup);
		if (!itemPickupEvents.empty())
		{
			for (const Event& event : itemPickupEvents)
			{
				ItemID itemID = static_cast<ItemID>(event.value[0]);
				int32_t amount = event.value[1];

				for (int32_t i = 0; i < amount; ++i)
				{
					AddItem(itemID);
				}
			}

			eventBus_->Clear(EventType::ItemPickup);
		}

		// HP変動イベント
		const std::vector<Event>& hpChangedEvents = eventBus_->GetEvents(EventType::PlayerHPChanged);
		if (!hpChangedEvents.empty())
		{
			for (const Event& event : hpChangedEvents)
			{
				HP_ += event.value[0];
				if (HP_ < 0) HP_ = 0;
				if (HP_ > maxHP_) HP_ = maxHP_;

				if (event.value[0] < 0)
				{
					eventBus_->Notify(Event{ EventType::PlayerDamaged, {  } });
				}
			}

			eventBus_->Clear(EventType::PlayerHPChanged);
		}
	}
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

void Player::SetViewCamera(int32_t cameraID)
{
	c_viewCameraID_ = cameraID;
	Game::Camera::Setter::SetDistance(0.1f, 0, EaseType::IN_BACK, c_viewCameraID_);
}



void Player::UpdateInput(int32_t cameraID)
{
	// マウスカーソル操作時の処理（視線操作）
	UpdateInputMouseCursor(cameraID);
	// SPACE入力時の処理(ジャンプ)
	UpdateInputSpace();
	// WASD入力時の処理(ダッシュ判定を含む移動)
	UpdateInputWASD(cameraID);

	// 左クリックで起きるイベント更新(ブロック破壊とか)
	UpdateInputLeftClick();
	// 右クリックで起きるイベント更新(ブロック設置とか)
	UpdateInputRightClick();
}

void Player::UpdateInputWASD(int32_t cameraID)
{
	// ダッシュ解除
	if (dash_ && Game::IO::Key::IsJustReleased('W'))
	{
		dash_ = false;
		speed_ = normalSpeed_;
	}

	// ダッシュ開始可能
	if (dashBufferTimer_ > 0)
	{
		dashBufferTimer_--;

		// ダッシュ開始
		if (Game::IO::Key::IsJustPressed('W'))
		{
			dash_ = true;
			speed_ = dashSpeed_;
			dashBufferTimer_ = 0;
		}
	}
	// 10フレーム以内の単タップを検知したら
	else if (Game::IO::Key::TestTapLong('W', 10))
	{
		// ダッシュ開始可能タイマーをセット
		dashBufferTimer_ = 30;
	}

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
		Vector3 cameraDir = Game::Camera::Getter::GetCameraDirection(cameraID);
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
void Player::UpdateInputSpace()
{
	// ジャンプ処置
	if (Game::IO::Key::IsJustPressed(VK_SPACE))
	{
		Jump();
	}
}
void Player::UpdateInputMouseCursor(int32_t cameraID)
{
	// マウス移動量（前フレームからの相対値。dtは掛けない）
	const Vector2 mouseDelta = Game::IO::Mouse::Get2DPositionDelta();

	viewTheta_ -= mouseDelta.x * lookSensitivity_;
	viewPhi_ += mouseDelta.y * lookSensitivity_;

	constexpr float limit = std::numbers::pi_v<float> / 2.0f - 0.01f;
	viewPhi_ = std::clamp(viewPhi_, -limit, limit);

	if (viewTheta_ > std::numbers::pi_v<float>) viewTheta_ -= std::numbers::pi_v<float> * 2.0f;
	if (viewTheta_ < -std::numbers::pi_v<float>) viewTheta_ += std::numbers::pi_v<float> * 2.0f;

	Game::Camera::Setter::SetRotate(Vector3(viewPhi_, viewTheta_, 0.0f), 0.0f, EaseType::LINEAR, cameraID);
}
void Player::UpdateInputLeftClick()
{
	if (!Game::IO::Mouse::IsHeld(0)) return;

}
void Player::UpdateInputRightClick()
{
	if (!Game::IO::Mouse::IsJustPressed(1)) return;
}

//C:\Users\K024G\AppData\Local\Temp\DevHub.DMP