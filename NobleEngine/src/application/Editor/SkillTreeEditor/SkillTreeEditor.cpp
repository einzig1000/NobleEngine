#include "SkillTreeEditor.h"
#include <App.h>
#include <externals/MagicEnum/magic_enum.hpp>
#include <algorithm>
#include <cfloat>
#include <cmath>

namespace
{
	// プレビューの大きさ(px)
	constexpr uint32_t kPreviewWidth = 640;
	constexpr uint32_t kPreviewHeight = 480;
	// ホイール1目盛りで何倍にズームするか
	constexpr float kZoomStep = 1.15f;
	// 追加したノードを、選んでいるノードからどれだけ離して置くか
	const Vector2 kAddOffset = { 120.0f, 0.0f };
	const Vector2 kDuplicateOffset = { 24.0f, 24.0f };
}

SkillTreeEditor::SkillTreeEditor()
{
	view_ = std::make_unique<SkillTreeView>();
	view_->SetViewSize(kPreviewWidth, kPreviewHeight);

	Revert();
	view_->ResetView(nodes_);
}

SkillTreeEditor::~SkillTreeEditor()
{}

void SkillTreeEditor::Update()
{
	view_->Update(nodes_, MakePreviewLooks(), hoverNode_);
}

void SkillTreeEditor::Draw()
{
	view_->Draw();
}

void SkillTreeEditor::DrawImGui()
{
	ImGui::Begin("Skill Tree Editor");
	DrawToolbar();
	ImGui::Separator();

	// 左: 一覧 / 右: 選んでいるノード
	if (ImGui::BeginTable("##layout", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
	{
		ImGui::TableSetupColumn("一覧", ImGuiTableColumnFlags_WidthStretch, 0.35f);
		ImGui::TableSetupColumn("ノード", ImGuiTableColumnFlags_WidthStretch, 0.65f);
		ImGui::TableNextColumn();
		DrawNodeList();
		ImGui::TableNextColumn();
		DrawSelectedNode();
		ImGui::EndTable();
	}
	ImGui::End();

	ImGui::Begin("Skill Tree Preview");
	DrawPreview();
	ImGui::End();
}

void SkillTreeEditor::Revert()
{
	nodes_ = App::Data::SkillTree::GetNodes();
	if (selected_ >= static_cast<int32_t>(nodes_.size())) selected_ = -1;
	hoverNode_ = -1;
	isDraggingNode_ = false;
	isDirty_ = false;
}

void SkillTreeEditor::Save()
{
	App::Data::SkillTree::Save(nodes_);
	isDirty_ = false;
	message_ = "保存しました(" + std::to_string(nodes_.size()) + "個)";
}

void SkillTreeEditor::DrawToolbar()
{
	if (ImGui::Button("保存"))
	{
		Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("元に戻す"))
	{
		Revert();
		message_ = "最後に保存した内容に戻しました";
	}
	ImGui::SameLine();
	if (isDirty_)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "保存していない変更があります");
	}
	else
	{
		ImGui::TextDisabled("保存済み");
	}

	if (!message_.empty())
	{
		ImGui::TextUnformatted(message_.c_str());
	}
}

void SkillTreeEditor::DrawNodeList()
{
	if (ImGui::Button("追加"))
	{
		SkillNodeData node;
		node.id = MakeNewID();
		node.name = "新しいスキル";
		node.position = Snap(view_->GetCenter());
		// 選んでいるノードがあれば、それを前提にして横に置く
		if (const SkillNodeData* selected = GetSelected())
		{
			node.parents.push_back(selected->id);
			node.position = Snap(selected->position + kAddOffset);
		}
		AddNode(node);
	}
	ImGui::SetItemTooltip("選んでいるノードがあれば、それを前提にして横に置く");

	ImGui::SameLine();
	ImGui::BeginDisabled(selected_ < 0);
	if (ImGui::Button("複製"))
	{
		if (const SkillNodeData* selected = GetSelected())
		{
			SkillNodeData node = *selected;
			node.id = MakeNewID();
			node.position = Snap(node.position + kDuplicateOffset);
			AddNode(node);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("削除"))
	{
		DeleteNode(selected_);
	}
	ImGui::EndDisabled();

	if (ImGui::BeginListBox("##nodes", ImVec2(-FLT_MIN, 300.0f)))
	{
		for (int32_t i = 0; i < static_cast<int32_t>(nodes_.size()); ++i)
		{
			ImGui::PushID(i);
			const std::string label = std::to_string(nodes_[i].id) + ": " + nodes_[i].name;
			if (ImGui::Selectable(label.c_str(), selected_ == i))
			{
				selected_ = i;
			}
			ImGui::PopID();
		}
		ImGui::EndListBox();
	}
}

void SkillTreeEditor::DrawSelectedNode()
{
	SkillNodeData* node = GetSelected();
	if (!node)
	{
		ImGui::TextDisabled("ノードを選んでください");
		return;
	}

	bool changed = false;

	ImGui::Text("番号: %d", node->id);
	changed |= ImGui::InputText("名前", &node->name);
	changed |= ImGui::InputTextMultiline("説明", &node->description, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4.0f));
	changed |= ImGui::DragFloat2("位置", &node->position.x, 1.0f);

	// アイコン(テクスチャをドラッグ&ドロップ)
	ImGui::Text("アイコン: %s", node->iconPath.empty() ? "(なし)" : node->iconPath.c_str());
	ImGui::Button("ここにテクスチャをドロップ", ImVec2(-FLT_MIN, 0.0f));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int32_t));
			const int32_t textureID = *reinterpret_cast<const int32_t*>(payload->Data);
			// ファイルから読んだテクスチャだけ(レンダーテクスチャはパスが無いので保存できない)
			const TextureData* textureData = Game::Asset::Texture::GetData(textureID);
			if (textureData && !textureData->filePath.empty())
			{
				node->iconPath = textureData->filePath;
				node->iconTextureID = textureID;
				changed = true;
			}
			else
			{
				message_ = "ファイルから読んだテクスチャではないので使えません";
			}
		}
		ImGui::EndDragDropTarget();
	}
	if (!node->iconPath.empty() && ImGui::SmallButton("アイコンを外す"))
	{
		node->iconPath.clear();
		node->iconTextureID = -1;
		changed = true;
	}

	changed |= DrawEffect(*node);
	changed |= DrawCost(*node);
	DrawParents(*node);

	// 見え方の確認用
	if (showLooks_)
	{
		ImGui::Separator();
		bool isUnlocked = previewUnlocked_.contains(node->id);
		if (ImGui::Checkbox("確認用: 開放済みにする", &isUnlocked))
		{
			if (isUnlocked) previewUnlocked_.insert(node->id);
			else previewUnlocked_.erase(node->id);
		}
	}

	if (changed)
	{
		isDirty_ = true;
	}
}

bool SkillTreeEditor::DrawEffect(SkillNodeData& node)
{
	bool changed = false;

	ImGui::SeparatorText("効果");
	if (ImGui::BeginCombo("種類", magic_enum::enum_name(node.effectType).data()))
	{
		for (SkillEffectType type : magic_enum::enum_values<SkillEffectType>())
		{
			if (ImGui::Selectable(magic_enum::enum_name(type).data(), node.effectType == type))
			{
				node.effectType = type;
				changed = true;
			}
		}
		ImGui::EndCombo();
	}

	// 機能の解放は値を使わない
	ImGui::BeginDisabled(node.effectType == SkillEffectType::None || IsFeatureEffect(node.effectType));
	changed |= ImGui::DragFloat("値", &node.effectValue, 0.01f);
	ImGui::EndDisabled();

	ImGui::TextDisabled("表示: %s", MakeSkillEffectText(node.effectType, node.effectValue).c_str());
	return changed;
}

bool SkillTreeEditor::DrawCost(SkillNodeData& node)
{
	bool changed = false;

	ImGui::SeparatorText("コスト");
	changed |= ImGui::DragInt("消費レベル", &node.levelCost, 0.1f, 0, 999);

	// 素材
	for (size_t k = 0; k < node.itemCosts.size(); ++k)
	{
		ImGui::PushID(static_cast<int>(k));
		SkillCostItem& cost = node.itemCosts[k];

		ImGui::SetNextItemWidth(200.0f);
		if (ImGui::BeginCombo("##item", magic_enum::enum_name(cost.id).data()))
		{
			for (ItemID id : magic_enum::enum_values<ItemID>())
			{
				if (id == ItemID::MAX) continue;
				if (ImGui::Selectable(magic_enum::enum_name(id).data(), cost.id == id))
				{
					cost.id = id;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		int count = static_cast<int>(cost.count);
		if (ImGui::DragInt("##count", &count, 0.1f, 1, 9999))
		{
			cost.count = static_cast<uint32_t>(std::max(count, 1));
			changed = true;
		}

		ImGui::SameLine();
		const bool remove = ImGui::SmallButton("外す");
		ImGui::PopID();

		if (remove)
		{
			node.itemCosts.erase(node.itemCosts.begin() + static_cast<std::ptrdiff_t>(k));
			changed = true;
			break;
		}
	}
	if (ImGui::Button("素材を追加"))
	{
		node.itemCosts.push_back(SkillCostItem{ ItemID::Block_Stone, 1 });
		changed = true;
	}
	return changed;
}

void SkillTreeEditor::DrawParents(SkillNodeData& node)
{
	ImGui::SeparatorText("前提(全部開放すると開放できる)");

	for (size_t k = 0; k < node.parents.size(); ++k)
	{
		const int32_t parentID = node.parents[k];
		const int32_t parentIndex = FindIndex(parentID);

		ImGui::PushID(static_cast<int>(k));
		ImGui::Text("%d: %s", parentID, parentIndex >= 0 ? nodes_[parentIndex].name.c_str() : "(ありません)");
		ImGui::SameLine();
		const bool remove = ImGui::SmallButton("外す");
		ImGui::PopID();

		if (remove)
		{
			node.parents.erase(node.parents.begin() + static_cast<std::ptrdiff_t>(k));
			isDirty_ = true;
			break;
		}
	}

	if (ImGui::BeginCombo("前提を追加", "選ぶ..."))
	{
		for (const SkillNodeData& other : nodes_)
		{
			if (other.id == node.id) continue;
			if (std::find(node.parents.begin(), node.parents.end(), other.id) != node.parents.end()) continue;

			const std::string label = std::to_string(other.id) + ": " + other.name;
			if (ImGui::Selectable(label.c_str()))
			{
				ToggleParent(selected_, other.id);
			}
		}
		ImGui::EndCombo();
	}
	ImGui::TextDisabled("プレビューで Ctrl+クリックしても、つなぐ・外すができます");
}

void SkillTreeEditor::DrawPreview()
{
	ImGui::Checkbox("見え方を確認する", &showLooks_);
	ImGui::SetItemTooltip("オン: 「確認用: 開放済み」にしたノードを開放済みとして、ゲームと同じ見え方で描く");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80.0f);
	ImGui::DragFloat("そろえる間隔", &snap_, 1.0f, 0.0f, 200.0f, "%.0f");
	ImGui::SameLine();
	if (ImGui::Button("表示を戻す"))
	{
		view_->ResetView(nodes_);
	}
	ImGui::TextDisabled("クリック: 選ぶ / ドラッグ: 動かす / Ctrl+クリック: 前提をつなぐ・外す / 右ドラッグ: 画面移動 / ホイール: ズーム");

	// ツリーの絵を出して、その上に透明なボタンを重ねてマウスを受け取る
	const ImVec2 size = ImVec2(view_->GetViewSize().x, view_->GetViewSize().y);
	const ImVec2 origin = ImGui::GetCursorScreenPos();
	ImGui::Image(ImTextureID(Game::Asset::RenderTexture::GetRenderTextureGPUPtr(view_->GetRenderTextureID())), size);
	ImGui::SetCursorScreenPos(origin);
	ImGui::InvisibleButton("##canvas", size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool isHovered = ImGui::IsItemHovered();
	const bool isActive = ImGui::IsItemActive();

	const ImGuiIO& io = ImGui::GetIO();
	const Vector2 viewPos = Vector2{ io.MousePos.x - origin.x, io.MousePos.y - origin.y };
	const Vector2 treePos = view_->ViewToTree(viewPos);

	hoverNode_ = isHovered ? view_->PickNode(nodes_, treePos) : -1;

	// ホイールでズーム
	if (isHovered && io.MouseWheel != 0.0f)
	{
		view_->ZoomAt(viewPos, std::pow(kZoomStep, io.MouseWheel));
	}

	// 右ドラッグで画面移動
	if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f))
	{
		view_->Pan(Vector2{ io.MouseDelta.x, io.MouseDelta.y });
	}

	// 左クリック: Ctrl なら前提をつなぐ・外す、それ以外は選ぶ
	if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		if (io.KeyCtrl && selected_ >= 0 && hoverNode_ >= 0 && hoverNode_ != selected_)
		{
			ToggleParent(selected_, nodes_[hoverNode_].id);
		}
		else
		{
			selected_ = hoverNode_;
			isDraggingNode_ = (hoverNode_ >= 0);
		}
	}

	// 左ドラッグでノードを動かす
	if (isDraggingNode_ && selected_ >= 0 && isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))
	{
		nodes_[selected_].position += Vector2{ io.MouseDelta.x, io.MouseDelta.y } / view_->GetZoom();
		isDirty_ = true;
	}
	// 離したら間隔にそろえる
	if (isDraggingNode_ && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		if (selected_ >= 0)
		{
			nodes_[selected_].position = Snap(nodes_[selected_].position);
		}
		isDraggingNode_ = false;
	}

	// 選んでいるノードの枠と、番号・名前を重ねて描く
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->PushClipRect(origin, ImVec2(origin.x + size.x, origin.y + size.y), true);
	for (int32_t i = 0; i < static_cast<int32_t>(nodes_.size()); ++i)
	{
		const Vector2 p = view_->TreeToView(nodes_[i].position);
		const ImVec2 center = ImVec2(origin.x + p.x, origin.y + p.y);
		const float half = SkillTreeView::kNodeSize * 0.5f * view_->GetZoom();

		if (i == selected_)
		{
			drawList->AddRect(ImVec2(center.x - half - 3.0f, center.y - half - 3.0f), ImVec2(center.x + half + 3.0f, center.y + half + 3.0f), IM_COL32(255, 220, 0, 255), 0.0f, 0, 3.0f);
		}
		const std::string label = std::to_string(nodes_[i].id) + " " + nodes_[i].name;
		drawList->AddText(ImVec2(center.x - half, center.y + half + 2.0f), IM_COL32(255, 255, 255, 220), label.c_str());
	}
	drawList->PopClipRect();

	// カーソルのノードの中身
	if (hoverNode_ >= 0 && !isDraggingNode_)
	{
		const SkillNodeData& node = nodes_[hoverNode_];
		ImGui::BeginTooltip();
		ImGui::Text("%d: %s", node.id, node.name.c_str());
		if (!node.description.empty()) ImGui::TextUnformatted(node.description.c_str());
		ImGui::TextColored(ImVec4(0.45f, 1.0f, 0.45f, 1.0f), "%s", MakeSkillEffectText(node.effectType, node.effectValue).c_str());
		ImGui::Text("レベル %d", node.levelCost);
		for (const SkillCostItem& cost : node.itemCosts)
		{
			ImGui::Text("%s x%u", magic_enum::enum_name(cost.id).data(), cost.count);
		}
		ImGui::EndTooltip();
	}
}

SkillNodeData* SkillTreeEditor::GetSelected()
{
	if (selected_ < 0 || selected_ >= static_cast<int32_t>(nodes_.size())) return nullptr;
	return &nodes_[selected_];
}

int32_t SkillTreeEditor::FindIndex(int32_t id) const
{
	for (int32_t i = 0; i < static_cast<int32_t>(nodes_.size()); ++i)
	{
		if (nodes_[i].id == id) return i;
	}
	return -1;
}

int32_t SkillTreeEditor::MakeNewID() const
{
	int32_t maxID = -1;
	for (const SkillNodeData& node : nodes_)
	{
		maxID = std::max(maxID, node.id);
	}
	return maxID + 1;
}

void SkillTreeEditor::AddNode(const SkillNodeData& node)
{
	nodes_.push_back(node);
	selected_ = static_cast<int32_t>(nodes_.size()) - 1;
	isDirty_ = true;
}

void SkillTreeEditor::DeleteNode(int32_t index)
{
	if (index < 0 || index >= static_cast<int32_t>(nodes_.size())) return;

	const int32_t id = nodes_[index].id;
	nodes_.erase(nodes_.begin() + index);

	// ほかのノードの前提からも消す
	for (SkillNodeData& node : nodes_)
	{
		std::erase(node.parents, id);
	}
	previewUnlocked_.erase(id);

	selected_ = -1;
	hoverNode_ = -1;
	isDraggingNode_ = false;
	isDirty_ = true;
	message_ = "ノード" + std::to_string(id) + " を削除しました";
}

void SkillTreeEditor::ToggleParent(int32_t childIndex, int32_t parentID)
{
	if (childIndex < 0 || childIndex >= static_cast<int32_t>(nodes_.size())) return;
	SkillNodeData& child = nodes_[childIndex];

	// もうつながっていれば外す
	auto it = std::find(child.parents.begin(), child.parents.end(), parentID);
	if (it != child.parents.end())
	{
		child.parents.erase(it);
		isDirty_ = true;
		message_ = "前提を外しました";
		return;
	}

	// 自分自身や、自分の先にあるノードを前提にするとループになって永遠に開放できない
	if (parentID == child.id || IsAncestor(child.id, parentID))
	{
		message_ = "ループになるのでつなげません";
		return;
	}

	child.parents.push_back(parentID);
	isDirty_ = true;
	message_ = "前提をつなぎました";
}

bool SkillTreeEditor::IsAncestor(int32_t ancestorID, int32_t nodeID) const
{
	// nodeID から前提を順にたどる
	std::vector<int32_t> stack = { nodeID };
	std::unordered_set<int32_t> visited;
	while (!stack.empty())
	{
		const int32_t id = stack.back();
		stack.pop_back();
		if (id == ancestorID) return true;
		if (!visited.insert(id).second) continue;

		const int32_t index = FindIndex(id);
		if (index < 0) continue;
		for (int32_t parentID : nodes_[index].parents)
		{
			stack.push_back(parentID);
		}
	}
	return false;
}

Vector2 SkillTreeEditor::Snap(const Vector2& position) const
{
	if (snap_ <= 0.0f) return position;
	return Vector2{ std::round(position.x / snap_) * snap_, std::round(position.y / snap_) * snap_ };
}

std::vector<SkillTreeView::NodeLook> SkillTreeEditor::MakePreviewLooks() const
{
	std::vector<SkillTreeView::NodeLook> looks(nodes_.size(), SkillTreeView::NodeLook::Unlocked);
	if (!showLooks_) return looks;

	for (size_t i = 0; i < nodes_.size(); ++i)
	{
		if (previewUnlocked_.contains(nodes_[i].id)) continue;

		// 前提が全部「確認用: 開放済み」なら1つ先、そうでなければその先
		const bool areParentsUnlocked = std::all_of(nodes_[i].parents.begin(), nodes_[i].parents.end(),
			[&](int32_t parentID) { return previewUnlocked_.contains(parentID); });
		looks[i] = areParentsUnlocked ? SkillTreeView::NodeLook::Available : SkillTreeView::NodeLook::Silhouette;
	}
	return looks;
}
