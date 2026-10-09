#pragma once
#include <EngineDefinition/EngineDefinition.h>

class ModelBank;
class DirectXManager;

class ModelCreator
{
public:
	ModelCreator(DirectXManager* dxManager, ModelBank* bank);
	~ModelCreator();

	/// <summary>
	/// モデルを作成する
	/// </summary>
	/// <param name="vertices">トライアングルリストの頂点配列</param>
	/// <param name="name">モデルのユニークな名前</param>
	/// <param name="optimize">頂点の最適化を行うかどうか</param>
	/// <returns></returns>
	int32_t CreateModel(const std::vector<VertexData>& vertices, const std::string& name, const bool optimize);

private:
	DirectXManager* dxManager_;
	ModelBank* bank_;

	// アップロード用一時リソースを保持するリスト
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateUploadResources_;
};

