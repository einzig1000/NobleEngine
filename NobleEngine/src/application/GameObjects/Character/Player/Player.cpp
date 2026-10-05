#include "Player.h"
#include <GameObjects/Character/SwingMining/SwingMining.h>
#include <GameObjects/Character/RangeMining/RangeMining.h>
#include <GameObjects/Map/Terrain/Terrain.h>
#include <GameObjects/Effect/Particle/GoTargetCurving/GoTargetCurving.h>
#include <App.h>
#include <System/EventBus/EventBus.h>

#include <numbers>
#include <algorithm>

Player::Player()
{
	// プレイヤーデータ初期化
	t_player_ = Game::Asset::Texture::Load("assets/engine/texture/white1x1.png");
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
	translate_.acceleration = Vector3{ 0.0f, Constexprs::kGravity, 0.0f };

	scale_.value = Vector3{ 0.6f, 0.6f, 0.6f };
	//scale_.value = Vector3{ 0.1f, 0.1f, 0.1f };
	scale_.velocity = Vector3{ 0.0f, 0.0f, 0.0f };
	scale_.acceleration = Vector3{ 0.0f, 0.0f, 0.0f };

	rotate_.value = Vector3{ 0.0f, 0.0f, 0.0f };
	rotate_.velocity = Vector3{ 0.0f, 0.0f, 0.0f };
	rotate_.acceleration = Vector3{ 0.0f, 0.0f, 0.0f };

	RegisterToMap();

	AddItem(ItemID::Tool_Hammer_of_Wood, 1);
}

//void Player::Update(int32_t 俯瞰カメラID, int32_t 自身の視点カメラID)
// 俯瞰カメラID は WorldMatrixとか作るのに必要
// 自身の視点カメラID は ViewRayの計算とかに必要
void Player::Update(int32_t cameraID)
{
	previousHP_ = static_cast<float>(hp_);

	// 入力に対する処理
	UpdateInput(cameraID);

	// マップ情報を見て適切に移動を適用する
	ApplyMove();

	// 移動後の視線レイ更新
	ComputeViewRay(cameraID);

	Game::Camera::Setter::CenterTarget(translate_.value, 0, EaseType::IN_BACK, cameraID);

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


	wvpMatrix_ = worldMatrix_ * Game::Camera::Getter::GetViewProjectionMatrix(cameraID);
}


void Player::CheckExternalEvents()
{
	if (eventBus_)
	{
		// ブロック破壊イベント(採掘ptをためて、たまった分だけオーブを出す)
		Vector3 orbStart;
		for (const Event& event : eventBus_->GetEvents(EventType::BlockDestroyed))
		{
			// 壊れたブロックの情報を取得
			const auto* destroyedData = std::any_cast<Terrain::BlockDestroyedData>(&event.data);
			if (!destroyedData) continue;

			// 壊れたブロックの情報を取得
			const BlockInfo* info = App::Data::Item::Get(destroyedData->id);
			if (!info) continue;

			// 採掘ptをためる
			const float miningPoint = miningPointGauge_.AddPending(info->miningPoint);

			// オーブを出す
			if (miningPoint > 0.0f)
			{
				GoTargetCurving::Params request;
				request.start = destroyedData->position;
				request.target = &translate_.value;
				request.targetOffset = Vector3{ 0.0f, -0.6f, 0.0f };
				request.height = 1.0f;
				request.speed = 8.0f;
				request.color = { 0.6f, 1.0f, 0.3f, 1.0f };
				request.scale = miningPoint * 0.0001f;
				Event arriveEvent;
				arriveEvent.type = EventType::MiningOrbAbsorbed;
				arriveEvent.data = miningPoint;
				request.arriveEventData = arriveEvent;

				Event event;
				event.type = EventType::ParticleRequest_GoTargetCurving;
				event.data = request;
				eventBus_->Notify(event);
			}
		}


		// ツールのグレードアップ依頼
		for (const Event& event : eventBus_->GetEvents(EventType::ToolUpgradeRequested))
		{
			const auto* request = std::any_cast<ToolUpgradeRequest>(&event.data);
			if (!request) continue;
			if (request->slotIndex < 0 || request->slotIndex >= ItemInventory::kSlotCount) continue;
			// 届くまでにスロットの中身が変わっていたら何もしない
			if (GetInventory()->GetInventorySlot(request->slotIndex).itemID != request->sourceID) continue;
			// レベルが足りなければ何もしない
			if (!miningPointGauge_.UseLevel(request->levelCost)) continue;
			// 元の道具を消して同じスロットにグレードアップ先を入れる
			ReplaceItem(request->slotIndex, request->resultID, 1);
			switch (request->upgradeType)
			{
			case UpgradeType::SpeedUp:
				SpeedUpItem();
				break;
			case UpgradeType::ScaleUp:
				ScaleUpItem();
				break;
			case UpgradeType::PowerUp:
				//PowerUpItem();
				break;
			}
		}

		// 採掘オーブが届いた
		for (const Event& event : eventBus_->GetEvents(EventType::MiningOrbAbsorbed))
		{
			if (const auto* amount = std::any_cast<float>(&event.data))
			{
				miningPointGauge_.AddPoint(*amount);
			}
		}

		// 採掘モード切り替えイベント
		const std::vector<Event>& miningModeEvents = eventBus_->GetEvents(EventType::MiningModeChanged);
		if (!miningModeEvents.empty())
		{
			// 1Fに一回しか変更フラグはされない
			miningMode_ = std::any_cast<MiningPattern>(miningModeEvents[0].data);
		}

		// アイテム取得イベント
		const std::vector<Event>& itemPickupEvents = eventBus_->GetEvents(EventType::ItemPickup);
		if (!itemPickupEvents.empty())
		{
			for (const Event& event : itemPickupEvents)
			{
				ItemID itemID = std::any_cast<ItemID>(event.data);
				int32_t amount = 1;

				AddItem(itemID, amount);
			}
		}

		// HP変動イベント
		const std::vector<Event>& hpChangedEvents = eventBus_->GetEvents(EventType::PlayerHPChanged);
		if (!hpChangedEvents.empty())
		{
			for (const Event& event : hpChangedEvents)
			{
				hp_ += std::any_cast<float>(event.data);
				if (hp_ < 0) hp_ = 0;
				if (hp_ > maxHP_) hp_ = maxHP_;
			}
		}
	}
}

void Player::Draw(int32_t renderTextureID)
{
	// プレイヤー描画
	//Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//render_.SetBRegisterData(0, ShaderType::VertexShader, &wvpMatrix_);
	//render_.SetBRegisterData(1, ShaderType::VertexShader, &worldMatrix_);
	//render_.SetBRegisterData(0, ShaderType::PixelShader, &color);
	//render_.SetBRegisterData(1, ShaderType::PixelShader, &t_player_);
	//render_.Draw(renderTextureID);

	DrawHaveItem(renderTextureID);
}

void Player::DrawImGui()
{
	miningPointGauge_.DrawImGui();

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
	Game::Camera::Setter::DistanceTarget(0.1f, 0, EaseType::IN_BACK, c_viewCameraID_);
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
	if (dashBufferTimer_.GetProgress() < 1.0f)
	{
		// ダッシュ開始
		if (Game::IO::Key::IsJustPressed('W'))
		{
			dash_ = true;
			speed_ = dashSpeed_;
		}
	}
	// 10/60秒以内の単タップを検知したら
	else if (Game::IO::Key::TestTapLong(5.0f / 60.0f, 'W'))
	{
		// ダッシュ開始可能タイマーをセット
		dashBufferTimer_.Initialize(1.0f);
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

	Game::Camera::Setter::ThetaTarget(viewTheta_, 0.0f, EaseType::LINEAR, cameraID);
	Game::Camera::Setter::PhiTarget(viewPhi_, 0.0f, EaseType::LINEAR, cameraID);
}
void Player::UpdateInputLeftClick()
{
	if (!Game::IO::Mouse::IsHeld(0)) return;

}
void Player::UpdateInputRightClick()
{
	if (!Game::IO::Mouse::IsJustPressed(1)) return;
}
