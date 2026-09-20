#include "AnimationComputer.h"
#include <AssetManager/Animation/AnimationBank/AnimationBank.h>
#include <Utilities/Logger/Logger.h>

namespace
{
	Vector3 CalculateValue(float time, const AnimationCurve<Vector3>& curve)
	{
		assert(!curve.keyFrames.empty());

		if (curve.keyFrames.size() == 1 || time <= curve.keyFrames[0].time)
			return curve.keyFrames[0].value;

		for (size_t i = 0; i < curve.keyFrames.size() - 1; ++i)
		{
			const auto& k0 = curve.keyFrames[i];
			const auto& k1 = curve.keyFrames[i + 1];

			if (k0.time <= time && time <= k1.time)
			{
				float t = (time - k0.time) / (k1.time - k0.time);
				return k0.value * (1.0f - t) + k1.value * t;
			}
		}

		return curve.keyFrames.back().value;
	}

	Quaternion CalculateValue(float time, const AnimationCurve<Quaternion>& curve)
	{
		assert(!curve.keyFrames.empty());

		if (curve.keyFrames.size() == 1 || time <= curve.keyFrames[0].time)
			return curve.keyFrames[0].value;

		for (size_t i = 0; i < curve.keyFrames.size() - 1; ++i)
		{
			const auto& k0 = curve.keyFrames[i];
			const auto& k1 = curve.keyFrames[i + 1];

			if (k0.time <= time && time <= k1.time)
			{
				float t = (time - k0.time) / (k1.time - k0.time);
				return Quaternion::Slerp(k0.value, k1.value, t);
			}
		}

		return curve.keyFrames.back().value;
	}
}


AnimationComputer::AnimationComputer(AnimationBank* bank)
	: bank_(bank)
{}

AnimationComputer::~AnimationComputer()
{}


Matrix4x4 AnimationComputer::SampleNodeHierarchy(int32_t animationID, const std::string& nodeName, float& time)
{
	AnimationData* animationData = bank_->GetAnimationData(animationID);
	time = fmod(time, animationData->duration);

	Matrix4x4 result = Matrix4x4::MakeIdentity4x4();
	std::string currentName = nodeName;

	while (!currentName.empty())
	{
		QuaternionTransforms local{};

		auto animIt = animationData->nodeAnimations.find(currentName);
		if (animIt != animationData->nodeAnimations.end())
		{
			local.translate = CalculateValue(time, animIt->second.translate);
			local.rotate = CalculateValue(time, animIt->second.rotate);
			local.scale = CalculateValue(time, animIt->second.scale);
		}

		auto hierIt = animationData->hierarchy.find(currentName);
		if (hierIt == animationData->hierarchy.end())
		{
			break;
		}

		if (animIt == animationData->nodeAnimations.end())
		{
			local = hierIt->second.restTransform;
		}

		result = result * Matrix4x4::MakeAffineMatrix(local.scale, local.rotate, local.translate);

		currentName = hierIt->second.hasParent ? hierIt->second.parentName : "";
	}

	return result;
}


void AnimationComputer::ComputeAnimationData(int32_t animationID, SkinInstance& skin, const SkinBindData& bind, float& time)
{
	if (skin.boundAnimationID != animationID)
	{
		AnimationData* animationData = bank_->GetAnimationData(animationID);
		BindChannels(skin, *animationData, animationID);
		skin.boundDuration = animationData->duration;
	}

	time = fmod(time, skin.boundDuration);
	ApplyAnimation(skin, time);
	UpdateSkeleton(skin.skeleton);
	UpdatePalette(skin.skeleton, bind, skin.palette);
}

void AnimationComputer::BindChannels(SkinInstance& skin, const AnimationData& animation, int32_t animationID)
{
	skin.boundChannels.resize(skin.skeleton.joints.size());

	for (size_t i = 0; i < skin.skeleton.joints.size(); ++i)
	{
		auto it = animation.nodeAnimations.find(skin.skeleton.joints[i].name);
		skin.boundChannels[i] = (it != animation.nodeAnimations.end()) ? &it->second : nullptr;
	}

	skin.boundAnimationID = animationID;
}

void AnimationComputer::UpdatePalette(const Skeleton& skeleton, const SkinBindData& bind, std::vector<WellForGPU>& palette)
{
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex)
	{
		assert(jointIndex < bind.inverseBindPoseMatrices.size());

		const Matrix4x4 skeletonSpace = bind.inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;

		palette[jointIndex].skeletonSpaceMatrix = skeletonSpace;
		palette[jointIndex].skeletonSpaceInverseTransposeMatrix = skeletonSpace.Inverse().Transpose();
	}
}

void AnimationComputer::ApplyAnimation(SkinInstance& skin, float time)
{
	for (size_t i = 0; i < skin.skeleton.joints.size(); ++i)
	{
		Joint& joint = skin.skeleton.joints[i];
		if (const NodeAnimation* na = skin.boundChannels[i])
		{
			joint.transform.translate = CalculateValue(time, na->translate);
			joint.transform.rotate = CalculateValue(time, na->rotate);
			joint.transform.scale = CalculateValue(time, na->scale);
		}
	}
}

void AnimationComputer::UpdateSkeleton(Skeleton& skeleton)
{
	for (Joint& joint : skeleton.joints)
	{
		joint.localMatrix = Matrix4x4::MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
		if (joint.parentIndex.has_value())
		{
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[joint.parentIndex.value()].skeletonSpaceMatrix;
		}
		else
		{
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}
