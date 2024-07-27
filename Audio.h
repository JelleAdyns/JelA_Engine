#ifndef AUDIO_H
#define AUDIO_H

#include "AudioService.h"

class Audio final : public AudioService
{
public:

    Audio();
    virtual ~Audio();

    Audio(const Audio&) = delete;
    Audio(Audio&&) noexcept = delete;
    Audio& operator= (const Audio&) = delete;
    Audio& operator= (Audio&&) noexcept = delete;

    virtual void AddSound(const std::wstring& filename, SoundID id) override;
    virtual void RemoveSound(SoundID id) override;
    virtual void PlaySoundClip(SoundID id, bool repeat) const override;
    virtual uint8_t GetMasterVolume() const override;
    virtual void SetMasterVolume(uint8_t newVolume) override;
    virtual void IncrementMasterVolume() override;
    virtual void DecrementMasterVolume() override;
    virtual void ToggleMute() override;
    virtual void PauseSound(SoundID id) const override;
    virtual void PauseAllSounds() const override;
    virtual void ResumeSound(SoundID id) const override;
    virtual void ResumeAllSounds() const override;
    virtual void StopSound(SoundID id) const override;
    virtual void StopAllSounds() const override;



private:

    class AudioFile;

    class AudioImpl;
    AudioImpl* m_pImpl;
};


#endif // !AUDIO_H
