#include "HResultHandler.h"

#include "Engine.h"


namespace jela
{

    HResultHandler& HResultHandler::operator=(HRESULT hr)
    {
        if (FAILED(hr))
            Engine::NotifyError(ENGINE.GetWindow(), std::format(_T("ERROR: HRESULT failed\n{}\n"), m_Message), hr);
        m_lastHR = hr;
        return *this;
    }
    bool HResultHandler::Succeeded() const
    {
        return SUCCEEDED(m_lastHR);
    }
    bool HResultHandler::Failed() const
    {
        return !Succeeded();
    }
    HRESULT HResultHandler::Get() const
    {
        return m_lastHR;
    }
}
