// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include "CPlayer.h"
#include "propvarutil.h"

IMFSimpleAudioVolume* CPlayer::m_pMasterVolume{nullptr};

template <class Q>
HRESULT GetEventObject(IMFMediaEvent* pEvent, Q** ppObject)
{
    *ppObject = NULL;   // zero output

    PROPVARIANT var;
    HRESULT hr = pEvent->GetValue(&var);
    if (SUCCEEDED(hr))
    {
        if (var.vt == VT_UNKNOWN) hr = var.punkVal->QueryInterface(ppObject);
        else hr = MF_E_INVALIDTYPE;

        PropVariantClear(&var);
    }
    return hr;
}

HRESULT CreateMediaSource(IMFMediaSource** ppSource, const std::wstring& fileName);

HRESULT CreatePlaybackTopology(IMFMediaSource* pSource,
    IMFPresentationDescriptor* pPD, HWND hAudioWnd, IMFTopology** ppTopology);

//  Static class method to create the CPlayer object.

HRESULT CPlayer::CreateInstance(HWND hAudio, CPlayer** ppPlayer)          
{
    if (ppPlayer == NULL) return E_POINTER;

    CPlayer* pPlayer = new (std::nothrow) CPlayer(hAudio);
    if (pPlayer == NULL) return E_OUTOFMEMORY;

    HRESULT hr = pPlayer->Initialize();

    if (SUCCEEDED(hr)) *ppPlayer = pPlayer;
    else pPlayer->Release();

    return hr;
}

HRESULT CPlayer::Initialize()
{
    // Start up Media Foundation platform. (replaced)
    HRESULT hr = S_OK;
    
    m_hCloseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hCloseEvent == NULL) hr = HRESULT_FROM_WIN32(GetLastError());
    
    return hr;
}

CPlayer::CPlayer(HWND hAudio) :
    m_nRefCount(1),
    m_Repeat(false),
    m_pSession(NULL),
    m_pSource(NULL),
    m_hwndAudio(hAudio),
    m_state(PlayerState::Closed),
    m_hCloseEvent(NULL)
{

   /* UINT32 channelCount{};
    if (m_pVolume) m_pVolume->GetChannelCount(&channelCount);
    OutputDebugString((std::wstring{ std::to_wstring(channelCount).c_str() } + std::wstring{ L"\n" }).c_str());*/
}

CPlayer::~CPlayer()
{
    assert(m_pSession == NULL);
    // If FALSE, the app did not call Shutdown().

    // When CPlayer calls IMediaEventGenerator::BeginGetEvent on the
    // media session, it causes the media session to hold a reference 
    // count on the CPlayer. 

    // This creates a circular reference count between CPlayer and the 
    // media session. Calling Shutdown breaks the circular reference 
    // count.

    // If CreateInstance fails, the application will not call 
    // Shutdown. To handle that case, call Shutdown in the destructor. 

    Shutdown();
}

// IUnknown methods

HRESULT CPlayer::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(CPlayer, IMFAsyncCallback),
        { 0 }
    };
    return QISearch(this, qit, riid, ppv);
}

ULONG CPlayer::AddRef()
{
    return InterlockedIncrement(&m_nRefCount);
}

ULONG CPlayer::Release()
{
    ULONG uCount = InterlockedDecrement(&m_nRefCount);
    
    if (uCount == 0)
    {
        delete this;
    }
    return uCount;
}

//  Open a URL for playback.
HRESULT CPlayer::OpenURL(const std::wstring& fileName)
{
    // 1. Create a new media session.
    // 2. Create the media source.
    // 3. Create the topology.
    // 4. Queue the topology [asynchronous]
    // 5. Start playback [asynchronous - does not happen in this method.]
  
    IMFTopology* pTopology = NULL;
    IMFPresentationDescriptor* pSourcePD = NULL;

    // Create the media session.
    HRESULT hr = CreateSession();

    // Create the media source.
    if (SUCCEEDED(hr)) hr = CreateMediaSource(&m_pSource, fileName);
    
    // Create the presentation descriptor for the media source.
    if (SUCCEEDED(hr)) hr = m_pSource->CreatePresentationDescriptor(&pSourcePD);

    // Create a partial topology.
    if (SUCCEEDED(hr)) hr = CreatePlaybackTopology(m_pSource, pSourcePD, m_hwndAudio, &pTopology);

    // Set the topology on the media session.
    if (SUCCEEDED(hr)) hr = m_pSession->SetTopology(0, pTopology);

    if (SUCCEEDED(hr)) m_state = PlayerState::OpenPending;
    if (FAILED(hr)) m_state = PlayerState::Closed;

    // If SetTopology succeeds, the media session will queue an 
    // MESessionTopologySet event.

    SafeRelease(&pSourcePD);
    SafeRelease(&pTopology);
    return hr;
}

//  Pause playback.
HRESULT CPlayer::Pause()
{
    
    if (m_pSession == NULL || m_pSource == NULL) return E_UNEXPECTED;

    HRESULT hr{ S_OK };
    if (m_state == PlayerState::Started)
    {
        m_pSession->Pause();
        if (SUCCEEDED(hr)) m_state = PlayerState::Paused;
    }
    return hr;
}

// Stop playback.
HRESULT CPlayer::Stop()
{
    if (m_state != PlayerState::Started && m_state != PlayerState::Paused && m_state != PlayerState::Stopped) return MF_E_INVALIDREQUEST;
    
    if (m_pSession == NULL) return E_UNEXPECTED;

    m_Repeat = false;
    HRESULT hr = S_OK;
    if (m_state != PlayerState::Stopped)
    {
        hr = m_pSession->Stop();
        if (SUCCEEDED(hr)) m_state = PlayerState::Stopped;
    }

    return hr;
}

//  Callback for the asynchronous BeginGetEvent method.
HRESULT CPlayer::Invoke(IMFAsyncResult* pResult)
{
    MediaEventType meType = MEUnknown;  // Event type

    IMFMediaEvent* pEvent = NULL;

    // Get the event from the event queue.
    HRESULT hr = m_pSession->EndGetEvent(pResult, &pEvent);

    // Get the event type. 
    if(SUCCEEDED(hr)) hr = pEvent->GetType(&meType);

    // Check the application state. 

    // If a call to IMFMediaSession::Close is pending, it means the 
    // application is waiting on the m_hCloseEvent event and
    // the application's message loop is blocked. 

    // Otherwise, post a private window message to the application. 
    if (SUCCEEDED(hr))
    { 
        if (meType == MESessionClosed)
        {
            // The session was closed. The application is waiting on the m_hCloseEvent event handle. 
            SetEvent(m_hCloseEvent);
        }
        else
        {
            // For all other events, get the next event in the queue.
            hr = m_pSession->BeginGetEvent(this, NULL);
        }

        if (SUCCEEDED(hr) and m_state != PlayerState::Closing)
        {
            switch (meType)
            {
            case MESessionTopologyStatus:
                hr = OnTopologyStatus(pEvent);
                break;

            case MEEndOfPresentation:
                hr = OnPresentationEnded(pEvent);
                break;

            case MESessionEnded:
                if (m_Repeat) hr = Play(true);
                break;

            case MENewPresentation:
                hr = OnNewPresentation(pEvent);
                break;
            case MESessionTopologySet:
                hr = MFGetService(m_pSession, MR_POLICY_VOLUME_SERVICE, IID_PPV_ARGS(&m_pMasterVolume));
                break;

            default:
                hr = OnSessionEvent(pEvent, meType);
                break;
            }
        }
       
    }

    SafeRelease(&pEvent);
    return hr;
}

HRESULT CPlayer::HandleEvent(UINT_PTR pEventPtr)
{

    HRESULT hrStatus = S_OK;
    MediaEventType meType = MEUnknown;

    IMFMediaEvent* pEvent = (IMFMediaEvent*)pEventPtr;

    if (pEvent == NULL)  return E_POINTER;

    // Get the event type.
    HRESULT hr = pEvent->GetType(&meType);

    // Get the event status. If the operation that triggered the event 
    // did not succeed, the status is a failure code.
    if(SUCCEEDED(hr)) hr = pEvent->GetStatus(&hrStatus);

    // Check if the async operation succeeded.
    if (SUCCEEDED(hr) && FAILED(hrStatus)) hr = hrStatus;
    if (SUCCEEDED(hr))
    {
        switch (meType)
        {
        case MESessionTopologyStatus:
            hr = OnTopologyStatus(pEvent);
            break;

        case MEEndOfPresentation:
            hr = OnPresentationEnded(pEvent);
            break;

        case MESessionEnded:
            if (m_Repeat) hr = Play(true);
            break;

        case MENewPresentation:
            hr = OnNewPresentation(pEvent);
            break;
        case MESessionTopologySet:
            hr = MFGetService(m_pSession, MR_POLICY_VOLUME_SERVICE, IID_PPV_ARGS(&m_pMasterVolume));
            break;
         
        default:
            hr = OnSessionEvent(pEvent, meType);
            break;
        }
    }

    SafeRelease(&pEvent);
    return hr;
}

uint8_t CPlayer::GetVolume()
{
    float volume;
    m_pMasterVolume->GetMasterVolume(&volume); 
    return static_cast<uint8_t>(volume * 100);
}

HRESULT CPlayer::SetVolume(uint8_t volumePercentage)
{
    int newVolume{ static_cast<int>(volumePercentage) };
    if (volumePercentage > 100) newVolume = 100;
    if (volumePercentage < 0) newVolume = 0;
    return m_pMasterVolume->SetMasterVolume(newVolume / 100.f);
}

//  Release all resources held by this object.
HRESULT CPlayer::Shutdown()
{
    // Close the session
    HRESULT hr = CloseSession();

    if (m_hCloseEvent)
    {
        CloseHandle(m_hCloseEvent);
        m_hCloseEvent = NULL;
    }

    return hr;
}

// Protected methods
HRESULT CPlayer::OnTopologyStatus(IMFMediaEvent* pEvent)
{
    UINT32 status;

    HRESULT hr = pEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &status);
    if (SUCCEEDED(hr) && (status == MF_TOPOSTATUS_READY))
    {
        m_state = PlayerState::ReadyToStart; //Setting the state 'stopped' because we want to manually play the file
        return hr;
    }

    return E_FAIL;
}

//  Handler for MEEndOfPresentation event.
HRESULT CPlayer::OnPresentationEnded(IMFMediaEvent* pEvent)
{
    // The session puts itself into the stopped state automatically.
    m_state = PlayerState::Stopped;
    return S_OK;
}

//  Handler for MENewPresentation event.
//  This event is sent if the media source has a new presentation, which 
//  requires a new topology. 
HRESULT CPlayer::OnNewPresentation(IMFMediaEvent* pEvent)
{
    IMFPresentationDescriptor* pPD = NULL;
    IMFTopology* pTopology = NULL;
 
    // Get the presentation descriptor from the event.
    HRESULT hr = GetEventObject(pEvent, &pPD);
    
    // Create a partial topology.
    if(SUCCEEDED(hr)) hr = CreatePlaybackTopology(m_pSource, pPD, m_hwndAudio, &pTopology);

    // Set the topology on the media session.
    if (SUCCEEDED(hr)) hr = m_pSession->SetTopology(0, pTopology);
    
    if (SUCCEEDED(hr)) m_state = PlayerState::OpenPending;

    SafeRelease(&pTopology);
    SafeRelease(&pPD);
    return hr;
}

//  Create a new instance of the media session.
HRESULT CPlayer::CreateSession()
{
    // Close the old session, if any.
    HRESULT hr = CloseSession();
    if (SUCCEEDED(hr))
    {
        assert(m_state == PlayerState::Closed);

        // Create the media session.
        hr = MFCreateMediaSession(NULL, &m_pSession);
        
    }

    // Start pulling events from the media session
    if(SUCCEEDED(hr)) hr = m_pSession->BeginGetEvent((IMFAsyncCallback*)this, NULL);
    if(SUCCEEDED(hr)) m_state = PlayerState::ReadyToOpen;
   
    return hr;
}

//  Close the media session. 
HRESULT CPlayer::CloseSession()
{
    //  The IMFMediaSession::Close method is asynchronous, but the 
    //  CPlayer::CloseSession method waits on the MESessionClosed event.
    //  
    //  MESessionClosed is guaranteed to be the last event that the 
    //  media session fires.

    HRESULT hr = S_OK;

    // First close the media session.
    if (m_pSession)
    {
        DWORD dwWaitResult = 0;

        m_state = PlayerState::Closing;

        hr = m_pSession->Close();
        // Wait for the close operation to complete
        if (SUCCEEDED(hr))
        {
            dwWaitResult = WaitForSingleObject(m_hCloseEvent, 5000);
            if (dwWaitResult == WAIT_TIMEOUT) assert(FALSE);
            // Now there will be no more events from this session.
        }
    }

    // Complete shutdown operations.
    if (SUCCEEDED(hr))
    {
        // Shut down the media source. (Synchronous operation, no events.)
        if (m_pSource) (void)m_pSource->Shutdown();
        // Shut down the media session. (Synchronous operation, no events.)
        if (m_pSession) (void)m_pSession->Shutdown();
    }

    SafeRelease(&m_pSource);
    SafeRelease(&m_pSession);
    SafeRelease(&m_pMasterVolume);
    m_state = PlayerState::Closed;
    return hr;
}

//  Start playback from the current position. 
HRESULT CPlayer::StartPlayback(bool resume)
{
    assert(m_pSession != NULL);

    PROPVARIANT varStart;

    //https://stackoverflow.com/questions/66867668/media-foundation-looping-video-unstable-on-6th-run
    if (resume) PropVariantInit(&varStart);
    else InitPropVariantFromInt64(0, &varStart);

    HRESULT hr = m_pSession->Start(&GUID_NULL, &varStart);
    if (SUCCEEDED(hr))
    {
        // Note: Start is an asynchronous operation. However, we
        // can treat our state as being already started. If Start
        // fails later, we'll get an MESessionStarted event with
        // an error code, and we will update our state then.
        m_state = PlayerState::Started;
    }
    PropVariantClear(&varStart);
    return hr;
}

//  Start playback from paused or stopped.
HRESULT CPlayer::Play(bool repeat, bool resume)
{
    m_Repeat = repeat;
  
    if (m_state != PlayerState::ReadyToStart && m_state != PlayerState::Paused && m_state != PlayerState::Stopped) return MF_E_INVALIDREQUEST;
    
    if (m_pSession == NULL || m_pSource == NULL) return E_UNEXPECTED;

    return StartPlayback(resume);
}


//  Create a media source from a URL.
HRESULT CreateMediaSource(IMFMediaSource** ppSource, const std::wstring& fileName)
{
    MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;

    IMFSourceResolver* pSourceResolver = NULL;
    IUnknown* pSource = NULL;

    // Create the source resolver.
    HRESULT hr = MFCreateSourceResolver(&pSourceResolver);

    // Note: For simplicity this sample uses the synchronous method to create 
    // the media source. However, creating a media source can take a noticeable
    // amount of time, especially for a network source. For a more responsive 
    // UI, use the asynchronous BeginCreateObjectFromURL method.
    if (SUCCEEDED(hr)) hr = pSourceResolver->CreateObjectFromURL(fileName.c_str(), MF_RESOLUTION_MEDIASOURCE | MF_RESOLUTION_READ, NULL, &ObjectType, &pSource);

    // Get the IMFMediaSource interface from the media source.
    if (SUCCEEDED(hr)) hr = pSource->QueryInterface(IID_PPV_ARGS(ppSource));

    SafeRelease(&pSourceResolver);
    SafeRelease(&pSource);
    return hr;
}

//  Create an activation object for a renderer, based on the stream media type.
HRESULT CreateMediaSinkActivate(IMFStreamDescriptor* pSourceSD,IMFActivate** ppActivate)
{
    IMFMediaTypeHandler* pHandler = NULL;
    IMFActivate* pActivate = NULL;

    // Get the media type handler for the stream.
    HRESULT hr = pSourceSD->GetMediaTypeHandler(&pHandler);

    // Get the major media type.
    GUID guidMajorType;
    if(SUCCEEDED(hr)) hr = pHandler->GetMajorType(&guidMajorType);

    // Create an IMFActivate object for the renderer, based on the media type.
    if(SUCCEEDED(hr))
    {
        if (MFMediaType_Audio == guidMajorType) hr = MFCreateAudioRendererActivate(&pActivate);// Create the audio renderer.
        else hr = E_FAIL;
    }

    // Return IMFActivate pointer to caller.
    if (SUCCEEDED(hr))
    {
        *ppActivate = pActivate;
        (*ppActivate)->AddRef();
    }

    SafeRelease(&pHandler);
    SafeRelease(&pActivate);
    return hr;
}

// Add a source node to a topology.
HRESULT AddSourceNode(IMFTopology* pTopology, IMFMediaSource* pSource, IMFPresentationDescriptor* pPD, IMFStreamDescriptor* pSD, IMFTopologyNode** ppNode)      
{
    IMFTopologyNode* pNode = NULL;

    // Create the node.
    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode);

    // Set the attributes.
    if (SUCCEEDED(hr)) hr = pNode->SetUnknown(MF_TOPONODE_SOURCE, pSource);

    if (SUCCEEDED(hr)) hr = pNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD);

    if (SUCCEEDED(hr)) hr = pNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);

    // Add the node to the topology.
    if (SUCCEEDED(hr)) hr = pTopology->AddNode(pNode);

    // Return the pointer to the caller.
    if (SUCCEEDED(hr))
    {
        *ppNode = pNode;
        (*ppNode)->AddRef();
    }
    
    SafeRelease(&pNode);
    return hr;
}

// Add an output node to a topology.
HRESULT AddOutputNode(IMFTopology* pTopology, IMFActivate* pActivate, DWORD dwId, IMFTopologyNode** ppNode)
{
    IMFTopologyNode* pNode = NULL;

    // Create the node.
    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode);

    // Set the object pointer.
    if (SUCCEEDED(hr)) hr = pNode->SetObject(pActivate);

    // Set the stream sink ID attribute.
    if (SUCCEEDED(hr)) hr = pNode->SetUINT32(MF_TOPONODE_STREAMID, dwId);

    if (SUCCEEDED(hr)) hr = pNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);

    // Add the node to the topology.
    if (SUCCEEDED(hr)) hr = pTopology->AddNode(pNode);

    // Return the pointer to the caller.
    if (SUCCEEDED(hr))
    {
        *ppNode = pNode;
        (*ppNode)->AddRef();
    }

    SafeRelease(&pNode);
    return hr;
}
//</SnippetPlayer.cpp>

//  Add a topology branch for one stream.
//
//  For each stream, this function does the following:
//
//    1. Creates a source node associated with the stream. 
//    2. Creates an output node for the renderer. 
//    3. Connects the two nodes.
//
//  The media session will add any decoders that are needed.

HRESULT AddBranchToPartialTopology(IMFTopology* pTopology, IMFMediaSource* pSource, IMFPresentationDescriptor* pPD, DWORD iStream)
{
    IMFStreamDescriptor* pSD = NULL;
    IMFActivate* pSinkActivate = NULL;
    IMFTopologyNode* pSourceNode = NULL;
    IMFTopologyNode* pOutputNode = NULL;

    BOOL fSelected = FALSE;

    HRESULT hr = pPD->GetStreamDescriptorByIndex(iStream, &fSelected, &pSD);

    if (SUCCEEDED(hr) && fSelected)
    {
        // Create the media sink activation object.
        hr = CreateMediaSinkActivate(pSD, &pSinkActivate);

        // Add a source node for this stream.
        if (SUCCEEDED(hr)) hr = AddSourceNode(pTopology, pSource, pPD, pSD, &pSourceNode);

        // Create the output node for the renderer.
        if (SUCCEEDED(hr)) hr = AddOutputNode(pTopology, pSinkActivate, 0, &pOutputNode);

        // Connect the source node to the output node.
        if (SUCCEEDED(hr)) hr = pSourceNode->ConnectOutput(0, pOutputNode, 0);
    }
    // else: If not selected, don't add the branch. 

    SafeRelease(&pSD);
    SafeRelease(&pSinkActivate);
    SafeRelease(&pSourceNode);
    SafeRelease(&pOutputNode);
    return hr;
}

//  Create a playback topology from a media source.
HRESULT CreatePlaybackTopology(IMFMediaSource* pSource, IMFPresentationDescriptor* pPD, HWND hAudioWnd, IMFTopology** ppTopology)      
{
    IMFTopology* pTopology = NULL;
    DWORD cSourceStreams = 0;

    // Create a new topology.
    HRESULT hr = MFCreateTopology(&pTopology);

    // Get the number of streams in the media source.
    if (SUCCEEDED(hr)) hr = pPD->GetStreamDescriptorCount(&cSourceStreams);

    // For each stream, create the topology nodes and add them to the topology.

    for (DWORD i = 0; i < cSourceStreams; i++)
    {
        if (SUCCEEDED(hr)) hr = AddBranchToPartialTopology(pTopology, pSource, pPD, i);
    }

    // Return the IMFTopology pointer to the caller.
    if (SUCCEEDED(hr))
    {
        *ppTopology = pTopology;
        (*ppTopology)->AddRef();
    }

    SafeRelease(&pTopology);
    return hr;
}