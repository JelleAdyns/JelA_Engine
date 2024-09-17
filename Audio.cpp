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
#include <thread>

class Audio::AudioFile
{
public:

	AudioFile(const tstring& filename) :
		m_pPlayer{ nullptr },
		m_FilePath{ ResourceManager::GetInstance().GetDataPath() + filename},
		m_hAudioWnd{}
	{
		HRESULT hr = CPlayer::CreateInstance(m_hAudioWnd, &m_pPlayer);

		if (SUCCEEDED(hr)) OpenFile(m_FilePath);
		else
		{
			Engine::NotifyError(m_hAudioWnd, L"Could not initialize the player object.", hr);
			m_pPlayer->Release();
			return;
		}
	}

	~AudioFile()
	{
		m_pPlayer->Stop();
		m_pPlayer->Shutdown();
		m_pPlayer->Release();
	}

	AudioFile(const AudioFile& other) = delete;
	AudioFile(AudioFile&& other) noexcept = delete;
	AudioFile& operator=(const AudioFile& other) = delete;
	AudioFile& operator=(AudioFile&& other) noexcept = delete;

	void Play(bool repeat, bool resume = false) const
	{
		HRESULT hr = m_pPlayer->Play(repeat, resume);
		if (FAILED(hr)) OutputDebugString((_T("\n\"Play\" reported on error. Filename: ") + m_FilePath + _T('\n')).c_str());
	}
	void Stop() const
	{
		HRESULT hr = m_pPlayer->Stop();
		if (FAILED(hr)) OutputDebugString((_T("\n\"Stop\" reported on error. Filename: ") + m_FilePath + _T('\n')).c_str());
	}
	void Pause() const
	{
		HRESULT hr = m_pPlayer->Pause();
		if (FAILED(hr)) OutputDebugString((_T("\n\"Pause\" reported on error. Filename: ") + m_FilePath + _T('\n')).c_str());
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
	bool IsCreated() const 
	{
		return m_pPlayer != nullptr;
	}
	bool IsReadyToPlay() const 
	{
		return IsCreated() && (m_pPlayer->GetState() == CPlayer::PlayerState::ReadyToStart || IsPlaying() || IsStopped());
	}

private:

	void OpenFile(const tstring& fileName) const
	{
		HRESULT hr = m_pPlayer->OpenURL(fileName);
		if (FAILED(hr)) Engine::NotifyError(m_hAudioWnd, to_wstring(_T("Could not open the file.") + fileName).c_str(), hr);
	}


	CPlayer* m_pPlayer;
	tstring m_FilePath;
	HWND m_hAudioWnd;
};


//Implementation
class Audio::AudioImpl
{
public:
	AudioImpl() { m_Thread = std::jthread{ &Audio::AudioImpl::HandleRequests, this }; }
	~AudioImpl() { m_ServiceIsActive = false; }

	AudioImpl(const AudioImpl&) = delete;
	AudioImpl(AudioImpl&&) noexcept = delete;
	AudioImpl& operator= (const AudioImpl&) = delete;
	AudioImpl& operator= (AudioImpl&&) noexcept = delete;

	void AddSoundImpl(const tstring& filename, SoundID id)
	{
		std::lock_guard<std::mutex> lck{ m_EventsMutex };
		m_Events.push(QueueInfo{ .id{id}, .playBackEvent{Event::Add}, .filename{filename}});
	}
	void RemoveSoundImpl(SoundID id)
	{
		std::lock_guard<std::mutex> lck{ m_EventsMutex };
		m_Events.push(QueueInfo{ .id{id}, .playBackEvent{Event::Remove} });
	}
	void PlaySoundClipImpl(SoundID id, bool repeat)
	{
		std::lock_guard<std::mutex> lck{ m_EventsMutex };
		m_Events.push(QueueInfo{ .repeat{repeat}, .id{id}, .playBackEvent{Event::Play}});
	}
	uint8_t GetMasterVolumeImpl() const
	{
		return CPlayer::GetVolume();
	}
	void SetMasterVolumeImpl(uint8_t volumePercentage)
	{
		HRESULT hr = CPlayer::SetVolume(volumePercentage);
		if (FAILED(hr)) Engine::NotifyError(NULL, to_wstring( _T("SetVolume reported on error.")).c_str() , hr);
	}
	void IncrementMasterVolumeImpl()
	{
		int newVolume{ CPlayer::GetVolume() + 1 };
		HRESULT hr = CPlayer::SetVolume(newVolume);
		if (FAILED(hr)) Engine::NotifyError(NULL, to_wstring( _T("IncrementVolume reported on error.")).c_str(), hr);
	}
	void DecrementMasterVolumeImpl()
	{
		int newVolume{ CPlayer::GetVolume() - 1 };
		HRESULT hr = CPlayer::SetVolume(newVolume);
		if (FAILED(hr)) Engine::NotifyError(NULL, to_wstring( _T("DecrementVolume reported on error.")).c_str(), hr);
	}
	void ToggleMuteImpl()
	{
		m_IsMute = !m_IsMute;
		if (m_IsMute) PauseAllSoundsImpl();
		else ResumeAllSoundsImpl();
	}
	void PauseSoundImpl(SoundID id) 
	{
		std::lock_guard<std::mutex> lck{ m_EventsMutex };
		m_Events.push(QueueInfo{ .id{id}, .playBackEvent{Event::Pause} });
	}
	void PauseAllSoundsImpl()
	{
		std::lock_guard<std::mutex> lck{ m_EventsMutex };
		m_Events.push(QueueInfo{ .allSounds{true}, .playBackEvent{ Event::Pause }});
	}
	void ResumeSoundImpl(SoundID id)
	{
		std::lock_guard<std::mutex> lck{ m_EventsMutex };
		m_Events.push(QueueInfo{ .id{id}, .playBackEvent{Event::Resume} });
	}
	void ResumeAllSoundsImpl()
	{
		std::lock_guard<std::mutex> lck{ m_EventsMutex };
		m_Events.push(QueueInfo{ .allSounds{true}, .playBackEvent{ Event::Resume } });
	}
	void StopSoundImpl(SoundID id)
	{
		std::lock_guard<std::mutex> lck{ m_EventsMutex };
		m_Events.push(QueueInfo{ .id{id}, .playBackEvent{Event::Stop} });
	}
	void StopAllSoundsImpl()
	{
		std::lock_guard<std::mutex> lck{ m_EventsMutex };
		m_Events.push(QueueInfo{ .allSounds{true}, .playBackEvent{ Event::Stop } });
	}

private:
	 
	// PRIVATE DATA
	enum class Event 
	{
		Add,
		Remove,
		Play,
		Pause,
		Resume,
		Stop
	};

	struct AudioInfo
	{
		std::unique_ptr<AudioFile> pAudioFile;
		bool repeat{};
	};

	struct QueueInfo
	{
		bool repeat{false};
		bool allSounds{false};
		SoundID id{};
		Event playBackEvent{};
		tstring filename{};
	};

	//std::map<SoundID,AudioInfo> m_pMapMusicClips{};
	std::queue<QueueInfo> m_Events{};
	std::jthread m_Thread;
	mutable std::mutex m_EventsMutex;

	bool m_ServiceIsActive{ true };
	bool m_IsMute{ false };


	// PRIVATE FUNCTIONS
	void HandleRequests();
	void Add(const tstring& filename, SoundID id, std::map<SoundID, AudioInfo>& audioMap)
	{
		if (not audioMap.contains(id))
			audioMap[id].pAudioFile = std::make_unique<Audio::AudioFile>(filename);
		else OutputDebugString((_T("\nTrying to Add SoundID that is already present. Filename: ") + filename + _T("ID: ") + to_tstring(id) + _T('\n')).c_str());
	}
	void Remove(SoundID id, std::map<SoundID, AudioInfo>& audioMap)
	{
		if (audioMap.contains(id)) audioMap.erase(id);
		else OutputDebugString((_T("\nTrying to remove sound that isn't present. ID: ") + to_tstring(id) + _T('\n')).c_str());
	}
	void Play(SoundID id, bool repeat, std::map<SoundID, AudioInfo>& audioMap)
	{
		if (!m_IsMute)
		{
			if (audioMap.contains(id))
			{
				audioMap.at(id).repeat = repeat;
				AudioFile* audioFile = audioMap.at(id).pAudioFile.get();
				if (audioFile->IsPlaying()) audioFile->Stop();
				audioFile->Play(repeat);
			}
			else OutputDebugString((_T("\nTrying to play sound that isn't present. ID: ") + to_tstring(id) + _T(" Repeat: ") + to_tstring(repeat) + _T('\n')).c_str());
		}
	}

	void Pause(SoundID id, std::map<SoundID, AudioInfo>& audioMap) const
	{
		if (audioMap.contains(id))
		{
			audioMap.at(id).pAudioFile->Pause();
		}
		else OutputDebugString((_T("\nTrying to pause sound that isn't present. ID:") + to_tstring(id) + _T('\n')).c_str());
	}
	void PauseAll(std::map<SoundID, AudioInfo>& audioMap) const
	{
		for (auto& [soundId, audioInfo] : audioMap)
		{
			audioInfo.pAudioFile->Pause();
		}
	}
	void Resume(SoundID id, std::map<SoundID, AudioInfo>& audioMap) const
	{
		if (audioMap.contains(id))
		{
			AudioFile* audioFile = audioMap.at(id).pAudioFile.get();
			if (audioFile->IsPaused()) audioFile->Play(audioMap.at(id).repeat, true);
		}
		else OutputDebugString((_T("\nTrying to resume sound that isn't present. ID: ") + to_tstring(id) + _T('\n')).c_str());
	}
	void ResumeAll(std::map<SoundID, AudioInfo>& audioMap) const
	{
		for (auto& [soundId, audioInfo] : audioMap)
		{
			AudioFile* audioFile = audioInfo.pAudioFile.get();
			if (audioFile->IsPaused()) audioFile->Play(audioInfo.repeat, true);
		}
	}
	void Stop(SoundID id, std::map<SoundID, AudioInfo>& audioMap) const
	{
		if (audioMap.contains(id))
		{
			audioMap.at(id).pAudioFile->Stop();
		}
		else OutputDebugString((_T("\nTrying to stop sound that isn't present. ID: ") + to_tstring(id) + _T('\n')).c_str() );
	}
	void StopAll(std::map<SoundID, AudioInfo>& audioMap) const
	{
		for (auto& [soundId, audioInfo] : audioMap)
		{
			audioInfo.pAudioFile->Stop();
		}
	}
};

void Audio::AudioImpl::HandleRequests()
{
	std::map<SoundID, AudioInfo> pMapMusicClips{};
	while (m_ServiceIsActive)
	{
		QueueInfo info{};
		////////////
		// Events lock
		std::unique_lock<std::mutex> eventsLock{ m_EventsMutex };

		if (m_Events.empty())
		{
			eventsLock.unlock();
			////////////
			continue;
		}
		info = m_Events.front();

		if (info.playBackEvent == Event::Play)
		{
			if (pMapMusicClips.at(info.id).pAudioFile->IsReadyToPlay()) m_Events.pop();
		}
		else m_Events.pop();
		eventsLock.unlock();
		////////////
		
		
		switch (info.playBackEvent)
		{
		case Event::Add:
			Add(info.filename, info.id, pMapMusicClips);
			break;
		case Event::Remove:
			Remove(info.id, pMapMusicClips);
			break;
		case Event::Play:
			if (pMapMusicClips.at(info.id).pAudioFile->IsReadyToPlay())
				Play(info.id, info.repeat, pMapMusicClips);
			break;
		case Event::Pause:
			if (info.allSounds) PauseAll(pMapMusicClips);
			else Pause(info.id, pMapMusicClips);
			break;
		case Event::Resume:
			if (info.allSounds) ResumeAll(pMapMusicClips);
			else Resume(info.id, pMapMusicClips);
			break;
		case Event::Stop:
			if (info.allSounds) StopAll(pMapMusicClips);
			else Stop(info.id, pMapMusicClips);
			break;

		}
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
void Audio::AddSound(const tstring& filename, SoundID id)
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

