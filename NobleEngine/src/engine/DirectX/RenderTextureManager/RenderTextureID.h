#pragma once

enum class RenderTextureID
{
	Test,

	UI,


	// BackBufferに描画する前のRenderTexture。実質最後の描画先RenderTexture。
	PreBackBuffer,
};