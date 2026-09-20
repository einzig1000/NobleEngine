#include "EngineDefinition.h"
#include <cmath>
#include <cstdint>
#include <numbers>

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
    if (std::fabs(det) < Constexprs::eps) return Return;

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

    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            Return.m[i][j] = m[j][i];
        }
    }

    return Return;
}

Matrix4x4 Matrix4x4::MakeIdentity4x4()
{
    Matrix4x4 Return{};

    for (size_t i = 0; i < 4; ++i)
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
    Matrix4x4 rotateXYZMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;
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
    if (dot > 1.0f - Constexprs::eps)
    {
        return MakeIdentity4x4();
    }

    // ほぼ逆方向
    if (dot < -1.0f + Constexprs::eps)
    {
        // from に直交する任意のベクトル
        Vector3 n;
        if (std::abs(fromNormalized.x) > Constexprs::eps || std::abs(fromNormalized.y) > Constexprs::eps)
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
    
    m.m[0][0] = x.x; m.m[1][0] = x.y; m.m[2][0] = x.z;
    m.m[0][1] = y.x; m.m[1][1] = y.y; m.m[2][1] = y.z;
    m.m[0][2] = z.x; m.m[1][2] = z.y; m.m[2][2] = z.z;

    m.m[3][0] = -x.Dot(eye);
    m.m[3][1] = -y.Dot(eye);
    m.m[3][2] = -z.Dot(eye);

    m.m[0][3] = 0.0f;
    m.m[1][3] = 0.0f;
    m.m[2][3] = 0.0f;
    m.m[3][3] = 1.0f;

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
    if (std::abs(norm) > Constexprs::eps)
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
    if (std::abs(normSq) > Constexprs::eps)
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
    result.m[0][1] = 2.0f * (x * y + z * w);
    result.m[0][2] = 2.0f * (x * z - y * w);
    result.m[1][0] = 2.0f * (x * y - z * w);
    result.m[1][1] = 1.0f - 2.0f * (x * x + z * z);
    result.m[1][2] = 2.0f * (y * z + x * w);
    result.m[2][0] = 2.0f * (x * z + y * w);
    result.m[2][1] = 2.0f * (y * z - x * w);
    result.m[2][2] = 1.0f - 2.0f * (x * x + y * y);
    return result;
}

Vector3 Quaternion::ToEuler() const
{
    Vector3 euler;

    // --- Yaw (Y軸) ---
    float siny = 2.0f * (y * w - x * z);
    if (std::abs(siny) >= 1.0f) euler.y = std::copysign(3.1415926535f / 2.0f, siny); // ±90°
    else euler.y = std::asin(siny);

    // --- Pitch (X軸) ---
    euler.x = std::atan2(2.0f * (y * z + x * w), 1.0f - 2.0f * (x * x + y * y));

    // --- Roll (Z軸) ---
    euler.z = std::atan2(2.0f * (x * y + z * w), 1.0f - 2.0f * (y * y + z * z));

    return euler;

}

Quaternion Quaternion::MakeFromEuler(const float& yaw, const float& pitch, const float& roll)
{
	float cy = std::cos(yaw * 0.5f);
	float sy = std::sin(yaw * 0.5f);
	float cp = std::cos(pitch * 0.5f);
	float sp = std::sin(pitch * 0.5f);
	float cr = std::cos(roll * 0.5f);
	float sr = std::sin(roll * 0.5f);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;


    Quaternion qx = MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), pitch);
    Quaternion qy = MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), yaw);
    Quaternion qz = MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), roll);
    return qz * qy * qx;
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
    this->min.x = std::min(aabb.min.x, aabb.max.x);
    this->max.x = std::max(aabb.min.x, aabb.max.x);
    this->min.y = std::min(aabb.min.y, aabb.max.y);
    this->max.y = std::max(aabb.min.y, aabb.max.y);
    this->min.z = std::min(aabb.min.z, aabb.max.z);
    this->max.z = std::max(aabb.min.z, aabb.max.z);
};

Vector3 AABB::GetCollisionDepth(const AABB& other)const
{
    Vector3 depth;
    depth.x = std::min(this->max.x, other.max.x) - std::max(this->min.x, other.min.x);
    depth.y = std::min(this->max.y, other.max.y) - std::max(this->min.y, other.min.y);
    depth.z = std::min(this->max.z, other.max.z) - std::max(this->min.z, other.min.z);
    if (depth.x <= 0.0f || depth.y <= 0.0f || depth.z <= 0.0f)
    {
        return { 0.0f, 0.0f, 0.0f };
    }
    return depth;
}

#pragma endregion

#pragma region OBB

OBB OBB::MakeFromAABB(const AABB& localAABB, const Matrix4x4& worldMatrix)
{
    OBB obb;

    // 行列の1〜3行目 ＝ ワールド空間での各ローカル軸方向 × スケール（行ベクトル規約）
    Vector3 rowX = { worldMatrix.m[0][0], worldMatrix.m[0][1], worldMatrix.m[0][2] };
    Vector3 rowY = { worldMatrix.m[1][0], worldMatrix.m[1][1], worldMatrix.m[1][2] };
    Vector3 rowZ = { worldMatrix.m[2][0], worldMatrix.m[2][1], worldMatrix.m[2][2] };

    // 各行の長さ＝スケール成分
    float scaleX = rowX.Length();
    float scaleY = rowY.Length();
    float scaleZ = rowZ.Length();

    // スケールを抜いた純粋な回転軸（0除算対策込み）
    obb.axis[0] = (scaleX > 0.00001f) ? (rowX / scaleX) : Vector3{ 1.0f, 0.0f, 0.0f };
    obb.axis[1] = (scaleY > 0.00001f) ? (rowY / scaleY) : Vector3{ 0.0f, 1.0f, 0.0f };
    obb.axis[2] = (scaleZ > 0.00001f) ? (rowZ / scaleZ) : Vector3{ 0.0f, 0.0f, 1.0f };

    // ローカルAABBの半径にスケールを掛けてワールドでの半径にする
    Vector3 localHalfSize = (localAABB.max - localAABB.min) * 0.5f;
    obb.halfSize = {
        localHalfSize.x * scaleX,
        localHalfSize.y * scaleY,
        localHalfSize.z * scaleZ,
    };

    // ローカルAABBの中心をワールド空間へ変換（アフィン行列前提でw=1固定）
    Vector3 localCenter = localAABB.center();
    obb.center.x = localCenter.x * worldMatrix.m[0][0] + localCenter.y * worldMatrix.m[1][0] + localCenter.z * worldMatrix.m[2][0] + worldMatrix.m[3][0];
    obb.center.y = localCenter.x * worldMatrix.m[0][1] + localCenter.y * worldMatrix.m[1][1] + localCenter.z * worldMatrix.m[2][1] + worldMatrix.m[3][1];
    obb.center.z = localCenter.x * worldMatrix.m[0][2] + localCenter.y * worldMatrix.m[1][2] + localCenter.z * worldMatrix.m[2][2] + worldMatrix.m[3][2];

    return obb;
}

#pragma endregion

