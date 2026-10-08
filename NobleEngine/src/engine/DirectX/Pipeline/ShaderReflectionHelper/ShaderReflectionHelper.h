#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <vector>
#include <DirectX/Pipeline/RenderPipelineTypes.h>

namespace ShaderReflection
{
    /// <summary>
    /// シェーダーからRootParameterを作成する関数(完成版)
    /// </summary>
    void BuildRootParamsFromShader(
        IDxcUtils* dxcUtils,
        IDxcBlob* shaderBlob,
        ShaderType shaderType,
        std::vector<RootParam>& outParams
    );

    /// <summary>
    /// DXCでコンパイルされたシェーダーからInputLayoutを取得
    /// </summary>
    /// <param name="shaderBlob">コンパイル済みシェーダーバイナリ</param>
    /// <returns>Input Element Descの配列</returns>
    std::vector<InputElement> GetInputLayoutFromShader(IDxcBlob* shaderBlob);
}
