#include "Logger.h"
#include <format>
#include <sstream>
#include <fstream>
#include <iostream>

#ifdef _MSC_VER
#define VSNPRINTF_FUNC _vsnprintf_s
#else
#define VSNPRINTF_FUNC vsnprintf
#endif

// ログを出す関数
void Log(const std::string& message)
{
    // string型からchar*型に変換した文字列
    OutputDebugStringA((message + "\n").c_str());
}
// Vector4型用のオーバーロード
void Log(const std::string& message, const Vector4& vector)
{
    Log(message);
    std::string a = std::format("x={}, y={}, z={}, w={}", vector.x, vector.y, vector.z, vector.w);
    Log(a);
}
// Matrix4x4型用のオーバーロード
void Log(const std::string& message, const Matrix4x4& matrix)
{
    Log(message);
    std::string a = ":\n";
    for (int i = 0; i < 4; ++i)
    {
        a += std::format("[{}, {}, {}, {}]\n", matrix.m[i][0], matrix.m[i][1], matrix.m[i][2], matrix.m[i][3]);
    }
    Log(a);
}
// barrier.Transitionの状態をログに出力する関数
void Log(const std::string& message, const D3D12_RESOURCE_BARRIER& barrier)
{
    Log(message);
    if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
    {
        std::string stateBefore = ResourceStateToString(barrier.Transition.StateBefore);
        std::string stateAfter = ResourceStateToString(barrier.Transition.StateAfter);
        std::string a = std::format("Barrier Transition - StateBefore: {}, StateAfter: {}", stateBefore, stateAfter);
        Log(a);
    }
    else
    {
        Log("Barrier is not of type TRANSITION.");
    }
}
// RootSignatureの状態をログに出力する関数
void Log(const D3D12_ROOT_SIGNATURE_DESC& desc)
{
    std::ostringstream oss;
    oss << "[Root Signature]\n"
        << "NumParameters: " << desc.NumParameters << "\n"
        << "NumStaticSamplers: " << desc.NumStaticSamplers << "\n"
        << "Flags: " << desc.Flags << "\n";

    Log(oss.str());

    for (UINT i = 0; i < desc.NumParameters; ++i)
    {
        const auto& param = desc.pParameters[i];
        oss.str(""); // バッファをクリア
        oss.clear(); // 状態をリセット
        oss << "[Root Parameter " << i << "]\n"
            << "Type: " << param.ParameterType << "\n"
            << "ShaderVisibility: " << param.ShaderVisibility << "\n";
        Log(oss.str());

        if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV ||
            param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV ||
            param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV)
        {
            oss.str("");
            oss.clear();
            oss << "ShaderRegister: " << param.Descriptor.ShaderRegister << "\n"
                << "RegisterSpace: " << param.Descriptor.RegisterSpace << "\n";
            Log(oss.str());
        }
        else if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
        {
            oss.str("");
            oss.clear();
            oss << "NumDescriptorRanges: " << param.DescriptorTable.NumDescriptorRanges << "\n";
            Log(oss.str());
            for (UINT j = 0; j < param.DescriptorTable.NumDescriptorRanges; ++j)
            {
                const auto& range = param.DescriptorTable.pDescriptorRanges[j];
                oss.str("");
                oss.clear();
                oss << "  [Descriptor Range " << j << "]\n"
                    << "  RangeType: " << range.RangeType << "\n"
                    << "  BaseShaderRegister: " << range.BaseShaderRegister << "\n"
                    << "  NumDescriptors: " << range.NumDescriptors << "\n"
                    << "  RegisterSpace: " << range.RegisterSpace << "\n";
                Log(oss.str());
            }
        }
    }

    for (UINT i = 0; i < desc.NumStaticSamplers; ++i)
    {
        const auto& sampler = desc.pStaticSamplers[i];
        oss.str("");
        oss.clear();
        oss << "[Static Sampler " << i << "]\n"
            << "ShaderRegister: " << sampler.ShaderRegister << "\n"
            << "Filter: " << sampler.Filter << "\n"
            << "AddressU: " << sampler.AddressU << "\n"
            << "AddressV: " << sampler.AddressV << "\n"
            << "AddressW: " << sampler.AddressW << "\n"
            << "ShaderVisibility: " << sampler.ShaderVisibility << "\n";
        Log(oss.str());
    }
}
// もはやこれしか使わないログ
void Log(const char* format, ...)
{
    // 最大バッファサイズを設定 (文字列の最大長)
    const int INITIAL_BUFFER_SIZE = 256;
    std::vector<char> charBuffer(INITIAL_BUFFER_SIZE); // char 型の動的バッファ

    va_list args;
    va_start(args, format);

    // 最初にバッファサイズを試行。BUFFER_SIZEが足りなければ、vsnprintf君は必要としているサイズをいつも返してくれる
    int written = VSNPRINTF_FUNC(charBuffer.data(), charBuffer.size(), _TRUNCATE, format, args);
    // _TRUNCATE は MSVC 固有のオプションで、バッファが足りない場合に切り詰める

    if (written < 0 || written >= charBuffer.size())
    {
        // バッファが足りなかった、またはエラーが発生した場合
        // 必要なサイズを計算してバッファをリサイズし再試行
        size_t required_size = charBuffer.size() * 2; // ジャスト必要な分あるはずだけど一応2倍にするとよいらしい
        if (written > 0)
        {
            required_size = static_cast<size_t>(written) + 1; // +1 for null terminator
        }
        charBuffer.resize(required_size);
        // va_list をリセットして再利用 (va_copy を使うのがより堅牢)
        va_end(args); // 一度終了
        va_start(args, format); // 再度開始

        written = VSNPRINTF_FUNC(charBuffer.data(), charBuffer.size(), _TRUNCATE, format, args);
    }

    va_end(args); // 可変引数リストの終了

    // ここで charBuffer.data() にフォーマットされた文字列が入っている

    // char (マルチバイト) から wchar_t (ワイド文字) に変換
    // 変換に必要なバッファサイズを取得
    int wlen = MultiByteToWideChar(CP_UTF8, 0, charBuffer.data(), written, nullptr, 0);
    if (wlen > 0)
    {
        std::vector<wchar_t> wcharBuffer(wlen + 2); // 改行とNULL終端のために+2
        MultiByteToWideChar(CP_UTF8, 0, charBuffer.data(), written, wcharBuffer.data(), wlen);

        // ワイド文字バッファに改行とNULL終端を追加
        wcharBuffer[wlen] = L'\n';
        wcharBuffer[wlen + 1] = L'\0';

        // OutputDebugStringW で出力
        OutputDebugStringW(wcharBuffer.data());
    }
    else
    {
        // 変換エラーの場合
        OutputDebugStringA("Log: Error converting multi-byte to wide-char.I can't speak Japanese hahaha Sushi\n");
        // 元の charBuffer をそのまま出力 (文字化け覚悟)
        OutputDebugStringA(charBuffer.data());
        OutputDebugStringA("\n");
    }
}
// ログをファイルに書き出す
void Log(std::ofstream& os, const std::string& message)
{
    os << message << std::endl;
    OutputDebugStringA(message.c_str());
}
// D3D12_RESOURCE_STATES を文字列に変換する関数
std::string ResourceStateToString(D3D12_RESOURCE_STATES state)
{
    switch (state)
    {
    case D3D12_RESOURCE_STATE_COMMON: return "COMMON";
    case D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER: return "VERTEX_AND_CONSTANT_BUFFER";
    case D3D12_RESOURCE_STATE_INDEX_BUFFER: return "INDEX_BUFFER";
    case D3D12_RESOURCE_STATE_RENDER_TARGET: return "RENDER_TARGET";
    case D3D12_RESOURCE_STATE_UNORDERED_ACCESS: return "UNORDERED_ACCESS";
    case D3D12_RESOURCE_STATE_DEPTH_WRITE: return "DEPTH_WRITE";
    case D3D12_RESOURCE_STATE_DEPTH_READ: return "DEPTH_READ";
    case D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE: return "NON_PIXEL_SHADER_RESOURCE";
    case D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE: return "PIXEL_SHADER_RESOURCE";
    case D3D12_RESOURCE_STATE_STREAM_OUT: return "STREAM_OUT";
    case D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT: return "INDIRECT_ARGUMENT";
    case D3D12_RESOURCE_STATE_COPY_DEST: return "COPY_DEST";
    case D3D12_RESOURCE_STATE_COPY_SOURCE: return "COPY_SOURCE";
    case D3D12_RESOURCE_STATE_RESOLVE_DEST: return "RESOLVE_DEST";
    case D3D12_RESOURCE_STATE_RESOLVE_SOURCE: return "RESOLVE_SOURCE";
    case D3D12_RESOURCE_STATE_VIDEO_DECODE_READ: return "VIDEO_DECODE_READ";
    case D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE: return "VIDEO_DECODE_WRITE";
    case D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ: return "VIDEO_PROCESS_READ";
    case D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE: return "VIDEO_PROCESS_WRITE";
    case D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ: return "VIDEO_ENCODE_READ";
    case D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE: return "VIDEO_ENCODE_WRITE";
    default: return "UNKNOWN_STATE";
    }
}
// HRESULT を文字列に変換する関数
std::string HrToString(HRESULT hr)
{
    switch (hr)
    {
    case S_OK: return "S_OK";
    case E_FAIL: return "E_FAIL";
    case E_INVALIDARG: return "E_INVALIDARG";
    case E_OUTOFMEMORY: return "E_OUTOFMEMORY";

    case DXGI_ERROR_DEVICE_REMOVED: return "DXGI_ERROR_DEVICE_REMOVED";
    case DXGI_ERROR_DEVICE_HUNG: return "DXGI_ERROR_DEVICE_HUNG";
    case DXGI_ERROR_DEVICE_RESET: return "DXGI_ERROR_DEVICE_RESET";
    case DXGI_ERROR_DRIVER_INTERNAL_ERROR: return "DXGI_ERROR_DRIVER_INTERNAL_ERROR";
    case DXGI_ERROR_INVALID_CALL: return "DXGI_ERROR_INVALID_CALL";
    case DXGI_ERROR_UNSUPPORTED: return "DXGI_ERROR_UNSUPPORTED";
    case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE: return "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE";

    case D3D12_ERROR_ADAPTER_NOT_FOUND: return "D3D12_ERROR_ADAPTER_NOT_FOUND";
    case D3D12_ERROR_DRIVER_VERSION_MISMATCH: return "D3D12_ERROR_DRIVER_VERSION_MISMATCH";

    default: return "UNKNOWN_HRESULT";
    }
}
