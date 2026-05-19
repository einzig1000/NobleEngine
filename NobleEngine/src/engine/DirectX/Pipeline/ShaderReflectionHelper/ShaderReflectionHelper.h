#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <DirectX/Pipeline/RenderPipelineTypes.h>

namespace ShaderReflection
{
    /// <summary>
	/// シェーダーからRootParameterを作成する関数(完成版)
    /// </summary>
    void BuildRootParamsFromShader(
        IDxcBlob* shaderBlob,
        ShaderType shaderType,
        std::vector<RootParam>& outParams,
        uint32_t& currentCBVOffsetBytes,
        uint32_t& currentSRVOffsetIndex
    );

	/// <summary>
    /// Texture2D<float4> textures[] があるか確認する
	/// </summary>
    bool HasBindlessTextureArray(IDxcBlob* shaderBlob);



    /// <summary>
    /// DXCでコンパイルされたシェーダーからInputLayoutを取得
    /// </summary>
    /// <param name="shaderBlob">コンパイル済みシェーダーバイナリ</param>
    /// <returns>Input Element Descの配列</returns>
    std::vector<InputElement> GetInputLayoutFromShader(IDxcBlob* shaderBlob);

    /// <summary>
    /// DXGIフォーマットをコンポーネント情報から取得
    /// </summary>
    DXGI_FORMAT GetDXGIFormatFromComponentType(D3D_REGISTER_COMPONENT_TYPE componentType, uint32_t componentCount);

    /// <summary>
    /// セマンティック名の標準化
    /// </summary>
    std::string NormalizeSemanticName(const std::string& name);
}