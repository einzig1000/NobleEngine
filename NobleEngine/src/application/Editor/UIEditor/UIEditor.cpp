#include "UIEditor.h"
#include <App.h>

UIEditor::UIEditor()
{
	rt_ID = Game::Asset::RenderTexture::CreateRenderTexture(Constexprs::kInitialWindowWidth, Constexprs::kInitialWindowHeight, "UIEditorTexture");

	c_ID = Game::Camera::AddCamera("UIEditorCamera");
}

UIEditor::~UIEditor()
{}

void UIEditor::Update()
{
	Game::Camera::Update(c_ID);

}

void UIEditor::Draw()
{

}

void UIEditor::DrawImGui()
{
	ImGui::Begin("UI Editor");


	// enum配列
	auto uiScreenTypeValues = magic_enum::enum_values<UIScreenType>();
	auto uiScreenTypeNames = magic_enum::enum_names<UIScreenType>();
	auto uiElementTypeValues = magic_enum::enum_values<UIElementType>();
	auto uiElementTypeNames = magic_enum::enum_names<UIElementType>();

	if (ImGui::TreeNodeEx("エレメントデータ作成", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// エレメントタイプ選択
		{
			if (ImGui::BeginCombo("エレメントタイプリスト", magic_enum::enum_name(elementType_).data()))
			{
				for (std::size_t i = 0; i < uiElementTypeValues.size(); i++)
				{
					UIElementType value = uiElementTypeValues[i];
					bool selected = (elementType_ == value);

					if (ImGui::Selectable(uiElementTypeNames[i].data(), selected))
					{
						elementType_ = value;
						elementData_ = *(App::Data::UI::Get(elementType_));
					}
					if (selected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}


		// 追加済エレメント
		if (ImGui::BeginListBox("エレメントリスト"))
		{
			const ImGuiStyle& style = ImGui::GetStyle();
			const float copyButtonWidth = ImGui::CalcTextSize("delete").x + style.FramePadding.x * 2.0f;
			const float selectableWidth = std::max(ImGui::GetContentRegionAvail().x - copyButtonWidth - style.ItemSpacing.x, 1.0f);
			for (int32_t i = 0; i < static_cast<int32_t>(elementData_.size()); ++i)
			{
				ImGui::PushID(i);
				ImGui::BeginGroup();

				const bool clicked = ImGui::Selectable("", selectedElementIndex_ == i, 0, ImVec2(selectableWidth, 0.0f));
				ImGui::SameLine();
				if (ImGui::SmallButton("delete"))
				{

				}
				if (clicked)
				{
					selectedElementIndex_ = i;
				}

				ImGui::EndGroup();
				ImGui::PopID();
			}
			ImGui::EndListBox();
		}

		ImGui::Separator();

		// 選択中のエレメントのパラメータ編集
		{
			if (selectedElementIndex_ > 0)
			{
				ImGui::DragFloat2("translate", &elementData_[selectedElementIndex_].translateRate.x, 0.1f);
				ImGui::DragFloat2("scale", &elementData_[selectedElementIndex_].scale.x, 0.1f);
				ImGui::DragFloat2("rotate", &elementData_[selectedElementIndex_].rotate.x, 0.1f);
			}


			//// テクスチャ表示 & DADの設定
			//if (ImGui::BeginDragDropTarget())
			//{
			//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
			//	{
			//		IM_ASSERT(payload->DataSize == sizeof(int32_t));
			//		textureID_ = *reinterpret_cast<const int32_t*>(payload->Data);
			//	}
			//	ImGui::EndDragDropTarget();
			//}
		}


		ImGui::TreePop();
	}

	ImGui::Separator();

	if (ImGui::TreeNodeEx("親データ作成", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// ミニプレビュー画面 兼 フルスクボタン
		if (ImGui::ImageButton("##ss", ImTextureID(Game::Asset::RenderTexture::GetRenderTextureGPUPtr(rt_ID)), ImVec2(128, 128)))
		{
			//fullscreen_ = !fullscreen_;
		}


		// なんのエレメントタイプを要求するか

		ImGui::TreePop();
	}

	//// フルスクリーン表示
	//if (fullscreen_)
	//{
	//	ImGui::Begin("Item Preview");
	//
	//	ImGui::Image(ImTextureID(Game::Asset::RenderTexture::GetRenderTextureGPUPtr(renderTextureID_)), ImVec2(512, 512));
	//
	//	if (ImGui::BeginDragDropTarget())
	//	{
	//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
	//		{
	//			IM_ASSERT(payload->DataSize == sizeof(int32_t));
	//			textureID_ = *reinterpret_cast<const int32_t*>(payload->Data);
	//		}
	//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_MODEL_ID"))
	//		{
	//			IM_ASSERT(payload->DataSize == sizeof(int32_t));
	//			renderObject_->modelID_ = *reinterpret_cast<const int32_t*>(payload->Data);
	//		}
	//		ImGui::EndDragDropTarget();
	//	}
	//
	//	ImGui::End();
	//}

	ImGui::End();
}

//#include "UIEditor.h"
//#include <App.h>
//#include <EngineDefinition/EngineConstexprs.h>
//#include <GameObjects/UI/UIElement/IUIElement.h>
//#include <GameObjects/UI/UIElement/Inventory/Inventory.h>
//#include <GameObjects/UI/UIElement/Hotbar/Hotbar.h>
//#include <GameObjects/UI/UIElement/MiningMode/MiningMode.h>
//#include <GameObjects/UI/UIElement/MiningPointGauge/MiningPointGaugeUI.h>
//#include <externals/imgui/imgui_stdlib.h>
//#include <algorithm>
//#include <cmath>
//#include <numbers>
//
//namespace
//{
//	// 基準解像度
//	constexpr float kCanvasWidth = static_cast<float>(Constexprs::kInitialWindowWidth);
//	constexpr float kCanvasHeight = static_cast<float>(Constexprs::kInitialWindowHeight);
//	// 目印(テクスチャなし)をクリックで選べる範囲(基準解像度のpx)
//	constexpr float kMarkerPickRadius = 10.0f;
//	// 度 → ラジアン
//	constexpr float kDegToRad = std::numbers::pi_v<float> / 180.0f;
//
//	// 色
//	const ImVec4 kErrorColor = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
//	const ImVec4 kWarningColor = ImVec4(1.0f, 0.85f, 0.3f, 1.0f);
//	const ImVec4 kNoteColor = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
//
//	// enum class Part の名前を並べる
//	// (Inventory::Part なら、コンパイラが "MainSlots", "HotbarSlots", ... を返す関数を作る)
//	template<typename PartEnum>
//	std::vector<std::string_view> ToNames()
//	{
//		const auto names = magic_enum::enum_names<PartEnum>();
//		return std::vector<std::string_view>(names.begin(), names.end());
//	}
//
//	// パスからファイル名だけ取り出す
//	std::string GetFileName(const std::string& path)
//	{
//		const size_t slash = path.find_last_of("/\\");
//		return (slash == std::string::npos) ? path : path.substr(slash + 1);
//	}
//
//	// 一覧に出す名前
//	std::string MakeListName(int32_t index, const ElementData& data)
//	{
//		const std::string label = data.label.empty() ? "(飾り)" : data.label + "[" + std::to_string(data.labelIndex) + "]";
//		const std::string texture = data.texturePath.empty() ? "目印" : GetFileName(data.texturePath);
//		return std::to_string(index) + ": " + label + "  " + texture;
//	}
//}
//
//
//UIEditor::UIEditor()
//{
//	rt_ID = Game::Asset::RenderTexture::CreateRenderTexture(Constexprs::kInitialWindowWidth, Constexprs::kInitialWindowHeight, "UIEditorTexture");
//
//	c_ID = Game::Camera::AddCamera("UIEditorCamera");
//
//	// 最初はInventoryを開いておく
//	SelectElementType(UIElementType::Inventory);
//}
//
//UIEditor::~UIEditor()
//{}
//
//void UIEditor::Update()
//{
//	Game::Camera::Update(c_ID);
//	const Matrix4x4 orthographic = Game::Camera::Getter::GetOrthoProjectionMatrix(c_ID);
//
//	// プレビュー用のスプライトを、編集中のデータに合わせる
//	if (previewSprites_.size() != elementData_.size())
//	{
//		previewSprites_.resize(elementData_.size());
//	}
//	for (size_t i = 0; i < elementData_.size(); ++i)
//	{
//		ElementInstanceData& sprite = previewSprites_[i];
//		if (!sprite.render)
//		{
//			sprite.render = IUIElement::CreateSpriteRender();
//		}
//		sprite.data = elementData_[i];
//		IUIElement::UpdateQuad(sprite, IUIElement::GetCenter(sprite.data), IUIElement::GetDrawSize(sprite.data), sprite.data.rotate, orthographic);
//	}
//}
//
//void UIEditor::Draw()
//{
//	// 並び順(奥から手前)に描く。目印(テクスチャなし)は描かない
//	for (const ElementInstanceData& sprite : previewSprites_)
//	{
//		if (!sprite.render || sprite.data.textureID < 0) continue;
//
//		sprite.render->SetBRegisterData(0, ShaderType::VertexShader, &sprite.wvp);
//		sprite.render->SetBRegisterData(1, ShaderType::VertexShader, &sprite.world);
//		sprite.render->SetBRegisterData(0, ShaderType::PixelShader, &sprite.color);
//		sprite.render->SetBRegisterData(1, ShaderType::PixelShader, &sprite.data.textureID);
//		sprite.render->Draw(rt_ID);
//	}
//}
//
//void UIEditor::DrawImGui()
//{
//	ImGui::Begin("UI Editor");
//
//	DrawTypeSelector();
//	if (elementType_ != UIElementType::MAX)
//	{
//		ImGui::Separator();
//		DrawPartCheck();
//		ImGui::Separator();
//		DrawElementList();
//		ImGui::Separator();
//		DrawSelectedElement();
//		ImGui::Separator();
//		DrawGridTool();
//	}
//
//	ImGui::End();
//
//	ImGui::Begin("UI Editor Preview");
//	DrawPreview();
//	ImGui::End();
//}
//
//void UIEditor::SelectElementType(UIElementType type)
//{
//	elementType_ = type;
//	Revert();
//}
//
//void UIEditor::Revert()
//{
//	elementData_.clear();
//	if (const std::vector<ElementData>* data = App::Data::UI::Get(elementType_))
//	{
//		elementData_ = *data;
//	}
//	selectedElementIndex_ = -1;
//	isDirty_ = false;
//	textureMessage_.clear();
//}
//
//void UIEditor::DrawTypeSelector()
//{
//	if (ImGui::BeginCombo("UIElement", magic_enum::enum_name(elementType_).data()))
//	{
//		for (UIElementType type : magic_enum::enum_values<UIElementType>())
//		{
//			if (type == UIElementType::MAX) continue;
//
//			const bool selected = (elementType_ == type);
//			if (ImGui::Selectable(magic_enum::enum_name(type).data(), selected))
//			{
//				// 保存していない変更は捨てる
//				SelectElementType(type);
//			}
//			if (selected) ImGui::SetItemDefaultFocus();
//		}
//		ImGui::EndCombo();
//	}
//
//	if (elementType_ == UIElementType::MAX) return;
//
//	// 保存すると、UIConfig.json とデータ置き場の両方が更新される
//	if (ImGui::Button("保存"))
//	{
//		App::Data::UI::Save(elementType_, elementData_);
//		isDirty_ = false;
//	}
//	ImGui::SameLine();
//	if (ImGui::Button("元に戻す"))
//	{
//		Revert();
//	}
//	ImGui::SameLine();
//	if (isDirty_)
//	{
//		ImGui::TextColored(kWarningColor, "保存していない変更があります");
//	}
//	else
//	{
//		ImGui::TextUnformatted("保存済み");
//	}
//	ImGui::TextColored(kNoteColor, "保存したあと、ゲーム側の各UIの「UIデータを読み直す」で反映される");
//}
//
//void UIEditor::DrawPartCheck()
//{
//	if (!ImGui::TreeNodeEx("コードが使うパーツ", ImGuiTreeNodeFlags_DefaultOpen)) return;
//
//	const std::vector<std::string_view> partNames = GetPartNames(elementType_);
//	if (partNames.empty())
//	{
//		ImGui::TextColored(kNoteColor, "このUIElementのコードはパーツを使っていない(全部飾り)");
//	}
//
//	for (std::string_view name : partNames)
//	{
//		// labelIndex ごとの個数を数える
//		std::vector<int32_t> counts;
//		for (const ElementData& data : elementData_)
//		{
//			if (data.label != name || data.labelIndex < 0) continue;
//			if (data.labelIndex >= static_cast<int32_t>(counts.size()))
//			{
//				counts.resize(static_cast<size_t>(data.labelIndex) + 1, 0);
//			}
//			++counts[data.labelIndex];
//		}
//
//		// 抜けと重複
//		std::string problems;
//		for (int32_t i = 0; i < static_cast<int32_t>(counts.size()); ++i)
//		{
//			if (counts[i] == 0) problems += " 抜け:" + std::to_string(i);
//			if (counts[i] > 1) problems += " 重複:" + std::to_string(i);
//		}
//
//		const std::string nameStr(name);
//		if (counts.empty())
//		{
//			ImGui::TextColored(kErrorColor, "%s : ありません", nameStr.c_str());
//		}
//		else if (!problems.empty())
//		{
//			ImGui::TextColored(kWarningColor, "%s : %d個 %s", nameStr.c_str(), static_cast<int32_t>(counts.size()), problems.c_str());
//		}
//		else
//		{
//			ImGui::Text("%s : %d個", nameStr.c_str(), static_cast<int32_t>(counts.size()));
//		}
//	}
//
//	// コードが使っていない label(打ち間違いに気づけるように)
//	std::vector<std::string> unusedLabels;
//	for (const ElementData& data : elementData_)
//	{
//		if (data.label.empty()) continue;
//		if (std::find(partNames.begin(), partNames.end(), data.label) != partNames.end()) continue;
//		if (std::find(unusedLabels.begin(), unusedLabels.end(), data.label) != unusedLabels.end()) continue;
//		unusedLabels.push_back(data.label);
//	}
//	for (const std::string& label : unusedLabels)
//	{
//		ImGui::TextColored(kNoteColor, "%s : コードは使っていない(飾りとして描くだけ)", label.c_str());
//	}
//
//	ImGui::TreePop();
//}
//
//void UIEditor::DrawElementList()
//{
//	ImGui::TextUnformatted("要素(上ほど奥に描かれる)");
//	if (ImGui::BeginListBox("##elements", ImVec2(-FLT_MIN, 10.0f * ImGui::GetTextLineHeightWithSpacing())))
//	{
//		for (int32_t i = 0; i < static_cast<int32_t>(elementData_.size()); ++i)
//		{
//			ImGui::PushID(i);
//			if (ImGui::Selectable(MakeListName(i, elementData_[i]).c_str(), selectedElementIndex_ == i))
//			{
//				selectedElementIndex_ = i;
//				textureMessage_.clear();
//			}
//			ImGui::PopID();
//		}
//		ImGui::EndListBox();
//	}
//
//	// 追加(選んでいる要素のすぐ後ろ。選んでいなければ一番最後)
//	if (ImGui::Button("追加"))
//	{
//		ElementData data;
//		data.translateRate = Vector2{ 50.0f, 50.0f };
//		const int32_t insertIndex = HasSelection() ? selectedElementIndex_ + 1 : static_cast<int32_t>(elementData_.size());
//		elementData_.insert(elementData_.begin() + insertIndex, data);
//		selectedElementIndex_ = insertIndex;
//		isDirty_ = true;
//	}
//
//	ImGui::BeginDisabled(!HasSelection());
//
//	// 複製(labelIndex は同じlabelの一番大きい番号の次にする)
//	ImGui::SameLine();
//	if (ImGui::Button("複製"))
//	{
//		ElementData copy = elementData_[selectedElementIndex_];
//		if (!copy.label.empty())
//		{
//			int32_t maxIndex = -1;
//			for (const ElementData& data : elementData_)
//			{
//				if (data.label == copy.label) maxIndex = std::max(maxIndex, data.labelIndex);
//			}
//			copy.labelIndex = maxIndex + 1;
//		}
//		elementData_.insert(elementData_.begin() + selectedElementIndex_ + 1, copy);
//		++selectedElementIndex_;
//		isDirty_ = true;
//	}
//
//	// 削除
//	ImGui::SameLine();
//	if (ImGui::Button("削除"))
//	{
//		elementData_.erase(elementData_.begin() + selectedElementIndex_);
//		if (selectedElementIndex_ >= static_cast<int32_t>(elementData_.size()))
//		{
//			selectedElementIndex_ = static_cast<int32_t>(elementData_.size()) - 1;
//		}
//		isDirty_ = true;
//	}
//
//	// 並べ替え(描く順番)
//	ImGui::SameLine();
//	if (ImGui::Button("奥へ") && selectedElementIndex_ > 0)
//	{
//		std::swap(elementData_[selectedElementIndex_], elementData_[selectedElementIndex_ - 1]);
//		--selectedElementIndex_;
//		isDirty_ = true;
//	}
//	ImGui::SameLine();
//	if (ImGui::Button("手前へ") && selectedElementIndex_ + 1 < static_cast<int32_t>(elementData_.size()))
//	{
//		std::swap(elementData_[selectedElementIndex_], elementData_[selectedElementIndex_ + 1]);
//		++selectedElementIndex_;
//		isDirty_ = true;
//	}
//
//	ImGui::EndDisabled();
//}
//
//void UIEditor::DrawSelectedElement()
//{
//	if (!HasSelection())
//	{
//		ImGui::TextColored(kNoteColor, "一覧かプレビューで要素を選んでください");
//		return;
//	}
//
//	ElementData& data = elementData_[selectedElementIndex_];
//	bool changed = false;
//
//	// コードが探す名前と番号(飾りなら label は空でよい)
//	changed |= ImGui::InputText("label", &data.label);
//	if (ImGui::InputInt("labelIndex", &data.labelIndex))
//	{
//		data.labelIndex = std::max(data.labelIndex, 0);
//		changed = true;
//	}
//
//	// テクスチャ(テクスチャプレビューからドラッグ&ドロップ)
//	ImGui::Text("テクスチャ: %s", data.texturePath.empty() ? "なし(目印)" : data.texturePath.c_str());
//	ImGui::Button("ここにテクスチャをドロップ");
//	if (ImGui::BeginDragDropTarget())
//	{
//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
//		{
//			IM_ASSERT(payload->DataSize == sizeof(int32_t));
//			const int32_t textureID = *reinterpret_cast<const int32_t*>(payload->Data);
//			if (SetTexture(data, textureID))
//			{
//				textureMessage_.clear();
//				changed = true;
//			}
//			else
//			{
//				textureMessage_ = "ファイルから読んだテクスチャだけ使えます(レンダーテクスチャは保存できない)";
//			}
//		}
//		ImGui::EndDragDropTarget();
//	}
//	ImGui::SameLine();
//	if (ImGui::Button("テクスチャを外す(目印にする)"))
//	{
//		data.texturePath.clear();
//		data.textureID = -1;
//		data.textureSize = Vector2{ 0.0f, 0.0f };
//		changed = true;
//	}
//	if (!textureMessage_.empty())
//	{
//		ImGui::TextColored(kErrorColor, "%s", textureMessage_.c_str());
//	}
//
//	// 位置・大きさ・回転
//	changed |= ImGui::DragFloat2("位置(画面の%)", &data.translateRate.x, 0.05f);
//	changed |= ImGui::DragFloat2("pivot(0〜1)", &data.pivot.x, 0.01f, 0.0f, 1.0f);
//	changed |= ImGui::DragFloat2("scale(1で等倍)", &data.scale.x, 0.01f);
//	// 回転は度で見せて、ラジアンで持つ
//	Vector3 degrees = Vector3(data.rotate.x / kDegToRad, data.rotate.y / kDegToRad, data.rotate.z / kDegToRad);
//	if (ImGui::DragFloat3("回転(度)", &degrees.x, 0.5f))
//	{
//		data.rotate = Vector3(degrees.x * kDegToRad, degrees.y * kDegToRad, degrees.z * kDegToRad);
//		changed = true;
//	}
//
//	if (changed) isDirty_ = true;
//}
//
//void UIEditor::DrawGridTool()
//{
//	if (!ImGui::TreeNode("グリッドで並べる")) return;
//
//	ImGui::TextWrapped("選んでいる要素を1マス目(左上)にして、右・下に並べて追加します。"
//		"labelIndex は1マス目の番号から、左から右へ、行が終わったら次の行の順に増えます。");
//	if (ImGui::InputInt2("個数(横・縦)", &gridCount_.x))
//	{
//		gridCount_.x = std::max(gridCount_.x, 1);
//		gridCount_.y = std::max(gridCount_.y, 1);
//	}
//	ImGui::DragFloat2("間隔(基準解像度のpx)", &gridPitch_.x, 0.5f);
//
//	ImGui::BeginDisabled(!HasSelection());
//	if (ImGui::Button("並べる"))
//	{
//		const ElementData base = elementData_[selectedElementIndex_];
//		std::vector<ElementData> cells;
//		for (int32_t row = 0; row < gridCount_.y; ++row)
//		{
//			for (int32_t column = 0; column < gridCount_.x; ++column)
//			{
//				// 1マス目は見本そのもの
//				if (row == 0 && column == 0) continue;
//
//				ElementData cell = base;
//				cell.labelIndex = base.labelIndex + row * gridCount_.x + column;
//				cell.translateRate.x += gridPitch_.x * static_cast<float>(column) / kCanvasWidth * 100.0f;
//				cell.translateRate.y += gridPitch_.y * static_cast<float>(row) / kCanvasHeight * 100.0f;
//				cells.push_back(cell);
//			}
//		}
//		elementData_.insert(elementData_.begin() + selectedElementIndex_ + 1, cells.begin(), cells.end());
//		isDirty_ = true;
//	}
//	ImGui::EndDisabled();
//
//	ImGui::TreePop();
//}
//
//void UIEditor::DrawPreview()
//{
//	ImGui::Checkbox("同じlabelをまとめて動かす", &moveSameLabel_);
//	ImGui::TextColored(kNoteColor, "クリックで選ぶ / ドラッグで動かす(枠: 要素、十字: 目印)");
//
//	// ウィンドウの幅に合わせて、基準解像度の比で表示する
//	const float width = std::max(ImGui::GetContentRegionAvail().x, 160.0f);
//	const ImVec2 size = ImVec2(width, width * kCanvasHeight / kCanvasWidth);
//	const ImVec2 imageMin = ImGui::GetCursorScreenPos();
//	ImGui::Image(ImTextureID(Game::Asset::RenderTexture::GetRenderTextureGPUPtr(rt_ID)), size);
//
//	// 画像の上に透明なボタンを重ねて、クリックとドラッグを受ける(ウィンドウごと動かないように)
//	ImGui::SetCursorScreenPos(imageMin);
//	ImGui::InvisibleButton("##previewInput", size);
//
//	// 表示のpx ↔ 基準解像度のpx
//	const float toCanvas = kCanvasWidth / width;
//	const float toScreen = width / kCanvasWidth;
//
//	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
//	{
//		const ImVec2 mouse = ImGui::GetMousePos();
//		selectedElementIndex_ = PickElement(Vector2{ (mouse.x - imageMin.x) * toCanvas, (mouse.y - imageMin.y) * toCanvas });
//		textureMessage_.clear();
//	}
//	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))
//	{
//		const ImVec2 delta = ImGui::GetIO().MouseDelta;
//		if (delta.x != 0.0f || delta.y != 0.0f)
//		{
//			// 表示のpx → 基準解像度のpx → 画面の%
//			MoveSelected(Vector2{ delta.x * toCanvas / kCanvasWidth * 100.0f, delta.y * toCanvas / kCanvasHeight * 100.0f });
//		}
//	}
//
//	// 枠と目印を、画像の上に重ねて描く
//	ImDrawList* drawList = ImGui::GetWindowDrawList();
//	for (int32_t i = 0; i < static_cast<int32_t>(elementData_.size()); ++i)
//	{
//		const ElementData& data = elementData_[i];
//		const bool selected = (i == selectedElementIndex_);
//		const ImU32 color = selected ? IM_COL32(255, 220, 0, 255) : IM_COL32(255, 255, 255, 70);
//		const float thickness = selected ? 2.0f : 1.0f;
//
//		const Vector2 center = IUIElement::GetCenter(data);
//		const ImVec2 c = ImVec2(imageMin.x + center.x * toScreen, imageMin.y + center.y * toScreen);
//
//		if (data.textureID < 0)
//		{
//			// 目印は十字
//			const float r = 6.0f;
//			drawList->AddLine(ImVec2(c.x - r, c.y), ImVec2(c.x + r, c.y), color, thickness);
//			drawList->AddLine(ImVec2(c.x, c.y - r), ImVec2(c.x, c.y + r), color, thickness);
//			continue;
//		}
//
//		const Vector2 drawSize = IUIElement::GetDrawSize(data);
//		const ImVec2 half = ImVec2(std::abs(drawSize.x) * 0.5f * toScreen, std::abs(drawSize.y) * 0.5f * toScreen);
//		drawList->AddRect(ImVec2(c.x - half.x, c.y - half.y), ImVec2(c.x + half.x, c.y + half.y), color, 0.0f, 0, thickness);
//	}
//}
//
//bool UIEditor::HasSelection() const
//{
//	return selectedElementIndex_ >= 0 && selectedElementIndex_ < static_cast<int32_t>(elementData_.size());
//}
//
//bool UIEditor::SetTexture(ElementData& data, int32_t textureID)
//{
//	// テクスチャ置き場に無い番号(レンダーテクスチャなど)はファイルのパスが無いので保存できない
//	const TextureData* texture = Game::Asset::Texture::GetData(textureID);
//	if (!texture || texture->filePath.empty()) return false;
//
//	data.texturePath = texture->filePath;
//	data.textureID = textureID;
//	data.textureSize = Vector2(float(texture->metadata.width), float(texture->metadata.height));
//	return true;
//}
//
//int32_t UIEditor::PickElement(const Vector2& canvasPos) const
//{
//	// 手前(後ろの番号)から探す
//	for (int32_t i = static_cast<int32_t>(elementData_.size()) - 1; i >= 0; --i)
//	{
//		const ElementData& data = elementData_[i];
//
//		// 目印は大きさが無いので、中心の近くをクリックしたら選ぶ
//		if (data.textureID < 0)
//		{
//			const Vector2 center = IUIElement::GetCenter(data);
//			if (std::abs(canvasPos.x - center.x) <= kMarkerPickRadius && std::abs(canvasPos.y - center.y) <= kMarkerPickRadius)
//			{
//				return i;
//			}
//			continue;
//		}
//
//		if (IUIElement::IsMouseOver(data, canvasPos)) return i;
//	}
//	return -1;
//}
//
//void UIEditor::MoveSelected(const Vector2& deltaRate)
//{
//	if (!HasSelection()) return;
//
//	const std::string label = elementData_[selectedElementIndex_].label;
//	for (int32_t i = 0; i < static_cast<int32_t>(elementData_.size()); ++i)
//	{
//		const bool isTarget = (i == selectedElementIndex_) || (moveSameLabel_ && !label.empty() && elementData_[i].label == label);
//		if (!isTarget) continue;
//
//		elementData_[i].translateRate.x += deltaRate.x;
//		elementData_[i].translateRate.y += deltaRate.y;
//	}
//	isDirty_ = true;
//}
//
//std::vector<std::string_view> UIEditor::GetPartNames(UIElementType type)
//{
//	switch (type)
//	{
//	case UIElementType::Inventory:			return ToNames<Inventory::Part>();
//	case UIElementType::Hotbar:				return ToNames<Hotbar::Part>();
//	case UIElementType::MiningMode:			return ToNames<MiningMode::Part>();
//	case UIElementType::MiningPointGauge:	return ToNames<MiningPointGaugeUI::Part>();
//		// Craft・Pause はまだパーツを使っていない
//	default:								return {};
//	}
//}