#include "ParticleManager.h"

ParticleManager::ParticleManager()
{
	goTargetCurving_ = std::make_unique<GoTargetCurving>();
}

void ParticleManager::SetEventBus(EventBus* eventBus)
{
	eventBus_ = eventBus;
	goTargetCurving_->SetEventBus(eventBus);
}

void ParticleManager::Update(int32_t c_ID)
{
	if (eventBus_)
	{
		// 操作モード変更イベント
		const std::vector<Event>& controlModeEvents = eventBus_->GetEvents(EventType::AbleMoveAllCharacters);
		if (!controlModeEvents.empty())
		{
			ableMoveAll_ = controlModeEvents[0].value[0];
		}

		// GoTargetCurvingの発生依頼
		for (const Event& event : eventBus_->GetEvents(EventType::ParticleRequest_GoTargetCurving))
		{
			if (const auto* request = std::any_cast<GoTargetCurving::Params>(&event.data))
			{
				goTargetCurving_->Emit(*request);
			}
		}
	}

	goTargetCurving_->Update(c_ID);
}

void ParticleManager::Draw(int32_t rt_ID)
{
	goTargetCurving_->Draw(rt_ID);
}
