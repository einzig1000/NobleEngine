#include <GameObjects/Map/Chunk/Block/Block.h>
#include <definition/constexprs.h>
#include <algorithm>


void Block::SetBlockID(BlockID info) { blockID_ = info; }

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
