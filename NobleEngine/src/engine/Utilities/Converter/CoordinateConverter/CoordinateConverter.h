#pragma once
#include "definition/definition.h"

namespace CoordinateConverter
{
	// 円柱座標系を直交座標系に変換
	Vector3 ToCartesian(const Coordinate_cylindrical& cylindrical);

	// 球座標系を直交座標系に変換
	Vector3 ToCartesian(const Coordinate_spherical& spherical);

	// 直交座標系を円柱座標系に変換
	Coordinate_cylindrical ToCylindrical(const Vector3& cartesian);

	// 球座標系を円柱座標系に変換
	Coordinate_cylindrical ToCylindrical(const Coordinate_spherical& spherical);

	// 直交座標系を球座標系に変換
	Coordinate_spherical ToSpherical(const Vector3& cartesian);

	// 円柱座標系を球座標系に変換
	Coordinate_spherical ToSpherical(const Coordinate_cylindrical& cylindrical);
};

