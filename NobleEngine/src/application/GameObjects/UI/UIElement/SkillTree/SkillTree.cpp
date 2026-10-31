#include "SkillTree.h"
#include <GameObjects/Character/ItemInventory/ItemInventory.h>
#include <GameObjects/Character/MiningPointGauge/MiningPointGauge.h>
#include <System/EventBus/EventBus.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <App.h>
#include <algorithm>
#include <cmath>

namespace
{
	// ホイール1目盛り(120)で何倍にズームするか
	constexpr float kZoomStep = 1.15f;
	// これ以上動かしたらクリックではなくドラッグ(px)
	constexpr float kDragThreshold = 6.0f;
	// ノードのある範囲から、これ以上外は見に行けない(ツリー上のpx)
	constexpr float kPanMargin = 200.0f;

	// ツールチップ
	constexpr float kTooltipPadding = 10.0f;
	constexpr float kTooltipLineSpacing = 1.25f;
	const Vector2 kTooltipCursorOffset = { 18.0f, 18.0f };
	const Vector4 kTooltipBackColor = { 0.10f, 0.03f, 0.16f, 0.92f };
	const Vector4 kTitleColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	const Vector4 kTextColor = { 0.80f, 0.80f, 0.80f, 1.0f };
	const Vector4 kEffectColor = { 0.45f, 1.0f, 0.45f, 1.0f };
	const Vector4 kShortColor = { 1.0f, 0.35f, 0.35f, 1.0f };
	const Vector4 kSubColor = { 0.60f, 0.60f, 0.60f, 1.0f };

	// 状態 → 見え方
	SkillTreeView::NodeLook ToLook(SkillNodeStatus status)
	{
		switch (status)
		{
		case SkillNodeStatus::Unlocked:			return SkillTreeView::NodeLook::Unlocked;
		case SkillNodeStatus::Unlockable:		return SkillTreeView::NodeLook::Affordable;
		case SkillNodeStatus::NotEnoughCost:	return SkillTreeView::NodeLook::Available;
		case SkillNodeStatus::Locked:
		default:								return SkillTreeView::NodeLook::Silhouette;
		}
	}
}

SkillTree::SkillTree()
{
	view_ = std::make_unique<SkillTreeView>();

	BuildLayout();

	// ツールチップの背景
	tooltipBack_.render = SkillTreeView::CreateSpriteRender();
	tooltipBack_.data.textureID = Game::Asset::Texture::Load("assets/engine/texture/white1x1.png");
	tooltipBack_.data.textureSize = Vector2{ 1.0f, 1.0f };
	tooltipBack_.color = kTooltipBackColor;
}

SkillTree::~SkillTree()
{}

void SkillTree::Initialize()
{
	// 開くたびに操作の途中状態は捨てる(見ている場所とズームはそのまま)
	isPressing_ = false;
	isDragging_ = false;
	hoverNode_ = -1;
	tooltipLines_.clear();
	prevMouse_ = GetMouseUIPosition();
}

void SkillTree::Update(int32_t cameraID)
{
	orthographic_ = Game::Camera::Getter::GetOrthoProjectionMatrix(cameraID);

	RefreshNodes();
	UpdateStatuses();
	UpdateInput();
	UpdateTooltip();

	// ツリーの行列
	view_->Update(nodes_, looks_, hoverNode_);

	// レベルの文字
	if (miningGauge_)
	{
		levelText_ = std::to_string(miningGauge_->GetLevel());
		if (ElementInstanceData* marker = GetPart(Part::LevelText))
		{
			levelTextPos_ = GetDigitTextPosition(levelText_, levelCharSize_, GetCenter(marker->data));
		}
	}

	// 静的スプライトの更新
	UpdateStaticRenderObject(orthographic_);
}

void SkillTree::Draw(int32_t rt_ID)
{
	// ツリーを専用のレンダーテクスチャに描く
	view_->Draw();

	// 背景とツリー(TreeArea の絵がツリーのレンダーテクスチャになっている)
	DrawStaticRenderObject(rt_ID);

	// レベル
	if (miningGauge_ && !levelText_.empty())
	{
		DrawOutlinedString(rt_ID, levelText_, levelCharSize_, levelTextPos_, levelColor_, 2.0f);
	}

	// ツールチップ(一番手前)
	if (!tooltipLines_.empty())
	{
		DrawRenderObject(rt_ID, tooltipBack_);

		const float lineHeight = static_cast<float>(tooltipCharSize_) * kTooltipLineSpacing;
		Vector2 pos = tooltipPos_ + Vector2{ kTooltipPadding, kTooltipPadding };
		for (const TooltipLine& line : tooltipLines_)
		{
			if (!line.text.empty())
			{
				Game::Asset::Font::DrawString(rt_ID, line.text, tooltipCharSize_, pos, line.color);
			}
			pos.y += lineHeight;
		}
	}
}

void SkillTree::DrawImGui()
{
	ImGui::Begin("SkillTree");
	if (ImGui::Button("UIデータを読み直す"))
	{
		BuildLayout();
	}
	ImGui::SameLine();
	if (ImGui::Button("表示を戻す"))
	{
		view_->ResetView(nodes_);
	}
	ImGui::Text("ズーム: %.2f  中心: (%.0f, %.0f)", view_->GetZoom(), view_->GetCenter().x, view_->GetCenter().y);
	ImGui::Text("カーソルのノード: %d", hoverNode_ >= 0 ? nodes_[hoverNode_].id : -1);
	ImGui::DragInt("ツールチップの文字の大きさ", &tooltipCharSize_, 1.0f, 8, 64);
	ImGui::End();
}

void SkillTree::BuildLayout()
{
	// sprites_にJsonのデータを入れてrenderObjectを生成する
	BuildSprites(UIElementType::SkillTree);
	// Partで分類しlabelIndex順に並べたarrayを生成する
	parts_ = FindParts<Part>();

	// ツリーを映す場所と同じ大きさのレンダーテクスチャを作り、その場所の絵をツリーに差し替える
	if (ElementInstanceData* area = GetPart(Part::TreeArea))
	{
		const Vector2 size = GetDrawSize(area->data);
		view_->SetViewSize(static_cast<uint32_t>(std::abs(size.x)), static_cast<uint32_t>(std::abs(size.y)));
		area->data.textureID = view_->GetRenderTextureID();
	}
}

ElementInstanceData* SkillTree::GetPart(Part part, int32_t labelIndex)
{
	return GetSprite(parts_[static_cast<size_t>(part)], labelIndex);
}

void SkillTree::RefreshNodes()
{
	const int32_t revision = App::Data::SkillTree::GetRevision();
	if (revision == nodesRevision_) return;

	const bool isFirst = (nodesRevision_ < 0);
	nodesRevision_ = revision;
	nodes_ = App::Data::SkillTree::GetNodes();
	hoverNode_ = -1;

	// 最初だけ、全体が見える位置にする
	if (isFirst)
	{
		view_->ResetView(nodes_);
	}
}

void SkillTree::UpdateStatuses()
{
	statuses_.resize(nodes_.size());
	looks_.resize(nodes_.size());
	for (size_t i = 0; i < nodes_.size(); ++i)
	{
		// 開放状況が無い(プレイヤーがいない)ときは全部開放済みとして見せる
		statuses_[i] = skillState_ ? skillState_->GetStatus(nodes_[i], miningGauge_, inventory_) : SkillNodeStatus::Unlocked;
		looks_[i] = ToLook(statuses_[i]);
	}
}

void SkillTree::UpdateInput()
{
	const Vector2 mouse = GetMouseUIPosition();

	// タブを押したらインベントリへ
	if (Game::IO::Mouse::IsJustPressed(0) && nextUIMode_)
	{
		const ElementInstanceData* tab = GetPart(Part::InventoryTab);
		if (tab && IsMouseOver(tab->data, mouse))
		{
			*nextUIMode_ = UIScreenType::Inventory;
			prevMouse_ = mouse;
			return;
		}
	}

	const ElementInstanceData* area = GetPart(Part::TreeArea);
	if (!area)
	{
		hoverNode_ = -1;
		prevMouse_ = mouse;
		return;
	}

	// ツリーを映している四角の左上を0,0にした座標
	const Vector2 areaSize = GetDrawSize(area->data);
	const Vector2 areaTopLeft = GetCenter(area->data) - Vector2{ std::abs(areaSize.x), std::abs(areaSize.y) } * 0.5f;
	const Vector2 viewPos = mouse - areaTopLeft;
	const bool isInArea = IsMouseOver(area->data, mouse);

	// ホイールでズーム(カーソルの下の点は動かない)
	const int32_t wheel = Game::IO::Mouse::GetWheel();
	if (isInArea && wheel != 0)
	{
		view_->ZoomAt(viewPos, std::pow(kZoomStep, static_cast<float>(wheel) / 120.0f));
	}

	// 左ボタンを押した時点では、ドラッグかクリックかはまだ分からない
	if (isInArea && Game::IO::Mouse::IsJustPressed(0))
	{
		isPressing_ = true;
		isDragging_ = false;
		pressMouse_ = mouse;
	}

	// 押したまま一定以上動かしたらドラッグ
	if (isPressing_ && Game::IO::Mouse::IsHeld(0))
	{
		const Vector2 fromPress = mouse - pressMouse_;
		if (!isDragging_ && fromPress.x * fromPress.x + fromPress.y * fromPress.y > kDragThreshold * kDragThreshold)
		{
			isDragging_ = true;
		}
		if (isDragging_)
		{
			view_->Pan(mouse - prevMouse_);
		}
	}
	view_->ClampCenter(nodes_, kPanMargin);

	// カーソルが乗っているノード(ドラッグ中は出さない)
	hoverNode_ = (isInArea && !isDragging_) ? view_->PickNode(nodes_, view_->ViewToTree(viewPos)) : -1;

	// 離したとき、動かしていなければクリック
	if (isPressing_ && Game::IO::Mouse::IsJustReleased(0))
	{
		if (!isDragging_ && hoverNode_ >= 0)
		{
			OnClickNode(hoverNode_);
		}
		isPressing_ = false;
		isDragging_ = false;
	}
	// ウィンドウの外で離した場合など
	if (!Game::IO::Mouse::IsHeld(0) && !Game::IO::Mouse::IsJustReleased(0))
	{
		isPressing_ = false;
		isDragging_ = false;
	}

	prevMouse_ = mouse;
}

void SkillTree::UpdateTooltip()
{
	tooltipLines_.clear();
	if (hoverNode_ < 0 || hoverNode_ >= static_cast<int32_t>(nodes_.size())) return;

	const SkillNodeData& node = nodes_[hoverNode_];
	const SkillNodeStatus status = statuses_[hoverNode_];

	if (status == SkillNodeStatus::Locked)
	{
		// さらに先のノードは中身を見せない
		tooltipLines_.push_back({ "???", kTitleColor });
		tooltipLines_.push_back({ "前のスキルを開放すると分かる", kSubColor });
	}
	else
	{
		// 名前
		tooltipLines_.push_back({ node.name, kTitleColor });

		// 説明(\nで改行)
		size_t start = 0;
		while (start <= node.description.size() && !node.description.empty())
		{
			const size_t end = node.description.find('\n', start);
			tooltipLines_.push_back({ node.description.substr(start, end == std::string::npos ? std::string::npos : end - start), kTextColor });
			if (end == std::string::npos) break;
			start = end + 1;
		}

		// 効果
		const std::string effect = MakeSkillEffectText(node.effectType, node.effectValue);
		if (!effect.empty())
		{
			tooltipLines_.push_back({ effect, kEffectColor });
		}

		if (status == SkillNodeStatus::Unlocked)
		{
			tooltipLines_.push_back({ "開放済み", kSubColor });
		}
		else
		{
			// レベル
			if (node.levelCost > 0)
			{
				const bool isEnough = !miningGauge_ || miningGauge_->GetLevel() >= node.levelCost;
				tooltipLines_.push_back({ "必要レベル " + std::to_string(node.levelCost), isEnough ? kTextColor : kShortColor });
			}
			// 素材(名前は今のところ ItemID の名前)
			for (const SkillCostItem& cost : node.itemCosts)
			{
				const uint32_t have = inventory_ ? inventory_->CountItem(cost.id) : 0;
				const std::string text = std::string(magic_enum::enum_name(cost.id)) + " x" + std::to_string(cost.count) + " (所持 " + std::to_string(have) + ")";
				tooltipLines_.push_back({ text, have >= cost.count ? kTextColor : kShortColor });
			}

			if (status == SkillNodeStatus::Unlockable)
			{
				tooltipLines_.push_back({ "クリックで開放", kEffectColor });
			}
			else
			{
				tooltipLines_.push_back({ "レベルか素材が足りない", kShortColor });
			}
		}
	}

	// 大きさを測る
	float width = 0.0f;
	for (const TooltipLine& line : tooltipLines_)
	{
		if (line.text.empty()) continue;
		const Vector2 textSize = Game::Asset::Font::MeasureJustTextureSize(line.text, tooltipCharSize_, Vector2{ 0.0f, 0.0f }, 0.0f);
		width = std::max(width, textSize.x);
	}
	const float lineHeight = static_cast<float>(tooltipCharSize_) * kTooltipLineSpacing;
	const Vector2 size = Vector2{ width + kTooltipPadding * 2.0f, lineHeight * static_cast<float>(tooltipLines_.size()) + kTooltipPadding * 2.0f };

	// カーソルの右下に置く。画面からはみ出るなら反対側へ
	const Vector2 mouse = GetMouseUIPosition();
	const float screenWidth = static_cast<float>(Constexprs::kInitialWindowWidth);
	const float screenHeight = static_cast<float>(Constexprs::kInitialWindowHeight);
	Vector2 pos = mouse + kTooltipCursorOffset;
	if (pos.x + size.x > screenWidth) pos.x = mouse.x - kTooltipCursorOffset.x - size.x;
	if (pos.y + size.y > screenHeight) pos.y = screenHeight - size.y;
	pos.x = std::max(pos.x, 0.0f);
	pos.y = std::max(pos.y, 0.0f);
	tooltipPos_ = pos;

	UpdateQuad(tooltipBack_, pos + size * 0.5f, size, Vector3{ 0.0f, 0.0f, 0.0f }, orthographic_);
}

void SkillTree::OnClickNode(int32_t nodeIndex)
{
	if (nodeIndex < 0 || nodeIndex >= static_cast<int32_t>(nodes_.size())) return;

	// 開放できないときは何もしない(失敗の音などは後で)
	if (statuses_[nodeIndex] != SkillNodeStatus::Unlockable) return;
	if (!eventBus_) return;

	// 支払いと開放はプレイヤーがやる(UIは頼むだけ)
	Event event;
	event.type = EventType::SkillUnlockRequested;
	event.data = nodes_[nodeIndex].id;
	eventBus_->Notify(event);
}