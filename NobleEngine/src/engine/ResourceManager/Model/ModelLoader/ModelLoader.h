//#pragma once
//#include <definition/definition.h>
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//class ModelBank;
//
//class ModelLoader
//{
//public:
//	ModelLoader(ID3D12Device* device);
//	~ModelLoader();
//
//	// モデル読み込み
//	int32_t LoadModel(const std::string& filePath);
//
//private:
//	ID3D12Device* device_;
//	ModelBank* bank_;
//
//	// mtlファイル読み込み
//	MaterialData LoadMaterialTemplateFile(const std::string& filePath);
//
//	// モデルファイル読み込み
//	void LoadModelFile(const std::string& filePath, ModelData& modelData);
//	Node ReadNode(const aiNode* node);
//};
//
