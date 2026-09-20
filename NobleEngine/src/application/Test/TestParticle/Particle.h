#pragma once
#include <Game.h>

struct ParticleData
{
	Vector3 translate;
	Vector3  scale;
	float lifeTime;
	Vector3 velocity;
	float currentTime;
	Vector4 color;
};

class Particle
{
public:
	Particle();
	~Particle();
	void Initialize();
	void Update(int32_t cameraID);
	void Draw();

	EmitterSphere emitterSphere;
	std::unique_ptr<RenderObject> render_;
	std::unique_ptr<ComputeObject> emitCompute_;
	std::unique_ptr<ComputeObject> initializeCompute_;
	std::unique_ptr<ComputeObject> updateCompute_;
	int32_t particleSRVID_ = -1;
	int32_t freeListIndexSRVID_ = -1;
	int32_t freeListSRVID_ = -1;
};

