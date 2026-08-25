#pragma once
#include "definition/definition.h"
#include <string>

/// <summary>
/// デバッグ用のログを出力する関数
/// </summary>
/// <param name="message">出力するメッセージ</param>
void Log(const std::string& message);

/// <summary>
/// デバッグ用のログを出力する関数 (Vector4型の値を含む)
/// </summary>
/// <param name="message">出力するメッセージ</param>
/// <param name="vector">ログに出力する Vector4</param>
void Log(const std::string& message, const Vector4& vector);

/// <summary>
/// デバッグ用のログを出力する関数 (Matrix4x4型の値を含む)
/// </summary>
/// <param name="message">出力するメッセージ</param>
/// <param name="matrix">ログに出力する Matrix4x4</param>
void Log(const std::string& message, const Matrix4x4& matrix);

/// <summary>
/// デバッグ用のログを出力する関数 (D3D12_RESOURCE_BARRIER の状態を含む)
/// </summary>
/// <param name="message">出力するメッセージ</param>
/// <param name="barrier">ログに出力するリソースバリア</param>
void Log(const std::string& message, const D3D12_RESOURCE_BARRIER& barrier);

/// <summary>
/// デバッグ用のログを出力する関数 (D3D12_ROOT_SIGNATURE_DESC の状態を含む)
/// </summary>
/// <param name="desc">ログに出力するルートシグネチャの記述子</param>
void Log(const D3D12_ROOT_SIGNATURE_DESC& desc);

/// <summary>
/// printfみたいに使えるログ関数
/// </summary>
void Log(const char* format, ...);

/// <summary>
/// ログをファイルに書き出す関数
/// </summary>
/// <param name="os">出力先のファイルストリーム</param>
/// <param name="message">出力するメッセージ</param>
void Log(std::ofstream& os, const std::string& message);


/// <summary>
/// D3D12_RESOURCE_STATES を文字列に変換する関数
/// </summary>
std::string ResourceStateToString(D3D12_RESOURCE_STATES state);

/// <summary>
/// HRESULT を文字列に変換する関数
/// </summary>
std::string HrToString(HRESULT hr);