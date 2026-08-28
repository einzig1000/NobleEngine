#pragma once
#include <EngineDefinition/EngineConstexprs.h>
#include <vector>
#include <array>
#include <span>
#include <string>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <optional>

#include <initguid.h>
#include <dxgidebug.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <xaudio2.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")

#include <wrl.h>

#include <externals/DirectXTex/DirectXTex.h>


#define PAD_A               0x00
#define PAD_B               0x01
#define PAD_X               0x02
#define PAD_Y               0x03
#define PAD_LB              0x04
#define PAD_RB              0x05
#define PAD_LS              0x06
#define PAD_RS              0x07
#define PAD_BACK            0x08
#define PAD_START           0x09
#define PAD_UP              0x0A
#define PAD_DOWN            0x0B
#define PAD_LEFT            0x0C
#define PAD_RIGHT           0x0D
#define PAD_BUTTON_MAX      0x0E


#pragma region 基盤構造体

struct Vector2
{
    float x = 0, y = 0;

    Vector2 operator+(const Vector2& rhs) const
    {
        return Vector2{ x + rhs.x, y + rhs.y };
    }
    Vector2 operator-(const Vector2& rhs) const
    {
        return Vector2{ x - rhs.x, y - rhs.y };
    }
    Vector2 operator*(float scalar) const
    {
        return Vector2{ x * scalar, y * scalar };
    }
    Vector2 operator/(float scalar) const
    {
        return Vector2{ x / scalar, y / scalar };
    }
    Vector2& operator+=(const Vector2& rhs)
    {
        x += rhs.x; y += rhs.y; return *this;
    }
    Vector2& operator-=(const Vector2& rhs)
    {
        x -= rhs.x; y -= rhs.y; return *this;
    }
    Vector2& operator*=(float scalar)
    {
        x *= scalar; y *= scalar; return *this;
    }
    Vector2& operator/=(float scalar)
    {
        x /= scalar; y /= scalar; return *this;
    }
    bool operator==(const Vector2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Vector2& rhs) const
    {
        return !(*this == rhs);
    }

    // ベクトルの長さ 
    float Length() const;
    // 平方根計算をしてない長澤
    float LengthSq() const;
    // ベクトルを正規化　※　自身を変更する　※
    Vector2& Normalize();
    // 正規化された新しいベクトルを返す　※　自身は変更されない　※
    Vector2 Normalized() const;
    // 内積
    float Dot(const Vector2& rhs) const;
};
struct Vector2int
{
    int32_t x = 0, y = 0;
    Vector2int operator+(const Vector2int& rhs) const
    {
        return Vector2int{ x + rhs.x, y + rhs.y };
    }
    Vector2int operator-(const Vector2int& rhs) const
    {
        return Vector2int{ x - rhs.x, y - rhs.y };
    }
    bool operator==(const Vector2int& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Vector2int& rhs) const
    {
        return x != rhs.x || y != rhs.y;
    }
    bool operator<(const Vector2int& rhs) const
    {
        if (y != rhs.y)
        {
            return y < rhs.y;
        }
        return x < rhs.x;
    }
};
struct Vector2intHash
{
    std::size_t operator()(const Vector2int& v) const noexcept
    {
        uint64_t x = static_cast<uint64_t>(v.x);
        uint64_t y = static_cast<uint64_t>(v.y);
        return (x * 73856093) ^ (y * 19349663);
    }
};
struct Vector2uint
{
	uint32_t x = 0, y = 0;
	Vector2uint operator+(const Vector2uint& rhs) const
	{
		return Vector2uint{ x + rhs.x, y + rhs.y };
	}
	Vector2uint operator-(const Vector2uint& rhs) const
	{
		return Vector2uint{ x - rhs.x, y - rhs.y };
	}
	bool operator==(const Vector2uint& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Vector2uint& rhs) const
	{
		return x != rhs.x || y != rhs.y;
	}
};

struct Quaternion;

struct Vector3
{
    float x = 0, y = 0, z = 0;

    Vector3 operator-() const
    {
        return Vector3(-x, -y, -z);
    }
    Vector3 operator+(const Vector3& rhs) const
    {
        return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    Vector3 operator-(const Vector3& rhs) const
    {
        return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    Vector3 operator*(float scalar) const
    {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    Vector3 operator/(float scalar) const
    {
        // ゼロ除算しようとした者を殺す
        if (std::abs(scalar) < 0.00001f)
        {
            return Vector3(0.0f, 0.0f, 0.0f);
        }
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    Vector3& operator+=(const Vector3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    Vector3& operator-=(const Vector3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    Vector3& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    Vector3& operator/=(float scalar)
    {
        // ゼロ除算しようとした者を殺す
        if (std::abs(scalar) < 0.00001f)
        {
            return *this;
        }
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    bool operator==(const Vector3& rhs) const
    {
        // 多少のズレは許容。
        const float EPSILON = 0.00001f;
        return std::abs(x - rhs.x) < EPSILON &&
            std::abs(y - rhs.y) < EPSILON &&
            std::abs(z - rhs.z) < EPSILON;
    }
    bool operator!=(const Vector3& rhs) const
    {
        return !(*this == rhs);
    }

    // ベクトルの長さ
    float Length() const;
    // 平方根計算をしてない長澤
    float LengthSq() const;
    // ベクトルを正規化　※　自身を変更する　※
    Vector3& Normalize();
    // 正規化された新しいベクトルを返す　※　自身は変更されない　※
    Vector3 Normalized() const;
    // 内積
    float Dot(const Vector3& rhs) const;
    // 外積
    Vector3 Cross(const Vector3& rhs) const;
    // 反射角
    Vector3 Reflect(const Vector3& input, const Vector3& normal);
	// ベクトルをQuaternionで回転させた結果のベクトルを返す
    Vector3 RotateByQuaternion(const Quaternion& q) const;
};
struct Vector3int
{
    int32_t x = 0, y = 0, z = 0;
    Vector3int operator+(const Vector3int& rhs) const
    {
        return Vector3int{ x + rhs.x, y + rhs.y, z + rhs.z };
    }
    Vector3int operator-(const Vector3int& rhs) const
    {
        return Vector3int{ x - rhs.x, y - rhs.y, z - rhs.z };
    }
    bool operator==(const Vector3int& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    bool operator!=(const Vector3int& rhs) const
    {
        return x != rhs.x || y != rhs.y || z != rhs.z;
    }
};
struct Vector3intHash
{
	std::size_t operator()(const Vector3int& v) const noexcept
	{
		uint64_t x = static_cast<uint64_t>(v.x);
		uint64_t y = static_cast<uint64_t>(v.y);
		uint64_t z = static_cast<uint64_t>(v.z);
		return (x * 73856093) ^ (y * 19349663) ^ (z * 83492791);
	}
};
struct Vector3uint
{
	uint32_t x = 0, y = 0, z = 0;
	Vector3uint operator+(const Vector3uint& rhs) const
	{
		return Vector3uint{ x + rhs.x, y + rhs.y, z + rhs.z };
	}
	Vector3uint operator-(const Vector3uint& rhs) const
	{
		return Vector3uint{ x - rhs.x, y - rhs.y, z - rhs.z };
	}
	bool operator==(const Vector3uint& rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	bool operator!=(const Vector3uint& rhs) const
	{
		return x != rhs.x || y != rhs.y || z != rhs.z;
	}
};

struct Vector4
{
    float x = 0, y = 0, z = 0, w = 0;

    Vector4 operator+(const Vector4& rhs) const
    {
        return Vector4{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }
    Vector4 operator-(const Vector4& rhs) const
    {
        return Vector4{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }
    Vector4 operator*(float scalar) const
    {
        return Vector4{ x * scalar, y * scalar, z * scalar, w * scalar };
    }
    Vector4 operator/(float scalar) const
    {
        return Vector4{ x / scalar, y / scalar, z / scalar, w / scalar };
    }
    Vector4& operator+=(const Vector4& rhs)
    {
        x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
    }
    Vector4& operator-=(const Vector4& rhs)
    {
        x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
    }
    Vector4& operator*=(float scalar)
    {
        x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this;
    }
    Vector4& operator/=(float scalar)
    {
        x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this;
    }
    bool operator==(const Vector4& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    bool operator!=(const Vector4& rhs) const
    {
        return !(*this == rhs);
    }

    // ベクトルの長さ
    float Length() const;
    // 平方根計算をしてない長澤
    float LengthSq() const;
    // ベクトルを正規化　※　自身を変更する　※
    Vector4& Normalize();
    // 正規化された新しいベクトルを返す　※　自身は変更されない　※
    Vector4 Normalized() const;
    // 内積
    float Dot(const Vector4& rhs) const;
};
struct Vector4int
{
    int32_t x = 0, y = 0, z = 0, w = 0;
    Vector4int operator+(const Vector4int& rhs) const
    {
        return Vector4int{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }
    Vector4int operator-(const Vector4int& rhs) const
    {
        return Vector4int{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }
    bool operator==(const Vector4int& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    bool operator!=(const Vector4int& rhs) const
    {
        return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
    }
};
struct Vector4uint
{
    uint32_t x = 0, y = 0, z = 0, w = 0;
    Vector4uint operator+(const Vector4uint& rhs) const
    {
        return Vector4uint{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }
    Vector4uint operator-(const Vector4uint& rhs) const
    {
        return Vector4uint{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }
    bool operator==(const Vector4uint& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    bool operator!=(const Vector4uint& rhs) const
    {
        return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
    }
};

struct Matrix3x3
{
    float m[3][3] = {
    { 0.0f,0.0f,0.0f },
    { 0.0f,0.0f,0.0f },
    { 0.0f,0.0f,0.0f } };

    Matrix3x3 operator+(const Matrix3x3& rhs) const
    {
        Matrix3x3 result;
        for (size_t i = 0; i < 3; ++i)
        {
            for (size_t j = 0; j < 3; ++j)
            {
                result.m[i][j] = m[i][j] + rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix3x3 operator-(const Matrix3x3& rhs) const
    {
        Matrix3x3 result;
        for (size_t i = 0; i < 3; ++i)
        {
            for (size_t j = 0; j < 3; ++j)
            {
                result.m[i][j] = m[i][j] - rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix3x3 operator*(float scalar) const
    {
        Matrix3x3 result;
        for (size_t i = 0; i < 3; ++i)
        {
            for (size_t j = 0; j < 3; ++j)
            {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }
    Matrix3x3 operator*(const Matrix3x3& rhs) const
    {
        Matrix3x3 result;
        for (size_t i = 0; i < 3; ++i)
        {
            for (size_t j = 0; j < 3; ++j)
            {
                result.m[i][j] = 0.0f;
                for (size_t k = 0; k < 3; ++k)
                {
                    result.m[i][j] += m[i][k] * rhs.m[k][j];
                }
            }
        }
        return result;
    }
    bool operator==(const Matrix3x3& rhs) const
    {
        for (size_t i = 0; i < 3; ++i)
        {
            for (size_t j = 0; j < 3; ++j)
            {
                if (m[i][j] != rhs.m[i][j]) return false;
            }
        }
        return true;
    }
    bool operator!=(const Matrix3x3& rhs) const
    {
        return !(*this == rhs);
    }

};
struct Matrix4x4
{
    float m[4][4] = {
    { 1.0f,0.0f,0.0f,0.0f },
    { 0.0f,1.0f,0.0f,0.0f },
    { 0.0f,0.0f,1.0f,0.0f },
    { 0.0f,0.0f,0.0f,1.0f } };

    Matrix4x4 operator+(const Matrix4x4& rhs) const
    {
        Matrix4x4 result;
        for (size_t j = 0; j < 4; ++j)
        {
            for (size_t i = 0; i < 4; ++i)
            {
                result.m[i][j] = m[i][j] + rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix4x4 operator-(const Matrix4x4& rhs) const
    {
        Matrix4x4 result;
        for (size_t j = 0; j < 4; ++j)
        {
            for (size_t i = 0; i < 4; ++i)
            {
                result.m[i][j] = m[i][j] - rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix4x4 operator*(float scalar) const
    {
        Matrix4x4 result;
        for (size_t j = 0; j < 4; ++j)
        {
            for (size_t i = 0; i < 4; ++i)
            {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }
    Matrix4x4 operator*(const Matrix4x4& rhs) const
    {
        Matrix4x4 result;
        for (size_t i = 0; i < 4; ++i)
        {
            for (size_t j = 0; j < 4; ++j)
            {
                result.m[i][j] = 0.0f;
                for (size_t k = 0; k < 4; ++k)
                {
                    result.m[i][j] += m[i][k] * rhs.m[k][j];
                }
            }
        }
        return result;
    }

    bool operator==(const Matrix4x4& rhs) const
    {
        for (size_t i = 0; i < 4; ++i)
            for (size_t j = 0; j < 4; ++j)
                if (m[i][j] != rhs.m[i][j]) return false;
        return true;
    }
    bool operator!=(const Matrix4x4& rhs) const
    {
        return !(*this == rhs);
    }

    // 逆行列
    Matrix4x4 Inverse() const;
    // 転置行列
    Matrix4x4 Transpose() const;
    // 単位行列
    static Matrix4x4 MakeIdentity4x4();
    // 平行移動行列
    static Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
    // 拡縮行列
    static Matrix4x4 MakeScaleMatrix(const Vector3& scale);
    // Ｘ軸回転行列
    static Matrix4x4 MakeRotateXMatrix(float radian);
    // Ｙ軸回転行列
    static Matrix4x4 MakeRotateYMatrix(float radian);
    // Ｚ軸回転行列
    static Matrix4x4 MakeRotateZMatrix(float radian);
    // ３次元アフィン変換行列
    static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate); 
    static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);


    // 透視投影行列
    static Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
    // 正射影行列(平行投影行列)
    static Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
    // ビューポート変換
    static Matrix4x4 MakeViewPortMatrix(float left, float top, float width, float height, float minD, float maxD);
    // 任意軸回転行列
    static Matrix4x4 MakeRotateAxisMatrix(const Vector3& axis, float radian);
	// ある方向からある方向に向ける回転行列(方向ベクトル同士と方向ベクトル)
	static Matrix4x4 DirectionToDirectionMatrix(const Vector3& from, const Vector3& to);
	// カメラの位置と注視点と上方向からビュー行列を作成(座標と座標とベクトル)
	static Matrix4x4 LookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up);
};

struct Quaternion
{
    float x = 0, y = 0, z = 0, w = 1;

    Quaternion operator*(const Quaternion& rhs) const
    {
        return Quaternion{
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
            w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
            w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
        };
    }
    Quaternion operator*(float scalar) const
    {
        return Quaternion{ x * scalar, y * scalar, z * scalar, w * scalar };
	}

    // 単位Quaternion
    static Quaternion MakeIdentityQuaternion();
    // 共役Quaternion
	Quaternion MakeConjugateQuaternion() const;

	float Dot(const Quaternion& rhs) const;
    // Quaternionのnormを返す
	float Norm() const;
    // 正規化したQuaternion
    Quaternion Normalize() const;
    // 逆Quaternion
    Quaternion Inverse() const;
	// 任意軸回転を表すQuaternion
	static Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float radian);
	// 単位Quaternionから回転行列を作成
	Matrix4x4 MakeRotateMatrix() const;
	// Quaternionからオイラー角を作成
	Vector3 ToEuler() const;
	// オイラー角からQuaternionを作成
	static Quaternion MakeFromEuler(const float& yaw, const float& pitch, const float& roll);
	// 2つのQuaternionの球面線形補間を返す
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);
};

#pragma endregion


#pragma region 座標系構造体

// 円柱座標系
struct Coordinate_cylindrical
{
    float radius = 1.0f;   // 半径
    float theta = 0.0f;    // 角度(ラジアン)
    float height = 0.0f;   // 高さ
};

// 球座標系
struct Coordinate_spherical
{
    float radius = 1.0f;   // 半径
    float theta = 0.0f;    // 方位角(ラジアン)
    float phi = 0.0f;      // 仰角(ラジアン)
};

#pragma endregion


#pragma region 幾何構造体

// 球
struct Sphere
{
    Vector3 center;
    float radius = 1.0f;
};

// 楕円体
struct SphereXYZ
{
    Vector3 center;
    Vector3 radius = { 1.0f, 1.0f, 1.0f };
};

// 円柱
struct Cylinder
{
    Vector3 topCenter;    // 上面中心座標
    Vector3 bottomCenter; // 底面中心座標
    float radius = 1.0f;  // 半径
};

// 平面
struct Plane
{
    Vector3 normal; // 法線
    float distance = 0.0f;
};

// 三角形
struct Triangle
{
    Vector3 vertices[3];
};

// AABB
struct AABB
{
    Vector3 min;
    Vector3 max;

    // 中心座標
    Vector3 center()const;
	// 移動　※自身が変化する※
	void Move(const Vector3& move);
    // min,maxが入れ替わる可能性があれば毎フレーム飛び出したい
    void Fix();
	// 衝突している時は深度ベクトルを返す. 衝突していない時は(0,0,0)を返す
    Vector3 GetCollisionDepth(const AABB& other)const;

    AABB operator+(const Vector3& rhs) const
    {
        return AABB{ min + rhs, max + rhs };
	}
};

// OBB
struct OBB
{
    Vector3 center;      // 中心座標
    Vector3 axis[3];     // 各軸方向（正規化済み・直交）
    Vector3 halfSize;    // 各軸方向の半径（辺の半分の長さ）

    // ローカル空間のAABBとワールド行列からOBBを作る
    static OBB MakeFromAABB(const AABB& localAABB, const Matrix4x4& worldMatrix);
};

// 線分
struct Line
{
    // 始点
    Vector3 origin;
    // 終点
    Vector3 end;
};

// 半直線
struct Ray
{
    // 始点
    Vector3 origin;
    // 終点ベクトル
    Vector3 diff;
};

// 直線
struct Segment
{
    // 始点ベクトル
    Vector3 origin;
    // 終点ベクトル
    Vector3 diff;
};

#pragma endregion


#pragma region イージング構造体

enum class EaseType
{
    LINEAR,
    IN_SINE,
    OUT_SINE,
    IN_OUT_SINE,
    IN_QUAD,
    OUT_QUAD,
    IN_OUT_QUAD,
    IN_CUBIC,
    OUT_CUBIC,
    IN_OUT_CUBIC,
    IN_QUART,
    OUT_QUART,
    IN_OUT_QUART,
    IN_QUINT,
    OUT_QUINT,
    IN_OUT_QUINT,
    IN_EXPO,
    OUT_EXPO,
    IN_OUT_EXPO,
    IN_CIRC,
    OUT_CIRC,
    IN_OUT_CIRC,
    IN_BACK,
    OUT_BACK,
    IN_OUT_BACK,
    IN_ELASTIC,
    OUT_ELASTIC,
    IN_OUT_ELASTIC,
    IN_BOUNCE,
    OUT_BOUNCE,
};

template <typename t>
struct EasingSet
{
    float durationMs = 0;
    float elapsedMs = 0;
    EaseType easetype = EaseType::LINEAR;
    t start{};
    t target{};
    bool easeing = false;
};

#pragma endregion


// 変換情報
struct EulerTransforms
{
    Vector3 scale = { 1,1,1 };
    Vector3 rotate = { 0,0,0 };
    Vector3 translate = { 0,0,0 };
};
struct QuaternionTransforms
{
	Vector3 scale = { 1,1,1 };
	Quaternion rotate;
	Vector3 translate = { 0,0,0 };
};

struct VectorDynamics
{
    Vector3 value = { 1.0f,1.0f,1.0f };
    Vector3 velocity;
    Vector3 acceleration;
};

struct TransformationMatrix
{
    Matrix4x4 WVP;
    Matrix4x4 World;
};



#pragma region 衝突判定構造体

enum class AABBFace
{
    NONE = -1,
    ZPlus = 0,  // z+
    ZMinus = 1,   // z-
    XPlus = 2,   // x+
    XMinus = 3,  // x-
    YPlus = 4,    // y+
    YMinus = 5, // y-
};

struct ColliderShape
{
    std::vector<AABB> aabbs;
    std::vector<Sphere> spheres;
};

#pragma endregion


#pragma region アニメーションデータ構造体

template <typename tValue>
struct Keyframe
{
    tValue value;
    float time;
};

template <typename tValue>
struct AnimationCurve
{
    std::vector<Keyframe<tValue>> keyFrames;
};

struct NodeAnimation
{
    AnimationCurve<Vector3> translate;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> scale;
};

struct AnimationData
{
    float duration = 0.0f;  // アニメーション全体の長さ(秒)
    std::map<std::string, NodeAnimation> nodeAnimations;
};


struct Node
{
	QuaternionTransforms transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

struct Joint
{
	QuaternionTransforms transform;      // ローカル座標系での変換情報
	Matrix4x4 localMatrix;              // ローカル座標系での変換行列
	Matrix4x4 skeletonSpaceMatrix;      // スケルトン空間での変換行列(スキニングのときに使う)
	std::string name;                   // 名前
	std::vector<int32_t> childrenIndex; // 子Jointのインデックス
	int32_t index;                      // 自分のインデックス
	std::optional<int32_t> parentIndex; // 親Jointのインデックス(親がいない場合はstd::nullopt)
};

struct Skeleton
{
    int32_t root = 0;
	std::map<std::string, int32_t> jointIndexByName;
	std::vector<Joint> joints;
};

#pragma endregion

#pragma region モデルデータ構造体

struct VertexInfluence
{
	std::array<float, 4> weights;
	std::array<int32_t, 4> jointIndices;
};

struct WellForGPU
{
	Matrix4x4 skeletonSpaceMatrix;
	Matrix4x4 skeletonSpaceInverseTransposeMatrix;
};

struct SkinCluster
{
	std::vector<Matrix4x4> inverseBindPoseMatrices;

    Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluences;

    Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
};

// 材質データ(今はテクスチャパスしかいれてない.質感とか追加するようになったら使うのかも)
struct MaterialData
{
    std::string textureFilePath;
};

// 頂点データ
struct VertexData
{
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};

struct VertexWeightData
{
	float weight = 0.0f;
	uint32_t vertexIndex = 0;
};

struct JointWeightData
{
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};



struct ResMeshlet
{
    uint32_t vertexOffset = 0;		// 頂点番号オフセット
    uint32_t primitiveOffset = 0;	// プリミティブ番号オフセット
    uint32_t vertexCount = 0;		// 頂点数
    uint32_t primitiveCount = 0;	// プリミティブ数
};

// モデルデータ
struct ModelData
{
    // 頂点データ
    std::vector<VertexData> vertices;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    uint32_t vertexHeapSlot = UINT32_MAX;
    // インデックスデータ
    std::vector<uint32_t> indices;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    // メッシュレットデータ
    std::vector<ResMeshlet> meshlets;
    Microsoft::WRL::ComPtr<ID3D12Resource> meshletBuffer;
    uint32_t meshletHeapSlot = UINT32_MAX;
    // メッシュレットの頂点インデックスのユニーク化された配列
    std::vector<uint32_t> uniqueVertexIndices;
    Microsoft::WRL::ComPtr<ID3D12Resource> uniqueVertexIndexBuffer;
    uint32_t uniqueVertexIndexHeapSlot = UINT32_MAX;
    // メッシュレットのプリミティブインデックスの配列
    std::vector<uint32_t> primitiveIndices;         // 10bit * 3 = 30bit, 残り2bitは予約領域
    Microsoft::WRL::ComPtr<ID3D12Resource> primitiveIndexBuffer;
    uint32_t primitiveIndexHeapSlot = UINT32_MAX;

	MaterialData material;              // 材質データ
	Node rootNode;                      // ノード
	Skeleton skeleton;                  // スケルトン
	uint32_t materialID = 0;            // マテリアルID
	SkinCluster skinCluster;            // スキンクラスタ(あにめーしょんデータに移行する予定)
	std::map<std::string, JointWeightData> skinClusterData; // ジョイントのウェイトデータ

    // ファイルパス
	std::string filePath;

    // HitBoxリスト
	ColliderShape colliderShape;
};

#pragma endregion

#pragma region テクスチャデータ構造体

// テクスチャデータ
struct TextureData
{
    // データ本体
	DirectX::TexMetadata metadata{};
    DirectX::ScratchImage mipImage;

	// テクスチャリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;

    // ファイルパス
    std::string filePath;
};

#pragma endregion

#pragma region オーディオデータ構造体

// オーディオデータとソースボイスを保持する構造体
//struct AudioData
//{
//	// データ本体
//    std::vector<BYTE> audioData;
//    UINT32 audioBytes = 0;
//
//    WAVEFORMATEX* pWfx = nullptr;
//    UINT32 wfxSize = 0;
//
//    IXAudio2SourceVoice* pSourceVoice = nullptr;
//    XAUDIO2_BUFFER xAudioBuffer = {};
//};

struct AudioData
{
    // データ本体
    std::vector<BYTE> audioData;
    UINT32 audioBytes = 0;

    WAVEFORMATEX* pWfx = nullptr;
    UINT32 wfxSize = 0;
};

#pragma endregion



#pragma region 入力構造体

struct KeyState
{
    bool curr = false;           // 今フレームの押下状態
    bool prev = false;           // 前フレームの押下状態
    uint32_t holdFrames = 0;     // 長押しフレーム数
    uint32_t lastHoldOnRelease = 0; // 直近のリリース時に押されていたフレーム数
};

struct PadButtonState
{
    bool curr = false;           // 今フレームの押下状態
    bool prev = false;           // 前フレームの押下状態
    uint32_t holdFrames = 0;     // 長押しフレーム数
	uint32_t lastHoldOnRelease = 0; // 直近のリリース時に押されていたフレーム数
};

struct mouseButtonState
{
    bool curr = false;           // 今フレームの押下状態
    bool prev = false;           // 前フレームの押下状態
    uint32_t holdFrames = 0;     // 長押しフレーム数
    uint32_t lastHoldOnRelease = 0; // 直近のリリース時に押されていたフレーム数
};

#pragma endregion


#pragma region 描画オプション構造体

struct alignas(16) PunctualLight
{
    // 共通
    Vector3 color = { 1.0f, 1.0f, 1.0f };     // 光の色
    float   intensity = 1.0f; // 明るさスケール

    // Directional
    Vector3 direction = { 0.0f, -1.0f, 0.0f }; // 光の向き

	// Spot 用
    float   spotCos = 0.0f;   // スポットライトの内側コーンの cos(角度)

    // Point / Spot 用
    Vector3 position = { 0.0f, 0.0f, 0.0f };  // 光源位置
    float   range = 10.0f;     // 有効距離

    // その他
    int32_t type = 0;      // 0:Dir, 1:Point, 2:Spot
    Vector3 _pad = { 0.0f, 0.0f, 0.0f };      // 16byte アラインメント用
};

struct LightDataForGPU
{
	PunctualLight lights[4];
	int32_t LightCount = 0;
	Vector3 ambientColor = { 0.0f, 0.0f, 0.0f };
};

struct Material
{
	Vector3 diffuseColor = { 1.0f, 1.0f, 1.0f };
	float  shininess = 1.0f;
	Vector3 specularColor = { 1.0f, 1.0f, 1.0f };
	float  _pad0 = 0.0f;
};

#pragma endregion


#pragma region パーティクルデータ構造体

// パーティクル情報

struct EmitterSphere
{
    Vector3 translate;  // 座標
    float radius = 1.0f;       // 射出半径
    uint32_t count = 1;     // 射出数
	float frequency = 1.0f;    // 射出頻度
	float frequencyTime = 0.0f; // 射出頻度タイマ
	uint32_t emit = 0;      // 射出するかどうか
};



struct ParticleResource
{
    uint32_t model = 0;
    uint32_t texture = 0;
};

struct ParticleEmitter
{
    AABB emitterAABB = { Vector3{ -1.0f, -1.0f, -1.0f }, Vector3{ 1.0f, 1.0f, 1.0f } };
    SphereXYZ emitterSphere;
    bool useSphereEmitter = false;  // 球体エミッターを使うかどうか
    bool emitFromInside = true;
};

struct ParticleTarget
{
    bool useTarget = false; // ターゲット方向に飛ばすかどうか
    bool spawnDependent = false; // 発生位置に依存した方向に飛ばすかどうか
    Vector3 target;
    float speed = 1.0f;      // 速度
    float spreadAngle = 0.0f; // 拡散角度
};

struct ParticleDensity
{
    int32_t particlesPerEmission = 1;   // 1フレで生む数
    int32_t emissionDelay = 10;         // 生成間隔フレーム
    int32_t liveMax = 300;              // 寿命フレーム(マイナスの時は不老)
    uint32_t frame = 0;             // 経過フレーム
};

struct ParticleOption
{
    bool isBillboard = true;    // ビルボードかどうか
};

struct ParticleSRT
{
    bool isRandom_value = false; // trueならランダム生成
    Vector3 value = { 1.0f,1.0f,1.0f };
    AABB randomRange_value;
    bool isRandom_velocity = false; // trueならランダム生成
    Vector3 velocity;
    AABB randomRange_velocity;
    bool isRandom_acceleration = false; // trueならランダム生成
    Vector3 acceleration;
    AABB randomRange_acceleration;
};

struct ParticleMaterial
{
    uint32_t color = 0xFFFFFFFF;
    Matrix4x4 uvTransform;
};

struct ParticleInf
{
    ParticleResource resource;
    ParticleEmitter emitter;
    ParticleTarget target;
    ParticleDensity density;
    ParticleOption option;
    ParticleSRT scale;
    ParticleSRT rotate;
    ParticleSRT translate;
    ParticleMaterial material;
};

// パーティクルインスタンス情報
struct ParticleMonoInf
{
    ParticleSRT scale;
    ParticleSRT rotate;
    ParticleSRT translate;

    Matrix4x4 World;
    Matrix4x4 WVP;

    uint32_t liveTime = 0;
    Vector4 color;
    uint32_t isBillboard;
};

struct ParticleMonoInfGPU
{
    Matrix4x4 World;
    Matrix4x4 WVP;
    Vector4 color;
    uint32_t liveTime;
    uint32_t isBillboard;
    uint32_t padding[2];
};

#pragma endregion


#pragma region カメラ構造体

enum class CameraMode_FirstPerson_ThirdPerson
{
    // 一人称視点
    FirstPerson,
    // 三人称後方視点
    ThirdPerson_Back,
    // 三人称前方視点
    ThirdPerson_Front,
};


enum class CameraMode_ORBIT_FPS
{
    ORBIT,
    FPS
};

#pragma endregion



#pragma region 方向


// 上下左右
enum class DirectionXY
{
    None = -1,
    Left = 0,
    Right = 1,
    Down = 2,
    Up = 3,
};

// 前後左右
enum class DirectionXZ
{
    None = -1,
	Left = 0,       // X-
	Right = 1,      // X+
	Back = 2,       // Z-
	Front = 3,      // Z+
};

// 前後左右 + 斜め
enum class DirectionXZ8Way
{
    None = -1,
    Front = 0,
    FrontLeft = 1,
    Left = 2,
    BackLeft = 3,
    Back = 4,
    BackRight = 5,
    Right = 6,
    FrontRight = 7,
};

// 上下左右前後
enum class DirectionXYZ
{
    None = -1,
    Left = 0,
    Right = 1,
    Back = 2,
    Front = 3,
    Down = 4,
    Up = 5,
};

#pragma endregion


struct D3DResourceLeakChecker
{
    ~D3DResourceLeakChecker()
    {
        // リソースリーク確認
        Microsoft::WRL::ComPtr <IDXGIDebug1> debug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
        {
            debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
            debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
            debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
        }
    }
};
