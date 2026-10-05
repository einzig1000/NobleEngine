#pragma once
#include <array>
#include <vector>
#include <any>
#include <cstdint>

enum class EventType
{
	// プレイヤーHPが変動した
	// data -> float(変動量)
	PlayerHPChanged,
	// プレイヤーがアイテムを取得した
	// data -> ItemPickupData構造体
	ItemPickup,
	// 採掘モードが変更された
	// data -> MiningPattern
	MiningModeChanged,
	// 採掘中
	Mining,
	// ゲームのセーブが要求された
	SaveGameRequested,
	// 全モブの移動可能フラグ
	AbleMoveAllCharacters,
	// ブロックが壊された
	BlockDestroyed,
	// 採掘オーブが届いた
	MiningOrbAbsorbed,


	// ツールのグレードアップを頼まれた
	ToolUpgradeRequested,


	// GoTargetCurvingを出してほしい
	ParticleRequest_GoTargetCurving,

	MAX
};

struct Event
{
	EventType type = EventType::MAX;
	std::any data;
};

class EventBus
{
public:
	void Update()
	{
		std::swap(readBuffer_, writeBuffer_);
		for (auto& bucket : writeBuffer_) bucket.clear();
	}

	void Notify(const Event& ev) { writeBuffer_[static_cast<size_t>(ev.type)].push_back(ev); }
	const std::vector<Event>& GetEvents(EventType type) const { return readBuffer_[static_cast<size_t>(type)]; }
	std::vector<Event> GetAllEvents() const;

private:
	std::array<std::vector<Event>, static_cast<size_t>(EventType::MAX)> readBuffer_;
	std::array<std::vector<Event>, static_cast<size_t>(EventType::MAX)> writeBuffer_;
};
