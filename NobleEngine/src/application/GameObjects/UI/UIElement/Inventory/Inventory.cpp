#include "Inventory.h"
#include <GameObjects/Character/ItemInventory/ItemInventory.h>
#include <GameObjects/Character/MiningPointGauge/MiningPointGauge.h>
#include <GameObjects/UI/ItemIcon/ItemIconManager.h>
#include <System/EventBus/EventBus.h>
#include <App.h>
#include <algorithm>
#include <cmath>


namespace
{
	// 採掘ptゲージの内側
	constexpr float kGaugeLeft = 336.0f;
	constexpr float kGaugeTop = 300.0f;
	constexpr float kGaugeWidth = 296.0f;
	constexpr float kGaugeHeight = 20.0f;
	// ゲージ右の隙間の中心(レベルの数字を置く)
	constexpr float kLevelCenterX = 664.0f;
	constexpr float kLevelCenterY = 310.0f;
	// 1px
	constexpr float kArtPixel = 4.0f;

	// レベルの文字サイズ(Hotbarの個数と同じ)
	constexpr float kLevelCharSize = 46.0f;
	// DotGothic16で行の上端から数字の縦の中心までの割合
	// (ascent 1160 - 数字の中心の高さ (787 - 28) / 2) / (ascent - descent = 1448)
	constexpr float kDigitCenterRatio = (1160.0f - (787.0f - 28.0f) * 0.5f) / 1448.0f;
	// 縁取りの太さ(文字サイズ46のときのフォント1ドット分)
	constexpr float kLevelOutlineWidth = 2.0f;

	// 右上の枠(ツールのグレードアップ)。以下もInventory.png上の座標(px)
	// 見本の中心X
	constexpr float kUpgradeSourceX = 384.0f;
	// グレードアップ先の中心X
	constexpr float kUpgradeResultX = 584.0f;
	// 矢印と消費レベルの中心X(見本とグレードアップ先の間)
	constexpr float kUpgradeArrowX = 484.0f;
	// 1行目の中心Yと、行の間隔
	constexpr float kUpgradeTopRowY = 72.0f;
	constexpr float kUpgradeRowPitch = 84.0f;
	// 行の中心から、矢印・消費レベルの中心までのずらし
	constexpr float kUpgradeArrowOffsetY = -12.0f;
	constexpr float kUpgradeCostOffsetY = 20.0f;
	// 文字サイズ(DotGothic16は23で1ドット≒1px、92で1ドット≒4px)
	constexpr float kUpgradeArrowCharSize = 92.0f;
	constexpr float kUpgradeCostCharSize = 23.0f;
	// 消費レベルの縁取りの太さ(文字サイズ23のときのフォント1ドット分)
	constexpr float kUpgradeCostOutlineWidth = 1.0f;
	// 色(RTがsRGBなのでリニアで書く)
	const Vector4 kArrowColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	const Vector4 kCostShortColor = Vector4{ 1.0f, 0.0f, 0.0f, 1.0f };

	// マウスの位置をUIの座標(1280x720)に直す
	// Get2DPositionはウィンドウのピクセル座標なので、全画面などでウィンドウの大きさが変わるとずれる
	Vector2 GetMouseUIPosition()
	{
		constexpr float kUIWidth = 1280.0f;
		constexpr float kUIHeight = 720.0f;
		const Vector2 mouse = Game::IO::Mouse::Get2DPosition();
		const float windowWidth = static_cast<float>(Game::Window::GetWidth());
		const float windowHeight = static_cast<float>(Game::Window::GetHeight());
		if (windowWidth <= 0.0f || windowHeight <= 0.0f) return mouse;
		return Vector2{ mouse.x * kUIWidth / windowWidth, mouse.y * kUIHeight / windowHeight };
	}

	// アイコンの四角の中にマウスがあるか(planeは-1〜1なので、scaleが半分の大きさ)
	bool IsMouseOver(const ElementData& icon, const Vector2& mouse)
	{
		const Vector3& center = icon.transforms.translate;
		const Vector3& half = icon.transforms.scale;
		return std::abs(mouse.x - center.x) <= half.x && std::abs(mouse.y - center.y) <= half.y;
	}

	// スロットの中身が道具ならその情報を返す。道具以外・空ならnullptr
	const ItemInfo* GetToolInfo(const InventorySlot& slot)
	{
		if (slot.itemID == ItemID::MAX) return nullptr;
		const ItemInfo* info = App::Data::Item::Get(slot.itemID);
		if (!info || info->genre != ItemGenre::Tool) return nullptr;
		return info;
	}

	// アイコンを1つ描く
	void DrawIcon(const ElementData& icon, const Matrix4x4& orthographic, int32_t rt_ID)
	{
		if (icon.textureID < 0) return;

		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(icon.transforms.scale, icon.transforms.rotate, icon.transforms.translate);
		Matrix4x4 wvp = world * orthographic;
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		icon.render->SetBRegisterData(0, ShaderType::VertexShader, &wvp);
		icon.render->SetBRegisterData(1, ShaderType::VertexShader, &world);
		icon.render->SetBRegisterData(0, ShaderType::PixelShader, &color);
		icon.render->SetBRegisterData(1, ShaderType::PixelShader, &icon.textureID);
		icon.render->Draw(rt_ID, { icon.textureID });
	}

	// 黒で上下左右にずらして縁取りしてから、本体を描く(DrawStringを5回呼ぶ)
	void DrawOutlinedString(int32_t rt_ID, const std::string& text, int32_t charSize, const Vector2& pos, const Vector4& color, float outlineWidth)
	{
		const Vector4 outlineColor = Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };
		const Vector2 outlineOffsets[] =
		{
			Vector2{ -outlineWidth, 0.0f },
			Vector2{ outlineWidth, 0.0f },
			Vector2{ 0.0f, -outlineWidth },
			Vector2{ 0.0f, outlineWidth },
		};
		for (const Vector2& offset : outlineOffsets)
		{
			Game::Asset::Font::DrawString(rt_ID, text, charSize, pos + offset, outlineColor);
		}
		Game::Asset::Font::DrawString(rt_ID, text, charSize, pos, color);
	}
}

Inventory::Inventory()
{
	// sprites_[0] : アイテムインベントリ
	sprites_.emplace_back(ElementData{});
	sprites_[0].render = std::make_unique<RenderObject>();
	sprites_[0].render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	sprites_[0].render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	sprites_[0].render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	sprites_[0].render->SetupFromShaders();
	sprites_[0].textureID = Game::Asset::Texture::Load("assets/application/texture/UI/Inventory/Inventory.png");
	const TextureData* textureData = Game::Asset::Texture::GetData(sprites_[0].textureID);
	Vector2 textureSize = Vector2(float(textureData->metadata.width), float(textureData->metadata.height));
	sprites_[0].transforms.scale = Vector3(float(textureSize.x * 0.5f), float(textureSize.y * 0.5f), 1.0f);
	Vector2 windowSize = Vector2(float(Game::Window::GetWidth()), float(Game::Window::GetHeight()));
	sprites_[0].transforms.translate = Vector3(windowSize.x * 0.5f, windowSize.y * 0.5f, 1.0f);
	sprites_[0].transforms.rotate = Vector3(0.0f, 0.0f, 0.0f);

	const float iconHalfSize = 32.0f;
	const int32_t planeModelID = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");

	// 4x9 のインベントリ用アイコン
	inventoryIcons_.reserve(static_cast<size_t>(ItemInventory::kSlotCount));
	for (int32_t i = 0; i < ItemInventory::kSlotCount; ++i)
	{
		ElementData icon{};
		icon.render = std::make_unique<RenderObject>();
		icon.render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
		icon.render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
		icon.render->modelID_ = planeModelID;
		icon.render->SetupFromShaders();

		icon.textureID = -1;
		icon.transforms.scale = Vector3(iconHalfSize, iconHalfSize, 1.0f);
		icon.transforms.rotate = Vector3(0.0f, 0.0f, 0.0f);
		icon.transforms.translate = GetSlotPosition(i);

		inventoryIcons_.emplace_back(std::move(icon));
	}

	// 4x2 のアクセサリ用アイコン
	accessoryIcons_.reserve(8);
	for (int32_t i = 0; i < 8; ++i)
	{
		ElementData icon{};
		icon.render = std::make_unique<RenderObject>();
		icon.render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
		icon.render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
		icon.render->modelID_ = planeModelID;
		icon.render->SetupFromShaders();

		icon.textureID = -1;
		icon.transforms.scale = Vector3(iconHalfSize, iconHalfSize, 1.0f);
		icon.transforms.rotate = Vector3(0.0f, 0.0f, 0.0f);
		icon.transforms.translate = GetSlotPosition(i);

		accessoryIcons_.emplace_back(std::move(icon));
	}

	// 装備アップグレード欄で使うアイコン([0] = 見本、[1]〜[3] = グレードアップ先)
	upgradeIcons_.reserve(static_cast<size_t>(1 + kMaxUpgradeCount));
	for (int32_t i = 0; i < 1 + kMaxUpgradeCount; ++i)
	{
		ElementData icon{};
		icon.render = std::make_unique<RenderObject>();
		icon.render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
		icon.render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
		icon.render->modelID_ = planeModelID;
		icon.render->SetupFromShaders();

		icon.textureID = -1;
		icon.transforms.scale = Vector3(iconHalfSize, iconHalfSize, 1.0f);
		icon.transforms.rotate = Vector3(0.0f, 0.0f, 0.0f);
		// 見本は1行目の左。グレードアップ先は右に縦に並べる
		icon.transforms.translate = (i == 0)
			? TextureToScreen(kUpgradeSourceX, kUpgradeTopRowY)
			: TextureToScreen(kUpgradeResultX, kUpgradeTopRowY + kUpgradeRowPitch * static_cast<float>(i - 1));

		upgradeIcons_.emplace_back(std::move(icon));
	}

	// gaugeFill_
	gaugeFill_.render = std::make_unique<RenderObject>();
	gaugeFill_.render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModelNonTexture.PS.hlsl";
	gaugeFill_.render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	gaugeFill_.render->modelID_ = planeModelID;
	gaugeFill_.render->SetupFromShaders();
	gaugeFill_.transforms.scale = Vector3(0.0f, 0.0f, 1.0f);
}

Inventory::~Inventory()
{}

void Inventory::Initialize()
{
	selectedSlot_ = -1;
	upgradeRowCount_ = 0;
}

void Inventory::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);


	// 採掘ptゲージとレベル
	if (miningGauge_)
	{
		// インベントリの左上の座標
		const Vector3 inventoryTopLeft = sprites_[0].transforms.translate - Vector3(sprites_[0].transforms.scale.x, sprites_[0].transforms.scale.y, 0.0f);

		// 塗りの幅
		const float ratio = std::clamp(miningGauge_->GetGaugeRatio(), 0.0f, 1.0f);
		const float fillWidth = std::floor(kGaugeWidth * ratio / kArtPixel) * kArtPixel;

		// 左端をゲージの内側の左端にそろえて右へ伸ばす
		gaugeFill_.transforms.scale = Vector3(fillWidth * 0.5f, kGaugeHeight * 0.5f, 1.0f);
		gaugeFill_.transforms.translate = inventoryTopLeft + Vector3((kGaugeLeft + fillWidth * 0.5f), (kGaugeTop + kGaugeHeight * 0.5f), 0.0f);

		// レベルの文字(隙間の真ん中にそろえる)
		levelText_ = std::to_string(miningGauge_->GetLevel());
		levelCharSize_ = static_cast<int32_t>(kLevelCharSize);
		const Vector2 textSize = Game::Asset::Font::MeasureJustTextureSize(levelText_, levelCharSize_, Vector2{ 0.0f, 0.0f }, 0.0f);
		const float centerX = inventoryTopLeft.x + kLevelCenterX;
		const float centerY = inventoryTopLeft.y + kLevelCenterY;
		levelTextPos_ = Vector2{ centerX - textSize.x * 0.5f, centerY - static_cast<float>(levelCharSize_) * kDigitCenterRatio };
	}

	// 持ち物アイコン
	if (inventory_)
	{
		// 4x9 のインベントリ用アイコン
		for (int32_t i = 0; i < ItemInventory::kSlotCount; ++i)
		{
			const InventorySlot& slot = inventory_->GetInventorySlot(i);
			// 空スロット
			if (slot.itemID == ItemID::MAX)
			{
				inventoryIcons_[i].textureID = -1;
				continue;
			}

			// アイコン更新(モデルから作ったアイコン)
			inventoryIcons_[i].textureID = iconManager_ ? iconManager_->GetIcon(slot.itemID) : -1;
		}

		// 4x2 のアクセサリ用アイコン
		for (int32_t i = 0; i < 8; ++i)
		{
			const InventorySlot& slot = inventory_->GetAccessorySlot(i);
			// 空スロット
			if (slot.itemID == ItemID::MAX)
			{
				accessoryIcons_[i].textureID = -1;
				continue;
			}
			// アイコン更新(モデルから作ったアイコン)
			accessoryIcons_[i].textureID = iconManager_ ? iconManager_->GetIcon(slot.itemID) : -1;
		}

		// クリック(グレードアップ先・スロットの選択)
		HandleClick();
		// 右上の枠(ツールのグレードアップ)
		UpdateUpgradeTable();
	}
}

void Inventory::Draw(int32_t rt_ID)
{
	// インベントリ本体
	for (const auto& sprite : sprites_)
	{
		Matrix4x4 worldMatrix_ = Matrix4x4::MakeAffineMatrix(sprite.transforms.scale, sprite.transforms.rotate, sprite.transforms.translate);
		Matrix4x4 wvpMatrix_ = worldMatrix_ * orthographic_;
		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		sprite.render->SetBRegisterData(0, ShaderType::VertexShader, &wvpMatrix_);
		sprite.render->SetBRegisterData(1, ShaderType::VertexShader, &worldMatrix_);
		sprite.render->SetBRegisterData(0, ShaderType::PixelShader, &color);
		sprite.render->SetBRegisterData(1, ShaderType::PixelShader, &sprite.textureID);
		sprite.render->Draw(rt_ID);
	}

	// 採掘ptゲージとレベル
	if (miningGauge_)
	{
		// 塗り(幅0のときは描かない)
		if (gaugeFill_.transforms.scale.x > 0.0f)
		{
			Matrix4x4 world = Matrix4x4::MakeAffineMatrix(gaugeFill_.transforms.scale, gaugeFill_.transforms.rotate, gaugeFill_.transforms.translate);
			Matrix4x4 wvp = world * orthographic_;

			gaugeFill_.render->SetBRegisterData(0, ShaderType::VertexShader, &wvp);
			gaugeFill_.render->SetBRegisterData(1, ShaderType::VertexShader, &world);
			gaugeFill_.render->SetBRegisterData(0, ShaderType::PixelShader, &gaugeFillColor_);
			gaugeFill_.render->Draw(rt_ID);
		}

		// レベル
		DrawOutlinedString(rt_ID, levelText_, levelCharSize_, levelTextPos_, gaugeFillColor_, kLevelOutlineWidth);
	}

	// 右上の枠(ツールのグレードアップ)
	DrawUpgradeTable(rt_ID);

	if (inventory_)
	{
		// アイコン
		for (const auto& icon : inventoryIcons_)
		{
			DrawIcon(icon, orthographic_, rt_ID);
		}

		// 個数
		const int32_t charSize = static_cast<int32_t>(46.0f);
		const float textOffset = 6.0f;
		for (int32_t i = 0; i < ItemInventory::kSlotCount; ++i)
		{
			if (inventoryIcons_[i].textureID < 0) continue;

			const InventorySlot& slot = inventory_->GetInventorySlot(i);
			if (slot.count <= 1) continue;

			const Vector3& iconPos = inventoryIcons_[i].transforms.translate;
			Game::Asset::Font::DrawString(rt_ID, std::to_string(slot.count), charSize, Vector2{ iconPos.x + textOffset, iconPos.y + textOffset }, Vector4{ 0.0f,0.0f,0.0f,1.0f });
		}
	}
}

void Inventory::DrawImGui()
{}

Vector3 Inventory::GetSlotPosition(int32_t index) const
{
	// 以下はInventory2x2.png上の座標(px)。スロットの内側は64x64
	// 1段のスロット数
	constexpr int32_t kColumnCount = 9;
	// スロットの間隔
	constexpr float kSlotPitch = 72.0f;
	// 左端のスロットの中心X
	constexpr float kLeftSlotCenterX = 64.0f;
	// メイン3段の一番上の段の中心Y
	constexpr float kMainTopRowCenterY = 368.0f;
	// ホットバー段(一番下)の中心Y
	constexpr float kHotbarRowCenterY = 600.0f;

	const int32_t column = index % kColumnCount;
	const int32_t row = index / kColumnCount;

	// テクスチャ上のスロットの中心
	const float texX = kLeftSlotCenterX + kSlotPitch * static_cast<float>(column);
	// 0〜8はホットバー段、9〜35はメイン3段(上の段から順)
	const float texY = (row == 0) ? kHotbarRowCenterY : kMainTopRowCenterY + kSlotPitch * static_cast<float>(row - 1);

	// インベントリの左上の座標
	Vector3 inventoryTopLeft = sprites_[0].transforms.translate - Vector3(sprites_[0].transforms.scale.x, sprites_[0].transforms.scale.y, 0.0f);
	// テクスチャ上の座標を画面上の座標に変換
	return inventoryTopLeft + Vector3(texX, texY, 0.0f);
}

void Inventory::HandleClick()
{
	if (!Game::IO::Mouse::IsJustPressed(0)) return;

	const Vector2 mouse = GetMouseUIPosition();

	// グレードアップ先をクリックしたら、Playerにグレードアップを頼む
	if (selectedSlot_ >= 0)
	{
		for (int32_t i = 0; i < upgradeRowCount_; ++i)
		{
			if (!IsMouseOver(upgradeIcons_[1 + i], mouse)) continue;

			// レベルが足りなければ受け取れない
			if (!upgradeRows_[i].canAfford) return;

			if (eventBus_)
			{
				ToolUpgradeRequest request;
				request.slotIndex = selectedSlot_;
				request.sourceID = inventory_->GetInventorySlot(selectedSlot_).itemID;
				request.resultID = upgradeRows_[i].resultID;
				if (i == 0) request.upgradeType = UpgradeType::SpeedUp;
				else if (i == 1) request.upgradeType = UpgradeType::ScaleUp;
				else if (i == 2) request.upgradeType = UpgradeType::PowerUp;
				request.levelCost = upgradeRows_[i].levelCost;

				Event event;
				event.type = EventType::ToolUpgradeRequested;
				event.data = request;

				eventBus_->Notify(event);
			}
			return;
		}
	}

	// スロットをクリックしたら、道具なら選ぶ。道具以外・空なら選択を外す
	for (int32_t i = 0; i < ItemInventory::kSlotCount; ++i)
	{
		if (!IsMouseOver(inventoryIcons_[i], mouse)) continue;

		selectedSlot_ = GetToolInfo(inventory_->GetInventorySlot(i)) ? i : -1;
		return;
	}
}

void Inventory::UpdateUpgradeTable()
{
	upgradeRowCount_ = 0;
	for (auto& icon : upgradeIcons_)
	{
		icon.textureID = -1;
	}

	// 選んでいるスロットの道具(道具でなくなっていたら選択を外す)
	const ItemInfo* sourceInfo = (selectedSlot_ >= 0) ? GetToolInfo(inventory_->GetInventorySlot(selectedSlot_)) : nullptr;
	if (!sourceInfo)
	{
		selectedSlot_ = -1;
		return;
	}

	// 見本
	upgradeIcons_[0].textureID = iconManager_ ? iconManager_->GetIcon(sourceInfo->id) : -1;

	constexpr int32_t kTemporaryCosts[kMaxUpgradeCount] = { 7, 8, 12 };
	upgradeRowCount_ = kMaxUpgradeCount;

	const int32_t level = miningGauge_ ? miningGauge_->GetLevel() : 0;
	arrowCharSize_ = static_cast<int32_t>(kUpgradeArrowCharSize);
	costCharSize_ = static_cast<int32_t>(kUpgradeCostCharSize);
	const Vector2 arrowSize = Game::Asset::Font::MeasureJustTextureSize("→", arrowCharSize_, Vector2{ 0.0f, 0.0f }, 0.0f);

	for (int32_t i = 0; i < upgradeRowCount_; ++i)
	{
		UpgradeRow& row = upgradeRows_[i];
		row.resultID = sourceInfo->id;
		row.levelCost = kTemporaryCosts[i];
		row.canAfford = level >= row.levelCost;

		// グレードアップ先のアイコン
		upgradeIcons_[1 + i].textureID = iconManager_ ? iconManager_->GetIcon(row.resultID) : -1;

		// 矢印と消費レベルは、見本とグレードアップ先の間に、横の真ん中をそろえて置く
		const float rowY = kUpgradeTopRowY + kUpgradeRowPitch * static_cast<float>(i);

		const Vector3 arrowCenter = TextureToScreen(kUpgradeArrowX, rowY + kUpgradeArrowOffsetY);
		row.arrowPos = Vector2{ arrowCenter.x - arrowSize.x * 0.5f, arrowCenter.y - static_cast<float>(arrowCharSize_) * kDigitCenterRatio };

		row.costText = "消費レベル:" + std::to_string(row.levelCost);
		const Vector2 costSize = Game::Asset::Font::MeasureJustTextureSize(row.costText, costCharSize_, Vector2{ 0.0f, 0.0f }, 0.0f);
		const Vector3 costCenter = TextureToScreen(kUpgradeArrowX, rowY + kUpgradeCostOffsetY);
		row.costTextPos = Vector2{ costCenter.x - costSize.x * 0.5f, costCenter.y - static_cast<float>(costCharSize_) * kDigitCenterRatio };
	}
}

void Inventory::DrawUpgradeTable(int32_t rt_ID)
{
	if (selectedSlot_ < 0) return;

	// 見本とグレードアップ先のアイコン
	for (int32_t i = 0; i < 1 + upgradeRowCount_; ++i)
	{
		DrawIcon(upgradeIcons_[i], orthographic_, rt_ID);
	}

	// 矢印と消費レベル(足りないときは赤)
	for (int32_t i = 0; i < upgradeRowCount_; ++i)
	{
		const UpgradeRow& row = upgradeRows_[i];
		Game::Asset::Font::DrawString(rt_ID, "→", arrowCharSize_, row.arrowPos, kArrowColor);

		const Vector4& costColor = row.canAfford ? gaugeFillColor_ : kCostShortColor;
		DrawOutlinedString(rt_ID, row.costText, costCharSize_, row.costTextPos, costColor, kUpgradeCostOutlineWidth);
	}
}

Vector3 Inventory::TextureToScreen(float x, float y) const
{
	// インベントリの左上の座標
	const Vector3 inventoryTopLeft = sprites_[0].transforms.translate - Vector3(sprites_[0].transforms.scale.x, sprites_[0].transforms.scale.y, 0.0f);
	return inventoryTopLeft + Vector3(x, y, 0.0f);
}