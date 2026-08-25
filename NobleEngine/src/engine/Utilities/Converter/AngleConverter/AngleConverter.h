#pragma once

namespace AngleConverter
{
	/// <summary>
	/// 度数法（degree）を弧度法（radian）に変換する
	/// </summary>
	/// <param name="degree">度数法の角度</param>
	/// <returns>弧度法の角度</returns>
	float ToRadian(float degree);

	/// <summary>
	/// 弧度法（radian）を度数法（degree）に変換する
	/// </summary>
	/// <param name="radian">弧度法の角度</param>
	/// <returns>度数法の角度</returns>
	float ToDegree(float radian);
};

