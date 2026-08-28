#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "ObservingObjects.h"
#include "HResultHandler.h"
#include <unordered_map>

namespace jela
{

    //---------------------------------------------------------------
    class Texture final
    {
    public:
        explicit Texture(const tstring& filename);

        Texture(const Texture& other);;
        Texture(Texture&& other) noexcept;;
        Texture& operator=(const Texture& other);
        Texture& operator=(Texture&& other) noexcept;;

        ~Texture();

        void Swap(Texture& other) noexcept;
        friend void Swap(Texture& first, Texture& second) noexcept {first.Swap(second);}

        ID2D1Bitmap* GetBitmap() const { return m_pDBitmap; }
        float GetWidth() const { return m_TextureWidth; }
        float GetHeight() const { return m_TextureHeight; }
        const tstring& GetFileName() const { return m_FileName; }

        static void InitFactory();
        static void DestroyFactory();

    private:

        static IWICImagingFactory* m_pWICFactory;
        ID2D1Bitmap1* m_pDBitmap{ nullptr };

        float m_TextureWidth;
        float m_TextureHeight;
        tstring m_FileName{};
    };
    //---------------------------------------------------------------


    //---------------------------------------------------------------
    //https://stackoverflow.com/questions/37572961/c-directwrite-load-font-from-file-at-runtime

    class Font final
    {
    public:
        explicit Font(const tstring& fontname, bool fromFile = false);

        Font(const Font& other);
        Font(Font&& other) noexcept;
        Font& operator=(const Font& other);
        Font& operator=(Font&& other) noexcept;

        ~Font();

        void Swap(Font& other) noexcept;
        friend void Swap(Font& first, Font& second) noexcept {first.Swap(second);}

        static void InitFactory();
        static void DestroyFactory();

        std::wstring GetFontName() const { return m_FontName; }
    private:
        // using friend class for tight coupling
        friend class TextFormat;
        HResultHandler Initialize(const std::wstring& filename);

        static IDWriteFactory5* m_pDWriteFactory;

        IDWriteFontCollection1* m_pFontCollection{ nullptr };

        std::wstring m_FontName;
    };

    class TextFormat final : public Observer<const Font*>
    {
    public:

        enum class HorAllignment
        {
            Left,
            Center,
            Right,
            Justified
        };
        enum class VertAllignment
        {
            Top,
            Center,
            Bottom,
        };

        explicit TextFormat(float fontSize, bool bold, bool italic, HorAllignment horAllign, VertAllignment vertAllign);

        TextFormat(const TextFormat& other);
        TextFormat(TextFormat&& other) noexcept;
        TextFormat& operator=(const TextFormat& other);
        TextFormat& operator=(TextFormat&& other) noexcept;

        ~TextFormat() override;

        void Swap(TextFormat& other);
        friend void swap(TextFormat& first, TextFormat& second) noexcept { first.Swap(second); }

        float GetFontSize() const { return m_Size; }
        IDWriteTextFormat* GetTextFormat() const { return m_pTextFormat; }
    private:

        using ObserverArg = const Font*;
        using Subject = Subject<ObserverArg>;
        void Notify(ObserverArg pFont) override
        {
            if(pFont) SetFont(pFont);
        }
        void OnSubjectDestroy(Subject*) override {}
        void OnSubjectCopied(Subject*, Subject*) override {}
        void OnSubjectMoved(Subject*, Subject*) override {}

        HResultHandler SetHorizontalAllignment(HorAllignment allignment);
        HResultHandler SetVerticalAllignment(VertAllignment allignment);
        HResultHandler SetFont(const Font* const pFont);

        IDWriteTextFormat* m_pTextFormat{ nullptr };
        float m_Size;
    };
    //---------------------------------------------------------------


    //---------------------------------------------------------------
    class ResourceManager final
    {
    public:
        ResourceManager(const tstring& dataPath)
        {
            m_DataPath = dataPath;
            Texture::InitFactory();
            Font::InitFactory();
        }
        ~ResourceManager()
        {
            m_OnFontChange.RemoveObserver(m_pCurrentTextFormat);

            m_pDefaultTextFormat = nullptr;
            RemoveAllFonts();
            RemoveAllTextures();

            Texture::DestroyFactory();
            Font::DestroyFactory();
        }

        void Start();

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) noexcept = delete;
        ResourceManager& operator= (const ResourceManager&) = delete;
        ResourceManager& operator= (ResourceManager&&) noexcept = delete;

        void GetTexture(const tstring& file, ResourcePtr<Texture>& resourcePtr);
        void RemoveTexture(const tstring& file);
        void RemoveAllTextures();

        void GetFont(const tstring& fontName, ResourcePtr<Font>& resourcePtr, bool fromFile = false);
        void RemoveFont(const tstring& fontName);
        void RemoveAllFonts();

        const tstring& GetDataPath() const { return m_DataPath; }
        const Font* GetCurrentFont() const { return m_pCurrentFont; }
        const TextFormat* GetCurrentTextFormat() const { return m_pCurrentTextFormat; }

        void SetDataPath(const tstring& newPath) { m_DataPath = newPath; }
        void SetCurrentFont(const Font* pFont);
        void SetCurrentTextFormat(TextFormat* pTextFormat);
        void SetDefaultFont();
        void SetDefaultTextFormat();


    private:

        template<typename ResourceType>
        using ResourceMap = std::unordered_map<tstring, ObjectObserved<ResourceType>>;
        //------------------------------------------------------
        // RESOURCES
        ResourceMap<Texture>            m_MapTextures{};
        ResourceMap<Font>               m_MapFonts{};

        // CURRENTLY USED FONT
        Subject<const Font*>            m_OnFontChange{};

        const Font*                     m_pCurrentFont{ nullptr };
        TextFormat*                     m_pCurrentTextFormat{ nullptr };

        ResourcePtr<Font>               m_pDefaultFont{};
        std::unique_ptr<TextFormat>     m_pDefaultTextFormat{ nullptr };

        // DATA PATH
        tstring m_DataPath;
        //------------------------------------------------------

        static ResourceManager* GetResourceManager();
    };
    //---------------------------------------------------------------

}

#endif // !RESOURCEMANAGER_H
