#include "DirectXObjects.h"

#include "Structs.h"
#include "Engine.h"

namespace jela::DX
{
    void TransformStack::Push()
    {
        m_VecTransformMatrices.emplace_back(D2D1::Matrix3x2F::Identity());
    }
    void TransformStack::Pop()
    {
        m_VecTransformMatrices.pop_back();
    }
    void TransformStack::Translate(float xTranslation, float yTranslation)
    {
        if (!m_VecTransformMatrices.empty())
        {
            auto& lastMatrix = m_VecTransformMatrices.back();
            lastMatrix = D2D1::Matrix3x2F::Translation(xTranslation, yTranslation) * lastMatrix;
        }
        else OutputDebugString(_T("Vector of matrices was empty while trying to add a Translation matrix."));
    }

    void TransformStack::Rotate(float angle, float xPivotPoint, float yPivotPoint)
    {
        if (!m_VecTransformMatrices.empty())
        {
            auto& lastMatrix = m_VecTransformMatrices.back();
            lastMatrix = D2D1::Matrix3x2F::Rotation(-angle, D2D1::Point2F(xPivotPoint, yPivotPoint)) * lastMatrix;
        }
        else OutputDebugString(_T("Vector of matrices was empty while trying to add a Rotation matrix."));
    }
    void TransformStack::Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom)
    {
        if (!m_VecTransformMatrices.empty())
        {
            auto& lastMatrix = m_VecTransformMatrices.back();
            lastMatrix = D2D1::Matrix3x2F::Scale(xScale, yScale, D2D1::Point2F(xPointToScaleFrom, yPointToScaleFrom)) * lastMatrix;
        }
        else OutputDebugString(_T("Vector of matrices was empty while trying to add a Scaling matrix."));
    }
    D2D1::Matrix3x2F TransformStack::GetMatrix() const
    {
        D2D1::Matrix3x2F combinedMatrix{D2D1::Matrix3x2F::Identity()};
        for (const auto& matrix : m_VecTransformMatrices)
            combinedMatrix = matrix * combinedMatrix;

        return combinedMatrix;
     }

    Debug::Debug()
    {
        HResultHandler& hr{StartHResult(_T("jela::DX::Debug ctor"))};
        SafeRelease(&m_pDDebug);
        hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_pDDebug));
    }
    Debug::~Debug()
    {
        if (!m_pDDebug) return;
        HResultHandler& hr{StartHResult(_T("jela::DX::Debug dtor"))};
        hr = m_pDDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        SafeRelease(&m_pDDebug);
    }


    Factory2D::Factory2D()
    {
        HResultHandler& hr{StartHResult(_T("jela::DX::Factory2D ctor"))};
        D2D1_FACTORY_OPTIONS options {};
#ifdef _DEBUG
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), &options, reinterpret_cast<void**>(&m_pD2DFactory));
    }
    Factory2D::~Factory2D()
    {
        SafeRelease(&m_pD2DFactory);
    }
    ID2D1Factory1* Factory2D::get() const
    {
        return m_pD2DFactory;
    }


    DeviceGI::DeviceGI(const Device3D& device3D)
    {
        if (device3D.IsInFaultyState()) return;

        HResultHandler& hr {StartHResult(_T("jela::DX::DeviceGI ctor"))};
        hr = device3D.get()->QueryInterface(IID_PPV_ARGS(&m_pDXGIDevice));

        // Ensure that DXGI doesn't queue more than one frame at a time.
        if (hr.Succeeded()) hr = m_pDXGIDevice->SetMaximumFrameLatency(1);
    }
    DeviceGI::~DeviceGI()
    {
        SafeRelease(&m_pDXGIDevice);
    }
    IDXGIDevice1* DeviceGI::get() const
    {
        return m_pDXGIDevice;
    }


    Device3D::Device3D()
    {
        HResultHandler& hr{StartHResult(_T("jela::DX::Device3D ctor"))};

        constexpr D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

#ifdef _DEBUG
        m_CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        ID3D11Device* pDevice = nullptr;
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, m_CreateDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &pDevice, nullptr, &m_pD3DDeviceContext);

        if (hr.Succeeded())
            hr = pDevice->QueryInterface(IID_PPV_ARGS(&m_pD3DDevice));
        SafeRelease(&pDevice);
    }
    Device3D::~Device3D()
    {
        if (m_pD3DDeviceContext)
        {
            m_pD3DDeviceContext->ClearState();
            m_pD3DDeviceContext->Flush();
        }
        SafeRelease(&m_pD3DDeviceContext);
        SafeRelease(&m_pD3DDevice);
    }
    void Device3D::SetViewport() const
    {
        if (!m_pD3DDeviceContext) return;

        const auto& vSize = ENGINE.GetViewportSize();
        const auto& vPos = ENGINE.GetViewportPos();
        D3D11_VIEWPORT vp{};
        vp.Width = vSize.x;
        vp.Height = vSize.y;
        vp.MaxDepth = 1.f;
        vp.TopLeftX = vPos.x;
        vp.TopLeftY = vPos.y;

        m_pD3DDeviceContext->RSSetViewports( 1, &vp );
    }
    bool Device3D::HasFlag(D3D11_CREATE_DEVICE_FLAG flagToCheck) const
    {
        const auto flagValue = static_cast<uint32_t>(flagToCheck);
        return (m_CreateDeviceFlags & flagValue) == flagValue;
    }
    ID3D11Device5* Device3D::get() const
    {
        return m_pD3DDevice;
    }

    Device2D::Device2D(const Factory2D& factory2D, const DeviceGI& deviceGI)
    {
        if (factory2D.IsInFaultyState()) return;
        if (deviceGI.IsInFaultyState()) return;

        HResultHandler& hr {StartHResult(_T("jela::DX::Device2D ctor"))};
        hr = factory2D.get()->CreateDevice(deviceGI.get(), &m_pD2DDevice);
    }
    Device2D::~Device2D()
    {
        SafeRelease(&m_pD2DDevice);
    }
    ID2D1Device* Device2D::get() const
    {
        return m_pD2DDevice;
    }
    DeviceContext2D::DeviceContext2D(const Device2D& device2D)
    {
        if (device2D.IsInFaultyState()) return;

        HResultHandler& hr {StartHResult(_T("jela::DX::Device2D ctor"))};

        hr = device2D.get()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_pD2DDeviceContext);
        if (hr.Succeeded())
            hr = m_pD2DDeviceContext->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f), &m_pDColorBrush);
    }
    DeviceContext2D::~DeviceContext2D()
    {
        if (m_pD2DDeviceContext) m_pD2DDeviceContext->SetTarget(nullptr);
        SafeRelease(&m_pDColorBrush);
        SafeRelease(&m_pDGameBitmap);
        SafeRelease(&m_pDTargetBitmap);
        SafeRelease(&m_pD2DDeviceContext);
    }
    void DeviceContext2D::ReleaseBuffers()
    {
        if (m_pD2DDeviceContext)
        {
            m_pD2DDeviceContext->SetTarget(nullptr);
            SafeRelease(&m_pDGameBitmap);
            SafeRelease(&m_pDTargetBitmap);
        }
    }
    void DeviceContext2D::SetBackgroundColor(COLORREF newColor, float opacity)
    {
        m_BGColor = D2D1::ColorF(
            GetRValue(newColor) / 255.f,
            GetGValue(newColor) / 255.f,
            GetBValue(newColor) / 255.f,
            std::clamp(opacity, 0.0f, 1.0f));
    }
    void DeviceContext2D::SetDrawColor(COLORREF newColor, float opacity) const
    {
        m_pDColorBrush->SetColor(D2D1::ColorF(
           GetRValue(newColor) / 255.f,
           GetGValue(newColor) / 255.f,
           GetBValue(newColor) / 255.f));

        m_pDColorBrush->SetOpacity(std::clamp(opacity, 0.0f, 1.0f));
    }
    HResultHandler DeviceContext2D::Draw(const std::function<void()>& drawer) const
    {
        if (IsInFaultyState()) return E_FAIL;
        HResultHandler& hr{StartHResult(_T("jela::DX::DeviceContext2D Draw"))};
        //-------------------------------------------------------
        // DRAW TO BITMAP
        m_pD2DDeviceContext->SetTarget(m_pDGameBitmap);
        m_pD2DDeviceContext->BeginDraw();
        // Clear background
        m_pD2DDeviceContext->Clear(m_BGColor);

        drawer();

        hr = m_pD2DDeviceContext->EndDraw();

        //-------------------------------------------------------

        //-------------------------------------------------------
        // DRAW BITMAP TO SCREEN
        m_pD2DDeviceContext->SetTarget(m_pDTargetBitmap);
        m_pD2DDeviceContext->BeginDraw();
        // Clear background
        m_pD2DDeviceContext->Clear(D2D1::ColorF(0.F, 0.F, 0.F, 1.F));

        // When the window changes in size,
        // the user draw calls should always appear in the middle of the screen,
        // not the left corner
        if (m_pDGameBitmap)
        {
            SetTransform();
            const auto& viewportSize = ENGINE.GetViewportSize();
            const auto& viewportPos = ENGINE.GetViewportPos();
            m_pD2DDeviceContext->DrawBitmap(
                m_pDGameBitmap,
                D2D1::RectF(
                    viewportPos.x,
                    viewportPos.y,
                    viewportPos.x + viewportSize.x,
                    viewportPos.y + viewportSize.y
                    ),
                1.f,
                D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
            );
        }

        hr = m_pD2DDeviceContext->EndDraw();
        //-------------------------------------------------------
        return hr;
    }

    void DeviceContext2D::PushTransform()
    {
        m_TransformStack.Push();
    }
    void DeviceContext2D::PopTransform()
    {
        m_TransformStack.Pop();
        m_TransformChanged = true;
    }
    void DeviceContext2D::Translate(float xTranslation, float yTranslation)
    {
        m_TransformStack.Translate(xTranslation, yTranslation);
        m_TransformChanged = true;
    }
    void DeviceContext2D::Rotate(float angle, float xPivotPoint, float yPivotPoint)
    {
        m_TransformStack.Rotate(angle, xPivotPoint, yPivotPoint);
        m_TransformChanged = true;
    }
    void DeviceContext2D::Scale(float xScale, float yScale, float xPointToScaleFrom, float yPointToScaleFrom)
    {
        m_TransformStack.Scale(xScale, yScale, xPointToScaleFrom, yPointToScaleFrom);
        m_TransformChanged = true;
    }
    void DeviceContext2D::DrawLine(float firstX, float firstY, float secondX, float secondY, float lineThickness) const
    {
        SetTransform();
        m_pD2DDeviceContext->DrawLine(
            D2D1::Point2F(firstX, firstY),
            D2D1::Point2F(secondX, secondY),
            m_pDColorBrush,
            lineThickness
        );
    }
    void DeviceContext2D::DrawRectangle(float left, float top, float right, float bottom, float lineThickness) const
    {
        SetTransform();
        m_pD2DDeviceContext->DrawRectangle(
            D2D1::RectF(left, top, right, bottom),
            m_pDColorBrush,
            lineThickness
        );
    }
    void DeviceContext2D::FillRectangle(float left, float top, float right, float bottom) const
    {
        SetTransform();
        m_pD2DDeviceContext->FillRectangle(
            D2D1::RectF(left, top, right, bottom),
            m_pDColorBrush);
    }
    void DeviceContext2D::DrawRoundedRect(float left, float top, float right, float bottom, float radiusX, float radiusY, float lineThickness) const
    {
        SetTransform();
        m_pD2DDeviceContext->DrawRoundedRectangle(
            D2D1::RoundedRect
            (
                D2D1::RectF(left, top, right, bottom),
                radiusX,
                radiusY
            ),
            m_pDColorBrush,
            lineThickness
        );
    }
    void DeviceContext2D::FillRoundedRect(float left, float top, float right, float bottom, float radiusX, float radiusY) const
    {
        SetTransform();
        m_pD2DDeviceContext->FillRoundedRectangle(
            D2D1::RoundedRect(
                D2D1::RectF(left, top, right, bottom),
                radiusX,
                radiusY
            ),
            m_pDColorBrush);
    }
    void DeviceContext2D::DrawEllipse(float centerX, float centerY, float radiusX, float radiusY, float lineThickness) const
    {
        SetTransform();
        m_pD2DDeviceContext->DrawEllipse(
            D2D1::Ellipse( D2D1::Point2F(centerX, centerY), radiusX, radiusY),
            m_pDColorBrush,
            lineThickness
        );
    }
    void DeviceContext2D::FillEllipse(float centerX, float centerY, float radiusX, float radiusY) const
    {
        SetTransform();
        m_pD2DDeviceContext->FillEllipse(
           D2D1::Ellipse( D2D1::Point2F(centerX, centerY), radiusX, radiusY),
           m_pDColorBrush);
    }
    void DeviceContext2D::DrawGeometry(ID2D1PathGeometry* pGeo, float lineThickness) const
    {
        SetTransform();
        m_pD2DDeviceContext->DrawGeometry(pGeo, m_pDColorBrush, lineThickness);
    }
    void DeviceContext2D::FillGeometry(ID2D1PathGeometry* pGeo) const
    {
        SetTransform();
        m_pD2DDeviceContext->FillGeometry(pGeo, m_pDColorBrush);
    }
    void DeviceContext2D::DrawString(const std::wstring& textToDisplay, float left, float top, float right, float bottom, IDWriteTextFormat* pTextFormat) const
    {
        SetTransform();
        m_pD2DDeviceContext->DrawText(
            textToDisplay.c_str(),
            static_cast<UINT32>(textToDisplay.length()),
            pTextFormat,
            D2D1::RectF(left, top, right, bottom),
            m_pDColorBrush,
           D2D1_DRAW_TEXT_OPTIONS_NONE,
           DWRITE_MEASURING_MODE_NATURAL);
    }
    void DeviceContext2D::DrawTexture(ID2D1Bitmap* bitmap, const D2D1_RECT_F& destRect, const D2D1_RECT_F& srcRect, float opacity) const
    {
        SetTransform();
        m_pD2DDeviceContext->DrawBitmap(
                bitmap,
                destRect,
                opacity,
                D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
                srcRect
            );
    }
    Rectf DeviceContext2D::GetSize() const
    {
        auto [width, height] = m_pD2DDeviceContext->GetSize();
        return Rectf{ 0, 0, width, height};
    }
    ID2D1DeviceContext* DeviceContext2D::get() const
    {
        return m_pD2DDeviceContext;
    }
    HResultHandler DeviceContext2D::SetTargetBitmap(const SwapChain& swapChain)
    {
        if (swapChain.IsInFaultyState()) return E_FAIL;

        HResultHandler& hr {StartHResult(_T("jela::DX::Device2D SetTargerBitmap"))};
        // Direct2D needs the dxgi version of the backbuffer surface pointer.
        IDXGISurface* dxgiBackBuffer = swapChain.GetBackBuffer();
        if (!dxgiBackBuffer) return hr;

        const D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(swapChain.GetFormat(), D2D1_ALPHA_MODE_IGNORE);

        // Now we set up the Direct2D render target bitmap linked to the swapchain.
        // Whenever we render to this bitmap, it is directly rendered to the
        // swap chain associated with the window.
        const auto targetBitmapProperties = D2D1::BitmapProperties1(
                           D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                           pixelFormat);

        // Get a D2D surface from the DXGI back buffer to use as the D2D render target.
        SafeRelease(&m_pDTargetBitmap);
        hr = m_pD2DDeviceContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &targetBitmapProperties, &m_pDTargetBitmap);

        SafeRelease(&dxgiBackBuffer);
        if (hr.Failed()) return hr;

        // Now we can set the Direct2D render target.
        m_pD2DDeviceContext->SetTarget(m_pDTargetBitmap);

        const auto gameBitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, pixelFormat);
        SafeRelease(&m_pDGameBitmap);
        const auto& gameSize = ENGINE.GetGameSize();
        hr = m_pD2DDeviceContext->CreateBitmap(
            D2D1::SizeU(
                static_cast<UINT32>(gameSize.x),
                static_cast<UINT32>(gameSize.y)
            ),
            nullptr, 0, &gameBitmapProperties, &m_pDGameBitmap);

        return hr;
    }
    void DeviceContext2D::SetTransform() const
    {
        if (m_TransformChanged)
        {
            m_pD2DDeviceContext->SetTransform(m_TransformStack.GetMatrix());
            m_TransformChanged = false;
        }
    }
    SwapChain::SwapChain(const Device3D& device3D, const DeviceGI& deviceGI)
    {
        if (device3D.IsInFaultyState()) return;
        if (deviceGI.IsInFaultyState()) return;

        HResultHandler& hr {StartHResult(_T("jela::DX::SwapChain ctor"))};
        // Identify the physical adapter (GPU or card) this device is runs on.
        IDXGIAdapter* dxgiAdapter = nullptr;
        IDXGIFactory2* dxgiFactory = nullptr;
        hr = deviceGI.get()->GetAdapter(&dxgiAdapter);
        if (hr.Succeeded())
        {
            // Get the factory object that created the DXGI device.
            hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
        }
        if (hr.Succeeded() && dxgiFactory)
        {
            // Allocate a descriptor.
            const auto& windowRect = ENGINE.GetWindowSize();
            m_CurrentWidth = static_cast<UINT>(windowRect.x);
            m_CurrentHeight = static_cast<UINT>(windowRect.y);
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
            swapChainDesc.Width = m_CurrentWidth;                           // use automatic sizing
            swapChainDesc.Height = m_CurrentHeight;
            swapChainDesc.Format = m_Format;
            swapChainDesc.Stereo = false;
            swapChainDesc.SampleDesc.Count = 1;                // don't use multi-sampling
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = 2;                     // use double buffering to enable flip
            swapChainDesc.Scaling = DXGI_SCALING_NONE;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect
            swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            // Get the final swap chain for this window from the DXGI factory.

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
            //fullscreenDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
            fullscreenDesc.Windowed = true;
            hr = dxgiFactory->CreateSwapChainForHwnd(
                device3D.get(),
                ENGINE.GetWindow(),
                &swapChainDesc,
                &fullscreenDesc,
                nullptr,    // allow on all displays
                &m_pDSwapChain);
        }
        SafeRelease(&dxgiFactory);
        SafeRelease(&dxgiAdapter);
    }
    SwapChain::~SwapChain()
    {
        SafeRelease(&m_pDSwapChain);
    }
    HResultHandler SwapChain::Resize() const
    {
        HResultHandler& hr{StartHResult(_T("jela::DX::SwapChain Resize"))};
        hr = m_pDSwapChain->ResizeBuffers(0, 0, 0, m_Format, 0);
        return hr;
    }
    DXGI_FORMAT SwapChain::GetFormat() const
    {
        return m_Format;
    }
    IDXGISurface* SwapChain::GetBackBuffer() const
    {
        HResultHandler& hr {StartHResult(_T("jela::DX::SwapChain GetBackBuffer"))};

        IDXGISurface* dxgiBackBuffer = nullptr;
        hr = m_pDSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
        if (hr.Failed()) SafeRelease(&dxgiBackBuffer);

        return dxgiBackBuffer;
    }
    IDXGISwapChain1* SwapChain::get() const
    {
        return m_pDSwapChain;
    }
    HResultHandler SwapChain::Present() const
    {
        if (IsInFaultyState()) return E_FAIL;
        HResultHandler& hr {StartHResult(_T("jela::DX::SwapChain Present"))};
        hr = m_pDSwapChain->Present(1, 0);
        return hr;
    }
}
