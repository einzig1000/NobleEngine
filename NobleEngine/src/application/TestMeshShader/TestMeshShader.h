#pragma once
#include <memory>
#include <DrawSystem/RenderData/RenderObject.h>


struct ModelInfo
{
	int32_t vertexSrvIndex;
	int32_t meshletSrvIndex;
	int32_t uniqueIndexSrvIndex;
	int32_t primIndexSrvIndex;
};


class TestMeshShader
{
public:
	TestMeshShader();
	~TestMeshShader();

	void Initialize();
	void Update(int32_t cameraID);
	void Draw(int32_t rtID);

private:
	std::unique_ptr<RenderObject> render_;
	ModelInfo modelInfo_;
};

