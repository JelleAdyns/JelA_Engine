#include "JelA/Managers/RenderManager.h"

#include <numbers>
#include "JelA/Geometry.h"
#include "JelA/Engine.h"
#include "JelA/GameWindow.h"

namespace jela
{
    RenderManager::RenderManager(GameWindow* pWindow):
        m_pWindow{pWindow}
    {}
    void RenderManager::SetColor(Color newColor) const
    {
        m_DXHandler.dDeviceContext2D.SetDrawColor(newColor);
    }
    void RenderManager::SetBackGroundColor(Color newColor)
    {
        m_DXHandler.dDeviceContext2D.SetBackgroundColor(newColor);
    }

    void RenderManager::DrawLine(const Point2f& firstPoint, const Point2f& secondPoint, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawLine(firstPoint.x, firstPoint.y, secondPoint.x, secondPoint.y, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawLine(float firstX, float firstY, const Point2f& secondPoint, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawLine(firstX, firstY, secondPoint.x, secondPoint.y, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawLine(const Point2f& firstPoint, float secondX, float secondY, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawLine(firstPoint.x, firstPoint.y, secondX, secondY, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawLine(float firstX, float firstY, float secondX, float secondY, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        m_DXHandler.dDeviceContext2D.DrawLine(firstX, firstY, secondX, secondY, lineThickness, ConvertStrokeStyle(pStrokeStyle));
    }

    void RenderManager::DrawVector(const Point2f& origin, const Vector2f& vector, float headLineLength, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawVector(origin.x, origin.y, vector.x, vector.y, headLineLength, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawVector(const Point2f& origin, float vectorX, float vectorY, float headLineLength, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawVector(origin.x, origin.y, vectorX, vectorY, headLineLength, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawVector(float originX, float originY, const Vector2f& vector, float headLineLength, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawVector(originX, originY, vector.x, vector.y, headLineLength, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawVector(float originX, float originY, float vectorX, float vectorY, float headLineLength, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        const float endX = originX + vectorX;
        const float endY = originY + vectorY;

        constexpr float desiredHeadAngle = std::numbers::pi_v<float> / 12.f;
        const float flippedVectorAngle = atan2f(-vectorY, -vectorX);

        const Point2f arrowP2{ endX + cosf(flippedVectorAngle - desiredHeadAngle) * headLineLength,
                                endY + sinf(flippedVectorAngle - desiredHeadAngle) * headLineLength };

        const Point2f arrowP3{ endX + cosf(flippedVectorAngle + desiredHeadAngle) * headLineLength,
                                endY + sinf(flippedVectorAngle + desiredHeadAngle) * headLineLength };

        DrawLine(originX, originY, endX, endY, lineThickness, pStrokeStyle);
        DrawLine(endX, endY, arrowP2.x, arrowP2.y, lineThickness, pStrokeStyle);
        DrawLine(endX, endY, arrowP3.x, arrowP3.y, lineThickness, pStrokeStyle);
    }

#ifdef MATHEMATICAL_COORDINATESYSTEM

    void RenderManager::DrawRectangle(const Point2f& leftBottom, float width, float height, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawRectangle( leftBottom.x, leftBottom.y, width, height, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawRectangle(const Rectf& rect, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawRectangle( rect.left, rect.bottom ,rect.width, rect.height, lineThickness, pStrokeStyle);
    }

    void RenderManager::DrawRectangle(float left, float bottom, float width, float height, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        m_DXHandler.dDeviceContext2D.DrawRectangle(
            left,         bottom ,
            left + width, bottom + height,
            lineThickness,
            ConvertStrokeStyle(pStrokeStyle));
    }

    void RenderManager::DrawRoundedRect(const Point2f& leftBottom, float width, float height, float radiusX, float radiusY, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawRoundedRect(leftBottom.x, leftBottom.y, width, height, radiusX, radiusY, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawRoundedRect(rect.left, rect.bottom, rect.width, rect.height, radiusX, radiusY, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawRoundedRect(const RoundedRectf& rect, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawRoundedRect(rect.left, rect.bottom, rect.width, rect.height, rect.xRadius, rect.yRadius, lineThickness, pStrokeStyle);
    }

    void RenderManager::DrawRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        m_DXHandler.dDeviceContext2D.DrawRoundedRect(
          left,         bottom,
          left + width, bottom + height,
          radiusX,radiusY,
          lineThickness,
          ConvertStrokeStyle(pStrokeStyle)
        );
    }

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
            left,         bottom ,
            left + width, bottom + height);

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
        const D2D1_RECT_F destination = D2D1::RectF(
                destRect.left,
                destRect.bottom,
                destRect.left + destRect.width,
                destRect.bottom + destRect.height
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
                texSize.y - (srcRect.bottom  + srcRect.height) + sliceMargin,
                srcRect.left + srcRect.width - sliceMargin,
                texSize.y - srcRect.bottom - sliceMargin);
        }

        m_DXHandler.dDeviceContext2D.DrawTexture(
            texture->GetBitmap(),
            destination, source,
            opacity
        );
    }

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
           left,         bottom,
           left + width, bottom + height
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
    void RenderManager::FillRoundedRect(const RoundedRectf& rect) const
    {
        FillRoundedRect(rect.left, rect.bottom, rect.width, rect.height, rect.xRadius, rect.yRadius);
    }
    void RenderManager::FillRoundedRect(float left, float bottom, float width, float height, float radiusX, float radiusY) const
    {
        m_DXHandler.dDeviceContext2D.FillRoundedRect(
           left,         bottom ,
           left + width, bottom + height,
           radiusX, radiusY
           );
    }

#else

    void RenderManager::DrawRectangle(const Point2f& leftTop, float width, float height, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawRectangle(leftTop.x, leftTop.y, width, height, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawRectangle(const Rectf& rect, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawRectangle(rect.left, rect.top, rect.width, rect.height, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawRectangle(float left, float top, float width, float height, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        m_DXHandler.dDeviceContext2D.DrawRectangle(left, top, left + width, top + height, lineThickness, ConvertStrokeStyle(pStrokeStyle));
    }

    void RenderManager::DrawRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawRoundedRect(leftTop.x, leftTop.y, width, height, radiusX, radiusY, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawRoundedRect(const Rectf& rect, float radiusX, float radiusY, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawRoundedRect(rect.left, rect.top, rect.width, rect.height, radiusX, radiusY, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawRoundedRect(const RoundedRectf& rect, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawRoundedRect(rect.left, rect.top, rect.width, rect.height, rect.xRadius, rect.yRadius, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        m_DXHandler.dDeviceContext2D.DrawRoundedRect(
           left, top, left + width, top + height,
           radiusX,radiusY,
           lineThickness,
           ConvertStrokeStyle(pStrokeStyle)
        );
    }

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

    void RenderManager::DrawString(const tstring& textToDisplay, const Point2f& leftTop, float width, bool showRect) const
    {
        DrawString(textToDisplay, leftTop.x, leftTop.y, width, showRect);
    }
    void RenderManager::DrawString(const tstring& textToDisplay, float left, float top, float width, bool showRect) const
    {
        DrawString(textToDisplay, left, top, width, ENGINE.ResourceMngr()->GetCurrentTextFormat()->GetFontSize(), showRect);
    }

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
        const D2D1_RECT_F destination
        {
            D2D1::RectF(destRect.left, destRect.top, destRect.left + destRect.width, destRect.top + destRect.height)
        };

        if (!texture)
        {
            m_DXHandler.dDeviceContext2D.FillRectangle(destination.left, destination.top, destination.right, destination.bottom);
            OutputDebugString(_T("ERROR! Texture was nullptr in DrawTexture!\n"));
            return;
        }

        const Vector2f texSize = texture->GetSize();
        D2D1_RECT_F source{};
        if (srcRect.width <= 0 || srcRect.height <= 0)
        {
            source = D2D1::RectF(0.F, 0.F, texSize.x, texSize.y);
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

    void RenderManager::FillRoundedRect(const Point2f& leftTop, float width, float height, float radiusX, float radiusY) const
    {
        FillRoundedRect(leftTop.x, leftTop.y, width, height, radiusX, radiusY);
    }
    void RenderManager::FillRoundedRect(const Rectf& rect, float radiusX, float radiusY) const
    {
        FillRoundedRect( rect.left, rect.top, rect.width, rect.height, radiusX, radiusY);
    }
    void RenderManager::FillRoundedRect(const RoundedRectf& rect) const
    {
        FillRoundedRect( rect.left, rect.top, rect.width, rect.height, rect.xRadius, rect.yRadius);
    }
    void RenderManager::FillRoundedRect(float left, float top, float width, float height, float radiusX, float radiusY) const
    {
        m_DXHandler.dDeviceContext2D.FillRoundedRect(left, top, left + width, top + height, radiusX, radiusY);
    }

#endif // MATHEMATICAL_COORDINATESYSTEM

    //Ellipse
    void RenderManager::DrawEllipse(const Point2f& center, float radiusX, float radiusY, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawEllipse(center.x, center.y, radiusX, radiusY, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawEllipse(const Ellipsef& ellipse, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawEllipse(ellipse.center.x, ellipse.center.y, ellipse.radiusX, ellipse.radiusY, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawCircle(const Circlef& circle, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        DrawEllipse(circle.center, circle.rad, circle.rad, lineThickness, pStrokeStyle);
    }
    void RenderManager::DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineThickness, const StrokeStyle* pStrokeStyle) const
    {
        m_DXHandler.dDeviceContext2D.DrawEllipse(
           centerX, centerY,
           radiusX, radiusY,
           lineThickness,
           ConvertStrokeStyle(pStrokeStyle)
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
        m_DXHandler.dDeviceContext2D.FillEllipse(
             centerX, centerY,
             radiusX, radiusY
             );
    }

    //Geometry
    void RenderManager::DrawPolygon(const PolygonGeometry& polygon, float lineThickness, const StrokeStyle* pStrokeStyle)
    {
        DrawGeometry(&polygon, lineThickness, pStrokeStyle);
    }
    void RenderManager::FillPolygon(const PolygonGeometry& polygon)
    {
        FillGeometry(&polygon);
    }
    void RenderManager::DrawArc(const ArcGeometry& arc, float lineThickness, const StrokeStyle* pStrokeStyle)
    {
        DrawGeometry(&arc, lineThickness, pStrokeStyle);
    }
    void RenderManager::FillArc(const ArcGeometry& arc)
    {
        FillGeometry(&arc);
    }

    void RenderManager::DrawGeometry(const Geometry* const pGeometryObject, float lineThickness, const StrokeStyle* pStrokeStyle)
    {
        PushTransform();
        Translate(pGeometryObject->GetTranslation());
        m_DXHandler.dDeviceContext2D.DrawGeometry(Geometry::RendererDoor::GetGeometry(pGeometryObject), lineThickness, ConvertStrokeStyle(pStrokeStyle));
        PopTransform();
    }
    void RenderManager::FillGeometry(const Geometry* const pGeometryObject)
    {
        PushTransform();
        Translate(pGeometryObject->GetTranslation());
        m_DXHandler.dDeviceContext2D.FillGeometry(Geometry::RendererDoor::GetGeometry(pGeometryObject));
        PopTransform();
    }
    ID2D1StrokeStyle1* RenderManager::ConvertStrokeStyle(const StrokeStyle* pStrokeStyle)
    {
        return pStrokeStyle ? StrokeStyle::RendererDoor::GetStyle(*pStrokeStyle) : nullptr;
    }


    void RenderManager::Translate(float xTranslation, float yTranslation)
    {
        m_DXHandler.dDeviceContext2D.Translate(xTranslation, yTranslation);
    }
    void RenderManager::Rotate(float angle, float xPivotPoint, float yPivotPoint)
    {
        m_DXHandler.dDeviceContext2D.Rotate(angle, xPivotPoint, yPivotPoint);
    }
    void RenderManager::Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom)
    {
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
    void RenderManager::ApplyMatrix(const Matrix3X3& matrix)
    {
        m_DXHandler.dDeviceContext2D.ApplyMatrix(matrix);
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
