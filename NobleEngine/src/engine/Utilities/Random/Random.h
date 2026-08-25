#pragma once
#include <cstdint>

namespace Random
{
	/// <summary>
	/// 指定範囲の整数乱数取得
	/// </summary>
	/// <param name="min"> 最小値 </param>
	/// <param name="max"> 最大値 </param>
	/// <returns> 乱数 </returns>
	int32_t RandomInt(int32_t min, int32_t max);

	/// <summary>
	/// 指定範囲の小数点付き乱数取得
	/// </summary>
	/// <param name="min"> 最小値 </param>
	/// <param name="max"> 最大値 </param>
	/// <param name="decimalPlaces"> 小数点以下の桁数 </param>
	/// <returns> 乱数 </returns>
	float RandomFloat(float min, float max, int32_t decimalPlaces = 2);
};

