#include "Character/Enemy/Enemy.h"
#include "Character/Player/Player.h"
#include "Window/WindowManager.h"
#include "UIManager/UIManager.h"

Enemy::Enemy(Vector3 spawnPos)
{
	// エネミーデータ初期化
	data_.SetModel(ResourceID::GetModelID(ModelID::Pig));
	data_.SetTexture(ResourceID::GetTextureID(TextureID::white1x1));
	data_.name = "Enemy";

	data_.translate.value = spawnPos;
	data_.translate.velocity = Vector3(0.0f, -0.0f, 0.0f);
	data_.translate.acceleration = Vector3(0.0f, GRAVITY, 0.0f);
	data_.scale.value = Vector3(0.6f, 0.6f, 0.6f);
	data_.rotate.value = Vector3(0.0f, 0.0f, 0.0f);

	breakPower_ = 1.0f;

	speed_ = 0.02f;

	SetHaveItem();

	//Game::Physics::RegisterDynamic(&data_);
}

Enemy::~Enemy()
{}

void Enemy::Initialize()
{}

void Enemy::Update()
{
	if (HP_ <= 0)
	{
		data_.translate.value.y = -1000.0f;
		return;
	}

	currentMode_ = uiManager_->GetCurrentUIMode();

	// プレイ中または非表示時のみ行動
	if (currentMode_ == UIMode::Playing || currentMode_ == UIMode::Hidden)
	{
		// 移動更新
		UpdateMove();
		// 視線レイ更新
		UpdateViewRay();
		// ターゲットブロック取得
		SetTargetBlock();
		// ブロック破壊
		UpdateLeftClick();
	}

	// 接地判定更新
	UpdateGrounded();

	data_.color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
}

void Enemy::Draw()
{
	if (HP_ <= 0)
	{
		return;
	}
	data_.Draw();
}

void Enemy::DrawImGui()
{
	if (HP_ <= 0)
	{
		return;
	}
	data_.DrawImGui();
}

void Enemy::UpdateViewRay()
{
	// プレイヤーの位置を取得
	Vector3 playerPos = playerptr_->data_.GetWorldPosition();
	// 自分の位置を取得
	Vector3 enemyPos = data_.GetWorldPosition();
	// プレイヤーへの方向ベクトルを計算
	Vector3 directionToPlayer = playerPos - enemyPos;
	// 視線レイを設定
	viewRay_.origin = enemyPos;
	viewRay_.diff = directionToPlayer.Normalized();
	SetViewRay(viewRay_);
}

void Enemy::UpdateMove()
{
	// プレイヤーの位置を取得
	Vector3 playerPos = playerptr_->data_.GetWorldPosition();
	// 自分の位置を取得
	Vector3 enemyPos = data_.GetWorldPosition();
	// プレイヤーへの方向ベクトルを計算
	Vector3 directionToPlayer = playerPos - enemyPos;
	// 距離を計算
	float distanceToPlayer = directionToPlayer.Length();
	// プレイヤーの方向に移動
	Vector3 moveDirection = directionToPlayer.Normalized();
	Move(moveDirection, speed_);

	// プレイヤーが上にいる場合、ジャンプを試みる
	if (playerPos.y - enemyPos.y > 1.0f)
	{
		Jump();
	}

	//// 移動方向に向く
	//data_.LookAtOnce(playerptr_->data_.GetWorldPosition());
	//// 位置履歴に現在の位置を追加
	recentPositions_.push_back(enemyPos);
	//// 直近60フレームの位置履歴を保持
	//if (recentPositions_.size() > 60)
	//{
	//	recentPositions_.pop_front();
	//}
	//// 60F前との位置との差分を計算
	//if (recentPositions_.size() == 60)
	//{
	//	Vector3 pastPos = recentPositions_.front();
	//	Vector3 movementDelta = enemyPos - pastPos;
	//	if (movementDelta.Length() < 0.5f)
	//	{
	//		// ほとんど動いていない場合、ジャンプを試みる
	//		Jump();
	//	}
	//}
}