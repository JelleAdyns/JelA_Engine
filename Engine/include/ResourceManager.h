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
        const tstring& GetFileName() const { return m_FileName; }

        static void InitFactory();
        static void DestroyFactory();

    private:

        static IWICImagingFactory* m_pWICFactory;
        ID2D1Bitmap* m_pDBitmap{ nullptr };

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

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) noexcept = delete;
        ResourceManager& operator= (const ResourceManager&) = delete;
        ResourceManager& operator= (ResourceManager&&) noexcept = delete;

        template <typename ResourceType>
        struct ResourcePtr;

        void GetTexture(const tstring& file, ResourcePtr<Texture>& resourcePtr);
        void RemoveTexture(const tstring& file);
        void RemoveAllTextures();

        void GetFont(const tstring& fontName, ResourcePtr<Font>& resourcePtr, bool fromFile = false);
        void RemoveFont(const tstring& fontName);
        void RemoveAllFonts();

        const tstring& GetDataPath() const { return m_DataPath; }
        const Font* const GetCurrentFont() const { return m_pCurrentFont; }
        const TextFormat* const GetCurrentTextFormat() const { return m_pCurrentTextFormat; }

        void SetDataPath(const tstring& newPath) { m_DataPath = newPath; }
        void SetCurrentFont(const Font* const pFont);
        void SetCurrentTextFormat(TextFormat* const pTextFormat);
    private:

        //-----------------------------------------------------------------------------------------------------------------
        // Private ManagedResource struct

        template <typename ResourceType>
        struct ManagedResource
        {
            template <typename ...Args>
            ManagedResource(Args&&... args) :
                resource{ args... },
                pOnResourceDestroy{std::make_unique<Subject<>>()}
            {}

            ~ManagedResource() { pOnResourceDestroy->NotifyObservers(); }

            ManagedResource(const ManagedResource&) = delete;
            ManagedResource(ManagedResource&&) noexcept = delete;
            ManagedResource& operator= (const ManagedResource&) = delete;
            ManagedResource& operator= (ManagedResource&&) noexcept = delete;

            ResourceType resource;

            void HandleObserver(ResourcePtr<ResourceType>& resourcePtr)
            {
                if (resourcePtr.pObject) resourcePtr.m_pSubject->RemoveObserver(&resourcePtr);
                pOnResourceDestroy->AddObserver(&resourcePtr);
                resourcePtr.SaveSubject(pOnResourceDestroy.get());
                resourcePtr.pObject = &resource;
            }

        private:
            std::unique_ptr<Subject<>> pOnResourceDestroy{};
        };
        //-----------------------------------------------------------------------------------------------------------------

    public:

        //-----------------------------------------------------------------------------------------------------------------
        // Public ResourcePtr struct
        template <typename ResourceType>
        struct ResourcePtr final : public Observer<>
        {
            const ResourceType* pObject = nullptr;

            ResourcePtr() = default;

            virtual ~ResourcePtr() { if (m_pSubject) m_pSubject->RemoveObserver(this); }

            ResourcePtr(const ResourcePtr& other)
                : pObject{other.pObject}
                  , m_pSubject{other.m_pSubject}
            {
                if (m_pSubject) m_pSubject->AddObserver(this);
            }

            ResourcePtr(ResourcePtr&& other) noexcept
                : pObject{std::move(other.pObject)}
                  , m_pSubject{std::move(other.m_pSubject)}
            {
                if (m_pSubject)
                {
                    m_pSubject->RemoveObserver(&other);
                    m_pSubject->AddObserver(this);
                }

                other.m_pSubject = nullptr;
                other.pObject = nullptr;
            }

            ResourcePtr& operator= (const ResourcePtr& other)
            {
                if (m_pSubject) m_pSubject->RemoveObserver(this);

                pObject = other.pObject;
                m_pSubject = other.m_pSubject;
                if (m_pSubject) m_pSubject->AddObserver(this);

                return *this;
            }

            ResourcePtr& operator= (ResourcePtr&& other) noexcept
            {
                if (m_pSubject) m_pSubject->RemoveObserver(this);

                m_pSubject = std::move(other.m_pSubject);
                pObject = std::move(other.pObject);

                if (m_pSubject)
                {
                    m_pSubject->RemoveObserver(&other);
                    m_pSubject->AddObserver(this);
                }

                other.m_pSubject = nullptr;
                other.pObject = nullptr;

                return *this;
            }

        private:
            friend struct ManagedResource<ResourceType>;

            virtual void Notify() override { pObject = nullptr; }
            virtual void OnSubjectDestroy(Subject<>* pSubject) override { if (pSubject == m_pSubject) m_pSubject = nullptr; }
            void SaveSubject(Subject<>* pSubject)
            {
                if (!pSubject) OutputDebugString(_T("Subject was nullptr when trying to save it to the ResourcePtr SingleSubjectsObserver."));
                else m_pSubject = pSubject;
            }
            Subject<>* m_pSubject{};
        };
        //-----------------------------------------------------------------------------------------------------------------

    private:

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

        ResourcePtr<Font>               m_pDefaultFont{};
        std::unique_ptr<TextFormat>     m_pDefaultTextFormat{ nullptr };

        // DATA PATH
        tstring m_DataPath;
        //------------------------------------------------------


        static ResourceManager* const GetResourceManager();
    };

    template <typename ResourceType>
    using ResourcePtr = ResourceManager::ResourcePtr<ResourceType>;
    //---------------------------------------------------------------

}

#endif // !RESOURCEMANAGER_H
