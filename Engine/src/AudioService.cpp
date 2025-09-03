#include "AudioService.h"
#include <iostream>
#include <windows.h>
#include <format>
#include "Defines.h"

namespace jela
{
	std::unique_ptr<AudioService> AudioLocator::m_Instance{ std::make_unique<NullAudio>() };

	void LogAudio::AddSound(const tstring& path, SoundID id)
	{
		OutputDebugString(std::format(_T("LogAudio: \nAddSound: path: {}, id: {}\n"), path, id).c_str());
		m_pRealService->AddSound(path, id);
	}
	void LogAudio::RemoveSound(SoundID id)
	{
		OutputDebugString(std::format(_T("LogAudio: \nRemoveSound: id: {}\n"), id).c_str());
		m_pRealService->RemoveSound(id);
	}
	void LogAudio::PlaySoundClip(SoundID id, bool repeat) const
	{
		OutputDebugString(std::format(_T("LogAudio: \nPlaySoundClip: id: {}, repeat: {}\n"), id, repeat).c_str());
		m_pRealService->PlaySoundClip(id, repeat);
	}
	uint8_t LogAudio::GetMasterVolume() const
	{
		const uint8_t volume = m_pRealService->GetMasterVolume();
		OutputDebugString(std::format(_T("LogAudio: \nReturned master volume: {}\n"), volume).c_str());
		return volume;
	}
	void LogAudio::SetMasterVolume(uint8_t newVolume)
	{
		OutputDebugString(std::format(_T("LogAudio: \nNew master volume: {}\n"), newVolume).c_str());
		m_pRealService->SetMasterVolume(newVolume);
	}
	void LogAudio::IncrementMasterVolume()
	{
		OutputDebugString(_T("LogAudio: \nIncrement master volume.\n"));
		m_pRealService->IncrementMasterVolume();
	}
	void LogAudio::DecrementMasterVolume()
	{
		OutputDebugString(_T("LogAudio: \nDecrement master volume.\n"));
		m_pRealService->DecrementMasterVolume();
	}
	void LogAudio::ToggleMute()
	{
		OutputDebugString(_T("LogAudio: \nToggling mute.\n"));
		m_pRealService->ToggleMute();
	}
	void LogAudio::PauseSound(SoundID id) const
	{
		OutputDebugString(std::format(_T("LogAudio: \nPauseSound: id: {}\n"), id).c_str());
		m_pRealService->PauseSound(id);
	}
	void LogAudio::PauseAllSounds() const
	{
		OutputDebugString(_T("LogAudio: \nPauseAllSounds\n"));
		m_pRealService->PauseAllSounds();
	}
	void LogAudio::ResumeSound(SoundID id) const
	{
		OutputDebugString(std::format(_T("LogAudio: \nResumeSound: id: {}\n"), id).c_str());
		m_pRealService->ResumeSound(id);
	}
	void LogAudio::ResumeAllSounds() const
	{
		OutputDebugString(_T("LogAudio: \nResumeAllSounds\n"));
		m_pRealService->ResumeAllSounds();
	}
	void LogAudio::StopSound(SoundID id) const
	{
		OutputDebugString(std::format(_T("LogAudio: \nStopSound: id: {}\n"), id).c_str());
		m_pRealService->StopSound(id);
	}
	void LogAudio::StopAllSounds() const
	{
		OutputDebugString(_T("LogAudio: \nStopAllSounds\n"));
		m_pRealService->StopAllSounds();
	}
}
