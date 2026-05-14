#ifndef DIRECTXOBJECTS_H
#define DIRECTXOBJECTS_H
#include "Defines.h"
#include "HResultHandler.h"
#include <functional>
namespace jela
{
    struct Point2f;
    struct Rectf;
}

namespace jela::DX
{
    class DeviceGI;
    class Device2D;
    class Device3D;
    class SwapChain;

    class TransformStack final
    {
    public:
        void Push();
        void Pop();
        void Translate(float xTranslation, float yTranslation);
        void Rotate(float angle, float xPivotPoint, float yPivotPoint);
        void Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom);

        D2D1::Matrix3x2F GetMatrix() const;

    private:
        std::vector<D2D1::Matrix3x2F> m_VecTransformMatrices{};
    };

    class DXObject
    {
    public:
        bool IsInValidState() const { return m_HrState.Succeeded(); }
        bool IsInFaultyState() const { return m_HrState.Failed(); }
    protected:
        HResultHandler& StartHResult(const tstring& message) const { return m_HrState = HResultHandler{S_OK, message}; }
    private:
        mutable HResultHandler m_HrState{E_FAIL}; // Faulty state by default. If StartHResult was never called, we know something went wrong
    };

    class Debug final: public DXObject
    {
#ifdef _DEBUG
    public:
        Debug();
        ~Debug();

        Debug(const Debug&) = delete;
        Debug(Debug&&) noexcept = delete;
        Debug& operator=(const Debug&) = delete;
        Debug& operator=(Debug&&) noexcept = delete;

    private:
        IDXGIDebug1* m_pDDebug{nullptr};
#endif
    };

    class Factory2D final: public DXObject
    {
    public:
        Factory2D();
        ~Factory2D();

        Factory2D(const Factory2D&) = delete;
        Factory2D(Factory2D&&) noexcept = delete;
        Factory2D& operator=(const Factory2D&) = delete;
        Factory2D& operator=(Factory2D&&) noexcept = delete;

        ID2D1Factory1* get() const;

    private:
        ID2D1Factory1* m_pD2DFactory{nullptr};
    };

    class DeviceGI final: public DXObject
    {
    public:
        explicit DeviceGI(const Device3D& device3D);
        ~DeviceGI();

        DeviceGI(const DeviceGI&) = delete;
        DeviceGI(DeviceGI&&) noexcept = delete;
        DeviceGI& operator=(const DeviceGI&) = delete;
        DeviceGI& operator=(DeviceGI&&) noexcept = delete;

        IDXGIDevice1* get() const;

    private:
        IDXGIDevice1* m_pDXGIDevice{nullptr};
    };

    class Device3D final: public DXObject
    {
    public:
        Device3D();
        ~Device3D();

        Device3D(const Device3D&) = delete;
        Device3D(Device3D&&) noexcept = delete;
        Device3D& operator=(const Device3D&) = delete;
        Device3D& operator=(Device3D&&) noexcept = delete;

        void SetViewport() const;
        bool HasFlag(D3D11_CREATE_DEVICE_FLAG flagToCheck) const;

        ID3D11Device5* get() const;
    private:
        ID3D11Device5* m_pD3DDevice{nullptr};
        ID3D11DeviceContext* m_pD3DDeviceContext{nullptr};
        uint32_t m_CreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    };

    class Device2D final: public DXObject
    {
    public:
        explicit Device2D(const Factory2D&, const DeviceGI&);
        ~Device2D();

        Device2D(const Device2D&) = delete;
        Device2D(Device2D&&) noexcept = delete;
        Device2D& operator=(const Device2D&) = delete;
        Device2D& operator=(Device2D&&) noexcept = delete;

        ID2D1Device* get() const;

    private:
        ID2D1Device* m_pD2DDevice{nullptr};
    };

    class DeviceContext2D final: public DXObject
    {
    public:
        explicit DeviceContext2D(const Device2D& device2D);
        ~DeviceContext2D();

        DeviceContext2D(const DeviceContext2D&) = delete;
        DeviceContext2D(DeviceContext2D&&) noexcept = delete;
        DeviceContext2D& operator=(const DeviceContext2D&) = delete;
        DeviceContext2D& operator=(DeviceContext2D&&) noexcept = delete;

        void ReleaseBuffers();
        void SetBackgroundColor(COLORREF newColor, float opacity);
        void SetDrawColor(COLORREF newColor, float opacity) const;
        HResultHandler Draw(const std::function<void()>& drawer) const;

        void PushTransform();
        void PopTransform();
        void Translate(float xTranslation, float yTranslation);
        void Rotate(float angle, float xPivotPoint, float yPivotPoint);
        void Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom);

        void DrawLine(float firstX, float firstY, float secondX, float secondY, float lineThickness = 1.f) const;

        void DrawRectangle(float left, float top, float right, float bottom, float lineThickness = 1.f) const;
        void FillRectangle(float left, float top, float right, float bottom) const;

        void DrawRoundedRect(float left, float top, float right, float bottom, float radiusX, float radiusY, float lineThickness = 1.f) const;
        void FillRoundedRect(float left, float top, float right, float bottom, float radiusX, float radiusY) const;

        void DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineThickness = 1.f) const;
        void FillEllipse(float centerX, float centerY, float radiusX, float radiusY) const;

        void DrawGeometry(ID2D1PathGeometry* pGeo, float lineThickness = 1.f) const;
        void FillGeometry(ID2D1PathGeometry* pGeo) const;

        void DrawString(const std::wstring& textToDisplay, float left, float top, float right, float bottom, IDWriteTextFormat* pTextFormat) const;
        void DrawTexture(ID2D1Bitmap* bitmap, const D2D1_RECT_F& destRect, const D2D1_RECT_F& srcRect = {}, float opacity = 1.f) const;

        Rectf GetSize() const;
        ID2D1DeviceContext* get() const;

        HResultHandler SetTargetBitmap(const SwapChain& swapChain);

    private:
        void SetTransform() const;

        ID2D1DeviceContext* m_pD2DDeviceContext{nullptr};
        ID2D1Bitmap1* m_pDTargetBitmap{nullptr};
        ID2D1Bitmap1* m_pDGameBitmap{nullptr};
        ID2D1SolidColorBrush* m_pDColorBrush{nullptr};
        D2D1_COLOR_F m_BGColor{D2D1::ColorF::Black };
        TransformStack m_TransformStack{};
        mutable bool m_TransformChanged{};
    };

    class SwapChain final: public DXObject
    {
    public:
        SwapChain(const Device3D& device3D, const DeviceGI& deviceGI);
        ~SwapChain();

        SwapChain(const SwapChain&) = delete;
        SwapChain(SwapChain&&) noexcept = delete;
        SwapChain& operator=(const SwapChain&) = delete;
        SwapChain& operator=(SwapChain&&) noexcept = delete;

        HResultHandler Resize() const;
        DXGI_FORMAT GetFormat() const;
        IDXGISurface* GetBackBuffer() const;
        IDXGISwapChain1* get() const;
        HResultHandler Present() const;

    private:
        IDXGISwapChain1* m_pDSwapChain{nullptr};
        UINT m_CurrentWidth{0};
        UINT m_CurrentHeight{0};
        DXGI_FORMAT m_Format{DXGI_FORMAT_B8G8R8A8_UNORM}; // this is the most common swapchain format
    };

    struct DXHandler
    {
        DXHandler()
        : dDeviceGI{dDevice3D}
        , dDevice2D{dFactory2D, dDeviceGI}
        , dDeviceContext2D{dDevice2D}
        , dSwapChain{dDevice3D, dDeviceGI}
        {
            HResultHandler hr {S_OK, _T("jela::DX::DXHandler ctor")};
            hr = ResizeWindow();
        }

        // Default constructed
        Debug                       dDebug{};
        Device3D                    dDevice3D{};
        Factory2D                   dFactory2D{};

        // constructed in default DXHandler constructor
        DeviceGI                    dDeviceGI;
        Device2D                    dDevice2D;
        DeviceContext2D             dDeviceContext2D;
        SwapChain                   dSwapChain;

        bool IsValid() const
        {
            return dDebug.IsInValidState() &&
                dDevice3D.IsInValidState() &&
                dFactory2D.IsInValidState() &&
                dDeviceGI.IsInValidState() &&
                dDevice2D.IsInValidState() &&
                dSwapChain.IsInValidState();
        }

        HResultHandler ResizeWindow()
        {
            HResultHandler hr{S_OK, _T("jela::DX::DXHandler ResizeWindow")};

            if (dSwapChain.IsInFaultyState()) return E_FAIL;

            dDeviceContext2D.ReleaseBuffers();
            hr = dSwapChain.Resize();
            if (hr.Failed()) return hr;

            hr = dDeviceContext2D.SetTargetBitmap(dSwapChain);

            if (hr.Failed()) return hr;

            dDevice3D.SetViewport();
            return hr;
        }
    };
}


#endif //DIRECTXOBJECTS_H
