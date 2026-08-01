#pragma once
#include <DrawSystem/RenderData/RenderObject.h>
#include <ComputeSystem/ComputeObject/ComputeObject.h>
#include <definition/definition.h>
#include <memory>

class TestAnimation
{
public:
	TestAnimation();
	~TestAnimation();
	void Initialize();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTextureID);
	Vector3 GetVetexPos(int32_t index);

	int32_t GetVertexIndexByJointName(const std::string& jointName);
	Matrix4x4 GetJointMatrixByName(const std::string& jointName);

private:
	std::unique_ptr<RenderObject> render_;
	std::unique_ptr<ComputeObject> compute_;

	float animationTime_ = 0.0f;

	int32_t WellSRVID_ = -1;
	int32_t vertexSRVID_ = -1;
	std::vector<VertexData> vertices;
	int32_t vertexInflenceSRVID_ = -1;
	std::vector<VertexInfluence> vertexInfluences;

	int32_t resultSRVID_ = -1;

	int32_t animationID_ = -1;
	uint32_t texID_ = -1;
	SkinCluster skinCluster_;
	Skeleton skeleton_;
	ModelData* modelData_ = nullptr;
};

