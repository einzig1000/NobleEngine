#include "AnimationManager.h"
#include <Utilities/Logger/Logger.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

AnimationManager::AnimationManager(DirectXManager* dxManager)
{
	bank_ = std::make_unique<AnimationBank>();
	loader_ = std::make_unique<AnimationLoader>(dxManager, bank_.get());
	computer_ = std::make_unique<AnimationComputer>(bank_.get());
}

AnimationManager::~AnimationManager()
{}
