#pragma once
#include <DrawSystem/RenderData/RenderObject.h>
#include <definition/definition.h>
#include <memory>

class TestParticle
{
public:
	TestParticle();
	~TestParticle();
	void Initialize();
	void Update();
	void Draw();

private:
	std::vector<std::unique_ptr<RenderObject>> renderObjects_;
	std::vector<Vector4> color;
	int32_t t_uvChecker;
	std::vector<Transforms> transforms;
};


