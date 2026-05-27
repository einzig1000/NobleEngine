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
	std::vector<std::unique_ptr<RenderObject>> renderPlanes_;
	std::vector<Vector4> planeColors;
	std::vector<Transforms> planeTransforms;
	int32_t t_uvChecker;
	std::vector<std::unique_ptr<RenderObject>> renderRings_;
	std::vector<Vector4> ringColors;
	std::vector<Transforms> ringTransforms;;
	int32_t t_gradationLine;
	std::unique_ptr<RenderObject> renderCylinder_;
	Vector4 cylinderColor;
	Transforms cylinderTransform;
};


