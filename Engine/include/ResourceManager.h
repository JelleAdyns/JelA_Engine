#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "framework.h"
#include "Observer.h"
#include <map>
#include <unordered_map>

namespace jela
{

    //---------------------------------------------------------------
    class Texture final
    {
    public:
        explicit Texture(const tstring& filename);

        Texture(const Texture& other) = delete;
        Texture(Texture&& other) noexcept = delete;
        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) noexcept = delete;

        ~Texture();

        ID2D1Bitmap* const  GetBitmap() const { return m_pDBitmap; }
        float GetWidth() const { return m_TextureWidth; }
        float GetHeight() const { return m_TextureHeight; }

        static void InitFactory();
        static void DestroyFactory();

    private:

        static IWICImagingFactory* m_pWICFactory;
        ID2D1Bitmap* m_pDBitmap{ nullptr };

        float m_TextureWidth;
        float m_TextureHeight;
    };
    //---------------------------------------------------------------


    //---------------------------------------------------------------
    //https://stackoverflow.com/questions/37572961/c-directwrite-load-font-from-file-at-runtime

    class Font final
    {
    public:
        explicit Font(const tstring& fontname, bool fromFile = false);

        Font(const Font& other) = delete;
        Font(Font&& other) noexcept = delete;
        Font& operator=(const Font& other) = delete;
        Font& operator=(Font&& other) noexcept = delete;

        ~Font();

        static void InitFactory();
        static void DestroyFactory();

        std::wstring GetFontName() const { return m_FontName; }
    private:
        // using friend class for tight coupling 
        friend class TextFormat;
        HRESULT Initialize(const std::wstring& filename);

        static IDWriteFactory5* m_pDWriteFactory;

        IDWriteFontCollection1* m_pFontCollection{ nullptr };

        std::wstring m_FontName;
    };

    class TextFormat final : public Observer<const Font* const>
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

        TextFormat(const TextFormat& other) = delete;
        TextFormat(TextFormat&& other) noexcept = delete;
        TextFormat& operator=(const TextFormat& other) = delete;
        TextFormat& operator=(TextFormat&& other) noexcept = delete;

        ~TextFormat();

        float GetFontSize() const { return m_Size; };
        IDWriteTextFormat* const GetTextFormat() const { return m_pTextFormat; };
    private:

        virtual void Notify(const Font* const pFont) override
        {
            if(pFont) SetFont(pFont);
        }
        virtual void OnSubjectDestroy(Subject<const Font* const>*) override
        {
        }

        void SetHorizontalAllignment(HorAllignment allignment);
        void SetVerticalAllignment(VertAllignment allignment);
        void SetFont(const Font* const pFont);

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

        template <typename ResourceType>
        struct ResourcePtr
        {
            const ResourceType* pObject = nullptr;

            ~ResourcePtr()
            {
                auto resMan = GetResourceManager();
                if (resMan)
                    resMan->RemoveResourcePtr(&pObject);
            }
        };

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) noexcept = delete;
        ResourceManager& operator= (const ResourceManager&) = delete;
        ResourceManager& operator= (ResourceManager&&) noexcept = delete;

        void GetTexture(const tstring& file, ResourcePtr<Texture>& pointerToAssignTo);
        void RemoveTexture(const tstring& file);
        void RemoveAllTextures();

        void GetFont(const tstring& fontName, ResourcePtr<Font>& pointerToAssignTo, bool fromFile = false);
        void RemoveFont(const tstring& fontName);
        void RemoveAllFonts();


        const tstring& GetDataPath() const { return m_DataPath; }
        const Font* const GetCurrentFont() const { return m_pCurrentFont; }
        const TextFormat* const GetCurrentTextFormat() const { return m_pCurrentTextFormat; }

        void SetDataPath(const tstring& newPath) { m_DataPath = newPath; }
        void SetCurrentFont(const Font* const pFont)
        {
            if (pFont != m_pCurrentFont)
            {
                if (pFont == nullptr && pFont != m_pDefaultFont.pObject)
                {
                    m_pCurrentFont = m_pDefaultFont.pObject;
                    OutputDebugString(_T("ERROR! New Font was 'nullptr'. Continuing with default Font!\n"));
                }
                else m_pCurrentFont = pFont;

                m_OnFontChange.NotifyObservers(m_pCurrentFont);
            }
        }
        void SetCurrentTextFormat(TextFormat* const pTextFormat)
        {
            if (pTextFormat != m_pCurrentTextFormat)
            {
                m_OnFontChange.RemoveObserver(m_pCurrentTextFormat);

                if (pTextFormat == nullptr && pTextFormat != m_pDefaultTextFormat.get())
                {
                    m_pCurrentTextFormat = m_pDefaultTextFormat.get();
                    OutputDebugString(_T("ERROR! New TextFormat was 'nullptr'. Continuing with default TextFormat!\n"));
                }
                else m_pCurrentTextFormat = pTextFormat;

                m_OnFontChange.AddObserver(m_pCurrentTextFormat);


                m_OnFontChange.NotifyObservers(m_pCurrentFont);
            }
        }
    private:

        tstring m_DataPath;

        template <typename ResourceType>
        struct ManagedResource
        {
            template <typename ...Args>
            ManagedResource(Args&&... args) :
                resource{ args... }
            {
            }

            ManagedResource(const ManagedResource&) = delete;
            ManagedResource(ManagedResource&&) noexcept = delete;
            ManagedResource& operator= (const ManagedResource&) = delete;
            ManagedResource& operator= (ManagedResource&&) noexcept = delete;

            ResourceType resource;
            std::vector<const ResourceType**> vecPointersToRefs = {};

            void RemoveInvalidRefs()
            {
                std::erase_if(vecPointersToRefs, [&](const ResourceType* const* const refToPointer)
                    {
                        return (*refToPointer) != (&resource);
                    });
            }
            void SetReferencesToNull()
            {
                for (const ResourceType** const pRefPointer : vecPointersToRefs)
                {
                    (*pRefPointer) = nullptr;
                }
            }
            void EraseRefPointerReference(const ResourceType** const referencePointer)
            {
                std::erase(vecPointersToRefs, referencePointer);
            }
        };

        template<typename ResourceType>
        using ResourceMap = std::unordered_map<tstring, ManagedResource<ResourceType>>;

        //------------------------------------------------------
        // RESOURCES
        ResourceMap<Texture>            m_MapTextures{};
        ResourceMap<Font>               m_MapFonts{};

        // CURRENTLY USED FONT
        Subject<const Font* const>      m_OnFontChange{};

        const Font*                     m_pCurrentFont{ nullptr };
        TextFormat*                     m_pCurrentTextFormat{ nullptr };

        ResourcePtr<Font>              m_pDefaultFont{};
        std::unique_ptr<TextFormat>     m_pDefaultTextFormat{ nullptr };
        //------------------------------------------------------

        template <typename ResourceType>
        void RemoveResourcePtr(const ResourceType** const referencePointer)
        {
            if constexpr (std::is_same_v<ResourceType, Texture>)
                EraseRefPointerReference(m_MapTextures, referencePointer);
            else if constexpr (std::is_same_v<ResourceType, Font>)
                EraseRefPointerReference(m_MapFonts, referencePointer);
        }

        template <typename ResourceType>
        void EraseRefPointerReference(ResourceMap<ResourceType>& resourceMap, const ResourceType** const referencePointer)
        {
            for (auto& [filename, managedResource] : resourceMap)
            {
                managedResource.EraseRefPointerReference(referencePointer);
            }
        }

        template <typename ResourceType>
        void SetReferencesToNull(ResourceMap<ResourceType>& resourceMap)
        {
            for (auto& [fileName, managedResource] : resourceMap)
            {
                managedResource.SetReferencesToNull();
            }
        }

        template <typename ResourceType>
        void RemoveInvalidRefs(ResourceMap<ResourceType>& resourceMap)
        {
            for (auto& [fileName, managedResource] : resourceMap)
            {
                managedResource.RemoveInvalidRefs();
            }
        }

        static ResourceManager* const GetResourceManager();

    };

    template <typename ResourceType>
    using ResourcePtr = ResourceManager::ResourcePtr<ResourceType>;
    //---------------------------------------------------------------

}

#endif // !RESOURCEMANAGER_H
