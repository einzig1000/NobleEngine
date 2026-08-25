#include "EventBus.h"

std::vector<Event> EventBus::GetAllEvents() const
{
    std::vector<Event> all;
    for (auto& bucket : events_)
        all.insert(all.end(), bucket.begin(), bucket.end());
    return all;
}
