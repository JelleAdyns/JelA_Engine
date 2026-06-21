#include "ResourceManager.h"
#include "Engine.h"
#include "FileExceptions.h"

namespace jela
{
    //---------------------------------------------------------------------------------------------------------------------------------
    //---------------------
    //TEXTURE
    //---------------------

    IWICImagingFactory* Texture::m_pWICFactory{ nullptr };

    Texture::Texture(const tstring& filename) : m_pDBitmap{nullptr},
                                                m_TextureWidth{ 0 },
                                                m_TextureHeight{ 0 }
    {
        HResultHandler creationResult{};

        IWICBitmapDecoder* pDecoder = nullptr;
        IWICBitmapFrameDecode* pSource = nullptr;
        IWICFormatConverter* pConverter = nullptr;

        if (const std::filesystem::path filePath{ ENGINE.ResourceMngr()->GetDataPath() + filename };
            std::filesystem::exists(filePath))
        {
            if (filename.find(_T(".png")) == std::string::npos &&
                filename.find(_T(".jpg")) == std::string::npos &&
                filename.find(_T(".jpeg")) == std::string::npos)
                throw FileTypeNotSupportedException{
                    std::format("File type of {} is not supported.", std::filesystem::path{ filename }.string()),
                    { ".png", ".jpg", ".jpeg" }
                };

            if (creationResult.Succeeded())
            {
                creationResult = m_pWICFactory->CreateDecoderFromFilename(
                    filePath.c_str(),
                    nullptr,
                    GENERIC_READ,
                    WICDecodeMetadataCacheOnLoad,
                    &pDecoder);
            }


            if (creationResult.Succeeded() && pDecoder)
            {
                // Create the initial frame.
                creationResult = pDecoder->GetFrame(0, &pSource);
            }


            // Convert the image format to 32bppPBGRA
            // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
            if (creationResult.Succeeded()) creationResult = m_pWICFactory->CreateFormatConverter(&pConverter);
            if (creationResult.Succeeded() && pConverter)
            {
                creationResult = pConverter->Initialize(
                    pSource,
                    GUID_WICPixelFormat32bppPBGRA,
                    WICBitmapDitherTypeNone,
                    nullptr,
                    0.f,
                    WICBitmapPaletteTypeMedianCut
                );
            }


            if (creationResult.Succeeded())
            {
                creationResult = ENGINE.Get2DDeviceContext().CreateBitmapFromWicBitmap(pConverter,m_pDBitmap);

                if (creationResult.Succeeded())
                {
                    m_TextureWidth = m_pDBitmap->GetSize().width;
                    m_TextureHeight = m_pDBitmap->GetSize().height;
                }
            }

            m_FileName = filename;
            SafeRelease(&pDecoder);
            SafeRelease(&pSource);
            SafeRelease(&pConverter);

            if (creationResult.Failed())
            {
                SafeRelease(&m_pDBitmap);
                throw FileLoadException{
                    std::format("ERROR! File \"{}\" couldn't load correctly. HRESULT Error code: {}\n",
                                filePath.string(), creationResult.Get())
                };
            }
        }
        else
            throw FileNotFoundException{
                std::format("Path \"{}\" does not exist. Error occurred when trying to create a Texture.\n",
                            filePath.string())
            };
    }

    Texture::~Texture()
    {
        SafeRelease(&m_pDBitmap);
    }

    void Texture::InitFactory()
    {
        if (!m_pWICFactory)
        {
            HResultHandler creationResult{};
            creationResult = CoCreateInstance(
                CLSID_WICImagingFactory,
                nullptr,
                CLSCTX_ALL,
                IID_PPV_ARGS(&m_pWICFactory)
            );
            if (creationResult.Failed()) throw std::runtime_error("WIC Factory not created correctly.");
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
            try
            {
                const tstring fullPath = ENGINE.ResourceMngr()->GetDataPath() + fontName;
                if (const std::filesystem::path filePath{ fullPath }; !std::filesystem::exists(filePath))
                    throw FileNotFoundException{
                        std::format(
                            "Path \"{}\" does not exist. Error occurred when trying to create a Font from a file.\n",
                            filePath.string())
                    };

                if (fontName.find(_T(".ttf")) == std::string::npos &&
                    fontName.find(_T(".otf")) == std::string::npos)
                    throw FileTypeNotSupportedException{
                        std::format("File type of {} is not supported.", std::filesystem::path{ fontName }.string()),
                        { ".ttf", ".otf" }
                    };

                if (const HResultHandler hr = Initialize(fullPath); hr.Failed())
                    throw FileLoadException{
                        std::format("Font {} wasn't initialized properly. HRESULT Error value: {}.",
                                    std::filesystem::path{ fontName }.string(), hr.Get())
                    };
            }
            catch (...)
            {
                SafeRelease(&m_pFontCollection);
                throw;
            }
        }
        else
        {
            m_FontName = to_wstring(fontName);
        }
    }

    Font::~Font()
    {
        if (!ENGINE.IsQuitting())
        {
            if (const auto mngr = ENGINE.ResourceMngr(); this == mngr->GetCurrentFont())
                mngr->SetDefaultFont();
        }

        SafeRelease(&m_pFontCollection);
    }

    HResultHandler Font::Initialize(const std::wstring& fontName)
    {
        HResultHandler hr{};

        IDWriteFontSetBuilder1* pFontSetBuilder{ nullptr };
        IDWriteFontSet* pFontSet{ nullptr };
        IDWriteFontFile* pFontFile{nullptr};
        m_pFontCollection = nullptr;

        hr = m_pDWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);
        if (hr.Succeeded()) hr = m_pDWriteFactory->CreateFontFileReference(fontName.c_str(), nullptr, &pFontFile);

        if (hr.Succeeded()) hr = pFontSetBuilder->AddFontFile(pFontFile);

        if (hr.Succeeded()) hr = pFontSetBuilder->CreateFontSet(&pFontSet);

        if (hr.Succeeded()) hr = m_pDWriteFactory->CreateFontCollectionFromFontSet(pFontSet, &m_pFontCollection);

        IDWriteFontFamily* pFontFamily{ nullptr };
        IDWriteLocalizedStrings* pStrings{ nullptr };

        UINT32 length{};
        std::wstring name{};

        if (hr.Succeeded() && m_pFontCollection) hr = m_pFontCollection->GetFontFamily(0, &pFontFamily);

        if (hr.Succeeded()) hr = pFontFamily->GetFamilyNames(&pStrings);

        if (hr.Succeeded()) hr = pStrings->GetStringLength(0, &length);

        if (hr.Succeeded())
        {
            name.resize(length);
            hr = pStrings->GetString(0, &name[0], length + 1);
        }

        if (hr.Succeeded()) m_FontName = name;

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
            HResultHandler hr{};
            hr = DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory5),
                reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
            if (hr.Failed()) throw std::runtime_error("DWrite Factory not created correctly.");
        }
    }

    void Font::DestroyFactory()
    {
        SafeRelease(&m_pDWriteFactory);
    }


    TextFormat::TextFormat(float fontSize, bool bold, bool italic, HorAllignment horAllign, VertAllignment vertAllign)
    {
        HResultHandler hr{};
        hr = Font::m_pDWriteFactory->CreateTextFormat(
            ENGINE.GetCurrentFont()->m_FontName.c_str(),
            ENGINE.GetCurrentFont()->m_pFontCollection,
            bold ? DWRITE_FONT_WEIGHT_EXTRA_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            fontSize,
            L"en-us",
            &m_pTextFormat);

        assert(hr.Succeeded());
        assert((m_pTextFormat) && _T("TextFormat was not loaded correctly"));
        m_Size = fontSize;
        SetHorizontalAllignment(horAllign);
        SetVerticalAllignment(vertAllign);
    }

    TextFormat::~TextFormat()
    {
        if (!ENGINE.IsQuitting())
        {
            if (const auto mngr = ENGINE.ResourceMngr(); this == mngr->GetCurrentTextFormat())
                mngr->SetDefaultTextFormat();
        }

        SafeRelease(&m_pTextFormat);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    HResultHandler TextFormat::SetHorizontalAllignment(HorAllignment allignment)
    {
        HResultHandler hr{};
        switch (allignment)
        {
            case HorAllignment::Left:
                if (m_pTextFormat) hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                break;
            case HorAllignment::Center:
                if (m_pTextFormat) hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                break;
            case HorAllignment::Right:
                if (m_pTextFormat) hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
                break;
            case HorAllignment::Justified:
                if (m_pTextFormat) hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
                break;
        }
        return hr;
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    HResultHandler TextFormat::SetVerticalAllignment(VertAllignment allignment)
    {
        HResultHandler hr{};
        switch (allignment)
        {
            case VertAllignment::Top:
                if (m_pTextFormat) hr = m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
                break;
            case VertAllignment::Center:
                if (m_pTextFormat) hr = m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                break;
            case VertAllignment::Bottom:
                if (m_pTextFormat) hr = m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
                break;
        }
        return hr;
    }

    HResultHandler TextFormat::SetFont(const Font* const pFont)
    {
        const auto vertAllign = m_pTextFormat->GetParagraphAlignment();
        const auto horAllign = m_pTextFormat->GetTextAlignment();
        const auto size = m_pTextFormat->GetFontSize();
        const auto weight = m_pTextFormat->GetFontWeight();
        const auto style = m_pTextFormat->GetFontStyle();

        SafeRelease(&m_pTextFormat);

        HResultHandler hr{};
        hr = Font::m_pDWriteFactory->CreateTextFormat(
            pFont->m_FontName.c_str(),
            pFont->m_pFontCollection,
            weight,
            style,
            DWRITE_FONT_STRETCH_NORMAL,
            size,
            L"en-us",
            &m_pTextFormat);


        assert(hr.Succeeded());
        assert((m_pTextFormat) && _T("TextFormat was not loaded correctly"));

        hr = m_pTextFormat->SetTextAlignment(horAllign);
        if (hr.Succeeded())
            hr = m_pTextFormat->SetParagraphAlignment(vertAllign);

        return hr;
    }


    //---------------------------------------------------------------------------------------------------------------------------------
    //---------------------
    //ResourceManager
    //---------------------

    void ResourceManager::Start()
    {
        SetDefaultFont();

        m_pDefaultTextFormat = std::make_unique<TextFormat>(12.f, false, false, TextFormat::HorAllignment::Left,
                                                            TextFormat::VertAllignment::Top);
        SetDefaultTextFormat();
    }

    void ResourceManager::GetTexture(const tstring& file, ResourcePtr<Texture>& resourcePtr)
    {
        try
        {
            m_MapTextures.try_emplace(file, file);
            m_MapTextures.at(file).HandleObserver(resourcePtr);
        }
        catch (const FileException& e)
        {
            ENGINE.NotifyException(e.what());
            OutputDebugStringA(e.what());
            m_MapTextures.erase(file);
            resourcePtr = ResourcePtr<Texture>{};
        }
        catch (const std::exception& e)
        {
            OutputDebugStringA(e.what());
            m_MapTextures.erase(file);
            resourcePtr = ResourcePtr<Texture>{};
        }
    }

    void ResourceManager::RemoveTexture(const tstring& file)
    {
        if (m_MapTextures.contains(file))
        {
            m_MapTextures.erase(file);
        }
        else OutputDebugString(std::format(_T("\nTexture to remove is not present. File: {}\n\n"), file).c_str());
    }

    void ResourceManager::RemoveAllTextures()
    {
        m_MapTextures.clear();
    }

    void ResourceManager::GetFont(const tstring& fontName, ResourcePtr<Font>& resourcePtr, bool fromFile)
    {
        try
        {
            m_MapFonts.try_emplace(fontName, fontName, fromFile);
            m_MapFonts.at(fontName).HandleObserver(resourcePtr);
        }
        catch (const FileException& e)
        {
            ENGINE.NotifyException(e.what());
            OutputDebugStringA(e.what());
            m_MapFonts.erase(fontName);
            resourcePtr = ResourcePtr<Font>{};
        }
        catch (const std::exception& e)
        {
            OutputDebugStringA(e.what());
            m_MapFonts.erase(fontName);
            resourcePtr = ResourcePtr<Font>{};
        }
    }

    void ResourceManager::RemoveFont(const tstring& fontName)
    {
        if (m_MapFonts.contains(fontName))
        {
            m_MapFonts.erase(fontName);
        }
        else OutputDebugString(std::format(_T("Font to remove is not present. Fontname: {}\n"), fontName).c_str());
    }

    void ResourceManager::RemoveAllFonts()
    {
        m_MapFonts.clear();
    }

    void ResourceManager::SetCurrentFont(const Font* pFont)
    {
        if (pFont == m_pCurrentFont) return;

        if (pFont == nullptr)
        {
            m_pCurrentFont = m_pDefaultFont.get();
            OutputDebugString(_T("ERROR! New Font was 'nullptr'. Continuing with default Font!\n"));
        }
        else m_pCurrentFont = pFont;

        m_OnFontChange.NotifyObservers(m_pCurrentFont);
    }

    void ResourceManager::SetCurrentTextFormat(TextFormat* pTextFormat)
    {
        if (pTextFormat == m_pCurrentTextFormat) return;

        m_OnFontChange.RemoveObserver(m_pCurrentTextFormat);

        if (pTextFormat == nullptr)
        {
            m_pCurrentTextFormat = m_pDefaultTextFormat.get();
            OutputDebugString(_T("ERROR! New TextFormat was 'nullptr'. Continuing with default TextFormat!\n"));
        }
        else m_pCurrentTextFormat = pTextFormat;

        m_OnFontChange.AddObserver(m_pCurrentTextFormat);

        m_OnFontChange.NotifyObservers(m_pCurrentFont);
    }

    void ResourceManager::SetDefaultFont()
    {
        m_pCurrentFont = nullptr;
        GetFont(_T("Verdana"), m_pDefaultFont);
        SetCurrentFont(m_pDefaultFont.get());
    }
    void ResourceManager::SetDefaultTextFormat()
    {
        SetCurrentTextFormat(m_pDefaultTextFormat.get());
    }
    ResourceManager* ResourceManager::GetResourceManager()
    {
        return ENGINE.ResourceMngr();
    }
}
