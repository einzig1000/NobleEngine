#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <wrl.h>
#include <d3d12.h>

struct stbtt_fontinfo;
class DirectXManager;
class RenderObject;
class ModelManager;


struct GlyphKey
{
    char32_t codepoint;
    int32_t pixelSize;
    bool operator==(const GlyphKey& rhs) const
    {
        return codepoint == rhs.codepoint && pixelSize == rhs.pixelSize;
    }
};
struct GlyphKeyHash
{
    size_t operator()(const GlyphKey& key) const
    {
        return (static_cast<size_t>(key.codepoint) << 32) ^ static_cast<size_t>(key.pixelSize);
    }
};

struct GlyphInfo
{
    Vector2 uvMin{};
    Vector2 uvMax{};
    Vector2 size{};     // ピクセルサイズ
    Vector2 bearing{};  // ペン位置から見た左上オフセット
    float advance = 0.0f;
};

// GPUに送るインスタンスデータ。Text.VS.hlslのGlyphInstanceとメモリレイアウトを揃えること
struct GlyphInstance
{
    Vector2 position{}; // 書き込み先RT内のピクセル座標(左上)
    Vector2 size{};
    Vector2 uvMin{};
    Vector2 uvMax{};
    Vector4 color{};
};

struct FontRender
{
    std::unique_ptr<RenderObject> render;
	uint32_t frameCount = std::numeric_limits<uint32_t>::max();
};

class FontManager
{
public:
    FontManager(DirectXManager* dxManager, ModelManager* modelManager);
    ~FontManager();

    bool Load(const std::string& filePath);

    /// <summary>
    /// 指定したテクスチャに文字を書きこむ
    /// </summary>
    /// <param name="renderTextureID">レンダーテクスチャID</param>
    /// <param name="text">書き込む文字列</param>
    /// <param name="charSize">文字サイズ</param>
    /// <param name="startPos">開始位置</param>
    /// <param name="color">文字色</param>
    /// <param name="extraSpacing">文字間隔</param>
    void DrawString(int32_t renderTextureID, const std::string& text, int32_t charSize,
        const Vector2& startPos, const Vector4& color, float extraSpacing);

	Vector2 MeasureJustTextureSize(const std::string& text, int32_t charSize,
        const Vector2& startPos, float extraSpacing);

private:
    DirectXManager* dxManager_ = nullptr;

    const GlyphInfo* GetOrBakeGlyph(char32_t codepoint, int32_t charSize);
    bool AllocateAtlasRect(int32_t width, int32_t height, Vector2int& outPos);
    void UpdateAtlasRegion(int32_t x, int32_t y, int32_t width, int32_t height, const uint8_t* pixels);


    std::vector<uint8_t> fontFileBuffer_;
    std::unique_ptr<stbtt_fontinfo> fontInfo_;

    Microsoft::WRL::ComPtr<ID3D12Resource> atlasResource_;
    int32_t atlasSrvIndex_ = -1;
    static constexpr int32_t kAtlasSize = 1024;

    std::unordered_map<GlyphKey, GlyphInfo, GlyphKeyHash> glyphCache_;

    // シェルフパッカーの状態
    int32_t packerCursorX_ = 0;
    int32_t packerCursorY_ = 0;
    int32_t packerRowHeight_ = 0;

	int32_t planeModelID_ = -1;

    std::vector<FontRender> fontRenders_;

    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> pendingUploadResources_;
};