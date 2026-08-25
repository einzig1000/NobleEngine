#pragma once
#include <DrawSystem/RenderData/RenderObject.h>
#include <definition/definition.h>
#include <memory>
#include <Game.h>

struct Particle
{
	Vector3 translate;
	Vector3  scale;
	float lifeTime;
	Vector3 velocity;
	float currentTime;
	Vector4 color;
};

class TestParticle
{
public:
	TestParticle();
	~TestParticle();
	void Initialize();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTextureID);

	void SetEmitterSpherePos(Vector3 pos);

private:
	int32_t t_circle2;
	int32_t t_uvChecker;
	int32_t t_gradationLine;

	std::vector<std::unique_ptr<RenderObject>> renderPlanes_;
	std::vector<Vector4> planeColors;
	std::vector<EulerTransforms> planeTransforms;

	std::vector<std::unique_ptr<RenderObject>> renderRings_;
	std::vector<Vector4> ringColors;
	std::vector<EulerTransforms> ringTransforms;

	std::unique_ptr<RenderObject> renderCylinder_;
	Vector4 cylinderColor;
	EulerTransforms cylinderTransform;

	std::unique_ptr<RenderObject> renderSphere_;
	Vector4 sphereColor;
	EulerTransforms sphereTransform;

	
	EmitterSphere emitterSphere;


	std::vector<Particle> particles;
	std::unique_ptr<RenderObject> particle_;
	std::unique_ptr<ComputeObject> emitCompute_;
	std::unique_ptr<ComputeObject> initializeCompute_;
	std::unique_ptr<ComputeObject> updateCompute_;
	int32_t particleSRVID_ = -1;
	int32_t freeListIndexSRVID_ = -1;
	int32_t freeListSRVID_ = -1;



};


