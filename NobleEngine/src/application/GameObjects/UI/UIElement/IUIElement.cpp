#include <GameObjects/UI/UIElement/IUIElement.h>
#include <GameObjects/UI/ItemIcon/ItemIconManager.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <App.h>
#include <cmath>

namespace
{
	// DotGothic16で行の上端から数字の縦の中心までの割合
	// (ascent 1160 - 数字の中心の高さ (787 - 28) / 2) / (ascent - descent = 1448)
	constexpr float kDigitCenterRatio = (1160.0f - (787.0f - 28.0f) * 0.5f) / 1448.0f;
}

// sprites_にJsonのデータを入れてrenderObjectを生成する
void IUIElement::BuildSprites(UIElementType type)
{
	sprites_.clear();

	const std::vector<ElementData>* dataList = App::Data::UI::Get(type);
	if (!dataList)
	{
		Log("UIデータがありません: %s", std::string(magic_enum::enum_name(type)).c_str());
		return;
	}

	sprites_.reserve(dataList->size());
	for (const ElementData& data : *dataList)
	{
		ElementInstanceData sprite;
		sprite.render = std::make_unique<RenderObject>();
		sprite.render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
		sprite.render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
		sprite.render->psoConfig_.depthStencilID = DepthStencilID::Disable;
		sprite.render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
		sprite.render->SetupFromShaders();
		sprite.data = data;

		sprites_.push_back(std::move(sprite));
	}
}

ElementInstanceData* IUIElement::GetSprite(const std::vector<int32_t>& indices, int32_t labelIndex)
{
	if (labelIndex < 0 || labelIndex >= static_cast<int32_t>(indices.size())) return nullptr;

	const int32_t spriteIndex = indices[labelIndex];
	if (spriteIndex < 0 || spriteIndex >= static_cast<int32_t>(sprites_.size())) return nullptr;
	return &sprites_[spriteIndex];
}

// 静的スプライトの全更新
void IUIElement::UpdateStaticRenderObject(const Matrix4x4& orthographic)
{
	for (ElementInstanceData& sprite : sprites_)
	{
		UpdateQuad(sprite, GetCenter(sprite.data), GetDrawSize(sprite.data), sprite.data.rotate, orthographic);
	}
}
// 静的スプライトの全描画
void IUIElement::DrawStaticRenderObject(int32_t rt_ID)
{
	for (const ElementInstanceData& sprite : sprites_)
	{
		DrawRenderObject(rt_ID, sprite);
	}
}
// スプライトの描画
void IUIElement::DrawRenderObject(int32_t rt_ID, const ElementInstanceData& sprite)
{
	if (sprite.data.textureID < 0) return;

	sprite.render->SetBRegisterData(0, ShaderType::VertexShader, &sprite.wvp);
	sprite.render->SetBRegisterData(1, ShaderType::VertexShader, &sprite.world);
	sprite.render->SetBRegisterData(0, ShaderType::PixelShader, &sprite.color);
	sprite.render->SetBRegisterData(1, ShaderType::PixelShader, &sprite.data.textureID);
	sprite.render->Draw(rt_ID, { sprite.data.textureID });
}

void IUIElement::UpdateQuad(ElementInstanceData& sprite, const Vector2& center, const Vector2& size, const Vector3& rotate, const Matrix4x4& orthographic)
{
	// 板ポリは-1〜1なので、半分の大きさを掛ける
	const Vector3 scale = Vector3(size.x * 0.5f, size.y * 0.5f, 1.0f);
	const Vector3 translate = Vector3(center.x, center.y, 0.0f);

	sprite.world = Matrix4x4::MakeAffineMatrix(scale, rotate, translate);
	sprite.wvp = sprite.world * orthographic;
}

Vector2 IUIElement::GetDrawSize(const ElementData& data)
{
	return Vector2{ data.textureSize.x * data.scale.x, data.textureSize.y * data.scale.y };
}

Vector2 IUIElement::GetCenter(const ElementData& data)
{
	// translateRateの位置(px)
	const float anchorX = static_cast<float>(Constexprs::kInitialWindowWidth) * data.translateRate.x * 0.01f;
	const float anchorY = static_cast<float>(Constexprs::kInitialWindowHeight) * data.translateRate.y * 0.01f;

	// pivotの分だけ中心をずらす(pivotが0.5なら、translateRateがそのまま中心)
	const Vector2 size = GetDrawSize(data);
	return Vector2{ anchorX + (0.5f - data.pivot.x) * size.x, anchorY + (0.5f - data.pivot.y) * size.y };
}

Vector2 IUIElement::GetMouseUIPosition()
{
	// Get2DPositionはウィンドウのピクセル座標なので、ウィンドウの大きさで割ってから基準解像度を掛ける
	const Vector2 mouse = Game::IO::Mouse::Get2DPosition();
	const float windowWidth = static_cast<float>(Game::Window::GetWidth());
	const float windowHeight = static_cast<float>(Game::Window::GetHeight());
	if (windowWidth <= 0.0f || windowHeight <= 0.0f) return mouse;

	return Vector2{
		mouse.x * static_cast<float>(Constexprs::kInitialWindowWidth) / windowWidth,
		mouse.y * static_cast<float>(Constexprs::kInitialWindowHeight) / windowHeight };
}

bool IUIElement::IsMouseOver(const ElementData& data, const Vector2& mouseUI)
{
	const Vector2 center = GetCenter(data);
	const Vector2 size = GetDrawSize(data);
	return std::abs(mouseUI.x - center.x) <= std::abs(size.x) * 0.5f
		&& std::abs(mouseUI.y - center.y) <= std::abs(size.y) * 0.5f;
}

Vector2 IUIElement::GetDigitTextPosition(const std::string& text, int32_t charSize, const Vector2& center)
{
	const Vector2 textSize = Game::Asset::Font::MeasureJustTextureSize(text, charSize, Vector2{ 0.0f, 0.0f }, 0.0f);
	return Vector2{ center.x - textSize.x * 0.5f, center.y - static_cast<float>(charSize) * kDigitCenterRatio };
}

void IUIElement::DrawOutlinedString(int32_t rt_ID, const std::string& text, int32_t charSize, const Vector2& pos, const Vector4& color, float outlineWidth)
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