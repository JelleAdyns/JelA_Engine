#ifndef ENGINE_H
#define ENGINE_H

#include "resource.h"
#include "framework.h"
#include "BaseGame.h"
#include "Structs.h"
#include "player.h"
#include "Audio.h"

class Texture;
class Font;

class Engine final
{
private:
    Engine();
public:

    Engine(const Engine& other) = delete;
    Engine(Engine&& other) noexcept = delete;
    Engine& operator=(const Engine& other) = delete;
    Engine& operator=(Engine&& other) noexcept = delete;

    ~Engine();

    static Engine& GetSingleton();

    int Run();
    LRESULT HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


    void DrawLine           (int firstX, int firstY, int secondX, int secondY, float lineThickness = 1.f) const;
    void DrawLine           (const Point2Int& firstPoint, const Point2Int& secondPoint, float lineThickness = 1.f) const;

#ifdef MATHEMATICAL_COORDINATESYSTEM
    void DrawRectangle      (int left, int bottom, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const Point2Int& leftBottom, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const RectInt& rect, float lineThickness = 1.f)const;

    void DrawRoundedRect    (int left, int bottom, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const RectInt& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;

    void DrawString         (const tstring& textToDisplay, Font* font, int left, int bottom, int width, int height, bool showRect = false)const;
    void DrawString         (const tstring& textToDisplay, Font* font, Point2Int leftBottom, int width, int height, bool showRect = false)const;
    void DrawString         (const tstring& textToDisplay, Font* font, RectInt destRect, bool showRect = false)const;

    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void DrawString         (const tstring& textToDisplay, Font* font, int left, int bottom, int width, bool showRect = false)const;
    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void DrawString         (const tstring& textToDisplay, Font* font, Point2Int leftBottom, int width, bool showRect = false)const;

    void DrawTexture        (const Texture& texture, int destLeft, int destBottom, const RectInt& srcRect = {}, float opacity = 1.f)const;
    void DrawTexture        (const Texture& texture, const Point2Int& destLeftBottom = {}, const RectInt& srcRect = {}, float opacity = 1.f)const;
    void DrawTexture        (const Texture& texture, const RectInt& destRect, const RectInt& srcRect = {}, float opacity = 1.f)const;

    void FillRectangle      (int left, int bottom, int width, int height)const;
    void FillRectangle      (const Point2Int& leftBottom, int width, int height)const;
    void FillRectangle      (const RectInt& rect)const;

    void FillRoundedRect    (int left, int bottom, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect    (const Point2Int& leftBottom, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect    (const RectInt& rect, float radiusX, float radiusY)const;
#else
    void DrawRectangle      (int left, int top, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const Point2Int& leftTop, int width, int height, float lineThickness = 1.f)const;
    void DrawRectangle      (const RectInt& rect, float lineThickness = 1.f)const;

    void DrawRoundedRect    (int left, int top, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const Point2Int& leftTop, int width, int height, float radiusX, float radiusY, float lineThickness = 1.f)const;
    void DrawRoundedRect    (const RectInt& rect, float radiusX, float radiusY, float lineThickness = 1.f)const;

    void DrawString         (const tstring& textToDisplay, Font* font, int left, int top, int width, int height, bool showRect = false)const;
    void DrawString         (const tstring& textToDisplay, Font* font, Point2Int leftTop, int width, int height, bool showRect = false)const;
    void DrawString         (const tstring& textToDisplay, Font* font, RectInt destRect, bool showRect = false)const;

    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void DrawString         (const tstring& textToDisplay, Font* font, int left, int top, int width, bool showRect = false)const;
    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void DrawString         (const tstring& textToDisplay, Font* font, Point2Int leftTop, int width, bool showRect = false)const;

    void DrawTexture        (const Texture& texture, int destLeft, int destTop, const RectInt& srcRect = {}, float opacity = 1.f)const;
    void DrawTexture        (const Texture& texture, const Point2Int& destLeftTop = {}, const RectInt& srcRect = {}, float opacity = 1.f)const;
    void DrawTexture        (const Texture& texture, const RectInt& destRect, const RectInt& srcRect = {}, float opacity = 1.f)const;

    void FillRectangle      (int left, int top, int width, int height)const;
    void FillRectangle      (const Point2Int& leftTop, int width, int height)const;
    void FillRectangle      (const RectInt& rect)const;

    void FillRoundedRect    (int left, int top, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect    (const Point2Int& leftTop, int width, int height, float radiusX, float radiusY)const;
    void FillRoundedRect    (const RectInt& rect, float radiusX, float radiusY)const;
#endif // MATHEMATICAL_COORDINATESYSTEM

    void DrawEllipse        (int centerX, int centerY, int radiusX, int radiusY, float lineThickness = 1.f)const;
    void DrawEllipse        (const Point2Int& center, int radiusX, int radiusY, float lineThickness = 1.f)const;
    void DrawEllipse        (const EllipseInt& ellipse, float lineThickness = 1.f)const;

    void FillEllipse        (int centerX, int centerY, int radiusX, int radiusY)const;
    void FillEllipse        (const Point2Int& center, int radiusX, int radiusY)const;
    void FillEllipse        (const EllipseInt& ellipse)const;

    //Use CAPITAL letters or the virtual keycodes
    bool IsKeyPressed(int virtualKeycode) const;

    void SetColor(COLORREF newColor, float opacity = 1.F);
    void SetBackGroundColor(COLORREF newColor);
    void SetInstance(HINSTANCE hInst);
    void SetTitle(const tstring& newTitle);
    void SetResourcePath(const std::wstring& newTitle);
    void SetWindowDimensions(int width, int height);
    void SetFrameRate(int FPS);

    void EndTransform();
    void Translate(int xTranslation, int yTranslation);
    void Translate(const Vector2Int& translation);
    void Rotate(float angle, int xPivotPoint, int yPivotPoint, bool translationFirst) ;
    void Rotate(float angle, const Point2Int& pivotPoint, bool translationFirst) ;
    void Scale(float xScale, float yScale, int xPointToScaleFrom, int yPointToScaleFrom);
    void Scale(float scale, int xPointToScaleFrom, int yPointToScaleFrom);
    void Scale(float xScale, float yScale, const Point2Int& PointToScaleFrom);
    void Scale(float scale, const Point2Int& PointToScaleFrom);

    const std::wstring& GetResourcePath() const;
    RectInt GetWindowSize() const;
    HWND GetWindow() const;
    HINSTANCE GetHInstance() const;

    ID2D1HwndRenderTarget* getRenderTarget() const;


    static void NotifyError(HWND hWnd, const WCHAR* pszErrorMessage, HRESULT hrErr)
    {
        const size_t MESSAGE_LEN = 512;
        WCHAR message[MESSAGE_LEN];

        if (SUCCEEDED(StringCchPrintf(message, MESSAGE_LEN, L"%s (HRESULT = 0x%X)",
            pszErrorMessage, hrErr)))
        {
            MessageBox(hWnd, message, NULL, MB_OK | MB_ICONERROR);
        }
    }

private:

    void DrawBorders(int rtWidth, int rtHeight) const;
    void SetWindowPosition();
    void SetFullscreen();
    void SetTransform() const;
    RectInt GetRenderTargetSize() const;
    HRESULT OnRender();
    HRESULT MakeWindow();
    HRESULT CreateRenderTarget();

    //Win32
    HWND m_hWindow;
    HINSTANCE m_hInstance;

    //Direct2D
    ID2D1Factory* m_pDFactory;
    ID2D1HwndRenderTarget*  m_pDRenderTarget;
    ID2D1SolidColorBrush* m_pDColorBrush;
    D2D1_COLOR_F m_DColorBackGround;

    //BaseGame
    BaseGame* m_pGame;

    //Transform
    FLOAT m_ViewPortTranslationX{};
    FLOAT m_ViewPortTranslationY{};
    FLOAT m_ViewPortScaling{};

    FLOAT m_ScalingX{ 1 };
    FLOAT m_ScalingY{ 1 };
    FLOAT m_PointToScaleFromX{};
    FLOAT m_PointToScaleFromY{};

    FLOAT m_TranslationX{};
    FLOAT m_TranslationY{};

    FLOAT m_Rotation{};
    FLOAT m_PivotPointX{};
    FLOAT m_PivotPointY{};

    bool m_TranslationBeforeRotation{};
    mutable bool m_TransformChanged{};

    //General datamembers
    std::wstring m_ResourcePath;
    tstring m_Title;
    int m_Width;
    int m_Height;

    float m_MilliSecondsPerFrame;

    bool m_IsFullscreen;
    bool m_KeyIsDown;

};





class Texture final
{
public:
    explicit Texture(const std::wstring& filename);

    Texture(const Texture& other) = delete;
    Texture(Texture&& other) noexcept = delete;
    Texture& operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) noexcept = delete;

    ~Texture();

    ID2D1Bitmap* const  GetBitmap() const { return m_pDBitmap; }
    float GetWidth() const { return m_TextureWidth; }
    float GetHeight() const { return m_TextureHeight; }

private:

    static IWICImagingFactory* m_pWICFactory;
    ID2D1Bitmap* m_pDBitmap;

    float m_TextureWidth;
    float m_TextureHeight;
};





//https://stackoverflow.com/questions/37572961/c-directwrite-load-font-from-file-at-runtime
class Font final
{
public:
    Font(const std::wstring& fontname, bool fromFile = false);
    Font(const std::wstring& fontname, int size, bool bold, bool italic, bool fromFile = false);

    Font(const Font& other) = delete;
    Font(Font&& other) noexcept = delete;
    Font& operator=(const Font& other) = delete;
    Font& operator=(Font&& other) noexcept = delete;

    ~Font();

    void SetTextFormat(int size, bool bold, bool italic);
    IDWriteTextFormat* GetFormat() const;
    int GetFontSize() const;

private:
    HRESULT Initialize(const std::wstring& filename);

    static IDWriteFactory5* m_pDWriteFactory;

    IDWriteFontCollection1* m_pFontCollection;
    IDWriteTextFormat* m_pTextFormat;

    tstring m_FontName;
    int m_FontSize;
};


#endif // !ENGINE_H