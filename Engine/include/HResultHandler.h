#ifndef HRESULTHANDLER_H
#define HRESULTHANDLER_H
#include <intsafe.h>

#include "Defines.h"

namespace jela
{
    class HResultHandler
    {
    public:
        HResultHandler(HRESULT hr = S_OK, const tstring& messageWhenFailed = _T("") ):
            m_lastHR{hr},
            m_Message{messageWhenFailed}
        {}
        HResultHandler& operator=(HRESULT hr);
        bool Succeeded() const;
        bool Failed() const;
        HRESULT Get() const;
    private:
        HRESULT m_lastHR{S_OK};
        tstring m_Message{};
    };
}

#endif //HRESULTHANDLER_H
