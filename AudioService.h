#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <string>
#include <memory>


using SoundID = unsigned int;

class AudioService
{
public:
	AudioService() = default;
	virtual ~AudioService() = default;

	AudioService(const AudioService&) = delete;
	AudioService(AudioService&&) noexcept = delete;
	AudioService& operator= (const AudioService&) = delete;
	AudioService& operator= (AudioService&&) noexcept = delete;

	virtual void AddSound(const std::wstring& filename, SoundID id) = 0;
	virtual void RemoveSound(SoundID id) = 0;
	virtual void PlaySoundClip(SoundID id, bool repeat) const = 0;
	virtual uint8_t GetMasterVolume() const = 0;
	virtual void SetMasterVolume(uint8_t newVolume) = 0;
	virtual void IncrementMasterVolume() = 0;
	virtual void DecrementMasterVolume() = 0;
	virtual void ToggleMute() = 0;
	virtual void PauseSound(SoundID id) const = 0;
	virtual void PauseAllSounds() const = 0;
	virtual void ResumeSound(SoundID id) const = 0;
	virtual void ResumeAllSounds() const = 0;
	virtual void StopSound(SoundID id) const = 0;
	virtual void StopAllSounds() const = 0;

};

class NullAudio final : public AudioService
{
public:
	NullAudio() = default;
	virtual ~NullAudio() = default;

	NullAudio(const NullAudio&) = delete;
	NullAudio(NullAudio&&) noexcept = delete;
	NullAudio& operator= (const NullAudio&) = delete;
	NullAudio& operator= (NullAudio&&) noexcept = delete;

	virtual void AddSound(const std::wstring&, SoundID) override {}
	virtual void RemoveSound(SoundID id) override {}
	virtual void PlaySoundClip(SoundID, bool) const override {}
	virtual uint8_t GetMasterVolume() const override { return 0; }
	virtual void SetMasterVolume(uint8_t) override {}
	virtual void IncrementMasterVolume() override {}
	virtual void DecrementMasterVolume() override {}
	virtual void ToggleMute() override {}
	virtual void PauseSound(SoundID) const override {}
	virtual void PauseAllSounds() const override {}
	virtual void ResumeSound(SoundID) const override {};
	virtual void ResumeAllSounds() const override {};
	virtual void StopSound(SoundID) const override {}
	virtual void StopAllSounds() const override {}
};

class LogAudio final : public AudioService
{
public:

	LogAudio(std::unique_ptr<AudioService>&& pService) :
		m_pRealService{ std::move(pService) }
	{}
	virtual ~LogAudio() = default;

	LogAudio(const LogAudio&) = delete;
	LogAudio(LogAudio&&) noexcept = delete;
	LogAudio& operator= (const LogAudio&) = delete;
	LogAudio& operator= (LogAudio&&) noexcept = delete;

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

	std::unique_ptr<AudioService> m_pRealService;
};


class AudioLocator final
{
public:

	AudioLocator() = delete;

	AudioLocator(const AudioLocator&) = delete;
	AudioLocator(AudioLocator&&) noexcept = delete;
	AudioLocator& operator= (const AudioLocator&) = delete;
	AudioLocator& operator= (AudioLocator&&) noexcept = delete;

	static AudioService& GetAudioService() { return *m_Instance; }
	static void RegisterAudioService(std::unique_ptr<AudioService>&& audioService)
	{
		if (audioService == nullptr) m_Instance = std::make_unique<NullAudio>();
		else m_Instance = std::move(audioService);
	}
private:
	static std::unique_ptr<AudioService> m_Instance;
};

#endif // !AUDIOSERVICE_H
