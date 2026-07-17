#include "AnimationLoader.h"
#include <AssetManager/Animation/AnimationBank/AnimationBank.h>
#include <Utilities/Logger/Logger.h>

AnimationLoader::AnimationLoader(DirectXManager* dxManager, AnimationBank* bank)
	: dxManager_(dxManager), bank_(bank)
{}

AnimationLoader::~AnimationLoader()
{}

int32_t AnimationLoader::LoadAnimation(const std::string & filePath, const std::string & animationName)
{
	// 既に読み込まれていたらそのIDを返す
	int32_t animationID = bank_->IsAnimationDataExist(filePath + ":" + animationName);
	if (animationID != -1)
	{
		return animationID;
	}

	Log("アニメーション読み込み開始:%s / %s", filePath.c_str(), animationName.c_str());

	std::unique_ptr<AnimationData> animationData = std::make_unique<AnimationData>();

	// アニメーションファイル読み込み
	LoadAnimationFile(filePath, animationName, animationData.get());

	// アニメーションデータをバンクに追加
	animationID = bank_->AddAnimationData(filePath + ":" + animationName, std::move(animationData));

	Log("成功 ID:%d", animationID);

	return animationID;
}

void AnimationLoader::LoadAnimationFile(const std::string& filePath, const std::string& animationName, AnimationData* animationData)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.c_str(),
		aiProcess_Triangulate               // 面を三角形に分割する
		| aiProcess_ConvertToLeftHanded     // 左手座標系に変換する(逆に言うとこのエンジンで使用するモデルは右手座標系で作成する必要がある)
		| aiProcess_GenSmoothNormals        // 法線データが存在しないときに自動生成する
		| aiProcess_JoinIdenticalVertices   // 重複頂点を結合する
	);
	assert(scene->HasAnimations());

	aiAnimation* animationAssimp = scene->mAnimations[0];
	if (animationAssimp->mName.C_Str() != "")	//名前がなかった場合先頭を利用する
	{
		for (uint32_t animationIndex = 1; animationIndex < scene->mNumAnimations; ++animationIndex)
		{
			aiAnimation* current = scene->mAnimations[animationIndex];

			if (animationName == current->mName.C_Str())
			{
				animationAssimp = current;
				break;
			}
		}
	}

	assert(animationAssimp);

	animationData->duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex)
	{
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animationData->nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex)
		{
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			Keyframe<Vector3> keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
			nodeAnimation.translate.keyFrames.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex)
		{
			const aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			Keyframe<Quaternion> keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z, keyAssimp.mValue.w };
			nodeAnimation.rotate.keyFrames.push_back(keyframe);
		}
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex)
		{
			const aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			Keyframe<Vector3> keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
			nodeAnimation.scale.keyFrames.push_back(keyframe);
		}
	}
}
