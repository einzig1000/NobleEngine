#pragma once

namespace Random
{
	/// <summary>
	/// 指定範囲の整数乱数取得
	/// </summary>
	/// <param name="min"> 最小値 </param>
	/// <param name="max"> 最大値 </param>
	/// <returns> 乱数 </returns>
	int RandomInt(int min, int max);

	/// <summary>
	/// 指定範囲の小数点付き乱数取得
	/// </summary>
	/// <param name="min"> 最小値 </param>
	/// <param name="max"> 最大値 </param>
	/// <param name="decimalPlaces"> 小数点以下の桁数 </param>
	/// <returns> 乱数 </returns>
	float RandomFloat(float min, float max, int decimalPlaces = 2);
};

