#include "AudioService.h"
#include <iostream>
#include <windows.h>
#include "Defines.h"

std::unique_ptr<AudioService> AudioLocator::m_Instance{ std::make_unique<NullAudio>() };

void LogAudio::AddSound(const tstring& path, SoundID id)
{
	m_pRealService->AddSound(path, id);
	OutputDebugString((_T("\nAddSound: path: ") + path + _T(", id: ") + to_tstring(id) + _T('\n')).c_str());
}
void LogAudio::RemoveSound(SoundID id)
{
	m_pRealService->RemoveSound(id);
	OutputDebugString((_T("\nRemoveSound: id: ") + to_tstring(id) + _T('\n')).c_str());
}
void LogAudio::PlaySoundClip(SoundID id, bool repeat) const
{
	m_pRealService->PlaySoundClip(id, repeat);
	OutputDebugString((_T("\nPlaySoundClip: id: ") + to_tstring(id) + _T(", repeat: ") + to_tstring(repeat) + _T('\n')).c_str());
}
uint8_t LogAudio::GetMasterVolume() const
{
	uint8_t volume = m_pRealService->GetMasterVolume();
	OutputDebugString((_T("\nReturned volume: ") + to_tstring(volume) + _T('\n')).c_str());
	return volume;
}
void LogAudio::SetMasterVolume(uint8_t newVolume)
{
	m_pRealService->SetMasterVolume(newVolume);
	OutputDebugString((_T("\nNew volume: ") + to_tstring(newVolume) + _T('\n')).c_str());
}
void LogAudio::IncrementMasterVolume()
{
	m_pRealService->IncrementMasterVolume();
	OutputDebugString(_T("\nIncrement volume.\n"));
}
void LogAudio::DecrementMasterVolume()
{
	m_pRealService->DecrementMasterVolume();
	OutputDebugString(_T("\nDecrement volume.\n"));
}
void LogAudio::ToggleMute()
{
	m_pRealService->ToggleMute();
	OutputDebugString(_T("\nToggling mute.\n"));
}
void LogAudio::PauseSound(SoundID id) const
{
	m_pRealService->PauseSound(id);
	OutputDebugString((_T("\nPauseSound: id: ") + to_tstring(id) + _T('\n')).c_str());
}
void LogAudio::PauseAllSounds() const
{
	m_pRealService->PauseAllSounds();
	OutputDebugString(_T("\nPauseAllSounds\n"));
}
void LogAudio::ResumeSound(SoundID id) const
{
	m_pRealService->ResumeSound(id);
	OutputDebugString((_T("\nResumeSound: id: ") + to_tstring(id) + _T('\n')).c_str());
}
void LogAudio::ResumeAllSounds() const
{
	m_pRealService->ResumeAllSounds();
	OutputDebugString(_T("\nResumeAllSounds\n"));
}
void LogAudio::StopSound(SoundID id) const
{
	m_pRealService->StopSound(id);
	OutputDebugString((_T("\nStopSound: id: ") + to_tstring(id) + _T('\n')).c_str());
}
void LogAudio::StopAllSounds() const
{
	m_pRealService->StopAllSounds();
	OutputDebugString(_T("\nStopAllSounds\n"));
}
