#pragma once

#include "definition/definition.h"

#include "externals/DirectXTex/d3dx12.h"

#include <Windows.h>
#include <sstream>

#include <dxcapi.h>
#include <optional>
#pragma comment(lib, "dxcompiler")

class RenderData_Model;

template <typename T, size_t Rows, size_t Cols>
void FlipYAxis(T(&arr)[Rows][Cols])
{
	for (size_t i = 0; i < Rows / 2; ++i)
	{
		std::swap(arr[i], arr[Rows - 1 - i]);
	}
}

template <typename T, size_t Rows, size_t Cols>
void FlipXAxis(T(&arr)[Rows][Cols])
{
	for (size_t i = 0; i < Rows; ++i)
	{
		for (size_t j = 0; j < Cols / 2; ++j)
		{
			std::swap(arr[i][j], arr[i][Cols - 1 - j]);
		}
	}
}

/// <summary>
// 三角形の法線ベクトルを計算し、正規化して返す
/// </summary>
/// <param name="v0">三角形の頂点0</param>
/// <param name="v1">三角形の頂点1</param>
/// <param name="v2">三角形の頂点2</param>
/// <returns>正規化された三角形の法線ベクトル</returns>
Vector3 TriangleNormal(const Vector4& v0, const Vector4& v1, const Vector4& v2);

/// <summary>
/// Vector3の座標変換
/// </summary>
/// <param name="vector">変換したいベクトル</param>
/// <param name="matrix">適用したいマトリックス</param>
/// <returns>座標変換されたvector3</returns>
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

Vector4 Transform(const Vector4& v, const Matrix4x4& m);



// fontCutImagePos(描画したい文字)
Vector2int fontCutImagePos(char c);


#pragma region collision

bool IsCollision(const Sphere& s1, const Sphere& s2);
bool IsCollision(const Sphere& s, const Plane& p);
bool IsCollision(const Segment& s, const Plane& p);
bool IsCollision(const Segment& s, const Triangle& t);
bool IsCollision(const Ray& r, const Plane& p);
bool IsCollision(const Ray& r, const AABB& aabb);
bool IsCollision(const Ray& r, const Triangle& t);
bool IsCollision(const AABB& aabb1, const AABB& aabb2);
bool IsOverLap(const AABB& aabb1, const AABB& aabb2);
bool IsLooseCollision(const AABB& aabb1, const AABB& aabb2, float threshold = 0.1f);
bool IsCollision(const AABB& aabb, const Sphere& s);
bool IsCollision(const AABB& aabb, const Segment& s);
//bool IsCollision(const Ray& ray, const std::vector<VertexData>& vertices, const AABB& aabb, const Transforms& data);
//bool IsCollision(const Ray& ray, const std::vector<VertexData>& vertices, const RenderData_Model* data);

std::optional<Vector3> IntersectRayTriangle(const Ray& ray, const Triangle& t);
//std::optional<Vector3> IntersectRayModel(const Ray& ray, const std::vector<VertexData>& vertices, const AABB& aabb, const Transforms& data);
std::optional<Vector3> IntersectRayModel(const Ray& ray, const std::vector<VertexData>& vertices, const RenderData_Model* data);
std::optional<Vector3> IntersectRayAABB(const Ray& ray, const AABB& box);


#pragma endregion

#pragma region Rand
#pragma endregion









/// <summary>
/// クラッシュ時にミニダンプを生成する関数
/// </summary>
/// <param name="exception">例外情報</param>
/// <returns>例外ハンドラの実行結果</returns>
LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);
