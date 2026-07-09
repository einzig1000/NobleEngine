#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include "AnimationBank/AnimationBank.h"
#include "AnimationLoader/AnimationLoader.h"
#include "AnimationComputer/AnimationComputer.h"
#include <memory>

class DirectXManager;

class AnimationManager
{
public:
	AnimationManager(DirectXManager* dxManager);
	~AnimationManager();
	
	AnimationBank* GetAnimationBank() const { return bank_.get(); }
	AnimationLoader* GetAnimationLoader() const { return loader_.get(); }
	AnimationComputer* GetAnimationComputer() const { return computer_.get(); }

private:
	std::unique_ptr<AnimationBank> bank_;
	std::unique_ptr<AnimationLoader> loader_;
	std::unique_ptr<AnimationComputer> computer_;

};

