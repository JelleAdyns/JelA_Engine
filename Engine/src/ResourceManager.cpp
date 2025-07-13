#include "ResourceManager.h"
#include "Engine.h"

namespace jela
{
    //---------------------------------------------------------------------------------------------------------------------------------
    //---------------------
    //TEXTURE
    //---------------------

    IWICImagingFactory* Texture::m_pWICFactory{ nullptr };

    Texture::Texture(const tstring& filename) :
        m_pDBitmap{ NULL },
        m_TextureWidth{ 0 },
        m_TextureHeight{ 0 }
    {

        HRESULT creationResult = S_OK;

        IWICBitmapDecoder* pDecoder = NULL;
        IWICBitmapFrameDecode* pSource = NULL;
        IWICFormatConverter* pConverter = NULL;

        std::wstring filePath = to_wstring(ENGINE.ResourceMngr()->GetDataPath() + filename);

        if (SUCCEEDED(creationResult))
        {
            creationResult = m_pWICFactory->CreateDecoderFromFilename(
                filePath.c_str(),
                NULL,
                GENERIC_READ,
                WICDecodeMetadataCacheOnLoad,
                &pDecoder);
        }


        if (SUCCEEDED(creationResult))
        {
            // Create the initial frame.
            creationResult = pDecoder->GetFrame(0, &pSource);
        }


        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        if (SUCCEEDED(creationResult)) creationResult = m_pWICFactory->CreateFormatConverter(&pConverter);
        if (SUCCEEDED(creationResult))
        {
            creationResult = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
            );
        }


        if (SUCCEEDED(creationResult))
        {
            creationResult = ENGINE.GetRenderTarget()->CreateBitmapFromWicBitmap(
                pConverter,
                NULL,
                &m_pDBitmap
            );


            if (SUCCEEDED(creationResult))
            {
                m_TextureWidth = m_pDBitmap->GetSize().width;
                m_TextureHeight = m_pDBitmap->GetSize().height;
            }
        }

        if (!SUCCEEDED(creationResult))
        {
            OutputDebugStringW((L"ERROR! File \"" + filePath + L"\" couldn't load correctly").c_str());
        }
        SafeRelease(&pDecoder);
        SafeRelease(&pSource);
        SafeRelease(&pConverter);

    }
    Texture::~Texture()
    {
        SafeRelease(&m_pDBitmap);
    }
    void Texture::InitFactory()
    {
        if (!m_pWICFactory)
        {
            HRESULT creationResult = CoCreateInstance(
                CLSID_WICImagingFactory,
                NULL,
                CLSCTX_ALL,
                IID_PPV_ARGS(&m_pWICFactory)
            );
            assert((SUCCEEDED(creationResult)));
        }
    }
    void Texture::DestroyFactory()
    {
        SafeRelease(&m_pWICFactory);
    }

    //---------------------------------------------------------------------------------------------------------------------------------



    //---------------------------------------------------------------------------------------------------------------------------------
    //---------------------
    //Font
    //---------------------

    IDWriteFactory5* Font::m_pDWriteFactory{ nullptr };


    Font::Font(const tstring& fontName, bool fromFile)
    {
        if (fromFile)
        {
            std::wstring filePath = to_wstring(ENGINE.ResourceMngr()->GetDataPath() + fontName);
            HRESULT hr = Initialize(filePath);
            if (hr != S_OK) Engine::NotifyError(NULL, _T("Font wasn't intialized properly"), hr);
        }
        else
        {
            m_FontName = to_wstring(fontName);
        }
    }
    Font::~Font()
    {
        SafeRelease(&m_pFontCollection);
    }
    HRESULT Font::Initialize(const std::wstring& fontName)
    {
        HRESULT hr = S_OK;

        IDWriteFontSetBuilder1* pFontSetBuilder{ nullptr };
        IDWriteFontSet* pFontSet{ nullptr };
        IDWriteFontFile* pFontFile{ nullptr };
        m_pFontCollection = nullptr;



        hr = m_pDWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);
        if (SUCCEEDED(hr))
        {
            hr = m_pDWriteFactory->CreateFontFileReference(fontName.c_str(), NULL, &pFontFile);
        }
        if (SUCCEEDED(hr))
        {
            hr = pFontSetBuilder->AddFontFile(pFontFile);
        }
        if (SUCCEEDED(hr))
        {
            hr = pFontSetBuilder->CreateFontSet(&pFontSet);
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pDWriteFactory->CreateFontCollectionFromFontSet(pFontSet, &m_pFontCollection);
        }

        IDWriteFontFamily* pFontFamily{ nullptr };
        IDWriteLocalizedStrings* pStrings{ nullptr };

        UINT32 length{};
        std::wstring name{};

        if (SUCCEEDED(hr))
        {
            hr = m_pFontCollection->GetFontFamily(0, &pFontFamily);
        }
        if (SUCCEEDED(hr))
        {
            hr = pFontFamily->GetFamilyNames(&pStrings);
        }
        if (SUCCEEDED(hr))
        {
            hr = pStrings->GetStringLength(0, &length);
        }

        if (SUCCEEDED(hr))
        {
            name.resize(length);
            hr = pStrings->GetString(0, &name[0], length + 1);
        }


        if (!SUCCEEDED(hr))
        {
            OutputDebugStringW((L"Something went wrong in the Font constructor using " + fontName).c_str());
        }
        else
        {
            m_FontName = name;
        }

        SafeRelease(&pFontSetBuilder);
        SafeRelease(&pFontSet);
        SafeRelease(&pFontFile);
        SafeRelease(&pFontFamily);
        SafeRelease(&pStrings);

        return hr;
    }

    void Font::InitFactory()
    {
        if (!m_pDWriteFactory)
        {
            DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory5),
                reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
        }
    }

    void Font::DestroyFactory()
    {
        SafeRelease(&m_pDWriteFactory);
    }


    TextFormat::TextFormat(int fontSize, bool bold, bool italic, HorAllignment horAllign, VertAllignment vertAllign)
    {

        Font::m_pDWriteFactory->CreateTextFormat(
            ENGINE.GetCurrentFont()->m_FontName.c_str(),
            ENGINE.GetCurrentFont()->m_pFontCollection,
            bold ? DWRITE_FONT_WEIGHT_EXTRA_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            static_cast<FLOAT>(fontSize),
            L"en-us",
            &m_pTextFormat);

        assert((m_pTextFormat) && _T("TextFormat was not loaded correctly"));
        m_Size = fontSize;
        SetHorizontalAllignment(horAllign);
        SetVerticalAllignment(vertAllign);
    }

    TextFormat::~TextFormat()
    {
        SafeRelease(&m_pTextFormat);
    }
    void TextFormat::SetHorizontalAllignment(HorAllignment allignment)
    {
        switch (allignment)
        {
        case HorAllignment::Left:
            if (m_pTextFormat) m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            break;
        case HorAllignment::Center:
            if (m_pTextFormat) m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            break;
        case HorAllignment::Right:
            if (m_pTextFormat) m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            break;
        case HorAllignment::Justified:
            if (m_pTextFormat) m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
            break;
        }
    }
    void TextFormat::SetVerticalAllignment(VertAllignment allignment)
    {
        switch (allignment)
        {
        case VertAllignment::Top:
            if (m_pTextFormat) m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            break;
        case VertAllignment::Center:
            if (m_pTextFormat) m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            break;
        case VertAllignment::Bottom:
            if (m_pTextFormat) m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
            break;
        }
    }
    void TextFormat::SetFont(const Font* const pFont)
    {
        const auto vertAllign = m_pTextFormat->GetParagraphAlignment();
        const auto horAllign = m_pTextFormat->GetTextAlignment();
        const auto size = m_pTextFormat->GetFontSize();
        const auto weight = m_pTextFormat->GetFontWeight();
        const auto style = m_pTextFormat->GetFontStyle();

        SafeRelease(&m_pTextFormat);

        Font::m_pDWriteFactory->CreateTextFormat(
            pFont->m_FontName.c_str(),
            pFont->m_pFontCollection,
            weight,
            style,
            DWRITE_FONT_STRETCH_NORMAL,
            static_cast<FLOAT>(size),
            L"en-us",
            &m_pTextFormat);

        assert((m_pTextFormat) && _T("TextFormat was not loaded correctly"));

        m_pTextFormat->SetTextAlignment(horAllign);
        m_pTextFormat->SetParagraphAlignment(vertAllign);
    }


    //---------------------------------------------------------------------------------------------------------------------------------
    //---------------------
    //ResourceManager
    //---------------------

    void ResourceManager::Start()
    {
        GetFont(_T("Verdana"), m_pDefaultFont);
        SetCurrentFont(m_pDefaultFont.pObject);

        m_pDefaultTextFormat = std::make_unique<TextFormat>(12, false, false, TextFormat::HorAllignment::Left, TextFormat::VertAllignment::Top);
        SetCurrentTextFormat(m_pDefaultTextFormat.get());
    }

    void ResourceManager::GetTexture(const tstring& file, ReferencePtr<Texture>& pointerToAssignTo)
    {
        m_MapTextures.try_emplace(file, file);

        m_MapTextures.at(file).vecPointersToRefs.push_back(&(pointerToAssignTo.pObject));
        pointerToAssignTo.pObject = &(m_MapTextures.at(file).resource);
    }

    void ResourceManager::RemoveTexture(const tstring& file)
    {
        if (m_MapTextures.contains(file))
        {
            RemoveInvalidRefs(m_MapTextures);
            m_MapTextures.at(file).SetReferencesToNull();

            m_MapTextures.erase(file);
        }
        else OutputDebugString((_T("\nTexture to remove is not present. File: ") + file + _T("\n\n")).c_str());
    }

    void ResourceManager::RemoveAllTextures()
    {
        RemoveInvalidRefs(m_MapTextures);
        SetReferencesToNull(m_MapTextures);

        m_MapTextures.clear();
    }

    void ResourceManager::GetFont(const tstring& fontName, ReferencePtr<Font>& pointerToAssignTo, bool fromFile)
    {
        m_MapFonts.try_emplace(fontName, fontName, fromFile);

        m_MapFonts.at(fontName).vecPointersToRefs.push_back(&(pointerToAssignTo.pObject));
        pointerToAssignTo.pObject = &(m_MapFonts.at(fontName).resource);
    }

    void ResourceManager::RemoveFont(const tstring& fontName)
    {
        if (m_MapFonts.contains(fontName))
        {
            RemoveInvalidRefs(m_MapFonts);
            m_MapFonts.at(fontName).SetReferencesToNull();

            m_MapFonts.erase(fontName);
        }
        else OutputDebugString((_T("Font to remove is not present. File: ") + fontName).c_str());
    }

    void ResourceManager::RemoveAllFonts()
    {
        RemoveInvalidRefs(m_MapFonts);
        SetReferencesToNull(m_MapFonts);

        m_MapFonts.clear();
    }

    ResourceManager* const ResourceManager::GetResourceManager()
    {
        return ENGINE.ResourceMngr();
    }

}
