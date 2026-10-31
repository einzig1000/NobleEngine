#include "SkillTreeView.h"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <string>

namespace
{
	// ノードの枠
	const std::string kFrameTexturePath = "assets/application/texture/UI/Inventory/Slot.png";
	// 線
	const std::string kLineTexturePath = "assets/engine/texture/white1x1.png";

	// 見え方ごとの色(枠・アイコン・そのノードに入ってくる線)
	struct LookColor
	{
		Vector4 frame;
		Vector4 icon;
		Vector4 line;
	};
	LookColor GetLookColor(SkillTreeView::NodeLook look)
	{
		switch (look)
		{
		case SkillTreeView::NodeLook::Unlocked:
			return { { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
		case SkillTreeView::NodeLook::Affordable:
			return { { 1.0f, 0.85f, 0.35f, 1.0f }, { 0.45f, 0.45f, 0.45f, 1.0f }, { 0.6f, 0.6f, 0.6f, 1.0f } };
		case SkillTreeView::NodeLook::Available:
			return { { 0.6f, 0.6f, 0.6f, 1.0f }, { 0.45f, 0.45f, 0.45f, 1.0f }, { 0.6f, 0.6f, 0.6f, 1.0f } };
		case SkillTreeView::NodeLook::Silhouette:
		default:
			return { { 0.4f, 0.4f, 0.4f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.3f, 0.3f, 0.3f, 1.0f } };
		}
	}

	// カーソルが乗っているノードの大きさ
	constexpr float kHoverScale = 1.1f;
}

SkillTreeView::SkillTreeView()
{
	static int32_t instanceCount = 0;
	instanceNumber_ = instanceCount++;

	frameTextureID_ = Game::Asset::Texture::Load(kFrameTexturePath);
	lineTextureID_ = Game::Asset::Texture::Load(kLineTexturePath);
}

SkillTreeView::~SkillTreeView()
{}

void SkillTreeView::SetViewSize(uint32_t width, uint32_t height)
{
	width = std::max(width, 1u);
	height = std::max(height, 1u);
	if (rt_ID_ >= 0 && static_cast<uint32_t>(viewSize_.x) == width && static_cast<uint32_t>(viewSize_.y) == height) return;

	// 同じ名前だと前のレンダーテクスチャが返ってくるので、大きさを名前に入れる(前のものは残る)
	const std::string label = "SkillTreeView" + std::to_string(instanceNumber_) + "_" + std::to_string(width) + "x" + std::to_string(height);
	rt_ID_ = Game::Asset::RenderTexture::CreateRenderTexture(width, height, label);
	viewSize_ = Vector2{ static_cast<float>(width), static_cast<float>(height) };
}

void SkillTreeView::Update(const std::vector<SkillNodeData>& nodes, const std::vector<NodeLook>& looks, int32_t hoverIndex)
{
	// 中心とズームから正射影を作る(この範囲の外は描かれない)
	const float halfWidth = viewSize_.x * 0.5f / zoom_;
	const float halfHeight = viewSize_.y * 0.5f / zoom_;
	projection_ = Matrix4x4::MakeOrthographicMatrix(center_.x - halfWidth, center_.y - halfHeight, center_.x + halfWidth, center_.y + halfHeight, -300.0f, 300.0f);

	auto getLook = [&](size_t index)
		{
			return index < looks.size() ? looks[index] : NodeLook::Unlocked;
		};

	// 線(前提のノード → このノード)
	lineCount_ = 0;
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		for (int32_t parentID : nodes[i].parents)
		{
			// 前提のノードを探す(数十個なので毎回探す)
			const auto parent = std::find_if(nodes.begin(), nodes.end(), [&](const SkillNodeData& node) { return node.id == parentID; });
			if (parent == nodes.end()) continue;

			const Vector2 from = parent->position;
			const Vector2 to = nodes[i].position;
			const Vector2 diff = to - from;
			const float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);
			if (length <= 0.0f) continue;

			Reserve(lines_, lineCount_ + 1);
			SetQuad(lines_[lineCount_], (from + to) * 0.5f, Vector2{ length, kLineWidth }, std::atan2(diff.y, diff.x), GetLookColor(getLook(i)).line, lineTextureID_);
			++lineCount_;
		}
	}

	// ノード(枠とアイコン)
	frameCount_ = 0;
	iconCount_ = 0;
	Reserve(frames_, nodes.size());
	Reserve(icons_, nodes.size());
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		const LookColor color = GetLookColor(getLook(i));
		const float scale = (static_cast<int32_t>(i) == hoverIndex) ? kHoverScale : 1.0f;

		SetQuad(frames_[frameCount_], nodes[i].position, Vector2{ kNodeSize * scale, kNodeSize * scale }, 0.0f, color.frame, frameTextureID_);
		++frameCount_;

		if (nodes[i].iconTextureID < 0) continue;
		SetQuad(icons_[iconCount_], nodes[i].position, Vector2{ kIconSize * scale, kIconSize * scale }, 0.0f, color.icon, nodes[i].iconTextureID);
		++iconCount_;
	}
}

void SkillTreeView::Draw()
{
	if (rt_ID_ < 0) return;

	for (size_t i = 0; i < lineCount_; ++i) DrawQuad(lines_[i]);
	for (size_t i = 0; i < frameCount_; ++i) DrawQuad(frames_[i]);
	for (size_t i = 0; i < iconCount_; ++i) DrawQuad(icons_[i]);
}

Vector2 SkillTreeView::ViewToTree(const Vector2& viewPos) const
{
	return center_ + (viewPos - viewSize_ * 0.5f) / zoom_;
}

Vector2 SkillTreeView::TreeToView(const Vector2& treePos) const
{
	return (treePos - center_) * zoom_ + viewSize_ * 0.5f;
}

void SkillTreeView::ZoomAt(const Vector2& viewPos, float rate)
{
	// カーソルの下にあるツリー上の点が、ズームした後も同じ場所に来るように中心をずらす
	const Vector2 treePos = ViewToTree(viewPos);
	zoom_ = std::clamp(zoom_ * rate, kMinZoom, kMaxZoom);
	center_ = treePos - (viewPos - viewSize_ * 0.5f) / zoom_;
}

void SkillTreeView::Pan(const Vector2& viewDelta)
{
	// マウスを右に動かしたら、ツリーも右に動く(=見ている場所は左へ)
	center_ -= viewDelta / zoom_;
}

void SkillTreeView::ClampCenter(const std::vector<SkillNodeData>& nodes, float margin)
{
	if (nodes.empty()) return;

	Vector2 minPos = { FLT_MAX, FLT_MAX };
	Vector2 maxPos = { -FLT_MAX, -FLT_MAX };
	for (const SkillNodeData& node : nodes)
	{
		minPos.x = std::min(minPos.x, node.position.x);
		minPos.y = std::min(minPos.y, node.position.y);
		maxPos.x = std::max(maxPos.x, node.position.x);
		maxPos.y = std::max(maxPos.y, node.position.y);
	}
	center_.x = std::clamp(center_.x, minPos.x - margin, maxPos.x + margin);
	center_.y = std::clamp(center_.y, minPos.y - margin, maxPos.y + margin);
}

void SkillTreeView::ResetView(const std::vector<SkillNodeData>& nodes)
{
	zoom_ = 1.0f;
	center_ = Vector2{ 0.0f, 0.0f };
	if (nodes.empty()) return;

	Vector2 minPos = { FLT_MAX, FLT_MAX };
	Vector2 maxPos = { -FLT_MAX, -FLT_MAX };
	for (const SkillNodeData& node : nodes)
	{
		minPos.x = std::min(minPos.x, node.position.x);
		minPos.y = std::min(minPos.y, node.position.y);
		maxPos.x = std::max(maxPos.x, node.position.x);
		maxPos.y = std::max(maxPos.y, node.position.y);
	}
	center_ = (minPos + maxPos) * 0.5f;
}

int32_t SkillTreeView::PickNode(const std::vector<SkillNodeData>& nodes, const Vector2& treePos) const
{
	// 後ろ(手前に描かれる方)から探す
	const float half = kNodeSize * 0.5f;
	for (int32_t i = static_cast<int32_t>(nodes.size()) - 1; i >= 0; --i)
	{
		const Vector2& pos = nodes[i].position;
		if (std::abs(treePos.x - pos.x) <= half && std::abs(treePos.y - pos.y) <= half)
		{
			return i;
		}
	}
	return -1;
}

std::unique_ptr<RenderObject> SkillTreeView::CreateSpriteRender()
{
	auto render = std::make_unique<RenderObject>();
	render->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	render->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	render->psoConfig_.depthStencilID = DepthStencilID::Disable;
	render->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	render->SetupFromShaders();
	return render;
}

void SkillTreeView::Reserve(std::vector<Quad>& pool, size_t count)
{
	while (pool.size() < count)
	{
		Quad quad;
		quad.render = CreateSpriteRender();
		pool.push_back(std::move(quad));
	}
}

void SkillTreeView::SetQuad(Quad& quad, const Vector2& center, const Vector2& size, float rotateZ, const Vector4& color, int32_t textureID) const
{
	// 板ポリは-1〜1なので、半分の大きさを掛ける
	const Vector3 scale = Vector3{ size.x * 0.5f, size.y * 0.5f, 1.0f };
	quad.world = Matrix4x4::MakeAffineMatrix(scale, Vector3{ 0.0f, 0.0f, rotateZ }, Vector3{ center.x, center.y, 0.0f });
	quad.wvp = quad.world * projection_;
	quad.color = color;
	quad.textureID = textureID;
}

void SkillTreeView::DrawQuad(const Quad& quad) const
{
	if (quad.textureID < 0) return;

	quad.render->SetBRegisterData(0, ShaderType::VertexShader, &quad.wvp);
	quad.render->SetBRegisterData(1, ShaderType::VertexShader, &quad.world);
	quad.render->SetBRegisterData(0, ShaderType::PixelShader, &quad.color);
	quad.render->SetBRegisterData(1, ShaderType::PixelShader, &quad.textureID);
	quad.render->Draw(rt_ID_, { quad.textureID });
}