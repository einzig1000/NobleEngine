#pragma once
#include <definition/definition.h>

class ModelBank;
class DirectXManager;

class ModelCreater
{
public:
	ModelCreater(DirectXManager* dxManager, ModelBank* bank);
	~ModelCreater();

	// モデル作成
	int32_t CreateModel(const std::vector<VertexData>& vertices, const std::string& name);

private:
	DirectXManager* dxManager_;
	ModelBank* bank_;

	// アップロード用一時リソースを保持するリスト
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateUploadResources_;
};

