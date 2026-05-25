#include "definition.h"
#include<cmath>
#include<cstdint>
#include<numbers>

#pragma region Vector2

float Vector2::Length() const
{
    return std::sqrt(x * x + y * y);
}

float Vector2::LengthSq() const
{
    return x * x + y * y;
}

Vector2& Vector2::Normalize()
{
    float len = Length();
    const float EPSILON = 0.00001f;
    if (std::abs(len) > EPSILON)
    {
        x /= len;
        y /= len;
    }
    return *this;
}

Vector2 Vector2::Normalized() const
{
    Vector2 result = *this;
    result.Normalize();
    return result;
}

float Vector2::Dot(const Vector2& rhs) const
{
    return x * rhs.x + y * rhs.y;
}

#pragma endregion

#pragma region Vector3

float Vector3::Length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

float Vector3::LengthSq() const
{
    return x * x + y * y + z * z;
}

Vector3& Vector3::Normalize()
{
    float len = Length();
    if (std::abs(len) > 0.00001f)
    {
        x /= len;
        y /= len;
        z /= len;
    }
    return *this;
}

Vector3 Vector3::Normalized() const
{
    Vector3 result = *this;
    result.Normalize();
    return result;
}

float Vector3::Dot(const Vector3& rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vector3 Vector3::Cross(const Vector3& rhs) const
{
    return Vector3(
        y * rhs.z - z * rhs.y,
        z * rhs.x - x * rhs.z,
        x * rhs.y - y * rhs.x
    );
}

Vector3 Vector3::Reflect(const Vector3& input, const Vector3& normal)
{
    Vector3 result;

    result = input - ((normal * (input.Dot(normal))) * 2);

    return result;
}

Vector3 Vector3::RotateByQuaternion(const Quaternion& q) const
{
    Vector3 qv(q.x, q.y, q.z);
    Vector3 t = qv.Cross(*this) * 2.0f;
	return *this + t * q.w + qv.Cross(t);
}

#pragma endregion

#pragma region Vector4

float Vector4::Length() const
{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

float Vector4::LengthSq() const
{
    return x * x + y * y + z * z + w * w;
}

Vector4& Vector4::Normalize()
{
    float len = Length();
    const float EPSILON = 0.00001f;
    if (std::abs(len) > EPSILON)
    {
        x /= len;
        y /= len;
        z /= len;
        w /= len;
    }
    return *this;
}

Vector4 Vector4::Normalized() const
{
    Vector4 result = *this;
    result.Normalize();
    return result;
}

float Vector4::Dot(const Vector4& rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

#pragma endregion

#pragma region Matrix4x4

Matrix4x4 Matrix4x4::Inverse() const
{
    Matrix4x4 Return{};

    float det = (m[0][0] * m[1][1] * m[2][2] * m[3][3]) + (m[0][0] * m[1][2] * m[2][3] * m[3][1]) + (m[0][0] * m[1][3] * m[2][1] * m[3][2])
        - (m[0][0] * m[1][3] * m[2][2] * m[3][1]) - (m[0][0] * m[1][2] * m[2][1] * m[3][3]) - (m[0][0] * m[1][1] * m[2][3] * m[3][2])
        - (m[0][1] * m[1][0] * m[2][2] * m[3][3]) - (m[0][2] * m[1][0] * m[2][3] * m[3][1]) - (m[0][3] * m[1][0] * m[2][1] * m[3][2])
        + (m[0][3] * m[1][0] * m[2][2] * m[3][1]) + (m[0][2] * m[1][0] * m[2][1] * m[3][3]) + (m[0][1] * m[1][0] * m[2][3] * m[3][2])
        + (m[0][1] * m[1][2] * m[2][0] * m[3][3]) + (m[0][2] * m[1][3] * m[2][0] * m[3][1]) + (m[0][3] * m[1][1] * m[2][0] * m[3][2])
        - (m[0][3] * m[1][2] * m[2][0] * m[3][1]) - (m[0][2] * m[1][1] * m[2][0] * m[3][3]) - (m[0][1] * m[1][3] * m[2][0] * m[3][2])
        - (m[0][1] * m[1][2] * m[2][3] * m[3][0]) - (m[0][2] * m[1][3] * m[2][1] * m[3][0]) - (m[0][3] * m[1][1] * m[2][2] * m[3][0])
        + (m[0][3] * m[1][2] * m[2][1] * m[3][0]) + (m[0][2] * m[1][1] * m[2][3] * m[3][0]) + (m[0][1] * m[1][3] * m[2][2] * m[3][0]);

    // Protect against near-zero determinant (use absolute value)
    if (std::fabs(det) < eps) return Return;

    const float invDet = 1.0f / det;

    Return.m[0][0] = invDet * ((m[1][1] * m[2][2] * m[3][3]) + (m[1][2] * m[2][3] * m[3][1]) + (m[1][3] * m[2][1] * m[3][2]) - (m[1][3] * m[2][2] * m[3][1]) - (m[1][2] * m[2][1] * m[3][3]) - (m[1][1] * m[2][3] * m[3][2]));
    Return.m[0][1] = invDet * ((m[0][3] * m[2][2] * m[3][1]) + (m[0][2] * m[2][1] * m[3][3]) + (m[0][1] * m[2][3] * m[3][2]) - (m[0][1] * m[2][2] * m[3][3]) - (m[0][2] * m[2][3] * m[3][1]) - (m[0][3] * m[2][1] * m[3][2]));
    Return.m[0][2] = invDet * ((m[0][1] * m[1][2] * m[3][3]) + (m[0][2] * m[1][3] * m[3][1]) + (m[0][3] * m[1][1] * m[3][2]) - (m[0][3] * m[1][2] * m[3][1]) - (m[0][2] * m[1][1] * m[3][3]) - (m[0][1] * m[1][3] * m[3][2]));
    Return.m[0][3] = invDet * ((m[0][3] * m[1][2] * m[2][1]) + (m[0][2] * m[1][1] * m[2][3]) + (m[0][1] * m[1][3] * m[2][2]) - (m[0][1] * m[1][2] * m[2][3]) - (m[0][2] * m[1][3] * m[2][1]) - (m[0][3] * m[1][1] * m[2][2]));

    Return.m[1][0] = invDet * ((m[1][3] * m[2][2] * m[3][0]) + (m[1][2] * m[2][0] * m[3][3]) + (m[1][0] * m[2][3] * m[3][2]) - (m[1][0] * m[2][2] * m[3][3]) - (m[1][2] * m[2][3] * m[3][0]) - (m[1][3] * m[2][0] * m[3][2]));
    Return.m[1][1] = invDet * ((m[0][0] * m[2][2] * m[3][3]) + (m[0][2] * m[2][3] * m[3][0]) + (m[0][3] * m[2][0] * m[3][2]) - (m[0][3] * m[2][2] * m[3][0]) - (m[0][2] * m[2][0] * m[3][3]) - (m[0][0] * m[2][3] * m[3][2]));
    Return.m[1][2] = invDet * ((m[0][3] * m[1][2] * m[3][0]) + (m[0][2] * m[1][0] * m[3][3]) + (m[0][0] * m[1][3] * m[3][2]) - (m[0][0] * m[1][2] * m[3][3]) - (m[0][2] * m[1][3] * m[3][0]) - (m[0][3] * m[1][0] * m[3][2]));
    Return.m[1][3] = invDet * ((m[0][0] * m[1][2] * m[2][3]) + (m[0][2] * m[1][3] * m[2][0]) + (m[0][3] * m[1][0] * m[2][2]) - (m[0][3] * m[1][2] * m[2][0]) - (m[0][2] * m[1][0] * m[2][3]) - (m[0][0] * m[1][3] * m[2][2]));

    Return.m[2][0] = invDet * ((m[1][0] * m[2][1] * m[3][3]) + (m[1][1] * m[2][3] * m[3][0]) + (m[1][3] * m[2][0] * m[3][1]) - (m[1][3] * m[2][1] * m[3][0]) - (m[1][1] * m[2][0] * m[3][3]) - (m[1][0] * m[2][3] * m[3][1]));
    Return.m[2][1] = invDet * ((m[0][3] * m[2][1] * m[3][0]) + (m[0][1] * m[2][0] * m[3][3]) + (m[0][0] * m[2][3] * m[3][1]) - (m[0][0] * m[2][1] * m[3][3]) - (m[0][1] * m[2][3] * m[3][0]) - (m[0][3] * m[2][0] * m[3][1]));
    Return.m[2][2] = invDet * ((m[0][0] * m[1][1] * m[3][3]) + (m[0][1] * m[1][3] * m[3][0]) + (m[0][3] * m[1][0] * m[3][1]) - (m[0][3] * m[1][1] * m[3][0]) - (m[0][1] * m[1][0] * m[3][3]) - (m[0][0] * m[1][3] * m[3][1]));
    Return.m[2][3] = invDet * ((m[0][3] * m[1][1] * m[2][0]) + (m[0][1] * m[1][0] * m[2][3]) + (m[0][0] * m[1][3] * m[2][1]) - (m[0][0] * m[1][1] * m[2][3]) - (m[0][1] * m[1][3] * m[2][0]) - (m[0][3] * m[1][0] * m[2][1]));

    Return.m[3][0] = invDet * ((m[1][2] * m[2][1] * m[3][0]) + (m[1][1] * m[2][0] * m[3][2]) + (m[1][0] * m[2][2] * m[3][1]) - (m[1][0] * m[2][1] * m[3][2]) - (m[1][1] * m[2][2] * m[3][0]) - (m[1][2] * m[2][0] * m[3][1]));
    Return.m[3][1] = invDet * ((m[0][0] * m[2][1] * m[3][2]) + (m[0][1] * m[2][2] * m[3][0]) + (m[0][2] * m[2][0] * m[3][1]) - (m[0][2] * m[2][1] * m[3][0]) - (m[0][1] * m[2][0] * m[3][2]) - (m[0][0] * m[2][2] * m[3][1]));
    Return.m[3][2] = invDet * ((m[0][2] * m[1][1] * m[3][0]) + (m[0][1] * m[1][0] * m[3][2]) + (m[0][0] * m[1][2] * m[3][1]) - (m[0][0] * m[1][1] * m[3][2]) - (m[0][1] * m[1][2] * m[3][0]) - (m[0][2] * m[1][0] * m[3][1]));
    Return.m[3][3] = invDet * ((m[0][0] * m[1][1] * m[2][2]) + (m[0][1] * m[1][2] * m[2][0]) + (m[0][2] * m[1][0] * m[2][1]) - (m[0][2] * m[1][1] * m[2][0]) - (m[0][1] * m[1][0] * m[2][2]) - (m[0][0] * m[1][2] * m[2][1]));

    return Return;
}

Matrix4x4 Matrix4x4::Transpose() const
{
    Matrix4x4 Return{};

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            Return.m[i][j] = m[j][i];
        }
    }

    return Return;
}

Matrix4x4 Matrix4x4::MakeIdentity4x4()
{
    Matrix4x4 Return{};

    for (int i = 0; i < 4; ++i)
    {
        Return.m[i][i] = 1.0f;
    }

    return Return;
}

Matrix4x4 Matrix4x4::MakeTranslateMatrix(const Vector3& translate)
{
    Matrix4x4 result = MakeIdentity4x4();
    result.m[3][0] = translate.x;
    result.m[3][1] = translate.y;
    result.m[3][2] = translate.z;
    return result;
}

Matrix4x4 Matrix4x4::MakeScaleMatrix(const Vector3& scale)
{
    Matrix4x4 result = MakeIdentity4x4();
    result.m[0][0] = scale.x;
    result.m[1][1] = scale.y;
    result.m[2][2] = scale.z;

    return result;
}

Matrix4x4 Matrix4x4::MakeRotateXMatrix(float radian)
{
    Matrix4x4 Return = MakeIdentity4x4();

    Return.m[1][1] = std::cos(radian);
    Return.m[2][1] = -std::sin(radian);
    Return.m[1][2] = std::sin(radian);
    Return.m[2][2] = std::cos(radian);

    return Return;
}

Matrix4x4 Matrix4x4::MakeRotateYMatrix(float radian)
{
    Matrix4x4 Return = MakeIdentity4x4();

    Return.m[0][0] = std::cos(radian);
    Return.m[2][0] = std::sin(radian);
    Return.m[0][2] = -std::sin(radian);
    Return.m[2][2] = std::cos(radian);

    return Return;
}

Matrix4x4 Matrix4x4::MakeRotateZMatrix(float radian)
{
    Matrix4x4 Return = MakeIdentity4x4();

    Return.m[0][0] = std::cos(radian);
    Return.m[1][0] = -std::sin(radian);
    Return.m[0][1] = std::sin(radian);
    Return.m[1][1] = std::cos(radian);

    return Return;
}

Matrix4x4 Matrix4x4::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate)
{
    Matrix4x4 Return{};

    Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
    Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
    Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
    Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
    Matrix4x4 rotateXYZMatrix = rotateZMatrix * rotateXMatrix * rotateYMatrix;
    Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
    Matrix4x4 resultMatrix = scaleMatrix * rotateXYZMatrix * translateMatrix;

    return resultMatrix;
}

Matrix4x4 Matrix4x4::MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate)
{
    Matrix4x4 Return{};

    Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
    Matrix4x4 rotateMatrix = rotate.MakeRotateMatrix();
    Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
    Matrix4x4 resultMatrix = scaleMatrix * rotateMatrix * translateMatrix;

    return resultMatrix;
}

Matrix4x4 Matrix4x4::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip)
{
    Matrix4x4 Return{};

    float tanHalfFovY = std::tan(fovY * 0.5f);
    Return.m[0][0] = 1.0f / (aspectRatio * tanHalfFovY);
    Return.m[1][1] = 1.0f / tanHalfFovY;
    Return.m[2][2] = farClip / (farClip - nearClip);
    Return.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
    Return.m[2][3] = 1.0f;
    Return.m[3][3] = 0.0f;

    return Return;
}

Matrix4x4 Matrix4x4::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip)
{
    Matrix4x4 Return{};

    Return.m[0][0] = 2.0f / (right - left);
    Return.m[0][1] = 0.0f;
    Return.m[0][2] = 0.0f;
    Return.m[0][3] = 0.0f;

    Return.m[1][0] = 0.0f;
    Return.m[1][1] = 2.0f / (top - bottom);
    Return.m[1][2] = 0.0f;
    Return.m[1][3] = 0.0f;

    Return.m[2][0] = 0.0f;
    Return.m[2][1] = 0.0f;
    Return.m[2][2] = 1.0f / (farClip - nearClip);
    Return.m[2][3] = 0.0f;

    Return.m[3][0] = (left + right) / (left - right);
    Return.m[3][1] = (top + bottom) / (bottom - top);
    Return.m[3][2] = (nearClip) / (nearClip - farClip);
    Return.m[3][3] = 1.0f;

    return Return;
}

Matrix4x4 Matrix4x4::MakeViewPortMatrix(float left, float top, float width, float height, float minD, float maxD)
{
    Matrix4x4 Return{};
    //// 最小深度値
    //float minD = 0;
    //// 最大深度値
    //float maxD = 1;


    Return.m[0][0] = width / 2.0f;
    Return.m[0][1] = 0.0f;
    Return.m[0][2] = 0.0f;
    Return.m[0][3] = 0.0f;

    Return.m[1][0] = 0.0f;
    Return.m[1][1] = -height / 2.0f;
    Return.m[1][2] = 0.0f;
    Return.m[1][3] = 0.0f;

    Return.m[2][0] = 0.0f;
    Return.m[2][1] = 0.0f;
    Return.m[2][2] = maxD - minD;
    Return.m[2][3] = 0.0f;

    Return.m[3][0] = left + (width / 2.0f);
    Return.m[3][1] = top + (height / 2.0f);
    Return.m[3][2] = minD;
    Return.m[3][3] = 1.0f;


    return Return;
}

Matrix4x4 Matrix4x4::MakeRotateAxisMatrix(const Vector3& axis, float radian)
{
    Matrix4x4 Return = MakeIdentity4x4();
    float cosTheta = std::cos(radian);
    float sinTheta = std::sin(radian);
    float oneSubCosTheta = 1.0f - cosTheta;
    Return.m[0][0] = axis.x * axis.x * oneSubCosTheta + cosTheta;
    Return.m[1][0] = axis.x * axis.y * oneSubCosTheta - axis.z * sinTheta;
    Return.m[2][0] = axis.x * axis.z * oneSubCosTheta + axis.y * sinTheta;
    Return.m[0][1] = axis.y * axis.x * oneSubCosTheta + axis.z * sinTheta;
    Return.m[1][1] = axis.y * axis.y * oneSubCosTheta + cosTheta;
    Return.m[2][1] = axis.y * axis.z * oneSubCosTheta - axis.x * sinTheta;
    Return.m[0][2] = axis.z * axis.x * oneSubCosTheta - axis.y * sinTheta;
    Return.m[1][2] = axis.z * axis.y * oneSubCosTheta + axis.x * sinTheta;
    Return.m[2][2] = axis.z * axis.z * oneSubCosTheta + cosTheta;
	return Return;
}

Matrix4x4 Matrix4x4::DirectionToDirectionMatrix(const Vector3& from, const Vector3& to)
{
    Vector3 fromNormalized = from.Normalized();
    Vector3 toNormalized = to.Normalized();
	float dot = fromNormalized.Dot(toNormalized);

    // ほぼ同じ方向
    if (dot > 1.0f - eps)
    {
        return MakeIdentity4x4();
    }

    // ほぼ逆方向
    if (dot < -1.0f + eps)
    {
        // from に直交する任意のベクトル
        Vector3 n;
        if (std::abs(fromNormalized.x) > eps || std::abs(fromNormalized.y) > eps)
        {
            n = Vector3(fromNormalized.y, -fromNormalized.x, 0.0f);
        }
        else
        {
            n = Vector3(fromNormalized.z, 0.0f, -fromNormalized.x);
        }

        Vector3 axis = n.Normalized();
        return MakeRotateAxisMatrix(axis, std::numbers::pi_v<float>);
    }

    Vector3 axis = fromNormalized.Cross(toNormalized);
    float angle = std::acos(dot);

    axis.Normalize();
	return MakeRotateAxisMatrix(axis, angle);
}

Matrix4x4 Matrix4x4::LookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up)
{
    Vector3 z = (target - eye).Normalized(); // forward
    Vector3 x = up.Cross(z).Normalized();    // right
    Vector3 y = z.Cross(x);                  // up

    Matrix4x4 m;

    // 回転部分（行ベクトル）
    m.m[0][0] = x.x; m.m[0][1] = x.y; m.m[0][2] = x.z;
    m.m[1][0] = y.x; m.m[1][1] = y.y; m.m[1][2] = y.z;
    m.m[2][0] = z.x; m.m[2][1] = z.y; m.m[2][2] = z.z;

    m.m[3][0] = -x.Dot(eye);
    m.m[3][1] = -y.Dot(eye);
    m.m[3][2] = -z.Dot(eye);

    return m;
}


#pragma endregion

#pragma region Quaternion

Quaternion Quaternion::MakeIdentityQuaternion()
{
	return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion Quaternion::MakeConjugateQuaternion() const
{
	return Quaternion(-x, -y, -z, w);
}

float Quaternion::Dot(const Quaternion& rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

float Quaternion::Norm() const
{
	return std::sqrt(x * x + y * y + z * z + w * w);
}

Quaternion Quaternion::Normalize() const
{
	float norm = Norm();
    if (std::abs(norm) > eps)
    {
        return Quaternion(
            x / norm,
            y / norm,
            z / norm,
            w / norm
        );
    }
	return Quaternion();
}

Quaternion Quaternion::Inverse() const
{
    const float normSq = x * x + y * y + z * z + w * w;
    if (std::abs(normSq) > eps)
    {
        const Quaternion conjugate = this->MakeConjugateQuaternion();
        return Quaternion(
            conjugate.x / normSq,
            conjugate.y / normSq,
            conjugate.z / normSq,
            conjugate.w / normSq
        );
    }
    return Quaternion();
}

Quaternion Quaternion::MakeRotateAxisAngleQuaternion(const Vector3& axis, float radian)
{
    Vector3 normAxis = axis.Normalized();
    float halfRadian = radian / 2.0f;
    float sinHalfRadian = std::sin(halfRadian);
    return Quaternion(
        normAxis.x * sinHalfRadian,
        normAxis.y * sinHalfRadian,
        normAxis.z * sinHalfRadian,
        std::cos(halfRadian)
	);
}

Matrix4x4 Quaternion::MakeRotateMatrix() const
{
    Matrix4x4 result = Matrix4x4::MakeIdentity4x4();
    result.m[0][0] = 1.0f - 2.0f * (y * y + z * z);
    result.m[0][1] = 2.0f * (x * y - z * w);
    result.m[0][2] = 2.0f * (x * z + y * w);
    result.m[1][0] = 2.0f * (x * y + z * w);
    result.m[1][1] = 1.0f - 2.0f * (x * x + z * z);
    result.m[1][2] = 2.0f * (y * z - x * w);
    result.m[2][0] = 2.0f * (x * z - y * w);
    result.m[2][1] = 2.0f * (y * z + x * w);
    result.m[2][2] = 1.0f - 2.0f * (x * x + y * y);
    return result;
}

Vector3 Quaternion::ToEuler() const
{
    Vector3 euler;

    // --- Pitch (X軸) ---
    float sinp = 2.0f * (w * x - y * z);
    if (std::abs(sinp) >= 1.0f)
        euler.x = std::copysign(3.1415926535f / 2.0f, sinp); // ±90°
    else
        euler.x = std::asin(sinp);

    // --- Yaw (Y軸) ---
    float siny_cosp = 2.0f * (w * y + z * x);
    float cosy_cosp = 1.0f - 2.0f * (x * x + y * y);
    euler.y = std::atan2(siny_cosp, cosy_cosp);

    // --- Roll (Z軸) ---
    float sinr_cosp = 2.0f * (w * z + x * y);
    float cosr_cosp = 1.0f - 2.0f * (y * y + z * z);
    euler.z = std::atan2(sinr_cosp, cosr_cosp);

    return euler;
}

Quaternion Quaternion::MakeFromEuler(const Vector3& euler)
{
    float cy = std::cos(euler.y * 0.5f);
    float sy = std::sin(euler.y * 0.5f);
    float cp = std::cos(euler.x * 0.5f);
    float sp = std::sin(euler.x * 0.5f);
    float cr = std::cos(euler.z * 0.5f);
    float sr = std::sin(euler.z * 0.5f);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}

Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
{
    // 内積を取る
    float dot = a.Dot(b);

    // 反転（最短経路補正）
    Quaternion b2 = b;
    if (dot < 0.0f)
    {
        dot = -dot;
        b2 = Quaternion(-b.x, -b.y, -b.z, -b.w);
    }

    // しきい値以下なら Lerp で十分（精度と速度のため）
    const float threshold = 0.9995f;
    if (dot > threshold)
    {
        Quaternion result(
            a.x + t * (b2.x - a.x),
            a.y + t * (b2.y - a.y),
            a.z + t * (b2.z - a.z),
            a.w + t * (b2.w - a.w)
        );
        return result.Normalize();
    }

    // Slerp 本体
    float theta = std::acos(dot);
    float sinTheta = std::sin(theta);

    float w1 = std::sin((1.0f - t) * theta) / sinTheta;
    float w2 = std::sin(t * theta) / sinTheta;

    return Quaternion(
        a.x * w1 + b2.x * w2,
        a.y * w1 + b2.y * w2,
        a.z * w1 + b2.z * w2,
        a.w * w1 + b2.w * w2
    );
}



#pragma endregion

#pragma region AABB

Vector3 AABB::center()const
{
    return Vector3{
        (min.x + max.x) / 2.0f,
        (min.y + max.y) / 2.0f,
        (min.z + max.z) / 2.0f,
    };
}

void AABB::Move(const Vector3& move)
{
    this->min = this->min + move;
	this->max = this->max + move;
}

void AABB::Fix()
{
    AABB aabb = *this;
    this->min.x = my_min(aabb.min.x, aabb.max.x);
    this->max.x = my_max(aabb.min.x, aabb.max.x);
    this->min.y = my_min(aabb.min.y, aabb.max.y);
    this->max.y = my_max(aabb.min.y, aabb.max.y);
    this->min.z = my_min(aabb.min.z, aabb.max.z);
    this->max.z = my_max(aabb.min.z, aabb.max.z);
};

Vector3 AABB::GetCollisionDepth(const AABB& other)const
{
    Vector3 depth;
    depth.x = my_min(this->max.x, other.max.x) - my_max(this->min.x, other.min.x);
    depth.y = my_min(this->max.y, other.max.y) - my_max(this->min.y, other.min.y);
    depth.z = my_min(this->max.z, other.max.z) - my_max(this->min.z, other.min.z);
    if (depth.x <= 0.0f || depth.y <= 0.0f || depth.z <= 0.0f)
    {
        return { 0.0f, 0.0f, 0.0f };
    }
    return depth;
}

#pragma endregion


ItemGenre GetItemGenre(ItemID id)
{
    switch (id)
    {
    case ItemID::木の剣:			return ItemGenre::Sword;   break;
    case ItemID::石の剣:			return ItemGenre::Sword;   break;
    case ItemID::鉄の剣:			return ItemGenre::Sword;   break;
    case ItemID::ダイヤの剣:		return ItemGenre::Sword;   break;

    case ItemID::木のツルハシ:		return ItemGenre::Pickel;  break;
    case ItemID::石のツルハシ:		return ItemGenre::Pickel;  break;
    case ItemID::鉄のツルハシ:		return ItemGenre::Pickel;  break;
    case ItemID::ダイヤのツルハシ:	return ItemGenre::Pickel;  break;

    case ItemID::木の斧:			return ItemGenre::Axe;     break;
    case ItemID::石の斧:			return ItemGenre::Axe;     break;
    case ItemID::鉄の斧:			return ItemGenre::Axe;     break;
    case ItemID::ダイヤの斧:		return ItemGenre::Axe;     break;

    case ItemID::鉄の頭:            return ItemGenre::Head;    break;
    case ItemID::鉄の胴:            return ItemGenre::Body;    break;
    case ItemID::鉄の脚:            return ItemGenre::Leg;     break;
    case ItemID::鉄の靴:            return ItemGenre::Boots;   break;

    case ItemID::ダイヤの頭:        return ItemGenre::Head;    break;
    case ItemID::ダイヤの胴:        return ItemGenre::Body;    break;
    case ItemID::ダイヤの脚:        return ItemGenre::Leg;     break;
    case ItemID::ダイヤの靴:        return ItemGenre::Boots;   break;

    case ItemID::芝ブロック:		return ItemGenre::Dirt;    break;
    case ItemID::土ブロック:		return ItemGenre::Dirt;    break;

    case ItemID::葉ブロック:		return ItemGenre::Wood;    break;
    case ItemID::原木ブロック:		return ItemGenre::Wood;    break;
	case ItemID::木材ブロック:		return ItemGenre::Wood;    break;
	case ItemID::作業台ブロック:	return ItemGenre::Wood;    break;

    case ItemID::ガラスブロック:	return ItemGenre::Stone;    break;
    case ItemID::石ブロック:		return ItemGenre::Stone;    break;
    case ItemID::鉄ブロック:		return ItemGenre::Stone;    break;
    case ItemID::金ブロック:		return ItemGenre::Stone;    break;
    case ItemID::ダイヤブロック:	return ItemGenre::Stone;    break;

    case ItemID::鉄インゴット:		return ItemGenre::Material; break;
    case ItemID::金インゴット:		return ItemGenre::Material; break;
    case ItemID::ダイヤモンド:		return ItemGenre::Material; break;
	case ItemID::棒:     			return ItemGenre::Material; break;

    case ItemID::岩盤ブロック:		return ItemGenre::None;    break;
    case ItemID::ビーコン:			return ItemGenre::None;    break;

    default:
		return ItemGenre::None;
        break;
    }
}

std::string EnumToString(BlockID id)
{
    switch (id)
    {
    case BlockID::Air:
        return "Air";
        break;
    case BlockID::Stone:
        return "Stone";
        break;
    case BlockID::Glass:
        return "Glass";
        break;
    case BlockID::Dirt:
        return "Dirt";
        break;
    case BlockID::Lawn:
        return "Lawn";
        break;
    case BlockID::Log:
        return "Log";
        break;
    case BlockID::Leaf:
        return "Leaf";
        break;
    case BlockID::MAX:
    default:
        return "Unknown";
        break;
    }
}

ItemID BlockIdToDropItemId(BlockID id)
{
    switch (id)
    {
    case BlockID::Air:          return ItemID::None;
    case BlockID::Stone:        return ItemID::石ブロック;
    case BlockID::Iron:         return ItemID::鉄インゴット;
    case BlockID::Gold:         return ItemID::金インゴット;
    case BlockID::Diamond:      return ItemID::ダイヤモンド;
    case BlockID::Dirt:         return ItemID::土ブロック;
    case BlockID::Lawn:         return ItemID::土ブロック;
    case BlockID::Log:          return ItemID::原木ブロック;
    case BlockID::Leaf:         return ItemID::葉ブロック;
    case BlockID::Bedrock:      return ItemID::岩盤ブロック;
    case BlockID::Glass:        return ItemID::ガラスブロック;
    case BlockID::craftTable:   return ItemID::作業台ブロック;
	case BlockID::Planks:       return ItemID::木材ブロック;

    default:               return ItemID::None;
    }
}

ItemID BlockIDToItemID(BlockID id)
{
    switch (id)
    {
    case BlockID::Air:          return ItemID::None;
    case BlockID::Stone:        return ItemID::石ブロック;
    case BlockID::Iron:         return ItemID::鉄ブロック;
    case BlockID::Gold:         return ItemID::金ブロック;
    case BlockID::Diamond:      return ItemID::ダイヤブロック;
    case BlockID::Dirt:         return ItemID::土ブロック;
    case BlockID::Lawn:         return ItemID::芝ブロック;
    case BlockID::Log:          return ItemID::原木ブロック;
    case BlockID::Leaf:         return ItemID::葉ブロック;
    case BlockID::Bedrock:      return ItemID::岩盤ブロック;
    case BlockID::Glass:        return ItemID::ガラスブロック;
    case BlockID::craftTable:   return ItemID::作業台ブロック;
    case BlockID::Planks:       return ItemID::木材ブロック;
    default:                       return ItemID::None;
	}

}

BlockID ItemIDToBlockID(ItemID id)
{
    switch (id)
    {
    case ItemID::作業台ブロック: return BlockID::craftTable; break;
    case ItemID::ガラスブロック: return BlockID::Glass; break;
    case ItemID::葉ブロック:     return BlockID::Leaf; break;
    case ItemID::原木ブロック:    return BlockID::Log; break;
    case ItemID::木材ブロック:    return BlockID::Planks; break;
    case ItemID::芝ブロック:     return BlockID::Lawn; break;
    case ItemID::土ブロック:     return BlockID::Dirt; break;
    case ItemID::石ブロック:     return BlockID::Stone; break;
    case ItemID::鉄ブロック:     return BlockID::Iron; break;
    case ItemID::金ブロック:     return BlockID::Gold; break;
    case ItemID::ダイヤブロック: return BlockID::Diamond; break;
    case ItemID::岩盤ブロック:   return BlockID::Bedrock; break;
    case ItemID::ビーコン:      return BlockID::Air; break;
    case ItemID::MAX:           return BlockID::Air; break;
    default: return BlockID::Air; break;
    }
}

std::string EnumToString(PHASE e)
{
    switch (e)
    {
    case PHASE::Phase_None:
        return "Phase_None";
    case PHASE::Phase_Test:
        return "Phase_Test";
    case PHASE::Phase_Title:
        return "Phase_Title";
    case PHASE::Phase_GameScene:
        return "Phase_GameScene";
    case PHASE::Phase_StageSelect:
        return "Phase_StageSelect";
    case PHASE::Phase_GameClear:
        return "Phase_GameClear";
    default:
        return "Unknown";
    }
}

std::string EnumToString(PrimitiveType e)
{
    switch (e)
    {
    case PrimitiveType::Sphere:
        return "Sphere";
    case PrimitiveType::SphereXYZ:
        return "SphereXYZ";
    case PrimitiveType::AABB:
        return "AABB";
    case PrimitiveType::Plane:
        return "Plane";
    case PrimitiveType::Circle:
        return "Circle";
    default:
        return "Unknown";
    }
}

std::string EnumToString(EaseType e)
{
    switch (e)
    {
    case EaseType::LINEAR:
        return "LINEAR";
    case EaseType::IN_SINE:
        return "IN_SINE";
    case EaseType::OUT_SINE:
        return "OUT_SINE";
    case EaseType::IN_OUT_SINE:
        return "IN_OUT_SINE";
    case EaseType::IN_QUAD:
        return "IN_QUAD";
    case EaseType::OUT_QUAD:
        return "OUT_QUAD";
    case EaseType::IN_OUT_QUAD:
        return "IN_OUT_QUAD";
    case EaseType::IN_CUBIC:
        return "IN_CUBIC";
    case EaseType::OUT_CUBIC:
        return "OUT_CUBIC";
    case EaseType::IN_OUT_CUBIC:
        return "IN_OUT_CUBIC";
    case EaseType::IN_QUART:
        return "IN_QUART";
    case EaseType::OUT_QUART:
        return "OUT_QUART";
    case EaseType::IN_OUT_QUART:
        return "IN_OUT_QUART";
    case EaseType::IN_QUINT:
        return "IN_QUINT";
    case EaseType::OUT_QUINT:
        return "OUT_QUINT";
    case EaseType::IN_OUT_QUINT:
        return "IN_OUT_QUINT";
    case EaseType::IN_EXPO:
        return "IN_EXPO";
    case EaseType::OUT_EXPO:
        return "OUT_EXPO";
    case EaseType::IN_OUT_EXPO:
        return "IN_OUT_EXPO";
    case EaseType::IN_CIRC:
        return "IN_CIRC";
    case EaseType::OUT_CIRC:
        return "OUT_CIRC";
    case EaseType::IN_OUT_CIRC:
        return "IN_OUT_CIRC";
    case EaseType::IN_BACK:
        return "IN_BACK";
    case EaseType::OUT_BACK:
        return "OUT_BACK";
    case EaseType::IN_OUT_BACK:
        return "IN_OUT_BACK";
    case EaseType::IN_ELASTIC:
        return "IN_ELASTIC";
    case EaseType::OUT_ELASTIC:
        return "OUT_ELASTIC";
    case EaseType::IN_OUT_ELASTIC:
        return "IN_OUT_ELASTIC";
    case EaseType::IN_BOUNCE:
        return "IN_BOUNCE";
    case EaseType::OUT_BOUNCE:
        return "OUT_BOUNCE";
    default:
        return "Unknown";
    }
}

std::string EnumToString(BlendMode e)
{
    switch (e)
    {
    case BlendMode::kBlendModeNone:
        return "kBlendModeNone";
    case BlendMode::kBlendModeNormal:
        return "kBlendModeNormal";
    case BlendMode::kBlendModeAdd:
        return "kBlendModeAdd";
    case BlendMode::kBlendModeSub:
        return "kBlendModeSub";
    case BlendMode::kBlendModeMul:
        return "kBlendModeMul";
    case BlendMode::kBlendModeScreen:
        return "kBlendModeScreen";
    case BlendMode::Wireframe:
        return "Wireframe";
    default:
        return "Unknown";
    }
}

std::string EnumToString(LightMode e)
{
    switch (e)
    {
    case LightMode::None:
        return "None";
    case LightMode::Lambert:
        return "Lambert";
    case LightMode::HalfLambert:
        return "HalfLambert";
    default:
        return "Unknown";
    }
}

std::string EnumToString(Anchor e)
{
    switch (e)
    {
    case Anchor::Center:
        return "Center";
    case Anchor::CenterLeft:
        return "CenterLeft";
    case Anchor::CenterRight:
        return "CenterRight";
    case Anchor::CenterTop:
        return "CenterTop";
    case Anchor::CenterDown:
        return "CenterDown";
    case Anchor::LeftTop:
        return "LeftTop";
    case Anchor::RightTop:
        return "RightTop";
    case Anchor::LeftDown:
        return "LeftDown";
    case Anchor::RightDown:
        return "RightDown";
    default:
        return "Unknown";
    }
}

std::string EnumToString(CollisionResult e)
{
    switch (e)
    {
    case CollisionResult::非衝突:
        return "非衝突";
    case CollisionResult::接触:
        return "接触";
    case CollisionResult::衝突:
        return "衝突";
    default:
        return "Unknown";
    }
}

std::string EnumToString(AABBFace e)
{
    switch (e)
    {
    case AABBFace::NONE:
        return "NONE";
    case AABBFace::LEFT:
        return "LEFT";
    case AABBFace::RIGHT:
        return "RIGHT";
    case AABBFace::BOTTOM:
        return "BOTTOM";
    case AABBFace::TOP:
        return "TOP";
    case AABBFace::BACK:
        return "BACK";
    case AABBFace::FRONT:
        return "FRONT";
    default:
        return "Unknown";
    }
}

std::string EnumToString(CameraMode_ORBIT_FPS e)
{
    switch (e)
    {
    case CameraMode_ORBIT_FPS::ORBIT:   return "ORBIT";
    case CameraMode_ORBIT_FPS::FPS:     return "FPS";
    default:
			return "Unknown";
    }
}

std::string EnumToString(DirectionXY e)
{
    switch (e)
    {
    case DirectionXY::None:
        return "None";
    case DirectionXY::Left:
        return "Left";
    case DirectionXY::Right:
        return "Right";
    case DirectionXY::Down:
        return "Down";
    case DirectionXY::Up:
        return "Up";
    default:
        return "Unknown";
    }
}

std::string EnumToString(DirectionXZ e)
{
    switch (e)
    {
    case DirectionXZ::None:
        return "None";
    case DirectionXZ::Left:
        return "Left";
    case DirectionXZ::Right:
        return "Right";
    case DirectionXZ::Back:
        return "Back";
    case DirectionXZ::Front:
        return "Front";
    default:
        return "Unknown";
    }
}

std::string EnumToString(DirectionXZ8Way e)
{
    switch (e)
    {
    case DirectionXZ8Way::None:
        return "None";
    case DirectionXZ8Way::Front:
        return "Front";
    case DirectionXZ8Way::FrontLeft:
        return "FrontLeft";
    case DirectionXZ8Way::Left:
        return "Left";
    case DirectionXZ8Way::BackLeft:
        return "BackLeft";
    case DirectionXZ8Way::Back:
        return "Back";
    case DirectionXZ8Way::BackRight:
        return "BackRight";
    case DirectionXZ8Way::Right:
        return "Right";
    case DirectionXZ8Way::FrontRight:
        return "FrontRight";
    default:
        return "Unknown";
    }
}

std::string EnumToString(DirectionXYZ e)
{
    switch (e)
    {
    case DirectionXYZ::None:
        return "None";
    case DirectionXYZ::Left:
        return "Left";
    case DirectionXYZ::Right:
        return "Right";
    case DirectionXYZ::Back:
        return "Back";
    case DirectionXYZ::Front:
        return "Front";
    case DirectionXYZ::Down:
        return "Down";
    case DirectionXYZ::Up:
        return "Up";
    default:
        return "Unknown";
    }
}

std::string EnumToString(LineType e)
{
    switch (e)
    {
    case LineType::Line:
        return "Line";
    case LineType::BezierCurve:
        return "BezierCurve";
    case LineType::SplineCurve:
        return "SplineCurve";
    default:
        return "Unknown";
    }
}
