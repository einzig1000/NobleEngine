#include "CameraPose.h"
#include <Utilities/Converter/CoordinateConverter/CoordinateConverter.h>

namespace
{
	// 画面の右・上方向を取り出すための回転行列
	Matrix4x4 MakeScreenRotation(const Coordinate_spherical& spherical)
	{
		return Matrix4x4::MakeRotateXMatrix(spherical.phi) * Matrix4x4::MakeRotateYMatrix(-spherical.theta);
	}
}

Vector3 CameraPose::GetOffset() const
{
	return CoordinateConverter::ToCartesian(spherical);
}

Vector3 CameraPose::GetForward() const
{
	Coordinate_spherical unit = spherical;
	unit.radius = 1.0f;
	return -CoordinateConverter::ToCartesian(unit);
}

Vector3 CameraPose::GetRight() const
{
	const Matrix4x4 rot = MakeScreenRotation(spherical);
	return Vector3{ rot.m[0][0], rot.m[1][0], rot.m[2][0] };
}

Vector3 CameraPose::GetUp() const
{
	const Matrix4x4 rot = MakeScreenRotation(spherical);
	return Vector3{ rot.m[0][1], rot.m[1][1], rot.m[2][1] };
}
