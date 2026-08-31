#pragma once
#include <array>
#include <vector>

enum class EventType
{
	// プレイヤーHPが変動した
	PlayerHPChanged,
	// プレイヤーがダメージを受けた
	PlayerDamaged,
	// プレイヤーがアイテムを取得した
    ItemPickup,
	// 採掘モードが変更された
	MiningModeChanged,
	// 採掘中
	Mining,

	MAX
};

struct Event
{
	EventType type = EventType::MAX;
	std::vector<int32_t> value;
};

class EventBus
{
public:
    void Notify(const Event& ev) { events_[static_cast<size_t>(ev.type)].push_back(ev); }
	const std::vector<Event>& GetEvents(EventType type) const { return events_[static_cast<size_t>(type)]; }
	void Clear(EventType type) { events_[static_cast<size_t>(type)].clear(); }
	std::vector<Event> GetAllEvents() const;

private:

	std::array<std::vector<Event>, static_cast<size_t>(EventType::MAX)> events_;
};

