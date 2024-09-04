#include "Engine.h"
#include "CPlayer.h"
#include "framework.h"
#include <map>
#include <queue>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <iostream>
#include <algorithm>

class Audio::AudioFile
{
public:

	AudioFile(const std::wstring & filename);
	~AudioFile();

	AudioFile(const AudioFile & other) = delete;
	AudioFile(AudioFile && other) noexcept = delete;
	AudioFile& operator=(const AudioFile & other) = delete;
	AudioFile& operator=(AudioFile && other) noexcept = delete;

	void Play(bool repeat, bool resume = false) const
	{
		HRESULT hr = m_pPlayer->Play(repeat, resume);
		if (FAILED(hr)) Engine::NotifyError(m_hAudioWnd, L"Play reported on error.", hr);
	}
	void Stop() const
	{
		HRESULT hr = m_pPlayer->Stop();
		if (FAILED(hr)) Engine::NotifyError(m_hAudioWnd, L"Stop reported on error.", hr);
	}
	void Pause() const
	{
		HRESULT hr = m_pPlayer->Pause();
		if (FAILED(hr)) Engine::NotifyError(m_hAudioWnd, L"Pause reported on error.", hr);
	}
	bool IsPlaying() const
	{
		return m_pPlayer->GetState() == CPlayer::PlayerState::Started;
	}
	bool IsStopped() const
	{
		return m_pPlayer->GetState() == CPlayer::PlayerState::Stopped;
	}
	bool IsPaused() const
	{
		return m_pPlayer->GetState() == CPlayer::PlayerState::Paused;
	}
	
private:

	static LRESULT CALLBACK AudioProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnEvent(WPARAM wParam);

	void OpenFile(const std::wstring & fileName) const;


	CPlayer* m_pPlayer;
	std::wstring m_FilePath;
	HWND m_hAudioWnd;
	HWND m_hEventWnd;
};

Audio::AudioFile::AudioFile(const std::wstring& filename) :
	m_pPlayer{ nullptr },
	m_FilePath{},
	m_hAudioWnd{},
	m_hEventWnd{}
{
	//next 3 lines are code from Kevin Hoefman, teacher at Howest, DAE in Kortrijk
	m_hEventWnd = CreateWindow(TEXT("STATIC"), TEXT(""), 0, 0, 0, 0, 0, 0, 0, ENGINE.GetHInstance(), 0);
	SetWindowLongPtr(m_hEventWnd, GWLA_WNDPROC, (LONG_PTR)AudioProc);	// set the custom message loop (subclassing)
	SetWindowLongPtr(m_hEventWnd, GWLP_USERDATA, (LONG_PTR)this);		// set this object as the parameter for the Proc

	HRESULT hr = CPlayer::CreateInstance(m_hAudioWnd, m_hEventWnd, &m_pPlayer);

	m_FilePath = ENGINE.GetResourcePath() + filename;

	if (SUCCEEDED(hr)) OpenFile(m_FilePath);
	else
	{
		Engine::NotifyError(m_hAudioWnd, L"Could not initialize the player object.", hr);
		m_pPlayer->Release();
		return;
	}

}
Audio::AudioFile::~AudioFile()
{
	m_pPlayer->Stop();
	m_pPlayer->Shutdown();
	m_pPlayer->Release();
}

LRESULT Audio::AudioFile::AudioProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//next 3 lines are code from Kevin Hoefman, teacher at Howest, DAE in Kortrijk
#pragma warning(disable: 4312)
	Audio::AudioFile* audio = reinterpret_cast<Audio::AudioFile*>(GetWindowLongPtr(hWnd, GWLA_USERDATA));
#pragma warning(default: 4312)

	switch (message)
	{
	case CPlayer::WM_APP_PLAYER_EVENT:
		audio->OnEvent(wParam);
		break;
	default:
		break;
	};

	return 0;
}

void Audio::AudioFile::OnEvent(WPARAM wParam)
{
	m_pPlayer->HandleEvent(wParam);
}

void Audio::AudioFile::OpenFile(const std::wstring& fileName) const
{
	HRESULT hr = m_pPlayer->OpenURL(fileName);
	if (FAILED(hr)) Engine::NotifyError(m_hAudioWnd, L"Could not open the file.", hr);
}

//Implementation
class Audio::AudioImpl
{
public:
	AudioImpl() = default;
	~AudioImpl() = default;

	AudioImpl(const AudioImpl&) = delete;
	AudioImpl(AudioImpl&&) noexcept = delete;
	AudioImpl& operator= (const AudioImpl&) = delete;
	AudioImpl& operator= (AudioImpl&&) noexcept = delete;

	void AddSoundImpl(const std::wstring& filename, SoundID id);
	void RemoveSoundImpl(SoundID id);
	void PlaySoundClipImpl(SoundID id, bool repeat);
	uint8_t GetMasterVolumeImpl() const;
	void SetMasterVolumeImpl(uint8_t newVolume);
	void IncrementMasterVolumeImpl();
	void DecrementMasterVolumeImpl();
	void ToggleMuteImpl();
	void PauseSoundImpl(SoundID id) const;
	void PauseAllSoundsImpl() const;
	void ResumeSoundImpl(SoundID id) const;
	void ResumeAllSoundsImpl() const;
	void StopSoundImpl(SoundID id) const;
	void StopAllSoundsImpl() const;

private:

	struct AudioInfo
	{
		std::unique_ptr<AudioFile> pAudioFile;
		bool repeat;
	};

	std::map<SoundID,AudioInfo> m_pMapMusicClips{};

	bool m_IsMute{ false };

};


void Audio::AudioImpl::AddSoundImpl(const std::wstring& filename, SoundID id)
{
	if(not m_pMapMusicClips.contains(id))
		m_pMapMusicClips[id].pAudioFile = std::make_unique<Audio::AudioFile>(filename);
}
void Audio::AudioImpl::RemoveSoundImpl(SoundID id)
{
	if (m_pMapMusicClips.contains(id)) m_pMapMusicClips.erase(id);
	else OutputDebugString((_T("Trying to remove sound that isn't present. ID: ") + to_tstring(id)).c_str());
}
void Audio::AudioImpl::PlaySoundClipImpl(SoundID id, bool repeat)
{
	if(!m_IsMute)
	{
		if (m_pMapMusicClips.contains(id))
		{
			m_pMapMusicClips.at(id).repeat = repeat;
			AudioFile* audioFile = m_pMapMusicClips.at(id).pAudioFile.get();
			if (audioFile->IsPlaying()) audioFile->Stop();
			audioFile->Play(repeat);
		}
		else OutputDebugString((_T("Trying to play sound that isn't present. ID: ") + to_tstring(id) + _T(" Repeat: ") + to_tstring(repeat)).c_str());
	}
		
}
uint8_t Audio::AudioImpl::GetMasterVolumeImpl() const
{
	return CPlayer::GetVolume();
}
void Audio::AudioImpl::SetMasterVolumeImpl(uint8_t volumePercentage)
{
	HRESULT hr = CPlayer::SetVolume(volumePercentage);
	if (FAILED(hr)) Engine::NotifyError(NULL, L"SetVolume reported on error.", hr);
}
void Audio::AudioImpl::IncrementMasterVolumeImpl()
{
	int newVolume{ CPlayer::GetVolume() + 1 };
	HRESULT hr = CPlayer::SetVolume(newVolume);
	if (FAILED(hr)) Engine::NotifyError(NULL, L"IncrementVolume reported on error.", hr);
}
void Audio::AudioImpl::DecrementMasterVolumeImpl()
{
	int newVolume{ CPlayer::GetVolume() - 1 };
	HRESULT hr = CPlayer::SetVolume(newVolume);
	if (FAILED(hr)) Engine::NotifyError(NULL, L"IncrementVolume reported on error.", hr);
}

void Audio::AudioImpl::ToggleMuteImpl()
{
	m_IsMute = !m_IsMute;
	if (m_IsMute) PauseAllSoundsImpl();
	else ResumeAllSoundsImpl();
}
void Audio::AudioImpl::PauseSoundImpl(SoundID id) const
{
	if (m_pMapMusicClips.contains(id))
	{
		m_pMapMusicClips.at(id).pAudioFile->Pause();
	}
	else OutputDebugString((_T("Trying to pause sound that isn't present. ID:") + to_tstring(id)).c_str());
}
void Audio::AudioImpl::PauseAllSoundsImpl() const
{
	for (auto& [soundId, audioInfo] : m_pMapMusicClips)
	{
		audioInfo.pAudioFile->Pause();
	}
}
void Audio::AudioImpl::ResumeSoundImpl(SoundID id) const
{
	if (m_pMapMusicClips.contains(id))
	{
		AudioFile* audioFile = m_pMapMusicClips.at(id).pAudioFile.get();
		if(audioFile->IsPaused()) audioFile->Play(m_pMapMusicClips.at(id).repeat, true);
	}
	else OutputDebugString((_T("Trying to resume sound that isn't present. ID: ") + to_tstring(id)).c_str());
}
void Audio::AudioImpl::ResumeAllSoundsImpl() const
{
	for (auto& [soundId, audioInfo] : m_pMapMusicClips)
	{
		AudioFile* audioFile = audioInfo.pAudioFile.get();
		if (audioFile->IsPaused()) audioFile->Play(soundId, true);
	}
}
void Audio::AudioImpl::StopSoundImpl(SoundID id) const
{
	if (m_pMapMusicClips.contains(id))
	{
		m_pMapMusicClips.at(id).pAudioFile->Stop();
	}
	else OutputDebugString((_T("Trying to stop sound that isn't present. ID: ") + to_tstring(id)).c_str());
}
void Audio::AudioImpl::StopAllSoundsImpl() const
{
	for (auto& [soundId, audioInfo] : m_pMapMusicClips)
	{
		audioInfo.pAudioFile->Stop();
	}
}

//Audio
Audio::Audio() :
	m_pImpl{ new AudioImpl{} }
{
}

Audio::~Audio()
{
	delete m_pImpl;
	MFShutdown();
}
void Audio::AddSound(const std::wstring& filename, SoundID id)
{
	m_pImpl->AddSoundImpl(filename, id);
}
void Audio::RemoveSound(SoundID id)
{
	m_pImpl->RemoveSoundImpl(id);
}
void Audio::PlaySoundClip(SoundID id, bool repeat) const
{
	m_pImpl->PlaySoundClipImpl(id, repeat);
}
uint8_t Audio::GetMasterVolume() const
{
	return m_pImpl->GetMasterVolumeImpl();
}
void Audio::SetMasterVolume(uint8_t newVolume)
{
	m_pImpl->SetMasterVolumeImpl(newVolume);
}
void Audio::IncrementMasterVolume()
{
	m_pImpl->IncrementMasterVolumeImpl();
}
void Audio::DecrementMasterVolume()
{
	m_pImpl->DecrementMasterVolumeImpl();
}
void Audio::ToggleMute()
{
	m_pImpl->ToggleMuteImpl();
}
void Audio::PauseSound(SoundID id) const
{
	m_pImpl->PauseSoundImpl(id);
}
void Audio::PauseAllSounds() const
{
	m_pImpl->PauseAllSoundsImpl();
}
void Audio::ResumeSound(SoundID id) const
{
	m_pImpl->ResumeSoundImpl(id);
}
void Audio::ResumeAllSounds() const
{
	m_pImpl->ResumeAllSoundsImpl();
}
void Audio::StopSound(SoundID id) const
{
	m_pImpl->StopSoundImpl(id);
}
void Audio::StopAllSounds() const
{
	m_pImpl->StopAllSoundsImpl();
}

