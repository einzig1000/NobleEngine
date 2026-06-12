#pragma once
#include <definition/definition.h>
class ModelBank;

class ModelCreater
{
public:
	ModelCreater(ID3D12Device2* device, ModelBank* bank);
	~ModelCreater();

	// モデル作成
	int32_t CreateModel(const std::vector<VertexData>& vertices, const std::string& name);

private:
	ID3D12Device2* device_;
	ModelBank* bank_;
};

