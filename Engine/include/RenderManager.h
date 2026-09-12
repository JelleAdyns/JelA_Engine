#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H
#include "DirectXObjects.h"
#include "GameWindow.h"
#include "Geometry.h"
#include "ResourceManager.h"


namespace jela
{
    class RenderManager
    {
    public:
        explicit RenderManager(GameWindow* pWindow):
            m_pWindow{pWindow}
        {}

        void SetColor(COLORREF newColor, float opacity = 1.F) const;
        void SetBackGroundColor(COLORREF newColor, float opacity = 1.F);

        void DrawLine(const Point2f& firstPoint, const Point2f& secondPoint, float lineThickness = 1.f) const;
        void DrawLine(float firstX, float firstY, const Point2f& secondPoint, float lineThickness = 1.f) const;
        void DrawLine(const Point2f& firstPoint, float secondX, float secondY, float lineThickness = 1.f) const;
        void DrawLine(float firstX, float firstY, float secondX, float secondY, float lineThickness = 1.f) const;

        void DrawVector(const Point2f& origin, const Vector2f& vector, float headLineLength = 30.f, float lineThickness = 1.f) const;
        void DrawVector(const Point2f& origin, float vectorX, float vectorY, float headLineLength = 30.f, float lineThickness = 1.f) const;
        void DrawVector(float originX, float originY, const Vector2f& vector, float headLineLength = 30.f, float lineThickness = 1.f) const;
        void DrawVector(float originX, float originY, float vectorX, float vectorY, float headLineLength = 30.f, float lineThickness = 1.f) const;

#ifdef MATHEMATICAL_COORDINATESYSTEM
        void DrawRectangle(const Point2f& leftBottom, float width, float height, float lineThickness = 1.f) const;
        void DrawRectangle(const Rectf& rect, float lineThickness = 1.f) const;
        void DrawRectangle(float left, float bottom, float width, float height, float lineThickness = 1.f) const;

        void DrawRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY, float lineThickness = 1.f) const;
        void DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness = 1.f) const;
        void DrawRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY, float lineThickness = 1.f) const;

        void DrawString(const tstring& textToDisplay, const Point2f& leftBottom, float width, float height, bool showRect = false) const;
        void DrawString(const tstring& textToDisplay, const Rectf& destRect, bool showRect = false) const;
        void DrawString(const tstring& textToDisplay, float left, float bottom, float width, float height, bool showRect = false) const;

        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, const Point2f& leftBottom, float width, bool showRect = false) const;
        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, float left, float bottom, float width, bool showRect = false) const;

        void DrawTexture(const Texture* texture, float destLeft, float destBottom, const Rectf& srcRect = {}, float opacity = 1.f) const;
        void DrawTexture(const Texture* texture, const Point2f& destLeftBottom = {}, const Rectf& srcRect = {}, float opacity = 1.f) const;
        void DrawTexture(const Texture* texture, const Rectf& destRect, const Rectf& srcRect = {}, float opacity = 1.f) const;

        void FillRectangle(const Point2f& leftBottom, float width, float height) const;
        void FillRectangle(const Rectf& rect) const;
        void FillRectangle(float left, float bottom, float width, float height) const;

        void FillRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY) const;
        void FillRoundedRect(const Rectf& rect, float radiusX, float radiusY) const;
        void FillRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY) const;
#else
        void DrawRectangle(const Point2f& leftTop, float width, float height, float lineThickness = 1.f) const;
        void DrawRectangle(const Rectf& rect, float lineThickness = 1.f) const;
        void DrawRectangle(float left, float top, float width, float height, float lineThickness = 1.f) const;

        void DrawRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY, float lineThickness = 1.f) const;
        void DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness = 1.f) const;
        void DrawRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY, float lineThickness = 1.f) const;

        void DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, float height, bool showRect = false) const;
        void DrawString(const tstring& textToDisplay, const Rectf& destRect, bool showRect = false) const;
        void DrawString(const tstring& textToDisplay, float left, float top, float width, float height, bool showRect = false) const;

        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, bool showRect = false) const;
        //Takes the size of the font as Height of the destination rectangle in order to have a logical position
        void DrawString(const tstring& textToDisplay, float left, float top, float width, bool showRect = false) const;

        void DrawTexture(const Texture* texture, float destLeft, float destTop, const Rectf& srcRect = {}, float opacity = 1.f) const;
        void DrawTexture(const Texture* texture, const Point2f& destLeftTop = {}, const Rectf& srcRect = {}, float opacity = 1.f) const;
        void DrawTexture(const Texture* texture, const Rectf& destRect, const Rectf& srcRect = {}, float opacity = 1.f) const;

        void FillRectangle(const Point2f& leftTop, float width, float height) const;
        void FillRectangle(const Rectf& rect) const;
        void FillRectangle(float left, float top, float width, float height) const;

        void FillRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY) const;
        void FillRoundedRect(const Rectf& rect, float radiusX, float radiusY) const;
        void FillRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY) const;
#endif // MATHEMATICAL_COORDINATESYSTEM

        void DrawPolygon(const Polygon& polygon, float lineThickness = 1.f);
        void FillPolygon(const Polygon& polygon);

        void DrawArc(const Arc& arc, float lineThickness = 1.f);
        void FillArc(const Arc& arc);

        void DrawEllipse(const Point2f& center, float radiusX, float radiusY, float lineThickness = 1.f) const;
        void DrawEllipse(const Ellipsef& ellipse, float lineThickness = 1.f) const;
        void DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineThickness = 1.f) const;
        void DrawCircle(const Circlef& circle, float lineThickness = 1.f) const;

        void FillEllipse(const Point2f& center, float radiusX, float radiusY) const;
        void FillEllipse(const Ellipsef& ellipse) const;
        void FillEllipse(float centerX, float centerY, float radiusX, float radiusY) const;
        void FillCircle(const Circlef& circle) const;

        void PushTransform();
        void PopTransform();
        void Translate(float xTranslation, float yTranslation);
        void Translate(const Vector2f& translation);
        void Rotate(float angle, float xPivotPoint, float yPivotPoint);
        void Rotate(float angle, const Point2f& pivotPoint);
        void Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom);
        void Scale(float scale, float xPointToScaleFrom, float yPointToScaleFrom);
        void Scale(float xScale, float yScale, const Point2f& PointToScaleFrom);
        void Scale(float scale, const Point2f& PointToScaleFrom);
        void Scale(float xScale, float yScale);
        void Scale(float scale);

        void Render() const;
        void ResizeBackBuffer();
        const DX::Factory2D& Get2DFactory() const;
        const DX::DeviceContext2D& Get2DDeviceContext() const;
    private:
        HResultHandler OnRender() const;
        HResultHandler OnResizeBackBuffer();
        void DrawGeometry(const Geometry* pGeometryObject, float lineThickness = 1.f);
        void FillGeometry(const Geometry* pGeometryObject);


        DX::DXHandler m_DXHandler{};
        GameWindow* m_pWindow;
    };
}


#endif //RENDERMANAGER_H
