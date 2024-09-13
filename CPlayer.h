// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#ifndef CPLAYER_H
#define CPLAYER_H

#include <windows.h>
#include <shobjidl.h> 
#include <shlwapi.h>
#include <assert.h>
#include <strsafe.h>

#include "Defines.h"
//https://learn.microsoft.com/en-us/windows/win32/medfound/how-to-play-unprotected-media-files

class CPlayer : public IMFAsyncCallback
{
public:

    enum class PlayerState
    {
        Closed = 0,     // No session.
        ReadyToOpen,    // Session was created, ready to open a file. 
        OpenPending,    // Session is opening a file.
        ReadyToStart,   // Opened file
        Started,        // Session is playing a file.
        Paused,         // Session is paused.
        Stopped,        // Session is stopped (ready to play). 
        Closing         // Application has closed the session, but is waiting for MESessionClosed.
    };

    static HRESULT CreateInstance(HWND hVideo, CPlayer** ppPlayer);

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IMFAsyncCallback methods
    STDMETHODIMP  GetParameters(DWORD*, DWORD*) { return E_NOTIMPL; };

    STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult);

    // Playback
    HRESULT       OpenURL(const std::wstring& fileName);
    HRESULT       Play(bool repeat, bool resume = false);
    HRESULT       Pause();
    HRESULT       Stop();
    HRESULT       Shutdown();
    HRESULT       HandleEvent(UINT_PTR pEventPtr);
    PlayerState   GetState() const { return m_state; }
    static uint8_t       GetVolume();
    static HRESULT       SetVolume(uint8_t volumePercentage);

    static const UINT WM_APP_PLAYER_EVENT = WM_APP + 1;
protected:

    // Constructor is private. Use static CreateInstance method to instantiate.
    CPlayer(HWND hVideo);

    // Destructor is private. Caller should call Release.
    virtual ~CPlayer();

    HRESULT Initialize();
    HRESULT CreateSession();
    HRESULT CloseSession();
    HRESULT StartPlayback(bool resume);

    // Media event handlers
    virtual HRESULT OnTopologyStatus(IMFMediaEvent* pEvent);
    virtual HRESULT OnPresentationEnded(IMFMediaEvent* pEvent);
    virtual HRESULT OnNewPresentation(IMFMediaEvent* pEvent);
    virtual HRESULT OnSessionEvent(IMFMediaEvent*, MediaEventType)
    {
        return S_OK;
    }

protected:
    long                    m_nRefCount;        // Reference count.
    bool                    m_Repeat;

    IMFMediaSession*        m_pSession;
    IMFMediaSource*         m_pSource;

    HWND                    m_hwndAudio;        // Audio window.
    PlayerState             m_state;            // Current state of the media session.
    HANDLE                  m_hCloseEvent;      // Event to wait on while closing.

    static IMFSimpleAudioVolume*   m_pMasterVolume;
};

#endif CPLAYER_H
