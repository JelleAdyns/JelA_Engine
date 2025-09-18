#ifndef AUDIO_H
#define AUDIO_H

#include "AudioService.h"

namespace jela
{

    class Audio final : public AudioService
    {
    public:

        Audio();
        virtual ~Audio();

        Audio(const Audio&) = delete;
        Audio(Audio&&) noexcept = delete;
        Audio& operator= (const Audio&) = delete;
        Audio& operator= (Audio&&) noexcept = delete;

        virtual void AddSound(const tstring& filename, SoundID id) override;
        virtual void RemoveSound(SoundID id) override;
        virtual void PlaySoundClip(SoundID id, bool repeat, uint8_t volume = 100, float frequency = 1.f) const override;
        virtual void PlaySoundInstance(SoundID, bool, SoundInstanceID&, uint8_t = 100, float = 1.f) const override { OutputDebugString(_T("The 'Audio' Service does not support instance sounds.")); }
        virtual uint8_t GetMasterVolume() const override;
        virtual void SetMasterVolume(uint8_t newVolume) override;
        virtual void IncrementMasterVolume() override;
        virtual void DecrementMasterVolume() override;
        virtual void ToggleMute() override;
        virtual void PauseSound(SoundID id) const override;
        virtual void PauseSound(SoundID, const SoundInstanceID&) const override { OutputDebugString(_T("The 'Audio' Service does not support instance sounds.")); }
        virtual void PauseAllSounds() const override;
        virtual void ResumeSound(SoundID id) const override;
        virtual void ResumeSound(SoundID, const SoundInstanceID&) const override { OutputDebugString(_T("The 'Audio' Service does not support instance sounds.")); }
        virtual void ResumeAllSounds() const override;
        virtual void StopSound(SoundID id) const override;
        virtual void StopSound(SoundID, const SoundInstanceID&) const override { OutputDebugString(_T("The 'Audio' Service does not support instance sounds.")); }
        virtual void StopAllSounds() const override;

    private:

        class AudioFile;

        class AudioImpl;
        AudioImpl* m_pImpl;
    };

    class XAudio final : public AudioService
    {
    public:

        XAudio();
        virtual ~XAudio();

        XAudio(const XAudio&) = delete;
        XAudio(XAudio&&) noexcept = delete;
        XAudio& operator= (const XAudio&) = delete;
        XAudio& operator= (XAudio&&) noexcept = delete;

        virtual void AddSound(const tstring& filename, SoundID id) override;
        virtual void RemoveSound(SoundID id) override;
        virtual void PlaySoundClip(SoundID id, bool repeat, uint8_t volume = 100, float frequency = 1.f) const override;
        virtual void PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId, uint8_t volume = 100, float frequency = 1.f) const override;
        virtual uint8_t GetMasterVolume() const override;
        virtual void SetMasterVolume(uint8_t newVolume) override;
        virtual void IncrementMasterVolume() override;
        virtual void DecrementMasterVolume() override;
        virtual void ToggleMute() override;
        virtual void PauseSound(SoundID id) const override;
        virtual void PauseSound(SoundID id, const SoundInstanceID& instanceId) const override;
        virtual void PauseAllSounds() const override;
        virtual void ResumeSound(SoundID id) const override;
        virtual void ResumeSound(SoundID id, const SoundInstanceID& instanceId) const override;
        virtual void ResumeAllSounds() const override;
        virtual void StopSound(SoundID id) const override;
        virtual void StopSound(SoundID id, const SoundInstanceID& instanceId) const override;
        virtual void StopAllSounds() const override;

        static void SetNrOfChannelsPerFormat(uint16_t amount);

    private:
        class AudioImpl;
        AudioImpl* m_pImpl;

        inline static uint16_t m_NrOfChannelsPerFormat{ 64 };
        inline static bool m_ChannelPoolCreated{ false };

    };

}

#endif // !AUDIO_H
