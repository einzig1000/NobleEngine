#include "FontManager.h"
#include <Engine.h>
#include <FixFPS/FixFPS.h>
#include <DirectX/DirectXManager.h>
#include <DirectX/ResourceUtilities/ResourceUtilities.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <RootBinding/StructuredBufferManager/StructuredBufferManager.h>
#include <AssetManager/AssetManager.h>
#include <Utilities/Logger/Logger.h>
#include <fstream>
#include <d3d12.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <externals/stb/stb_truetype.h>

namespace
{
    // 簡易UTF-8デコーダ。日本語(3バイト)まで含めて対応
    size_t Utf8Decode(const std::string& text, size_t pos, char32_t& outCodepoint)
    {
        uint8_t c0 = static_cast<uint8_t>(text[pos]);
        if (c0 < 0x80) { outCodepoint = c0; return 1; }
        if ((c0 & 0xE0) == 0xC0 && pos + 1 < text.size())
        {
            outCodepoint = (c0 & 0x1Fu) << 6 | (static_cast<uint8_t>(text[pos + 1]) & 0x3Fu);
            return 2;
        }
        if ((c0 & 0xF0) == 0xE0 && pos + 2 < text.size())
        {
            outCodepoint = (c0 & 0x0Fu) << 12
                | (static_cast<uint8_t>(text[pos + 1]) & 0x3Fu) << 6
                | (static_cast<uint8_t>(text[pos + 2]) & 0x3Fu);
            return 3;
        }
        if ((c0 & 0xF8) == 0xF0 && pos + 3 < text.size())
        {
            outCodepoint = (c0 & 0x07u) << 18
                | (static_cast<uint8_t>(text[pos + 1]) & 0x3Fu) << 12
                | (static_cast<uint8_t>(text[pos + 2]) & 0x3Fu) << 6
                | (static_cast<uint8_t>(text[pos + 3]) & 0x3Fu);
            return 4;
        }
        outCodepoint = U'?';
        return 1;
    }
}

FontManager::FontManager(DirectXManager* dxManager, ModelManager* modelManager)
    : dxManager_(dxManager)
{
    auto* device = dxManager_->GetDevice();
    auto* srvManager = dxManager_->GetDescriptorHeapManager()->GetSRV_UAVManager();

    DirectX::TexMetadata meta{};
    meta.width = kAtlasSize;
    meta.height = kAtlasSize;
    meta.depth = 1;
    meta.arraySize = 1;
    meta.mipLevels = 1;
    meta.format = DXGI_FORMAT_R8_UNORM;
    meta.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;

    atlasResource_ = Dx12ResourceFactory::CreateTextureResource(device, meta);

    auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
    auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);
	Dx12ResourceTransition::Transition(cmdList, atlasResource_.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_GENERIC_READ);

    std::vector<uint8_t> clearPixels(static_cast<size_t>(kAtlasSize) * kAtlasSize, 0);
    UpdateAtlasRegion(0, 0, kAtlasSize, kAtlasSize, clearPixels.data());




    atlasSrvIndex_ = srvManager->CreateSRVforTexture(atlasResource_.Get(), meta).index;

	planeModelID_ = modelManager->GetModelLoader()->LoadModel("assets/engine/model/plane/plane.obj");
}

FontManager::~FontManager()
{
    pendingUploadResources_.clear();
}

bool FontManager::Load(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) return false;

    size_t size = static_cast<size_t>(file.tellg());
    fontFileBuffer_.resize(size);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(fontFileBuffer_.data()), size);

    fontInfo_ = std::make_unique<stbtt_fontinfo>();
    if (!stbtt_InitFont(fontInfo_.get(), fontFileBuffer_.data(), 0))
    {
        Log("FontManager: フォント初期化に失敗しました: %s", filePath.c_str());
        return false;
    }
    return true;
}

bool FontManager::AllocateAtlasRect(int32_t width, int32_t height, Vector2int& outPos)
{
    constexpr int32_t kPadding = 1;
    if (packerCursorX_ + width + kPadding > kAtlasSize)
    {
        packerCursorX_ = 0;
        packerCursorY_ += packerRowHeight_ + kPadding;
        packerRowHeight_ = 0;
    }
    if (packerCursorY_ + height + kPadding > kAtlasSize) return false;

    outPos = { packerCursorX_, packerCursorY_ };
    packerCursorX_ += width + kPadding;
    packerRowHeight_ = (std::max)(packerRowHeight_, height);
    return true;
}

void FontManager::UpdateAtlasRegion(int32_t x, int32_t y, int32_t width, int32_t height, const uint8_t* pixels)
{
    auto* device = dxManager_->GetDevice();
    auto backBufferIndex = dxManager_->GetSwapChain()->GetCurrentBackBufferIndex();
    auto* cmdList = dxManager_->GetCommandContextManager()->GetCommandList(backBufferIndex);

    // CopyTextureRegionの転送元バッファは行の先頭アドレスが256バイト境界である必要がある
    const UINT rowPitch = (width + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1);
    const UINT uploadSize = rowPitch * height;

    Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer = Dx12ResourceFactory::CreateBufferResource(device, uploadSize);

    uint8_t* mapped = nullptr;
    uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
    for (int32_t row = 0; row < height; ++row)
    {
        std::memcpy(mapped + row * rowPitch, pixels + row * width, width);
    }
    uploadBuffer->Unmap(0, nullptr);


    Dx12ResourceTransition::Transition(cmdList, atlasResource_.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);


    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource = atlasResource_.Get();
    dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource = uploadBuffer.Get();
    src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src.PlacedFootprint.Offset = 0;
    src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8_UNORM;
    src.PlacedFootprint.Footprint.Width = width;
    src.PlacedFootprint.Footprint.Height = height;
    src.PlacedFootprint.Footprint.Depth = 1;
    src.PlacedFootprint.Footprint.RowPitch = rowPitch;

    cmdList->CopyTextureRegion(&dst, x, y, 0, &src, nullptr);

    Dx12ResourceTransition::Transition(cmdList, atlasResource_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

    pendingUploadResources_.push_back(uploadBuffer);
}

const GlyphInfo* FontManager::GetOrBakeGlyph(char32_t codepoint, int32_t charSize)
{
    GlyphKey key{ codepoint, charSize };
    auto it = glyphCache_.find(key);
    if (it != glyphCache_.end()) return &it->second;

    float scale = stbtt_ScaleForPixelHeight(fontInfo_.get(), static_cast<float>(charSize));

    int width = 0, height = 0, xoff = 0, yoff = 0;
    uint8_t* bitmap = stbtt_GetCodepointBitmap(fontInfo_.get(), scale, scale,
        static_cast<int>(codepoint), &width, &height, &xoff, &yoff);

    Vector2int atlasPos{};
    if (width > 0 && height > 0)
    {
        if (!AllocateAtlasRect(width, height, atlasPos))
        {
            Log("FontManager: フォントアトラスの空きがありません");
            stbtt_FreeBitmap(bitmap, nullptr);
            return nullptr;
        }
        UpdateAtlasRegion(atlasPos.x, atlasPos.y, width, height, bitmap);
        stbtt_FreeBitmap(bitmap, nullptr);
    }

    int advanceWidth = 0, leftSideBearing = 0;
    stbtt_GetCodepointHMetrics(fontInfo_.get(), static_cast<int>(codepoint), &advanceWidth, &leftSideBearing);

    GlyphInfo info{};
    info.uvMin = { atlasPos.x / static_cast<float>(kAtlasSize), atlasPos.y / static_cast<float>(kAtlasSize) };
    info.uvMax = { (atlasPos.x + width) / static_cast<float>(kAtlasSize), (atlasPos.y + height) / static_cast<float>(kAtlasSize) };
    info.size = { static_cast<float>(width), static_cast<float>(height) };
    info.bearing = { static_cast<float>(xoff), static_cast<float>(yoff) };
    info.advance = advanceWidth * scale;

    auto [insertedIt, ok] = glyphCache_.emplace(key, info);
    return &insertedIt->second;
}

void FontManager::DrawString(int32_t renderTextureID, const std::string& text, int32_t charSize,
    const Vector2& startPos, const Vector4& color, float extraSpacing)
{
    int rawAscent = 0, rawDescent = 0, rawLineGap = 0;
    stbtt_GetFontVMetrics(fontInfo_.get(), &rawAscent, &rawDescent, &rawLineGap);
    float scale = stbtt_ScaleForPixelHeight(fontInfo_.get(), static_cast<float>(charSize));
    float lineHeight = (rawAscent - rawDescent + rawLineGap) * scale;

    std::vector<GlyphInstance> instances;
    instances.reserve(text.size());

    float cursorX = startPos.x;
    float cursorY = startPos.y + rawAscent * scale;

    size_t i = 0;
    while (i < text.size())
    {
        char32_t codepoint = 0;
        i += Utf8Decode(text, i, codepoint);

        if (codepoint == U'\n')
        {
            cursorX = startPos.x;
            cursorY += lineHeight;
            continue;
        }

        const GlyphInfo* glyph = GetOrBakeGlyph(codepoint, charSize);
        if (!glyph) continue;

        if (glyph->size.x > 0 && glyph->size.y > 0)
        {
            GlyphInstance inst{};
            inst.position = { cursorX + glyph->bearing.x, cursorY + glyph->bearing.y };
            inst.size = glyph->size;
            inst.uvMin = glyph->uvMin;
            inst.uvMax = glyph->uvMax;
            inst.color = color;
            instances.push_back(inst);
        }
        cursorX += glyph->advance + extraSpacing;
    }
    if (instances.empty()) return;

    auto* sbManager = Engine::Instance().GetStructuredBufferManager();
    int32_t bufferID = sbManager->CreateDynamic();
    sbManager->UpdateData(bufferID, instances.data(), sizeof(GlyphInstance), instances.size());

    const RenderTarget* target = dxManager_->GetRenderTextureManager()->Get(renderTextureID);

    struct { Vector2 targetSize; Vector2 pad0; } vsConstants{};
    vsConstants.targetSize = { static_cast<float>(target->width), static_cast<float>(target->height) };

    struct { int32_t atlasTextureIndex; float pad0[3]; } psConstants{};
    psConstants.atlasTextureIndex = atlasSrvIndex_;

	uint32_t elapsedTime = Engine::Instance().GetFixFPS()->GetFrameCount();

    for (auto& render : fontRenders_)
    {
        if (render.frameCount < elapsedTime)
        {
            render.render->instanceNum_ = static_cast<uint32_t>(instances.size());
            render.render->SetSBufferData(0, ShaderType::VertexShader, sbManager->GetSRV(bufferID));
            render.render->SetCBufferData(0, ShaderType::VertexShader, &vsConstants);
            render.render->SetCBufferData(0, ShaderType::PixelShader, &psConstants);
            render.render->Draw(renderTextureID);
            render.frameCount = elapsedTime;

            return;
        }
    }

	fontRenders_.emplace_back();
    fontRenders_.back().render = std::make_unique<RenderObject>();
    fontRenders_.back().render->psoConfig_.vs = "assets/shaders/Text/Text.VS.hlsl";
    fontRenders_.back().render->psoConfig_.ps = "assets/shaders/Text/Text.PS.hlsl";
    fontRenders_.back().render->psoConfig_.rasterizerID = RasterizerID::Solid_BackCull;
	fontRenders_.back().render->modelID_ = planeModelID_;
    fontRenders_.back().render->SetupFromShaders();

    fontRenders_.back().render->instanceNum_ = static_cast<uint32_t>(instances.size());
    fontRenders_.back().render->SetSBufferData(0, ShaderType::VertexShader, sbManager->GetSRV(bufferID));
    fontRenders_.back().render->SetCBufferData(0, ShaderType::VertexShader, &vsConstants);
    fontRenders_.back().render->SetCBufferData(0, ShaderType::PixelShader, &psConstants);
    fontRenders_.back().render->Draw(renderTextureID);
    fontRenders_.back().frameCount = elapsedTime;
}

Vector2 FontManager::MeasureJustTextureSize(const std::string& text, int32_t charSize, const Vector2& startPos, float extraSpacing)
{
    // DrawStringと同じ行送り計算(実測フォントメトリクス)
    int rawAscent = 0, rawDescent = 0, rawLineGap = 0;
    stbtt_GetFontVMetrics(fontInfo_.get(), &rawAscent, &rawDescent, &rawLineGap);
    float scale = stbtt_ScaleForPixelHeight(fontInfo_.get(), static_cast<float>(charSize));
    float lineHeight = (rawAscent - rawDescent + rawLineGap) * scale;

    // 行ごとの幅(実グリフ送り幅の合計)
    std::vector<float> lineWidths;
    for (size_t i = 0; i < text.size();)
    {
        char32_t codepoint = 0;
        i += Utf8Decode(text, i, codepoint);

        if (codepoint == U'\n')
        {
            lineWidths.push_back(0.0f);
            continue;
        }
        if (lineWidths.empty()) lineWidths.push_back(0.0f);

        int advanceWidth = 0, leftSideBearing = 0;
        stbtt_GetCodepointHMetrics(fontInfo_.get(), static_cast<int>(codepoint), &advanceWidth, &leftSideBearing);

        if (lineWidths.back() > 0.0f) lineWidths.back() += extraSpacing;
        lineWidths.back() += advanceWidth * scale;
    }

    // 文字のみのサイズ(実測の送り幅・行送りに基づく)
    Vector2 textSize{};
    textSize.y = lineWidths.size() * lineHeight;
    for (float width : lineWidths)
    {
        if (width > textSize.x) textSize.x = width;
    }

    // 上下左右の余白を加えたサイズ
    Vector2 paddingSize{ startPos.x * 2.0f, startPos.y * 2.0f };

    return textSize + paddingSize;
}