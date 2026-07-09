#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AnimationBank;
class DirectXManager;

class AnimationLoader
{
public:
	AnimationLoader(DirectXManager* dxManager, AnimationBank* bank);
	~AnimationLoader();

	// アニメーション読み込み
	int32_t LoadAnimation(const std::string& filePath, const std::string& animationName);

private:
	DirectXManager* dxManager_;
	AnimationBank* bank_;

	// アニメーションファイル読み込み
	void LoadAnimationFile(const std::string& filePath, const std::string& animationName, AnimationData* animationData);

};

