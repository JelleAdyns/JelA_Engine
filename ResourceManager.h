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
        Font(const tstring& fontname, bool fromFile = false);

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

        TextFormat(int fontSize, bool bold, bool italic, HorAllignment horAllign, VertAllignment vertAllign);

        TextFormat(const TextFormat& other) = delete;
        TextFormat(TextFormat&& other) noexcept = delete;
        TextFormat& operator=(const TextFormat& other) = delete;
        TextFormat& operator=(TextFormat&& other) noexcept = delete;

        ~TextFormat();

        int GetFontSize() const { return m_Size; };
        IDWriteTextFormat* const GetTextFormat() const { return m_pTextFormat; };
    private:

        virtual void Notify(const Font* const pFont) override
        {
            if(pFont) SetFont(pFont);
        }
        virtual void OnSubjectDestroy() override
        {
        }

        void SetHorizontalAllignment(HorAllignment allignment);
        void SetVerticalAllignment(VertAllignment allignment);
        void SetFont(const Font* const pFont);

        IDWriteTextFormat* m_pTextFormat{ nullptr };
        int m_Size;
    };
    //---------------------------------------------------------------



    class ResourceManager final
    {

    private:
        static inline ResourceManager* m_pInstance = nullptr;

    public:
        static ResourceManager& GetInstance()
        {
            if (!m_pInstance)
                m_pInstance = new ResourceManager{};

            return *m_pInstance;
        }
        static void ShutDown()
        {
            delete m_pInstance;
            m_pInstance = nullptr;

            Texture::DestroyFactory();
            Font::DestroyFactory();
        }
        ~ResourceManager() = default;

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) noexcept = delete;
        ResourceManager& operator= (const ResourceManager&) = delete;
        ResourceManager& operator= (ResourceManager&&) noexcept = delete;

        void Init(const tstring& dataPath)
        {
            m_DataPath = dataPath;
            Texture::InitFactory();
            Font::InitFactory();
        }

        template <typename ResourceType>
        struct ReferencePtr
        {
            const ResourceType* pObject = nullptr;

            ~ReferencePtr()
            {
                ResourceManager::GetInstance().RemoveReferencePtr(&pObject);
            }
            //Subject
        };

        void GetTexture(const tstring& file, ReferencePtr<Texture>& pointerToAssignTo);
        void RemoveTexture(const tstring& file);
        void RemoveAllTextures();

        void GetFont(const tstring& fontName, ReferencePtr<Font>& pointerToAssignTo, bool fromFile = false);
        void RemoveFont(const tstring& fontName);
        void RemoveAllFonts();


        const tstring& GetDataPath() const { return m_DataPath; }
        void SetDataPath(const tstring& newPath) { m_DataPath = newPath; }

    private:
        ResourceManager() = default;
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
                vecPointersToRefs.erase(
                    std::remove_if(vecPointersToRefs.begin(), vecPointersToRefs.end(), [&](const ResourceType* const* const refToPointer)
                        {
                            return (*refToPointer) != (&resource);
                        }
                    ),
                    vecPointersToRefs.end());
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
                vecPointersToRefs.erase(
                    std::remove(vecPointersToRefs.begin(), vecPointersToRefs.end(), referencePointer),
                    vecPointersToRefs.end());
            }
        };

        template<typename ResourceType>
        using ResourceMap = std::unordered_map<tstring, ManagedResource<ResourceType>>;

        //------------------------------------------------------
        // RESOURCES
        ResourceMap<Texture> m_MapTextures{};
        ResourceMap<Font> m_MapFonts{};
        //------------------------------------------------------

        template <typename ResourceType>
        void RemoveReferencePtr(const ResourceType** const referencePointer)
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
    };

    template <typename ResourceType>
    using ReferencePtr = ResourceManager::ReferencePtr<ResourceType>;
    //---------------------------------------------------------------

}

#endif // !RESOURCEMANAGER_H
