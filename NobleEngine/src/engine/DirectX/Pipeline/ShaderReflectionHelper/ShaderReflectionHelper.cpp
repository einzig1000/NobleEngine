#include "ShaderReflectionHelper.h"
#include <Utilities/Logger/Logger.h>
#include <d3d12shader.h>
#include <cassert>
#include <wrl/client.h>
#include <d3d12shader.h>

#pragma comment(lib, "dxcompiler.lib")

namespace ShaderReflection
{
    std::vector<InputElement> GetInputLayoutFromShader(IDxcBlob* shaderBlob)
    {
        std::vector<InputElement> inputElements;

        // DXILコンテナからリフレクション情報を取得
        Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
        HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
        if (FAILED(hr))
        {
            Log("IDxcUtilsの生成に失敗しました");
            return inputElements;
        }

        DxcBuffer dxcBuffer = {};
        dxcBuffer.Ptr = shaderBlob->GetBufferPointer();
        dxcBuffer.Size = shaderBlob->GetBufferSize();
        dxcBuffer.Encoding = DXC_CP_ACP;

        Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflection;
        hr = dxcUtils->CreateReflection(&dxcBuffer, __uuidof(ID3D12ShaderReflection), (void**)&reflection);
        if (FAILED(hr))
        {
            Log("シェーダーリフレクションの取得失敗");
            return inputElements;
        }

        D3D12_SHADER_DESC shaderDesc;
        reflection->GetDesc(&shaderDesc);

        // 入力パラメータを列挙
        for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
        {
            D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
            reflection->GetInputParameterDesc(i, &paramDesc);

            D3D12_INPUT_ELEMENT_DESC elemDesc{};
            elemDesc.Format = GetDXGIFormatFromComponentType(paramDesc.ComponentType, __popcnt(paramDesc.Mask));
            elemDesc.InputSlot = 0;
            elemDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
            elemDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            elemDesc.InstanceDataStepRate = 0;

            InputElement elem{};
            elem.semanticName = NormalizeSemanticName(paramDesc.SemanticName);
            elem.semanticIndex = paramDesc.SemanticIndex;
            elem.desc = elemDesc;

            inputElements.push_back(elem);
        }

        if (inputElements.empty())
        {
            Log("シェーダーから入力レイアウトが見つかりませんでした。");
        }

        return inputElements;
    }

    DXGI_FORMAT GetDXGIFormatFromComponentType(D3D_REGISTER_COMPONENT_TYPE componentType, uint32_t componentCount)
    {
        switch (componentType)
        {
        case D3D_REGISTER_COMPONENT_FLOAT32:
            switch (componentCount)
            {
            case 1: return DXGI_FORMAT_R32_FLOAT;
            case 2: return DXGI_FORMAT_R32G32_FLOAT;
            case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
            case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
            }
            break;
        case D3D_REGISTER_COMPONENT_UINT32:
            switch (componentCount)
            {
            case 1: return DXGI_FORMAT_R32_UINT;
            case 2: return DXGI_FORMAT_R32G32_UINT;
            case 4: return DXGI_FORMAT_R32G32B32A32_UINT;
            }
            break;
        }
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }

    std::string NormalizeSemanticName(const std::string& name)
    {

        return name;
    }



    void BuildRootParamsFromShader(IDxcBlob* shaderBlob, ShaderType shaderType, std::vector<RootParam>& outParams, uint32_t& currentCBVOffsetBytes, uint32_t& currentSRVOffsetIndex)
    {
        Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
        HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
        if (FAILED(hr))
        {
            Log("IDxcUtilsの生成に失敗しました");
            return;
        }

        DxcBuffer dxcBuffer{};
        dxcBuffer.Ptr = shaderBlob->GetBufferPointer();
        dxcBuffer.Size = shaderBlob->GetBufferSize();
        dxcBuffer.Encoding = DXC_CP_ACP;

        Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflection;
        hr = dxcUtils->CreateReflection(&dxcBuffer, __uuidof(ID3D12ShaderReflection), (void**)&reflection);
        if (FAILED(hr))
        {
            Log("シェーダーリフレクションの取得失敗");
            return;
        }

        D3D12_SHADER_DESC shaderDesc;
        reflection->GetDesc(&shaderDesc);

        for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
        {
            D3D12_SHADER_INPUT_BIND_DESC bind{};
            reflection->GetResourceBindingDesc(i, &bind);

            // CBV
            if (bind.Type == D3D_SIT_CBUFFER)
            {
                ID3D12ShaderReflectionConstantBuffer* cb = reflection->GetConstantBufferByIndex(bind.BindPoint);
                D3D12_SHADER_BUFFER_DESC cbDesc{};
                cb->GetDesc(&cbDesc);

                RootParam p{};
                p.paramType = ParamType::CBV;
                p.shaderType = shaderType;
                p.key = bind.BindPoint;
				p.registerSpace = bind.Space;
                p.ComputeHash();

                p.sizeBytes = cbDesc.Size;
                p.offsetBytes = currentCBVOffsetBytes;

                currentCBVOffsetBytes += cbDesc.Size;
				outParams.push_back(p);
            }
			// SRV
            else if (bind.Type == D3D_SIT_STRUCTURED || bind.Type == D3D_SIT_BYTEADDRESS)
            {
                RootParam p{};
                p.paramType = ParamType::SRV;
                p.shaderType = shaderType;
                p.key = bind.BindPoint;
                p.registerSpace = bind.Space;
				p.ComputeHash();

				p.srvStorageIndex = currentSRVOffsetIndex;
                currentSRVOffsetIndex++;
				outParams.push_back(p);
            }
			// Bindlessテクスチャ配列 (BindCountが0で、型がテクスチャ)
            else if (bind.Type == D3D_SIT_TEXTURE && bind.BindCount == 0)
            {
                RootParam p{};
                p.paramType = ParamType::SRV;
                p.shaderType = shaderType;
                p.key = bind.BindPoint;
                p.registerSpace = bind.Space;
                p.ComputeHash();

                switch (bind.Dimension)
                {
                case D3D_SRV_DIMENSION_TEXTURE2D:
                    p.srvAllocIndex = 0;
                    break;

                case D3D_SRV_DIMENSION_TEXTURECUBE:
                    p.srvAllocIndex = 0;
                    break;

                default:
                    Log("未対応のテクスチャタイプ");
                    break;
                }

                outParams.push_back(p);
            }
            else if (bind.Type == D3D_SIT_TEXTURE && bind.BindCount > 0)
            {
                RootParam p{};
                p.paramType = ParamType::SRV;
                p.shaderType = shaderType;
                p.key = bind.BindPoint;
                p.registerSpace = bind.Space;
                p.ComputeHash();

                // BindCountが0でないテクスチャは通常のSRVとして扱う
                p.srvStorageIndex = currentSRVOffsetIndex;
                currentSRVOffsetIndex++;
                outParams.push_back(p);
            }
        }
    }

    bool HasBindlessTextureArray(IDxcBlob* shaderBlob)
    {
        Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
        HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
        if (FAILED(hr))
        {
            Log("IDxcUtilsの生成に失敗しました");
            return false;
        }

        DxcBuffer dxcBuffer{};
        dxcBuffer.Ptr = shaderBlob->GetBufferPointer();
        dxcBuffer.Size = shaderBlob->GetBufferSize();
        dxcBuffer.Encoding = DXC_CP_ACP;

        Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflection;
        hr = dxcUtils->CreateReflection(&dxcBuffer, __uuidof(ID3D12ShaderReflection), (void**)&reflection);
        if (FAILED(hr))
        {
            Log("シェーダーリフレクションの取得失敗");
            return false;
        }

        D3D12_SHADER_DESC shaderDesc;
        reflection->GetDesc(&shaderDesc);

        for (UINT i = 0; i < shaderDesc.BoundResources; i++)
        {
            D3D12_SHADER_INPUT_BIND_DESC bind{};
            reflection->GetResourceBindingDesc(i, &bind);

            if (bind.Type == D3D_SIT_TEXTURE &&
                bind.BindCount == 0 &&
                std::string(bind.Name) == "textures")
            {
                return true;
            }
        }
        return false;
    }
}