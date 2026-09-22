#include "RenderManager.h"

#include <numbers>

#include "Engine.h"

namespace jela
{
    void RenderManager::SetColor(COLORREF newColor, float opacity) const
    {
        m_DXHandler.dDeviceContext2D.SetDrawColor(newColor, opacity);
    }
    void RenderManager::SetBackGroundColor(COLORREF newColor, float opacity)
    {
        m_DXHandler.dDeviceContext2D.SetBackgroundColor(newColor, opacity);
    }

    //lines
    void RenderManager::DrawLine(const Point2f& firstPoint, const Point2f& secondPoint, float lineThickness) const
    {
        DrawLine(firstPoint.x, firstPoint.y, secondPoint.x, secondPoint.y, lineThickness);
    }
    void RenderManager::DrawLine(float firstX, float firstY, const Point2f& secondPoint, float lineThickness) const
    {
        DrawLine(firstX, firstY, secondPoint.x, secondPoint.y, lineThickness);
    }
    void RenderManager::DrawLine(const Point2f& firstPoint, float secondX, float secondY, float lineThickness) const
    {
        DrawLine(firstPoint.x, firstPoint.y, secondX, secondY, lineThickness);
    }
    void RenderManager::DrawLine(float firstX, float firstY, float secondX, float secondY, float lineThickness) const
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM)
        {
            firstY = m_pWindow->GetGameSize().y - firstY;
            secondY = m_pWindow->GetGameSize().y - secondY;
        }

        m_DXHandler.dDeviceContext2D.DrawLine(firstX, firstY, secondX, secondY, lineThickness);
    }

    void RenderManager::DrawVector(const Point2f& origin, const Vector2f& vector, float headLineLength, float lineThickness) const
    {
        DrawVector(origin.x, origin.y, vector.x, vector.y, headLineLength, lineThickness);
    }
    void RenderManager::DrawVector(const Point2f& origin, float vectorX, float vectorY, float headLineLength, float lineThickness) const
    {
        DrawVector(origin.x, origin.y, vectorX, vectorY, headLineLength, lineThickness);
    }
    void RenderManager::DrawVector(float originX, float originY, const Vector2f& vector, float headLineLength, float lineThickness) const
    {
        DrawVector(originX, originY, vector.x, vector.y, headLineLength, lineThickness);
    }
    void RenderManager::DrawVector(float originX, float originY, float vectorX, float vectorY, float headLineLength, float lineThickness) const
    {
        const float endX = originX + vectorX;
        const float endY = originY + vectorY;

        constexpr float desiredHeadAngle = std::numbers::pi_v<float> / 12.f;
        const float flippedVectorAngle = atan2f(-vectorY, -vectorX);

        const Point2f arrowP2{ endX + cosf(flippedVectorAngle - desiredHeadAngle) * headLineLength,
                                endY + sinf(flippedVectorAngle - desiredHeadAngle) * headLineLength };

        const Point2f arrowP3{ endX + cosf(flippedVectorAngle + desiredHeadAngle) * headLineLength,
                                endY + sinf(flippedVectorAngle + desiredHeadAngle) * headLineLength };

        DrawLine(originX, originY, endX, endY, lineThickness);
        DrawLine(endX, endY, arrowP2.x, arrowP2.y, lineThickness);
        DrawLine(endX, endY, arrowP3.x, arrowP3.y, lineThickness);
    }


#ifdef MATHEMATICAL_COORDINATESYSTEM

    //Rectangles
    void RenderManager::DrawRectangle(const Point2f& leftBottom, float width, float height, float lineThickness) const
    {
        DrawRectangle( leftBottom.x, leftBottom.y, width, height, lineThickness);
    }
    void RenderManager::DrawRectangle(const Rectf& rect, float lineThickness) const
    {
        DrawRectangle( rect.left, rect.bottom ,rect.width, rect.height, lineThickness);
    }

    void RenderManager::DrawRectangle(float left, float bottom, float width, float height, float lineThickness) const
    {
        m_DXHandler.dDeviceContext2D.DrawRectangle(
            left,         m_pWindow->GetGameSize().y - (bottom + height),
            left + width, m_pWindow->GetGameSize().y - bottom,
            lineThickness);
    }

    //RoundedRects
    void RenderManager::DrawRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY, float lineThickness) const
    {
        DrawRoundedRect(leftBottom.x, leftBottom.y, width, height, radiusX, radiusY, lineThickness);
    }
    void RenderManager::DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness) const
    {
        DrawRoundedRect(rect.left, rect.bottom, rect.width, rect.height, radiusX, radiusY, lineThickness);
    }

    void RenderManager::DrawRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY, float lineThickness) const
    {
        m_DXHandler.dDeviceContext2D.DrawRoundedRect(
            left,         m_pWindow->GetGameSize().y - (bottom + height),
            left + width, m_pWindow->GetGameSize().y - bottom,
            radiusX,radiusY,
            lineThickness
        );
    }

    // Strings
    void RenderManager::DrawString(const tstring& textToDisplay, const Point2f& leftBottom, float width, float height, bool showRect) const
    {
        DrawString(textToDisplay, leftBottom.x, leftBottom.y, width, height, showRect);
    }
    void RenderManager::DrawString(const tstring& textToDisplay, const Rectf& destRect, bool showRect) const
    {
        DrawString(textToDisplay, destRect.left, destRect.bottom, destRect.width, destRect.height, showRect);
    }
    void RenderManager::DrawString(const tstring& textToDisplay, float left, float bottom, float width, float height, bool showRect) const
    {
        const auto [l, t, r, b] = D2D1::RectF(
            left,
            m_pWindow->GetGameSize().y - (bottom + height),
            left + width,
            m_pWindow->GetGameSize().y - bottom);

        if (showRect) m_DXHandler.dDeviceContext2D.DrawRectangle(l, t, r, b);

        m_DXHandler.dDeviceContext2D.DrawString(
            to_wstring(textToDisplay),
            l, t, r, b,
            ENGINE.ResourceMngr()->GetCurrentTextFormat()->GetTextFormat()
            );
    }

    void RenderManager::DrawString(const tstring& textToDisplay, const Point2f& leftBottom, float width, bool showRect) const
    {
       DrawString(textToDisplay, leftBottom.x, leftBottom.y, width, showRect);
    }
    void RenderManager::DrawString(const tstring& textToDisplay, float left, float bottom, float width, bool showRect) const
    {
        DrawString(textToDisplay, left, bottom, width, ENGINE.ResourceMngr()->GetCurrentTextFormat()->GetFontSize(), showRect);
    }

    //Textures
    void RenderManager::DrawTexture(const Texture* texture, float destLeft, float destBottom, const Rectf& srcRect, float opacity) const
    {
        DrawTexture(texture, Rectf{ destLeft, destBottom, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void RenderManager::DrawTexture(const Texture* texture, const Point2f& destLeftBottom, const Rectf& srcRect, float opacity) const
    {
        DrawTexture(texture, Rectf{ destLeftBottom.x, destLeftBottom.y, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void RenderManager::DrawTexture(const Texture* texture, const Rectf& destRect, const Rectf& srcRect, float opacity) const
    {
        const Vector2f& wndwSize = m_pWindow->GetGameSize();

        const D2D1_RECT_F destination = D2D1::RectF(
            destRect.left,
            wndwSize.y - (destRect.bottom + destRect.height),
            destRect.left + destRect.width,
            wndwSize.y - destRect.bottom
        );

        if (!texture)
        {
            m_DXHandler.dDeviceContext2D.FillRectangle(destination.left, destination.top, destination.right, destination.bottom);
            OutputDebugString(_T("ERROR! Texture was nullptr in DrawTexture!\n"));
            return;
        }

        const Vector2f& texSize = texture->GetSize();
        D2D1_RECT_F source{};
        if (srcRect.width <= 0 || srcRect.height <= 0)
        {
            source = D2D1::RectF(0.F,0.F,texSize.x,texSize.y);
        }
        else
        {
            constexpr float sliceMargin{ 0.07f };
            source = D2D1::RectF(
                srcRect.left + sliceMargin,
                texSize.y - (srcRect.bottom  + srcRect.height + sliceMargin),
                srcRect.left + srcRect.width - sliceMargin,
                texSize.y - (srcRect.bottom + sliceMargin));
        }

        m_DXHandler.dDeviceContext2D.DrawTexture(
            texture->GetBitmap(),
            destination, source,
            opacity
        );
    }


    //-----------------
    //Fill
    //----------------

    //Rectangles
    void RenderManager::FillRectangle(const Point2f& leftBottom, float width, float height) const
    {
        FillRectangle(leftBottom.x, leftBottom.y, width, height);
    }
    void RenderManager::FillRectangle(const Rectf& rect) const
    {
        FillRectangle(rect.left, rect.bottom, rect.width, rect.height);
    }
    void RenderManager::FillRectangle(float left, float bottom, float width, float height) const
    {
        m_DXHandler.dDeviceContext2D.FillRectangle(
            left,         m_pWindow->GetGameSize().y - (bottom + height),
            left + width, m_pWindow->GetGameSize().y - bottom
            );
    }

    //RoundedRects
    void RenderManager::FillRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY) const
    {
        m_DXHandler.dDeviceContext2D.FillRoundedRect(
            left,         m_pWindow->GetGameSize().y - (bottom + height),
            left + width, m_pWindow->GetGameSize().y - bottom,
            radiusX, radiusY
            );
    }
    void RenderManager::FillRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY) const
    {
        FillRoundedRect(leftBottom.x, leftBottom.y, width, height, radiusX, radiusY);
    }
    void RenderManager::FillRoundedRect(const Rectf& rect, float radiusX, float radiusY) const
    {
        FillRoundedRect(rect.left, rect.bottom, rect.width, rect.height, radiusX, radiusY);
    }

#else

    //Rectangles

    void RenderManager::DrawRectangle(const Point2f& leftTop, float width, float height, float lineThickness) const
    {
        DrawRectangle(leftTop.x, leftTop.y, width, height, lineThickness);
    }
    void RenderManager::DrawRectangle(const Rectf& rect, float lineThickness) const
    {
        DrawRectangle(rect.left, rect.top, rect.width, rect.height, lineThickness);
    }
    void RenderManager::DrawRectangle(float left, float top, float width, float height, float lineThickness) const
    {
        m_DXHandler.dDeviceContext2D.DrawRectangle(left, top, left + width, top + height, lineThickness);
    }

    //RoundedRects
    void RenderManager::DrawRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY, float lineThickness) const
    {
        DrawRoundedRect(leftTop.x, leftTop.y, width, height, radiusX, radiusY, lineThickness);
    }
    void RenderManager::DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness) const
    {
        DrawRoundedRect(rect.left, rect.top, rect.width, rect.height, radiusX, radiusY, lineThickness);
    }
    void RenderManager::DrawRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY, float lineThickness) const
    {
        m_DXHandler.dDeviceContext2D.DrawRoundedRect(
            left, top, left + width, top + height,
            radiusX,radiusY,
            lineThickness
        );
    }

    //String
    void RenderManager::DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, float height, bool showRect) const
    {
        DrawString(textToDisplay, leftTop.x, leftTop.y, width, height, showRect);
    }
    void RenderManager::DrawString(const tstring& textToDisplay, const Rectf& destRect, bool showRect) const
    {
        DrawString(textToDisplay, destRect.left, destRect.top, destRect.width, destRect.height, showRect);
    }
    void RenderManager::DrawString(const tstring& textToDisplay, float left, float top, float width, float height, bool showRect) const
    {
        auto [l,t,r,b] = D2D1::RectF(left, top, left + width, top + height);

        if (showRect) m_DXHandler.dDeviceContext2D.DrawRectangle(l, t, r, b);

        m_DXHandler.dDeviceContext2D.DrawString(
            to_wstring(textToDisplay),
            l,t,r,b,
            ENGINE.ResourceMngr()->GetCurrentTextFormat()->GetTextFormat()
            );
    }

    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void RenderManager::DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, bool showRect) const
    {
        DrawString(textToDisplay, leftTop.x, leftTop.y, width, showRect);
    }
    //Takes the size of the font as Height of the destination rectangle in order to have a logical position
    void RenderManager::DrawString(const tstring& textToDisplay, float left, float top, float width, bool showRect) const
    {
        DrawString(textToDisplay, left, top, width, ENGINE.ResourceMngr()->GetCurrentTextFormat()->GetFontSize(), showRect);
    }


    //Textures
    void RenderManager::DrawTexture(const Texture* texture, float destLeft, float destTop, const Rectf& srcRect, float opacity) const
    {
        DrawTexture(texture, Rectf{ destLeft, destTop, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void RenderManager::DrawTexture(const Texture* texture, const Point2f& destLeftTop, const Rectf& srcRect, float opacity) const
    {
        DrawTexture(texture, Rectf{ destLeftTop.x, destLeftTop.y, texture ? texture->GetWidth() : 50, texture ? texture->GetHeight() : 50 }, srcRect, opacity);
    }
    void RenderManager::DrawTexture(const Texture* texture, const Rectf& destRect, const Rectf& srcRect, float opacity) const
    {
        const D2D1_RECT_F destination = D2D1::RectF(destRect.left, destRect.top, destRect.left + destRect.width, destRect.top + destRect.height);

        if (!texture)
        {
            m_DXHandler.dDeviceContext2D.FillRectangle(destination.left, destination.top, destination.right, destination.bottom);
            OutputDebugString(_T("ERROR! Texture was nullptr in DrawTexture!\n"));
            return;
        }

        D2D1_RECT_F source{};
        if (srcRect.width <= 0 || srcRect.height <= 0)
        {
            source = D2D1::RectF(0.F, 0.F, texture->GetWidth(), texture->GetHeight());
        }
        else
        {
            constexpr float sliceMargin{ 0.07f };
            source = D2D1::RectF(
                srcRect.left + sliceMargin,
                srcRect.top + sliceMargin,
                srcRect.left + srcRect.width - sliceMargin,
                srcRect.top + srcRect.height - sliceMargin
            );
        }
        m_DXHandler.dDeviceContext2D.DrawTexture(texture->GetBitmap(), destination, source, opacity);

    }

    //-----------------
    //Fill
    //-----------------

    //Rectangles
    void RenderManager::FillRectangle(const Point2f& leftTop, float width, float height) const
    {
        FillRectangle(leftTop.x, leftTop.y, width, height);
    }
    void RenderManager::FillRectangle(const Rectf& rect) const
    {
        FillRectangle(rect.left, rect.top, rect.width, rect.height);
    }
    void RenderManager::FillRectangle(float left, float top, float width, float height) const
    {
        m_DXHandler.dDeviceContext2D.FillRectangle(left, top, left + width, top + height);
    }

    //RoundedRects
    void RenderManager::FillRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY) const
    {
        FillRoundedRect(leftTop.x, leftTop.y, width, height, radiusX, radiusY);
    }
    void RenderManager::FillRoundedRect(const Rectf& rect, float radiusX, float radiusY) const
    {
        FillRoundedRect( rect.left, rect.top, rect.width, rect.height, radiusX, radiusY);
    }
    void RenderManager::FillRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY) const
    {
        m_DXHandler.dDeviceContext2D.FillRoundedRect(
            left, top, left + width, top + height,
            radiusX, radiusY
            );
    }

#endif // MATHEMATICAL_COORDINATESYSTEM

    //Ellipse
    void RenderManager::DrawEllipse(const Point2f& center, float radiusX, float radiusY, float lineThickness) const
    {
        DrawEllipse(center.x, center.y, radiusX, radiusY, lineThickness);
    }
    void RenderManager::DrawEllipse(const Ellipsef& ellipse, float lineThickness) const
    {
        DrawEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY, lineThickness);
    }
    void RenderManager::DrawCircle(const Circlef& circle, float lineThickness) const
    {
        DrawEllipse(circle.center, circle.rad, circle.rad, lineThickness);
    }
    void RenderManager::DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineThickness) const
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM) centerY = m_pWindow->GetGameSize().y - centerY;

        m_DXHandler.dDeviceContext2D.DrawEllipse(
            centerX, centerY,
            radiusX, radiusY,
            lineThickness
        );
    }

    void RenderManager::FillEllipse(const Point2f& center, float radiusX, float radiusY) const
    {
        FillEllipse(center.x, center.y, radiusX, radiusY);
    }
    void RenderManager::FillEllipse(const Ellipsef& ellipse) const
    {
        FillEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY);
    }
    void RenderManager::FillCircle(const Circlef& circle) const
    {
        FillEllipse(circle.center, circle.rad, circle.rad);
    }
    void RenderManager::FillEllipse(float centerX, float centerY, float radiusX, float radiusY) const
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM) centerY = m_pWindow->GetGameSize().y - centerY;

        m_DXHandler.dDeviceContext2D.FillEllipse(
            centerX, centerY,
            radiusX, radiusY
            );
    }

    //Geometry
    void RenderManager::DrawPolygon(const Polygon& polygon, float lineThickness)
    {
        DrawGeometry(&polygon, lineThickness);
    }
    void RenderManager::FillPolygon(const Polygon& polygon)
    {
        FillGeometry(&polygon);
    }
    void RenderManager::DrawArc(const Arc& arc, float lineThickness)
    {
        DrawGeometry(&arc, lineThickness);
    }
    void RenderManager::FillArc(const Arc& arc)
    {
        FillGeometry(&arc);
    }

    void RenderManager::DrawGeometry(const Geometry* const pGeometryObject, float lineThickness)
    {
        PushTransform();
        Translate(pGeometryObject->GetTranslation());
        m_DXHandler.dDeviceContext2D.DrawGeometry(pGeometryObject->GetGeometry(), lineThickness);
        PopTransform();
    }
    void RenderManager::FillGeometry(const Geometry* const pGeometryObject)
    {
        PushTransform();
        Translate(pGeometryObject->GetTranslation());
        m_DXHandler.dDeviceContext2D.FillGeometry(pGeometryObject->GetGeometry());
        PopTransform();
    }


    void RenderManager::Translate(float xTranslation, float yTranslation)
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM) yTranslation *= -1;
        m_DXHandler.dDeviceContext2D.Translate(xTranslation, yTranslation);
    }
    void RenderManager::Rotate(float angle, float xPivotPoint, float yPivotPoint)
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM) yPivotPoint = m_pWindow->GetGameSize().y - yPivotPoint;
        m_DXHandler.dDeviceContext2D.Rotate(angle, xPivotPoint, yPivotPoint);
    }
    void RenderManager::Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom)
    {
        if (USE_MATHEMATICAL_COORDINATESYSTEM) yPointToScaleFrom = m_pWindow->GetGameSize().y - yPointToScaleFrom;
        m_DXHandler.dDeviceContext2D.Scale(xScale, yScale, xPointToScaleFrom, yPointToScaleFrom);
    }
    void RenderManager::PushTransform()
    {
        m_DXHandler.dDeviceContext2D.PushTransform();
    }
    void RenderManager::PopTransform()
    {
        m_DXHandler.dDeviceContext2D.PopTransform();
    }
    void RenderManager::Translate(const Vector2f& translation)
    {
        Translate(translation.x, translation.y);
    }
    void RenderManager::Rotate(float angle, const Point2f& pivotPoint)
    {
        Rotate(angle, pivotPoint.x, pivotPoint.y);
    }
    void RenderManager::Scale(float scale, float xPointToScaleFrom, float yPointToScaleFrom)
    {
        Scale(scale, scale, xPointToScaleFrom, yPointToScaleFrom);
    }
    void RenderManager::Scale(float xScale, float yScale, const Point2f& PointToScaleFrom)
    {
        Scale(xScale, yScale, PointToScaleFrom.x, PointToScaleFrom.y);
    }
    void RenderManager::Scale(float scale, const Point2f& PointToScaleFrom)
    {
        Scale(scale, scale, PointToScaleFrom.x, PointToScaleFrom.y);
    }
    void RenderManager::Scale(float xScale, float yScale)
    {
        Scale(xScale, yScale, 0, 0);
    }
    void RenderManager::Scale(float scale)
    {
        Scale(scale, 0, 0);
    }
    void RenderManager::Render() const
    {
        auto hr = OnRender();
    }
    void RenderManager::ResizeBackBuffer()
    {
        auto hr = OnResizeBackBuffer();
    }
    const DX::Factory2D& RenderManager::Get2DFactory() const
    {
        return m_DXHandler.dFactory2D;
    }
    const DX::DeviceContext2D& RenderManager::Get2DDeviceContext() const
    {
        return m_DXHandler.dDeviceContext2D;
    }

    HResultHandler RenderManager::OnRender() const
    {
        HResultHandler hr{S_OK, _T("ENGINE::OnRender")};

        hr = m_DXHandler.dDeviceContext2D.Draw([]{ ENGINE.SceneMngr()->Draw(); });
        hr = m_DXHandler.dSwapChain.Present();

        ValidateRect(m_pWindow->GetWindow(), nullptr);
        return hr;
    }
    HResultHandler RenderManager::OnResizeBackBuffer()
    {
        OutputDebugString(_T("ResizeWindow\n"));
        HResultHandler hr{S_OK, _T("ENGINE::ResizeWindow")};
        hr = m_DXHandler.ResizeWindow();
        return hr;
    }
}
