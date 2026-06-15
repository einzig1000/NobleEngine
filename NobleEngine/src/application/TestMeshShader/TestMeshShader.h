#pragma once
#include <memory>
#include <DrawSystem/RenderData/RenderObject.h>

class TestMeshShader
{
public:
	TestMeshShader();
	~TestMeshShader();

	void Initialize();
	void Update();
	void Draw(int32_t rtID);


private:
	std::unique_ptr<RenderObject> render_;

};

