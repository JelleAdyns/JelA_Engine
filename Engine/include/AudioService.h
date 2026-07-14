#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <string>
#include <memory>
#include "Observer.h"
#include "Defines.h"

namespace jela
{

	using SoundID = unsigned int;
	struct SoundInstanceID final : public Observer<uint8_t, std::vector<SoundInstanceID*>&>
	{
        std::optional<uint8_t> GetID() const { return m_Id; }

        SoundInstanceID() = default;

        //--------------------------------------------------------------
        // IMPLEMENTED

        ~SoundInstanceID() override;
		SoundInstanceID(const SoundInstanceID& other);
		SoundInstanceID(SoundInstanceID&& other) noexcept;
		SoundInstanceID& operator= (const SoundInstanceID& other);
		SoundInstanceID& operator= (SoundInstanceID&& other) noexcept;
		//--------------------------------------------------------------

		void Init(uint8_t index);
		void SaveSubject(Subject<uint8_t, std::vector<SoundInstanceID*>&>* pSubject);
	private:

		void Notify(uint8_t index, std::vector<SoundInstanceID*>& vecThisObservers) override;
		void OnSubjectDestroy(Subject<uint8_t, std::vector<SoundInstanceID*>&>* pSubject) override;
		void swap(SoundInstanceID& other) noexcept
		{
			std::swap(m_Id, other.m_Id);

			other.m_pSubject->RemoveObserver(&other);
			m_pSubject->RemoveObserver(this);
			std::swap(m_pSubject, other.m_pSubject);
			other.m_pSubject->AddObserver(&other);
			m_pSubject->AddObserver(this);
		}
        std::optional<uint8_t> m_Id{};

		Subject<uint8_t, std::vector<SoundInstanceID*>&>* m_pSubject{};
	};

	class AudioService
	{
	public:
		AudioService() = default;
		virtual ~AudioService() = default;

		AudioService(const AudioService&) = delete;
		AudioService(AudioService&&) noexcept = delete;
		AudioService& operator= (const AudioService&) = delete;
		AudioService& operator= (AudioService&&) noexcept = delete;

		virtual void AddSound(const tstring& filename, SoundID id) = 0;
		virtual void RemoveSound(SoundID id) = 0;
		virtual void PlaySoundClip(SoundID id, bool repeat) const = 0;
		virtual void PlaySoundClip(SoundID id, bool repeat, uint8_t volume) const = 0;
		virtual void PlaySoundClip(SoundID id, bool repeat, uint8_t volume, float frequency) const = 0;
		virtual void PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId) const = 0;
		virtual void PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId, uint8_t volume) const = 0;
		virtual void PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId, uint8_t volume, float frequency) const = 0;
		virtual uint8_t GetMasterVolume() const = 0;
		virtual void SetMasterVolume(uint8_t newVolume) = 0;
		virtual void IncrementMasterVolume() = 0;
		virtual void DecrementMasterVolume() = 0;
		virtual void ToggleMute() = 0;
		virtual void PauseSound(SoundID id) const = 0;
		virtual void PauseSound(SoundID id, const SoundInstanceID& instanceId) const = 0;
		virtual void PauseAllSounds() const = 0;
		virtual void ResumeSound(SoundID id) const = 0;
		virtual void ResumeSound(SoundID id, const SoundInstanceID& instanceId) const = 0;
		virtual void ResumeAllSounds() const = 0;
		virtual void StopSound(SoundID id) const = 0;
		virtual void StopSound(SoundID id, const SoundInstanceID& instanceId) const = 0;
		virtual void StopAllSounds() const = 0;

	};

	class NullAudio final : public AudioService
	{
	public:
		NullAudio() = default;
		~NullAudio() override = default;

		NullAudio(const NullAudio&) = delete;
		NullAudio(NullAudio&&) noexcept = delete;
		NullAudio& operator= (const NullAudio&) = delete;
		NullAudio& operator= (NullAudio&&) noexcept = delete;

		void AddSound(const tstring&, SoundID) override {}
		void RemoveSound(SoundID) override {}
		void PlaySoundClip(SoundID, bool) const override {}
		void PlaySoundClip(SoundID, bool, uint8_t) const override {}
		void PlaySoundClip(SoundID, bool, uint8_t, float) const override {}
		void PlaySoundInstance(SoundID, bool, SoundInstanceID&) const override {}
		void PlaySoundInstance(SoundID, bool, SoundInstanceID&, uint8_t) const override {}
		void PlaySoundInstance(SoundID, bool, SoundInstanceID&, uint8_t, float) const override {}
		uint8_t GetMasterVolume() const override { return 0; }
		void SetMasterVolume(uint8_t) override {}
		void IncrementMasterVolume() override {}
		void DecrementMasterVolume() override {}
		void ToggleMute() override {}
		void PauseSound(SoundID) const override {};
		void PauseSound(SoundID, const SoundInstanceID&) const override{};
		void PauseAllSounds() const override{};
		void ResumeSound(SoundID) const override{};
		void ResumeSound(SoundID, const SoundInstanceID&) const override{};
		void ResumeAllSounds() const override{};
		void StopSound(SoundID) const override{};
		void StopSound(SoundID, const SoundInstanceID&) const override{};
		void StopAllSounds() const override{};
	};

	class LogAudio final : public AudioService
	{
	public:
		explicit LogAudio(std::unique_ptr<AudioService>&& pService) :
			m_pRealService{ std::move(pService) }
		{}
		~LogAudio() override = default;

		LogAudio(const LogAudio&) = delete;
		LogAudio(LogAudio&&) noexcept = delete;
		LogAudio& operator= (const LogAudio&) = delete;
		LogAudio& operator= (LogAudio&&) noexcept = delete;

		void AddSound(const tstring& filename, SoundID id) override;
		void RemoveSound(SoundID id) override;
		void PlaySoundClip(SoundID id, bool repeat) const override;
		void PlaySoundClip(SoundID id, bool repeat, uint8_t volume) const override;
		void PlaySoundClip(SoundID id, bool repeat, uint8_t volume, float frequency) const override;
		void PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId) const override;
		void PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId, uint8_t volume) const override;
		void PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId, uint8_t volume, float frequency) const override;
		uint8_t GetMasterVolume() const override;
		void SetMasterVolume(uint8_t newVolume) override;
		void IncrementMasterVolume() override;
		void DecrementMasterVolume() override;
		void ToggleMute() override;
		void PauseSound(SoundID id) const override;
		void PauseSound(SoundID id, const SoundInstanceID& instanceId) const override;
		void PauseAllSounds() const override;
		void ResumeSound(SoundID id) const override;
		void ResumeSound(SoundID id, const SoundInstanceID& instanceId) const override;
		void ResumeAllSounds() const override;
		void StopSound(SoundID id) const override;
		void StopSound(SoundID id, const SoundInstanceID& instanceId) const override;
		void StopAllSounds() const override;
	private:

		std::unique_ptr<AudioService> m_pRealService;
	};

	class XAudio final : public AudioService
    {
    public:

        XAudio();
        ~XAudio() override;

        XAudio(const XAudio&) = delete;
        XAudio(XAudio&&) noexcept = delete;
        XAudio& operator= (const XAudio&) = delete;
        XAudio& operator= (XAudio&&) noexcept = delete;

        void AddSound(const tstring& filename, SoundID id) override;
        void RemoveSound(SoundID id) override;
        void PlaySoundClip(SoundID id, bool repeat) const override;
        void PlaySoundClip(SoundID id, bool repeat, uint8_t volume) const override;
        void PlaySoundClip(SoundID id, bool repeat, uint8_t volume, float frequency) const override;
        void PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId) const override;
        void PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId, uint8_t volume) const override;
        void PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId, uint8_t volume, float frequency) const override;
        uint8_t GetMasterVolume() const override;
        void SetMasterVolume(uint8_t newVolume) override;
        void IncrementMasterVolume() override;
        void DecrementMasterVolume() override;
        void ToggleMute() override;
        void PauseSound(SoundID id) const override;
        void PauseSound(SoundID id, const SoundInstanceID& instanceId) const override;
        void PauseAllSounds() const override;
        void ResumeSound(SoundID id) const override;
        void ResumeSound(SoundID id, const SoundInstanceID& instanceId) const override;
        void ResumeAllSounds() const override;
        void StopSound(SoundID id) const override;
        void StopSound(SoundID id, const SoundInstanceID& instanceId) const override;
        void StopAllSounds() const override;

        static void SetNrOfChannelsPerFormat(uint16_t amount);

    private:
        class AudioImpl;
        AudioImpl* m_pImpl;

        inline static uint16_t m_NrOfChannelsPerFormat{ 64 };
        inline static bool m_ChannelPoolCreated{ false };

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
}

#endif // !AUDIOSERVICE_H
