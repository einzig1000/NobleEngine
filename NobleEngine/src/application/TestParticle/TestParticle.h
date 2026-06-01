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
	int32_t t_uvChecker;
	int32_t t_gradationLine;

	std::vector<std::unique_ptr<RenderObject>> renderPlanes_;
	std::vector<Vector4> planeColors;
	std::vector<EulerTransform> planeTransforms;

	std::vector<std::unique_ptr<RenderObject>> renderRings_;
	std::vector<Vector4> ringColors;
	std::vector<EulerTransform> ringTransforms;

	std::unique_ptr<RenderObject> renderCylinder_;
	Vector4 cylinderColor;
	EulerTransform cylinderTransform;
};


