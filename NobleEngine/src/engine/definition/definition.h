#pragma once
#include <vector>
#include <string>
#include <cstdint>

#include <initguid.h>
#include <dxgidebug.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>     // DIK_〇系
#include <xaudio2.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")

#include <wrl.h>

#include "externals/DirectXTex/DirectXTex.h"


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


#define WIDTH 1280
#define HEIGHT 720
#define eps 1e-6f
#define CHUNK_X 8
#define CHUNK_Y 24
#define CHUNK_Z 8
#define BLOCK_SIZE 1.0f
#define PLAYER_SPEED 0.1f

#define GRAVITY -0.0061f

enum class CharacterID
{
    Player,
	Zombie,
    
    MAX,
};

// 全てのアイテムID
enum class ItemID
{
    None,

	木の剣,
	石の剣,
	鉄の剣,
	ダイヤの剣,

	木のツルハシ,
	石のツルハシ,
	鉄のツルハシ,
	ダイヤのツルハシ,

	木の斧,
	石の斧,
	鉄の斧,
	ダイヤの斧,

	鉄の頭,
	鉄の胴,
    鉄の脚,
	鉄の靴,

	ダイヤの頭,
	ダイヤの胴,
	ダイヤの脚,
	ダイヤの靴,

	作業台ブロック,
	棒,            

	ガラスブロック,
    葉ブロック,
    原木ブロック,
    木材ブロック,    // *
    芝ブロック,
    土ブロック,
	石ブロック,
	鉄ブロック,      // 鉱石状態
	金ブロック,      // 鉱石状態
	ダイヤブロック,  // 鉱石状態
	岩盤ブロック,

	鉄インゴット,
	金インゴット,
	ダイヤモンド,

    ビーコン,

    MAX,
};

enum class ItemGenre
{
    None,
	// 防具
	Head,
    Body,
	Leg,
	Boots,
	// ツール
	Axe,
    Pickel,
    Sword,
	// 木材系(オノで採掘速度アップ)
	Wood,
	// 鉱石系(ツルハシで採掘速度アップ)
	Stone,
	// 土系(シャベルで採掘速度アップ)
	Dirt,
    // 素材
	Material,
    MAX,
};


// 全てのブロックID
enum class BlockID
{
    Air,
    Stone,	// 石
    Iron,   // 鉄
    Gold,	// 金
    Diamond,// ダイヤ
    Bedrock,// 岩盤
    Glass,	// ガラス
    Lawn,	// 草付き土
	Log,    // 原木
	Planks, // 木材
    Dirt,	// 草なし土
 
    Leaf,	// 葉っぱ

	craftTable, // 作業台


    MAX,
};
std::string EnumToString(BlockID id);

// idからドロップするアイテムIDを取得
ItemID BlockIdToDropItemId(BlockID id);
// BlockID -> ItemIDの純粋変換
ItemID BlockIDToItemID(BlockID id);
// ItemID -> BlockIDの純粋変換
BlockID ItemIDToBlockID(ItemID id);
// アイテムIDからジャンルを取得
ItemGenre GetItemGenre(ItemID id);

// ブロックごとの情報
struct Blockinfo
{
	// ブロックID
	BlockID type = BlockID::Air;
    // アイテムジャンル
	ItemGenre genre = ItemGenre::None;
    // 右クリックされたとき特殊な動作をするかどうか(作業台は右クリックでUIを開く)
	bool isExtraAction = false;
	// 耐久値
	float durability = 1.0f;
	// 透過ブロックかどうか
	bool isTransparent = false;
};

// UIモード
enum class UIMode
{
	None,
    // 非表示
	Hidden,
	// プレイ中　(手持ちアイテムのみ表示)
	Playing,
	// インベントリ表示中
	Inventory,
	// クラフト画面
	Crafting,
    // ポーズ画面
	Pause,


    Max
};

// ゲームのフェーズ
enum class PHASE
{
    Phase_None,
    Phase_Test,
    Phase_Title,
    Phase_GameScene,
    Phase_StageSelect,
    Phase_GameClear,
};
std::string EnumToString(PHASE e);

#pragma region 演算

template <typename T>
constexpr const T& my_min(const T& a, const T& b)
{
    return (a < b) ? a : b;
}

template <typename T>
constexpr T my_min(std::initializer_list<T> list)
{
    auto it = list.begin();
    T result = *it;
    ++it;
    for (; it != list.end(); ++it)
    {
        if (*it < result)
            result = *it;
    }
    return result;
}


template <typename T>
constexpr const T& my_max(const T& a, const T& b)
{
    return (a > b) ? a : b;
}

template <typename T>
constexpr T my_max(std::initializer_list<T> list)
{
    auto it = list.begin();
    T result = *it;
    ++it;
    for (; it != list.end(); ++it)
    {
        if (*it > result)
            result = *it;
    }
    return result;
}

#pragma endregion


#pragma region 基盤構造体

struct Vector2int
{
    int x = 0;
    int y = 0;
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

struct Vector2
{
    float x = 0;
    float y = 0;

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

struct Vector3int
{
    int x = 0;
    int y = 0;
    int z = 0;
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

struct Quaternion;

struct Vector3
{
    float x = 0;
    float y = 0;
    float z = 0;

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

struct Vector4int
{
    int x = 0, y = 0, z = 0, w = 0;
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

struct Matrix3x3
{
    float m[3][3] = {
    { 0,0,0 },
    { 0,0,0 },
    { 0,0,0 } };

    Matrix3x3 operator+(const Matrix3x3& rhs) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.m[i][j] = m[i][j] + rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix3x3 operator-(const Matrix3x3& rhs) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.m[i][j] = m[i][j] - rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix3x3 operator*(float scalar) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }
    Matrix3x3 operator*(const Matrix3x3& rhs) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 3; ++k)
                {
                    result.m[i][j] += m[i][k] * rhs.m[k][j];
                }
            }
        }
        return result;
    }
    bool operator==(const Matrix3x3& rhs) const
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
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
    { 1,0,0,0 },
    { 0,1,0,0 },
    { 0,0,1,0 },
    { 0,0,0,1 } };

    Matrix4x4 operator+(const Matrix4x4& rhs) const
    {
        Matrix4x4 result;
        for (int j = 0; j < 4; ++j)
        {
            for (int i = 0; i < 4; ++i)
            {
                result.m[i][j] = m[i][j] + rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix4x4 operator-(const Matrix4x4& rhs) const
    {
        Matrix4x4 result;
        for (int j = 0; j < 4; ++j)
        {
            for (int i = 0; i < 4; ++i)
            {
                result.m[i][j] = m[i][j] - rhs.m[i][j];
            }
        }
        return result;
    }
    Matrix4x4 operator*(float scalar) const
    {
        Matrix4x4 result;
        for (int j = 0; j < 4; ++j)
        {
            for (int i = 0; i < 4; ++i)
            {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }
    Matrix4x4 operator*(const Matrix4x4& rhs) const
    {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k)
                {
                    result.m[i][j] += m[i][k] * rhs.m[k][j];
                }
            }
        }
        return result;
    }

    bool operator==(const Matrix4x4& rhs) const
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
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
	static Quaternion MakeFromEuler(const Vector3& euler);
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

// 軸平行境界ボックス(Axis Aligned Bounding Box)
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

enum class PrimitiveType
{
    // 球
    Sphere,
    // 楕円体
    SphereXYZ,
	// 円柱
	Cylinder,
    // 立方体
    AABB,
    // 四角形
    Plane,
    // 円
    Circle,
};
std::string EnumToString(PrimitiveType e);

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
std::string EnumToString(EaseType e);

struct EasingSetVector3
{
    Vector3 start;
    Vector3 end;
    bool easingFlag = 0;
    int flame = 0;
    int maxFrame = 0;
    EaseType easetype = EaseType::OUT_QUART;
};

struct EasingSetFloat
{
    float start;
    float end;
    bool easingFlag = 0;
    int flame = 0;
    int maxFrame = 0;
    EaseType easetype = EaseType::OUT_QUART;
};

#pragma endregion


// 変換情報
struct Transforms
{
    Vector3 scale = { 1,1,1 };
    Vector3 rotate = { 0,0,0 };
    Vector3 translate = { 0,0,0 };
};
struct VectorDynamics
{
    Vector3 value = { 1.0f,1.0f,1.0f };
    Vector3 velocity;
    Vector3 acceleration;
};

#pragma region モデルデータ構造体

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

// モデルデータ
struct ModelData
{
	// データ本体
    std::vector<VertexData> vertices;
    MaterialData material;

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    UINT vertexBufferSize = 0;

    // AABB
    std::vector<AABB> aabb;

    // 識別ナンバー
    int32_t number = 0;

    // ファイルパス
    std::string filePath;
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
	// SRVのGPUハンドル
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU{};
    
	// 識別ナンバー   
	int32_t number = -1;

	// ファイルパス
    std::string filePath;
};

#pragma endregion

#pragma region オーディオデータ構造体

// オーディオデータとソースボイスを保持する構造体
struct AudioData
{
	// データ本体
    std::vector<BYTE> audioData;
    UINT32 audioBytes = 0;

    WAVEFORMATEX* pWfx = nullptr;
    UINT32 wfxSize = 0;

    IXAudio2SourceVoice* pSourceVoice = nullptr;
    XAUDIO2_BUFFER xAudioBuffer = {};
};

#pragma endregion

struct TransformationMatrix
{
    Matrix4x4 WVP;
    Matrix4x4 World;
};


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

struct AtlasInfo
{
	// アトラスの列数(縦に伸ばすため1固定)
    uint32_t atlasCols = 1;
	// アトラスの行数
    uint32_t atlasRows = 1;

	// １面の幅(実際のブロックとパディングを含む)
    uint32_t faceStrideX = 24;
	// １面の高さ(実際のブロックとパディングを含む)
    uint32_t faceStrideY = 24;

	// 実際のブロックの幅
    uint32_t innerSizeX = 16;
    uint32_t innerSizeY = 16;

	// パディングの幅
    uint32_t padX = 4;
    uint32_t padY = 4;

    uint32_t facesPerBlock = 6;
    uint32_t padding0 = 0;

    Vector2 invAtlasSize = { 0.0f, 0.0f }; // 1/width, 1/height
    Vector2 padding1 = { 0.0f, 0.0f };
};

enum class BlendMode
{
    // ブレンドなし
    kBlendModeNone,
    // 通常アルファブレンド
    kBlendModeNormal,
    // 加算
    kBlendModeAdd,
    // 減算
    kBlendModeSub,
    // 乗算
    kBlendModeMul,
    // スクリーン
    kBlendModeScreen,
    // 使用したら殺す
    Wireframe
};
std::string EnumToString(BlendMode e);

enum class LightMode
{
    None = 0,
    Lambert,
    HalfLambert,
};
std::string EnumToString(LightMode e);


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

struct DirectionalLight
{
    Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector3 direction = { 0.0f, -1.0f, 0.0f };
    float intensity = 1.0f;//輝度
    LightMode mode = LightMode::HalfLambert;
    bool phong = false;
};

struct DrawOptions
{
    // ワイヤーフレーム
    bool wireframe = false;
    // 固有ライトを使うか共有ライトを使うか
    bool useOwnLight = false;
    // ライト
    DirectionalLight dirLight;
    // ブレンドモード
    BlendMode blendMode = BlendMode::kBlendModeNormal;
};

//struct Material
//{
//    Vector4 color;
//    Matrix4x4 uvTransform;
//    float shininess;
//};

struct Material
{
	Vector3 diffuseColor = { 1.0f, 1.0f, 1.0f };
	float  shininess = 1.0f;
	Vector3 specularColor = { 1.0f, 1.0f, 1.0f };
    float  _pad0;
};

// スプライトのアンカー位置
enum class Anchor
{
    Center,

    CenterLeft,
    CenterRight,
    CenterTop,
    CenterDown,

    LeftTop,
    RightTop,
    LeftDown,
    RightDown,
};
std::string EnumToString(Anchor e);

#pragma endregion


#pragma region 衝突判定構造体

enum class CollisionResult
{
    非衝突,
    接触,
    衝突
};
std::string EnumToString(CollisionResult e);

struct CollisionFlags
{
    enum
    {
        NONE = 0x00000000,
        FRONT = 0x00000001,
        BACK = 0x00000002,
        LEFT = 0x00000004,
        RIGHT = 0x00000008,
        TOP = 0x00000010,
        BOTTOM = 0x00000020,
        INSIDE = 0x00000040,
    };
};

enum class AABBFace
{
    NONE = -1,
    LEFT = 0,
    RIGHT = 1,
    BOTTOM = 2,
    TOP = 3,
    BACK = 4,
    FRONT = 5,
};
std::string EnumToString(AABBFace e);

struct CollisionPair
{
    // 軽い方
    int light = 0;
    // 重い方
    int heavy = 0;

    bool operator==(const CollisionPair& rhs) const
    {
        return light == rhs.light && heavy == rhs.heavy;
    }
    bool operator!=(const CollisionPair& rhs) const
    {
        return !(*this == rhs);
    }
};

struct CollisionAABBFace
{
    AABBFace light = AABBFace::NONE;
    AABBFace heavy = AABBFace::NONE;
};

struct CollisionInf
{
    CollisionPair IDpair;    // 衝突したオブジェクトの識別番号ペア
    CollisionPair AABBpair;  // 衝突したオブジェクトのAABBの番号ペア
    CollisionAABBFace face;  // 衝突したオブジェクトのAABBの衝突面ペア
    Vector3 depth;    // 浸入深度
};

#pragma endregion


#pragma region パーティクルデータ構造体

// パーティクル情報

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
    int particlesPerEmission = 1;   // 1フレで生む数
    int emissionDelay = 10;         // 生成間隔フレーム
    int liveMax = 300;              // 寿命フレーム(マイナスの時は不老)
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
std::string EnumToString(CameraMode_ORBIT_FPS e);

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
std::string EnumToString(DirectionXY e);

// 前後左右
enum class DirectionXZ
{
    None = -1,
	Left = 0,       // X-
	Right = 1,      // X+
	Back = 2,       // Z-
	Front = 3,      // Z+
};
std::string EnumToString(DirectionXZ e);

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
std::string EnumToString(DirectionXZ8Way e);

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
std::string EnumToString(DirectionXYZ e);

#pragma endregion

// GPU用カメラ構造体
struct CameraForGPU
{
    Vector3 worldPosition;
};

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

enum class LineType
{
    Line,
    BezierCurve,
    SplineCurve,
};
std::string EnumToString(LineType e);


class Block;
struct lookAtBlock
{
    Block* block = nullptr;
	Vector2int chunkIndex = { 0,0 };
	Vector3int localIndex = { 0,0,0 };
    AABBFace face = AABBFace::NONE;
	float distance = 0.0f;
};

class BaseCharacter;
struct RayHitResult
{
    enum class Type
    {
        None,
        Block,
        Character
    };

    Type type = Type::None;

    // type == Block のとき有効
    lookAtBlock blockHit{};

    // type == Character のとき有効
    BaseCharacter* Character = nullptr;

    // 共通：レイ原点からの距離
    float distance = 0.0f;
};

struct Rect
{
    int minX, minY;
    int maxX, maxY;
    bool empty = true;
};
