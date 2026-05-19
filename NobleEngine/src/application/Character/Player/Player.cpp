#include "Character/Player/Player.h"
#include "Window/WindowManager.h"
#include "UIManager/UIManager.h"

Player::Player()
{
	// プレイヤーデータ初期化
	data_.SetModel(ResourceID::GetModelID(ModelID::Cube));
	data_.SetTexture(ResourceID::GetTextureID(TextureID::UVChecker));
	data_.color.w = 0;
	data_.name = "Player";

	// レティクル初期化
	reticle_.texture = ResourceID::GetTextureID(TextureID::white1x1);
	reticle_.transforms.scale = Vector3(5.0f, 5.0f, 5.0f);
	reticle_.transforms.translate = Vector3(
		WindowManager::winWidth_ / 2.0f,
		WindowManager::winHeight_ / 2.0f,
		0.0f
	);
	reticle_.color = 0x00000099;

	breakPower_ = 1.0f;

	SetHaveItem();

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
	data_.translate.value = Vector3(0.0f, 20.0f, 0.0f);
	data_.translate.velocity = Vector3(0.0f, -0.0f, 0.0f);
	data_.translate.acceleration = Vector3(0.0f, GRAVITY, 0.0f);
	data_.scale.value = Vector3(0.6f, 1.8f, 0.6f);
	data_.rotate.value = Vector3(0.0f, 0.0f, 0.0f);
}

void Player::Update()
{
	if (Game::IO::Key::IsJustPressed(DIK_X))
	{
		data_.translate.value.y = 100.0f;
		data_.translate.velocity.y = 0.0f;
		data_.translate.acceleration.y = 0.0f;
	}


	currentMode_ = uiManager_->GetCurrentUIMode();

	// プレイ中または非表示時のみ操作可能
	if (currentMode_ == UIMode::Playing || currentMode_ == UIMode::Hidden)
	{
		// 移動更新
		UpdateDash();
		UpdateMove();
		UpdateJump();

		// 移動後の視線レイ更新
		UpdateViewRay();

		// ターゲットブロック取得
		SetTargetBlock();

		// 左クリック処理
		if (Game::IO::Mouse::IsHeld(0))
		{
			UpdateLeftClick();
		}

		// ブロック設置
		if (Game::IO::Mouse::IsJustPressed(1))
		{
			SetNewBlock(ItemIDToBlockID(haveItem_->GetCurrentSelectedItemID()));
		}

		// アイテムポイ捨て
		if (Game::IO::Key::IsJustPressed(DIK_Q))
		{
			haveItem_->DropCurrentSelectedItem(data_.aabbs[0].center());
		}
	}
	else
	{
		// 落下処理
		Move(Vector3(0.0f, 0.0f, 0.0f), speed_);

		// 移動後の視線レイ更新
		UpdateViewRay();

		// ターゲットブロック取得
		SetTargetBlock();
	}

	if (currentMode_ == UIMode::Crafting)
	{
		haveItem_->craftMode3x3_ = true;
	}
	if (currentMode_ == UIMode::Inventory)
	{
		haveItem_->craftMode3x3_ = false;
	}

	// 接地判定更新
	UpdateGrounded();
}

void Player::Draw()
{
	data_.Draw();
	reticle_.Draw();
}

void Player::DrawCrafting()
{
	// インベントリのアイコンを動かせる
	haveItem_->UpdateInventory();
	// インベントリのアイコン描画
	haveItem_->DrawInventory();

	// ホットバーのアイコン描画
	haveItem_->DrawHotbar();
}

void Player::DrawInventory()
{
	// インベントリのアイコンを動かせる
	haveItem_->UpdateInventory();
	// インベントリのアイコン描画
	haveItem_->DrawInventory();

	// ホットバーのアイコン描画
	haveItem_->DrawHotbar();
}

void Player::DrawHotbar()
{
	// マウスホイールでホットバー選択
	haveItem_->UpdateHotbar();
	// ホットバーのアイコン描画
	haveItem_->DrawHotbar();
}

void Player::DrawImGui()
{
	ImGui::Begin("Player Info");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", data_.translate.value.x, data_.translate.value.y, data_.translate.value.z);
	ImGui::Text("View Origin: (%.2f, %.2f, %.2f)", viewRay_.origin.x, viewRay_.origin.y, viewRay_.origin.z);
	ImGui::Text("AABB Center: (%.2f, %.2f, %.2f)", data_.aabbs[0].center().x, data_.aabbs[0].center().y, data_.aabbs[0].center().z);
	ImGui::End();
}

void Player::UpdateViewRay()
{
	Vector3 cameraRot = Game::Camera::Getter::GetCurrentRotate();
	Vector3 direction = Game::Math::DirectionFromYawPitch(cameraRot.y, cameraRot.x);

	viewRay_.origin = data_.aabbs[0].center();
	viewRay_.origin.y += data_.scale.value.y * 0.4f; // プレイヤーの目線の高さに調整
	viewRay_.diff = direction.Normalized();

	SetViewRay(viewRay_);
}

void Player::UpdateDash()
{
	if (Game::IO::Key::IsJustReleased(DIK_W))
	{
		speed_ = normalSpeed_;
		if (wHeldFrames_ < 20)dashBufferTimer_ = 20;
	}
	if (dashBufferTimer_ > 0)
	{
		dashBufferTimer_--;
		if (Game::IO::Key::IsJustPressed(DIK_W))
		{
			speed_ = dashSpeed_;
		}
	}

	wHeldFrames_ = Game::IO::Key::HoldFrames(DIK_W);
}

void Player::UpdateMove()
{
	Vector3 cameraRot = Game::Camera::Getter::GetCurrentRotate();
	Vector3 forward = Game::Math::DirectionFromYawPitch(cameraRot.y, 0.0f);
	forward.Normalize();

	// 移動処理
	Vector2 input(0.0f, 0.0f);

	if (Game::IO::Key::IsHeld(DIK_W)) input.y += 1.0f;
	if (Game::IO::Key::IsHeld(DIK_S)) input.y -= 1.0f;
	if (Game::IO::Key::IsHeld(DIK_A)) input.x += 1.0f;
	if (Game::IO::Key::IsHeld(DIK_D)) input.x -= 1.0f;

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

void Player::UpdateJump()
{
	// ジャンプ処置
	if (Game::IO::Key::IsJustPressed(DIK_SPACE))
	{
		Jump();
	}
}

void Player::AddItemToItemslot(ItemID itemID)
{
	haveItem_->AddItem(itemID);
}
