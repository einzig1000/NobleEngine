#include <GameObjects/Map/Chunk/Block/Block.h>
#include <definition/constexprs.h>
#include <algorithm>

Block::Block()
{
}

Block::~Block()
{
}

void Block::Initialize()
{
}

void Block::SetBlockType(BlockInfo info)
{
	blockInfo_ = info;
}

void Block::SetBlockPosition(const Vector3& position)
{
	position_ = position;
	aabb_.min = position - Vector3(Constexprs::kBlockSize / 2.0f, Constexprs::kBlockSize / 2.0f, Constexprs::kBlockSize / 2.0f);
	aabb_.max = position + Vector3(Constexprs::kBlockSize / 2.0f, Constexprs::kBlockSize / 2.0f, Constexprs::kBlockSize / 2.0f);
}

void Block::Update()
{
	// 表面に露出していなかったらreturn
	if (!IsExposed()) return;

	// 色更新
	//UpdateColor();
}

void Block::UpdateColor()
{
	lightEmission_ = std::clamp(lightEmission_, 0, 9);

	float emission = 1.0f * (float(lightEmission_) / 9.0f);

	// 輝度に応じて色を変更
	//color_ = Vector4(emission, emission, emission, 1.0f);
}

bool Block::IsExposed()
{
	return exposedFace_ != 0;
}

bool Block::IsExposed(AABBFace face) const
{
	switch (face)
	{
	case AABBFace::ZPlus:
		return (exposedFace_ & 0b100000) != 0;
		break;
	case AABBFace::ZMinus:
		return (exposedFace_ & 0b010000) != 0;
		break;
	case AABBFace::XPlus:
		return (exposedFace_ & 0b001000) != 0;
		break;
	case AABBFace::XMinus:
		return (exposedFace_ & 0b000100) != 0;
		break;
	case AABBFace::YPlus:
		return (exposedFace_ & 0b000001) != 0;
		break;
	case AABBFace::YMinus:
		return (exposedFace_ & 0b000010) != 0;
		break;
	default:
		break;
	}

	return false;
}

void Block::SetExposedFace(AABBFace face, bool isExposed)
{
	switch (face)
	{
	case AABBFace::ZPlus:
		if (isExposed)
		{
			exposedFace_ |= 0b100000; // 前の面を露出させる
		}
		else
		{
			exposedFace_ &= ~0b100000; // 前の面を非露出にする
		}
		break;
	case AABBFace::ZMinus:
		if (isExposed)
		{
			exposedFace_ |= 0b010000; // 後ろの面を露出させる
		}
		else
		{
			exposedFace_ &= ~0b010000; // 後ろの面を非露出にする
		}
		break;
	case AABBFace::XPlus:
		if (isExposed)
		{
			exposedFace_ |= 0b001000; // 左の面を露出させる
		}
		else
		{
			exposedFace_ &= ~0b001000; // 左の面を非露出にする
		}
		break;
	case AABBFace::XMinus:
		if (isExposed)
		{
			exposedFace_ |= 0b000100; // 右の面を露出させる
		}
		else
		{
			exposedFace_ &= ~0b000100; // 右の面を非露出にする
		}
		break;
	case AABBFace::YMinus:
		if (isExposed)
		{
			exposedFace_ |= 0b000010; // 下の面を露出させる
		}
		else
		{
			exposedFace_ &= ~0b000010; // 下の面を非露出にする
		}
		break;
	case AABBFace::YPlus:
		if (isExposed)
		{
			exposedFace_ |= 0b000001; // 上の面を露出させる
		}
		else
		{
			exposedFace_ &= ~0b000001; // 上の面を非露出にする
		}
		break;
	}
}
