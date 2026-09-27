#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <externals/stb/stb_truetype.h>
#include <unordered_map>
#include <memory>

// グリフキャッシュのキー(文字 + 文字サイズ)
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

// アトラスに焼き込んだ1文字分の情報
struct GlyphInfo
{
	Vector2 uvMin{};
	Vector2 uvMax{};
	Vector2 size{};     // ピクセルサイズ
	Vector2 bearing{};  // ペン位置から見た左上オフセット
	float advance = 0.0f;
};

// フォントデータ(1フォントにつきアトラス1枚)
struct FontData
{
	// アトラスの一辺のピクセル数
	static constexpr int32_t kAtlasSize = 1024;

	// ファイルパス
	std::string filePath;

	// フォントファイルの中身。fontInfoがこのバッファを参照し続けるので、読み込み後は触らないこと
	std::vector<uint8_t> fontFileBuffer;
	stbtt_fontinfo fontInfo{};

	// グリフを焼き込むアトラステクスチャ
	Microsoft::WRL::ComPtr<ID3D12Resource> atlasResource;
	int32_t atlasSrvIndex = -1;

	// 焼き込み済みグリフ
	std::unordered_map<GlyphKey, GlyphInfo, GlyphKeyHash> glyphCache;

	// シェルフパッカーの状態
	int32_t packerCursorX = 0;
	int32_t packerCursorY = 0;
	int32_t packerRowHeight = 0;
};

class FontBank
{
public:
	// フォントデータを追加
	int32_t AddFontData(const std::string& filePath, std::unique_ptr<FontData> fontData);
	// filePathが同じフォントデータが存在するか
	int32_t IsFontDataExist(const std::string& filePath) const;
	// fontIDからフォントデータを取得(グリフの焼き込みで書き換えるので非const)
	FontData* GetFontData(int32_t fontID);
	// フォントリストを取得
	const std::vector<std::unique_ptr<FontData>>& GetFontList() const { return objects_; }

private:
	// キー：フォントのファイルパス、値：ID
	std::unordered_map<std::string, int32_t> pathToIDMap_;
	// キー：ID、値：フォントデータ
	std::vector<std::unique_ptr<FontData>> objects_;
};