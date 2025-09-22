#include "AudioService.h"
#include <windows.h>
#include <format>
#include "Defines.h"

namespace jela
{
	//------------------------------------------------------------------------------------------------------------------------------
	// AudioLocator
	std::unique_ptr<AudioService> AudioLocator::m_Instance{ std::make_unique<NullAudio>() };
	//------------------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------------------
	// SoundInstanceID
	SoundInstanceID::~SoundInstanceID() { if (m_pSubject) m_pSubject->RemoveObserver(this); }

	SoundInstanceID::SoundInstanceID(const SoundInstanceID& other)
        : m_Id{other.m_Id}
          , m_pSubject{other.m_pSubject}
    {
        if (m_pSubject) m_pSubject->AddObserver(this);
    }

	SoundInstanceID::SoundInstanceID(SoundInstanceID&& other) noexcept
        : m_Id{std::move(other.m_Id)}
          , m_pSubject{std::move(other.m_pSubject) }
	{
		if (m_pSubject)
		{
			m_pSubject->RemoveObserver(&other);
			m_pSubject->AddObserver(this);
		}

		other.m_pSubject = nullptr;
        other.m_Id = std::nullopt;
	}

	SoundInstanceID& SoundInstanceID::operator= (const SoundInstanceID& other)
	{
		m_pSubject = other.m_pSubject;
		m_Id = other.m_Id;
		if (m_pSubject) m_pSubject->AddObserver(this);

		return *this;
	}

	SoundInstanceID& SoundInstanceID::operator= (SoundInstanceID&& other) noexcept
	{
		m_pSubject = std::move(other.m_pSubject);
        m_Id = std::move(other.m_Id);

		if (m_pSubject)
		{
			m_pSubject->RemoveObserver(&other);
			m_pSubject->AddObserver(this);
        }

        other.m_pSubject = nullptr;
		other.m_Id = std::nullopt;

		return *this;
    }

    void SoundInstanceID::Init(uint8_t index)
	{
		if (!m_Id.has_value()) m_Id = index;
		else OutputDebugString(_T("SoundInstanceID was already initialized when trying to initialize."));
	}
	void SoundInstanceID::SaveSubject(Subject<uint8_t, std::vector<SoundInstanceID*>&>* pSubject)
	{
		if (pSubject) m_pSubject = pSubject;
		else OutputDebugString(_T("Subject was nullptr when trying to save it to a SoundInstanceID SingleSubjectsObserver."));
	}

	void SoundInstanceID::Notify(uint8_t index, std::vector<SoundInstanceID*>& vecThisObservers)
    {
        if (m_Id.has_value() && m_Id.value() == index)
		{
			m_Id = std::nullopt;
			m_pSubject = nullptr;
			vecThisObservers.emplace_back(this);
		}
	}
	void SoundInstanceID::OnSubjectDestroy(Subject<uint8_t, std::vector<SoundInstanceID*>&>* pSubject)
	{
		if (pSubject == m_pSubject) m_Id = std::nullopt;
		m_pSubject = nullptr;
	}
	//------------------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------------------
	// LogAudio
	void LogAudio::AddSound(const tstring& path, SoundID id)
	{
		OutputDebugString(std::format(_T("LogAudio: AddSound: path: {}, id: {}\n"), path, id).c_str());
		m_pRealService->AddSound(path, id);
	}
	void LogAudio::RemoveSound(SoundID id)
	{
		OutputDebugString(std::format(_T("LogAudio: RemoveSound: id: {}\n"), id).c_str());
		m_pRealService->RemoveSound(id);
	}
	void LogAudio::PlaySoundClip(SoundID id, bool repeat, uint8_t volume, float frequency) const
    {
        OutputDebugString(
            std::format(
                _T("LogAudio: PlaySoundClip: id: {}, repeat: {}, Volume {}, Frequency {}\n"), id, repeat,
                static_cast<int>(volume), frequency).c_str());
		m_pRealService->PlaySoundClip(id, repeat, volume, frequency);
	}
	void LogAudio::PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId, uint8_t volume, float frequency) const
	{
		OutputDebugString(std::format(_T("LogAudio: PlaySoundInstance: id: {}, repeat: {}, Instance id: {}, Volume {}, Frequency {}\n"),
			id,
			repeat,
			instanceId.GetID().has_value() ? to_tstring(instanceId.GetID().value()) : _T("std::nullopt"),
			volume,
			frequency
		).c_str());
		m_pRealService->PlaySoundInstance(id, repeat, instanceId, volume);
	}
	uint8_t LogAudio::GetMasterVolume() const
	{
		const uint8_t volume = m_pRealService->GetMasterVolume();
		OutputDebugString(std::format(_T("LogAudio: Returned master volume: {}\n"), volume).c_str());
		return volume;
	}
	void LogAudio::SetMasterVolume(uint8_t newVolume)
	{
		OutputDebugString(std::format(_T("LogAudio: New master volume: {}\n"), newVolume).c_str());
		m_pRealService->SetMasterVolume(newVolume);
	}
	void LogAudio::IncrementMasterVolume()
	{
		OutputDebugString(_T("LogAudio: Increment master volume.\n"));
		m_pRealService->IncrementMasterVolume();
	}
	void LogAudio::DecrementMasterVolume()
	{
		OutputDebugString(_T("LogAudio: Decrement master volume.\n"));
		m_pRealService->DecrementMasterVolume();
	}
	void LogAudio::ToggleMute()
	{
		OutputDebugString(_T("LogAudio: Toggling mute.\n"));
		m_pRealService->ToggleMute();
	}
	void LogAudio::PauseSound(SoundID id) const
	{
		OutputDebugString(std::format(_T("LogAudio: PauseSound: id: {}\n"), id).c_str());
		m_pRealService->PauseSound(id);
	}
	void LogAudio::PauseSound(SoundID id, const SoundInstanceID& instanceId) const
	{
		OutputDebugString(std::format(_T("LogAudio: PauseSound: id: {}, Instance id: {}\n"), id, instanceId.GetID().has_value() ? to_tstring(instanceId.GetID().value()) : _T("std::nullopt")).c_str());
		m_pRealService->PauseSound(id, instanceId);
	}
	void LogAudio::PauseAllSounds() const
	{
		OutputDebugString(_T("LogAudio: PauseAllSounds\n"));
		m_pRealService->PauseAllSounds();
	}
	void LogAudio::ResumeSound(SoundID id) const
	{
		OutputDebugString(std::format(_T("LogAudio: ResumeSound: id: {}\n"), id).c_str());
		m_pRealService->ResumeSound(id);
	}
	void LogAudio::ResumeSound(SoundID id, const SoundInstanceID& instanceId) const
	{
		OutputDebugString(std::format(_T("LogAudio: ResumeSound: id: {}, Instance id: {}\n"), id, instanceId.GetID().has_value() ? to_tstring(instanceId.GetID().value()) : _T("std::nullopt")).c_str());
		m_pRealService->ResumeSound(id, instanceId);
	}
	void LogAudio::ResumeAllSounds() const
	{
		OutputDebugString(_T("LogAudio: ResumeAllSounds\n"));
		m_pRealService->ResumeAllSounds();
	}
	void LogAudio::StopSound(SoundID id) const
	{
		OutputDebugString(std::format(_T("LogAudio: StopSound: id: {}\n"), id).c_str());
		m_pRealService->StopSound(id);
	}
	void LogAudio::StopSound(SoundID id, const SoundInstanceID& instanceId) const
	{
		OutputDebugString(std::format(_T("LogAudio: StopSound: id: {}, Instance id: {}\n"), id, instanceId.GetID().has_value() ? to_tstring(instanceId.GetID().value()) : _T("std::nullopt")).c_str());
		m_pRealService->StopSound(id, instanceId);
	}
	void LogAudio::StopAllSounds() const
	{
		OutputDebugString(_T("LogAudio: StopAllSounds\n"));
		m_pRealService->StopAllSounds();
	}
	//------------------------------------------------------------------------------------------------------------------------------
}
