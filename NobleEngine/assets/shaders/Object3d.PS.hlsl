// Object3d.PS.hlsl

#include "object3d.hlsli"

// サンプラーのレジスタ番号を s0 に設定
SamplerState gSampler : register(s0);
// テクスチャのレジスタ番号を t0 に設定
Texture2D<float32_t4> gTexture0 : register(t0);
// Material: レジスタ b0 に設定
ConstantBuffer<Material> gMaterial0 : register(b0);
// DirectionalLight: レジスタ b2 に設定
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);
// Camera: レジスタ b3 に設定
ConstantBuffer<Camera> gCamera : register(b3);

PixelShaderOutput main(VertexShaderOutput input)
{
	// UV同次座標
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial0.uvTransform);
	float2 uv = transformedUV.xy / transformedUV.w;

	PixelShaderOutput output;
	float32_t4 textureColor = gTexture0.Sample(gSampler, uv);

	if (gDirectionalLight.mode == 2) // ハーフランバート
	{
		// ハーフランバート用角度補正
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

		// 拡散反射(ハーフランバート)
		float32_t3 diffuse =
			// マテリアルの色 *
			gMaterial0.color.rgb *
			// テクスチャ自体の色 *
			textureColor.rgb *
			// ライトの色 *
			gDirectionalLight.color.rgb *
			// 角度補正
			cos *
			// ライト輝度 *
			gDirectionalLight.intensity;

		// 鏡面反射
		float32_t3 specular = { 0.0f,0.0f,0.0f };

		if (gDirectionalLight.phong == true)
		{
			// 視線ベクトル
			float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
			// 反射ベクトル
			float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
			// 視線と反射ベクトルの内積
			float RdotE = dot(reflectLight, toEye);
			// 反射強度
			float specularPow = pow(saturate(RdotE), gMaterial0.shininess);

			specular =
				// ライトの色 *
				gDirectionalLight.color.rgb *
				// ライト輝度 *
				gDirectionalLight.intensity *
				// 反射強度 * 
				specularPow *
				// 反射色
				float32_t3(1.0f, 1.0f, 1.0f);
		}

		output.color.rgb = diffuse + specular;
		output.color.a = gMaterial0.color.a * textureColor.a;
		return output;
	}
	else if (gDirectionalLight.mode == 1) // ランバート
	{
		// ランバート用角度補正
		float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));

		// 拡散反射(ランバート)
		float32_t3 diffuse =
			// マテリアルの色 *
			gMaterial0.color.rgb *
			// テクスチャ自体の色 *
			textureColor.rgb *
			// ライトの色 *
			gDirectionalLight.color.rgb *
			// 角度補正
			cos *
			// ライト輝度 *
			gDirectionalLight.intensity;

		// 鏡面反射
		float32_t3 specular = { 0.0f,0.0f,0.0f };

		if (gDirectionalLight.phong == true)
		{
			// 視線ベクトル
			float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
			// 反射ベクトル
			float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
			// 視線と反射ベクトルの内積
			float RdotE = dot(reflectLight, toEye);
			// 反射強度
			float specularPow = pow(saturate(RdotE), gMaterial0.shininess);

			specular =
				// ライトの色 *
				gDirectionalLight.color.rgb *
				// ライト輝度 *
				gDirectionalLight.intensity *
				// 反射強度 * 
				specularPow *
				// 反射色
				float32_t3(1.0f, 1.0f, 1.0f);
		}

		output.color.rgb = diffuse + specular;
		output.color.a = gMaterial0.color.a * textureColor.a;
		return output;
	}
	else // ライティングなし
	{
		// 拡散反射(ライティングなし)
		float32_t3 diffuse =
			// マテリアルの色 *
			gMaterial0.color.rgb *
			// テクスチャ自体の色 *
			textureColor.rgb;

		// 鏡面反射
		float32_t3 specular = { 0.0f,0.0f,0.0f };

		if (gDirectionalLight.phong == true)
		{
			// 視線ベクトル
			float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
			// 反射ベクトル
			float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
			// 視線と反射ベクトルの内積
			float RdotE = dot(reflectLight, toEye);
			// 反射強度
			float specularPow = pow(saturate(RdotE), gMaterial0.shininess);

			specular =
				// ライトの色 *
				gDirectionalLight.color.rgb *
				// ライト輝度 *
				gDirectionalLight.intensity *
				// 反射強度 * 
				specularPow *
				// 反射色
				float32_t3(1.0f, 1.0f, 1.0f);
		}

		output.color.rgb = diffuse + specular;
		output.color.a = gMaterial0.color.a * textureColor.a;
		return output;
	}
}