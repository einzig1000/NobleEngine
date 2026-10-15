#include "UIEditor.h"
#include <App.h>

UIEditor::UIEditor()
{
	rt_ID = Game::Asset::RenderTexture::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "UIEditorTexture");

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
	ImGui::Begin("Item Editor");


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
						//elementData_ = *(App::Data::UI::Get(elementType_));
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
			ImGui::DragFloat2("translate", &elementData_[selectedElementIndex_].transforms.translate.x, 0.1f);
			ImGui::DragFloat2("scale", &elementData_[selectedElementIndex_].transforms.scale.x, 0.1f);
			ImGui::DragFloat2("rotate", &elementData_[selectedElementIndex_].transforms.rotate.x, 0.1f);


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
