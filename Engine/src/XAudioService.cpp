#include "Audio.h"
#include "Engine.h"
#include "FileExceptions.h"
#include <xaudio2.h>
#include <ranges>
#include <filesystem>
#include <mutex>

// Shoutout: ChiliTomatoNoodle
// https://www.youtube.com/watch?v=T51Eqbbald4&t=2888s
// Github:
// https://github.com/planetchili/HUGS

namespace jela
{
    void XAudio::SetNrOfChannelsPerFormat(uint16_t amount)
    {
        if (m_ChannelPoolCreated)
        {
            OutputDebugString(std::format(_T("WARNING! When setting the amount of channels per format to {1}, a channel pool was already created with a amount of {0} channels. From this point, channel pools will create {1} channels.\n"),
                                          m_NrOfChannelsPerFormat, amount).c_str());
			OutputDebugString(_T("Tip: use SetNrOfChannelsPerFormat before you add any audio file if you want a consistent amount of channels for every WAVE format.\n\n"));
		}
		m_NrOfChannelsPerFormat = amount;
	}
	bool operator==(const WAVEFORMATEX& lhs, const WAVEFORMATEX& rhs)
	{
		return lhs.cbSize == rhs.cbSize &&
			lhs.wFormatTag == rhs.wFormatTag &&
			lhs.nChannels == rhs.nChannels &&
			lhs.nSamplesPerSec == rhs.nSamplesPerSec &&
			lhs.nAvgBytesPerSec == rhs.nAvgBytesPerSec &&
			lhs.nBlockAlign == rhs.nBlockAlign &&
			lhs.wBitsPerSample == rhs.wBitsPerSample;
	}
	//Implementation
	class XAudio::AudioImpl final
	{
	public:
        AudioImpl()
		{

#ifdef _DEBUG
			HRESULT hr = XAudio2Create(&m_pAudioEngine, XAUDIO2_DEBUG_ENGINE, XAUDIO2_DEFAULT_PROCESSOR);
#else
			HRESULT hr = XAudio2Create(&m_pAudioEngine, 0, XAUDIO2_DEFAULT_PROCESSOR);
#endif // _DEBUG
			if (FAILED(hr)) OutputDebugString(_T("ERROR! Unable to create the XAudio2 Engine!"));


			hr = m_pAudioEngine->CreateMasteringVoice(&m_pMasteringVoice);
			if (FAILED(hr)) OutputDebugString(_T("ERROR! Unable to create the XAudio2 Mastering Voice!"));
        }

        ~AudioImpl()
        {
            m_ChannelPools.clear();
            // First destroy the channels because, in their destructor, they still need access to the audio files.
			m_MapAudioFiles.clear();

			m_VecSupportedFormats.clear();

			SafeRelease(&m_pAudioEngine);
			m_pMasteringVoice = nullptr;
		}

		AudioImpl(const AudioImpl&) = delete;
		AudioImpl(AudioImpl&&) noexcept = delete;
		AudioImpl& operator= (const AudioImpl&) = delete;
		AudioImpl& operator= (AudioImpl&&) noexcept = delete;

		void AddSoundImpl(const tstring& filename, SoundID id)
        {
            if (m_MapAudioFiles.contains(id))
                OutputDebugString(std::format(_T("\nSoundID {} bound to file {} was already added.\n\n"), id, filename).c_str());
            else
            {
                try
                {
                    m_MapAudioFiles.try_emplace(id, filename, this);
                }
                catch (const FileException& e)
                {
                    ENGINE.NotifyException(e.what());
                    OutputDebugStringA(e.what());
                    m_MapAudioFiles.erase(id);
                }
                catch (const std::exception& e)
                {
                    OutputDebugStringA(e.what());
                    m_MapAudioFiles.erase(id);
                }
			}
		}
		void RemoveSoundImpl(SoundID id)
		{
			m_MapAudioFiles.erase(id);
        }

        void PlaySoundInstanceImpl(SoundID id, bool repeat, uint8_t volume, SoundInstanceID& instanceId, float frequency)
		{
			if (m_MapAudioFiles.contains(id))
			{
				const float fVolume = volume / 100.f;
				PlayAudioFile(m_MapAudioFiles.at(id), repeat, fVolume, frequency, instanceId);
            }
            else
                OutputDebugString(
                    std::format(_T("Sound file with id {} was not added before trying to play the sound."), id)
                    .c_str());
		}
		void PlaySoundClipImpl(SoundID id, bool repeat, uint8_t volume, float frequency)
		{
			if (m_MapAudioFiles.contains(id))
			{
				const float fVolume = volume / 100.f;
				SoundInstanceID instanceId{};
				PlayAudioFile(m_MapAudioFiles.at(id), repeat, fVolume, frequency, instanceId);
			}
			else OutputDebugString(std::format(_T("Sound file with id {} was not added before trying to play the sound."), id).c_str());
        }

        uint8_t GetMasterVolumeImpl() const
        {
            float fVolume{};
            m_pMasteringVoice->GetVolume(&fVolume);
            return static_cast<uint8_t>(std::round(fVolume * 100));
        }

        void SetMasterVolumeImpl(uint8_t newVolume)
        {
            m_IsMute = false;
            const float fVolume = newVolume / 100.f;
			m_pMasteringVoice->SetVolume(fVolume);
            m_LatestVolume = newVolume;
		}
		void IncrementMasterVolumeImpl()
		{
			if (const uint8_t vol = GetMasterVolumeImpl(); vol < UINT8_MAX)
				SetMasterVolumeImpl(vol + 1);
		}
		void DecrementMasterVolumeImpl()
		{
			if (const uint8_t vol = GetMasterVolumeImpl(); vol > 0)
				SetMasterVolumeImpl(vol - 1);
		}
		void ToggleMuteImpl()
		{
			m_IsMute = !m_IsMute;
			if (m_IsMute) m_pMasteringVoice->SetVolume(0);
			else m_pMasteringVoice->SetVolume(m_LatestVolume / 100.f);
		}
		void PauseSoundImpl(SoundID id)
		{
			if (m_MapAudioFiles.contains(id))
			{
				m_MapAudioFiles.at(id).PauseChannels();
			}
			else OutputDebugString(std::format(_T("Sound file with id {} was not added before trying to pause the sound."), id).c_str());
		}
		void PauseSoundImpl(SoundID id, const SoundInstanceID& instanceId)
		{
			if (m_MapAudioFiles.contains(id))
			{
				if (!instanceId.GetID().has_value()) return;
				m_MapAudioFiles.at(id).PauseChannel(instanceId);
			}
			else OutputDebugString(std::format(_T("Sound file with id {} was not added before trying to pause instance {}."), id, instanceId.GetID().value()).c_str());
		}
		void PauseAllSoundsImpl()
		{
			std::ranges::for_each(m_MapAudioFiles, [](auto& pair)
				{
					auto& [soundId, audioFile] = pair;
					audioFile.PauseChannels();
				});
		}

		void ResumeSoundImpl(SoundID id)
		{
			if (m_MapAudioFiles.contains(id))
			{
				m_MapAudioFiles.at(id).ResumeChannels();
			}
			else OutputDebugString(std::format(_T("Sound file with id {} was not added before trying to resume the sound."), id).c_str());
		}
		void ResumeSoundImpl(SoundID id, const SoundInstanceID& instanceId)
		{
			if (m_MapAudioFiles.contains(id))
			{
				if (!instanceId.GetID().has_value()) return;
				m_MapAudioFiles.at(id).ResumeChannel(instanceId);
			}
			else OutputDebugString(std::format(_T("Sound file with id {} was not added before trying to resume instance {}."), id, instanceId.GetID().value()).c_str());
		}
		void ResumeAllSoundsImpl()
		{
			std::ranges::for_each(m_MapAudioFiles, [](auto& pair)
				{
					auto& [soundId, audioFile] = pair;
					audioFile.ResumeChannels();
				});
		}

		void StopSoundImpl(SoundID id)
		{
			if (m_MapAudioFiles.contains(id))
			{
				m_MapAudioFiles.at(id).StopChannels();
			}
			else OutputDebugString(std::format(_T("Sound file with id {} was not added before trying to stop the sound."), id).c_str());
		}
		void StopSoundImpl(SoundID id, const SoundInstanceID& instanceId)
		{
			if (m_MapAudioFiles.contains(id))
			{
				if (!instanceId.GetID().has_value()) return;
				m_MapAudioFiles.at(id).StopChannel(instanceId);
			}
			else OutputDebugString(std::format(_T("Sound file with id {} was not added before trying to resume instance {}."), id, instanceId.GetID().value()).c_str());
		}
		void StopAllSoundsImpl()
		{
			std::ranges::for_each(m_MapAudioFiles, [](auto& pair)
            {
                auto& [soundId, audioFile] = pair;
                audioFile.StopChannels();
            });
        }

        class Channel;
        class AudioFile;

        //----------------------------------------------------------------------------------------------------------------------------
        // AudioFile class
        class AudioFile final
        {
            friend XAudio::AudioImpl::Channel;

        public:
            AudioFile(const tstring& filename, AudioImpl* const pAudioSystem) :
                m_FileName{ filename },
                m_pAudioSystem{ pAudioSystem }
            {
                try
                {
                    OpenFile(m_FileName);
                    m_ActiveChannelPtrs.assign(FormatChannelPool::amountOfChannels, nullptr);
                }
                catch (...)
                {
                    m_FileName.clear();
                    m_pData.clear();
                    m_ActiveChannelPtrs.clear();
                    m_pFormat = nullptr;
                    m_pOnChannelRelease.reset();
                    m_Exists = false;
			        throw;
			    }
			}

			~AudioFile()
			{
				StopChannels();
			}

			AudioFile(const AudioFile& other) = delete;
			AudioFile(AudioFile&& other) noexcept = delete;
			AudioFile& operator=(const AudioFile& other) = delete;
			AudioFile& operator=(AudioFile&& other) noexcept = delete;

			const WAVEFORMATEX* const GetFormatPtr() const { return m_pFormat; }
			const tstring& GetFileName() const { return m_FileName; }
			void StopChannel(const SoundInstanceID& id) { m_ActiveChannelPtrs.at(id.GetID().value())->Stop(); }
			void PauseChannel(const SoundInstanceID& id) { m_ActiveChannelPtrs.at(id.GetID().value())->Pause(); }
			void ResumeChannel(const SoundInstanceID& id) { m_ActiveChannelPtrs.at(id.GetID().value())->Resume(); }
			void StopChannels() { std::ranges::for_each(m_ActiveChannelPtrs, [](const auto& pChannel) { if (pChannel) pChannel->Stop(); }); }
            void PauseChannels() { std::ranges::for_each(m_ActiveChannelPtrs, [](const auto& pChannel) { if (pChannel) pChannel->Pause(); }); }
            void ResumeChannels() { std::ranges::for_each(m_ActiveChannelPtrs, [](const auto& pChannel) { if (pChannel) pChannel->Resume(); }); }

        private:
            enum class WaveCode
            {
#ifdef _XBOX //Big-Endian
                RIFF='RIFF',
                DATA='data',
                FMT='fmt ',
                WAVE='WAVE',
                XWMA = 'XWMA',
				DPDS = 'dpds'
#endif

#ifndef _XBOX //Little-Endian
                RIFF = 'FFIR',
                DATA = 'atad',
                FMT = ' tmf',
                WAVE = 'EVAW',
                XWMA = 'AMWX',
                DPDS = 'sdpd'
#endif
            };

            void OpenFile(const tstring& fileName)
            {
                constexpr static unsigned int nrOfFourccChars{4};
                WAVEFORMATEX extractedFormat{};

                std::streampos filePosition{0}; // Debug Purposes

                tstring pathString{ENGINE.ResourceMngr()->GetDataPath() + fileName};

                if (const std::filesystem::path filePath{pathString}; std::filesystem::exists(filePath))
                {
                    if (fileName.find(_T(".wav")) == std::string::npos)
                        throw FileTypeNotSupportedException{
                            std::format("File type of {} is not supported.", std::filesystem::path{fileName}.string()),
                            {".wav"}
                        };

                    if (std::ifstream file{filePath, std::ios_base::binary}; file.is_open())
                    {
                        int fourccResult{};

                        filePosition = file.read(reinterpret_cast<char*>(&fourccResult), nrOfFourccChars).tellg();
                        if (fourccResult != static_cast<int>(WaveCode::RIFF))
                            throw FileLoadException{ std::format("Expected {} (WAVE_CODE_RIFF) when reading Sound file. Got {} instead.\n",static_cast<int>(WaveCode::RIFF), fourccResult)};

                        unsigned int fileSize{0};
                        filePosition = file.read(reinterpret_cast<char*>(&fileSize), nrOfFourccChars).tellg();
                        fileSize += 8;
                        if (fileSize <= 44)
                            throw FileLoadException{
                                "Expected a filesize larger than 44 bytes when reading Sound file.\n"
                            };

                        filePosition = file.read(reinterpret_cast<char*>(&fourccResult), nrOfFourccChars).tellg();
                        if (fourccResult != static_cast<int>(WaveCode::WAVE))
                            throw FileLoadException{ std::format("Expected {} (WAVE_CODE_WAVE) when reading Sound file. Got {} instead.\n", static_cast<int>(WaveCode::WAVE), fourccResult) };

                        unsigned int chunkSize{};

                        auto findChunk = [&](WaveCode waveCode) -> bool
                        {
                            filePosition = file.seekg(nrOfFourccChars * 3, std::ifstream::beg).tellg();
                            bool bFilledData{ false };

                            while (!bFilledData && !file.eof())
                            {
                                filePosition = file.read(reinterpret_cast<char*>(&fourccResult), nrOfFourccChars).
                                        tellg();

                                if (fourccResult != static_cast<int>(waveCode)) // did not find chuck
                                {
                                    filePosition = file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize)).
                                            tellg();
                                    filePosition = file.seekg(chunkSize, std::ifstream::cur).tellg();
                                }
                                else // found chunk
                                {
                                    filePosition = file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize)).
                                            tellg();

                                    if (waveCode == WaveCode::FMT)
                                        filePosition = file.read(
                                            reinterpret_cast<char*>(&extractedFormat),
                                            sizeof(extractedFormat)).tellg();
                                    else if (waveCode == WaveCode::DATA)
                                    {
                                        m_pData.assign(chunkSize, 0);
                                        filePosition = file.read(reinterpret_cast<char*>(m_pData.data()),
                                                                 m_pData.size()).tellg();
                                    }

                                    bFilledData = true;
                                }
                            }

                            return bFilledData;
                        };

                        if (!findChunk(WaveCode::FMT))
                            throw FileLoadException{ std::format("Expected {} (WAVE_CODE_FMT) format not found when reading Sound file.\n", static_cast<int>(WaveCode::FMT)) };

                        if (!findChunk(WaveCode::DATA))
                            throw FileLoadException{ std::format("Expected {} (WAVE_CODE_DATA) data not found when reading Sound file.\n", static_cast<int>(WaveCode::DATA)) };
                    }
                    else throw FileLoadException{ std::format("File path {} was found but file could not be opened. Error occurred when trying to add a sound file.", filePath.string()) };
                }
                else throw FileNotFoundException{ std::format("File path {} could not be found. Error occurred when trying to add a sound file.", filePath.string()) };

                m_Exists = true;

                if (extractedFormat.wFormatTag == WAVE_FORMAT_PCM || extractedFormat.wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
                    extractedFormat.cbSize = 0;

                m_pFormat = m_pAudioSystem->AddFormat(extractedFormat);

                tstring formatStringSummary = std::format(
                    _T(
                        "wFormatTag: {}\nnChannels: {}\nnSamplesPerSec: {}\nnAvgBytesPerSec: {}\nnBlockAlign: {}\nwBitsPerSample: {}\ncbSize: {}\n"),
                    m_pFormat->wFormatTag, m_pFormat->nChannels, m_pFormat->nSamplesPerSec, m_pFormat->nAvgBytesPerSec, m_pFormat->nBlockAlign, m_pFormat->wBitsPerSample, m_pFormat->cbSize);

                OutputDebugString(std::format(_T("Audio File {} was successfully loaded! Format:\n{}\n"), fileName, formatStringSummary).c_str());
			}

			void RemoveChannel(const AudioImpl::Channel *pChannel)
		    {
				std::lock_guard<std::mutex> lock{ m_ChannelsMutex };
				for (std::size_t index = 0; index < m_ActiveChannelPtrs.size(); ++index)
				{
					auto& pCurrChannel = m_ActiveChannelPtrs.at(index);
					if (pCurrChannel == pChannel)
					{
						pCurrChannel = nullptr;
						std::vector<SoundInstanceID*> instanceIDs{};
						m_pOnChannelRelease->NotifyObservers(static_cast<uint8_t>(index), instanceIDs);
                        for (const auto& id : instanceIDs)
                            m_pOnChannelRelease->RemoveObserver(id);
                        return;
                    }
                }
            }

            void AddChannel(AudioImpl::Channel* pChannel, SoundInstanceID& instanceId)
            {
                // No lock_guard needed, because AddChannel will never be called when the audio voice already started.
				for (std::size_t index = 0; index < m_ActiveChannelPtrs.size(); ++index)
				{
					auto& pCurrChannel= m_ActiveChannelPtrs.at(index);
					if (!pCurrChannel)
					{
						if(!m_pOnChannelRelease->HasObserver(&instanceId))
                            m_pOnChannelRelease->AddObserver(&instanceId);

                        instanceId.SaveSubject(m_pOnChannelRelease.get());
						pCurrChannel = pChannel;
						instanceId.Init(static_cast<uint8_t>(index));
						return;
					}
				}
			}

			std::mutex m_ChannelsMutex{};
            tstring m_FileName{};
            std::vector<BYTE> m_pData{};
            std::vector<AudioImpl::Channel*> m_ActiveChannelPtrs{};
            const WAVEFORMATEX* m_pFormat{};
			AudioImpl* const m_pAudioSystem{};

			std::unique_ptr<Subject<uint8_t, std::vector<SoundInstanceID*>&>> m_pOnChannelRelease{ std::make_unique<Subject<uint8_t, std::vector<SoundInstanceID*>&>>() };
			bool m_Exists{ false };
        };

        //----------------------------------------------------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------------------------------------------------
        // Channel class
        class Channel final
        {
        public:
            Channel(AudioImpl* const pAudioSystem, const WAVEFORMATEX* const pFormat) :
                m_pFormat{ pFormat },
                m_pAudioSystem{ pAudioSystem }
            {
                ZeroMemory(&m_XAudioBuffer, sizeof(m_XAudioBuffer));
                m_XAudioBuffer.pContext = this;
                HRESULT hr = pAudioSystem->m_pAudioEngine->CreateSourceVoice(&m_pAudioVoice, pFormat, 0u, XAUDIO2_MAX_FREQ_RATIO, &m_Callback);
                if (FAILED(hr)) OutputDebugString(std::format(_T("Creating Source Voice failed. HRESULT {}"), hr).c_str());
			}
			Channel(const Channel&) = delete;

            Channel(Channel&&) noexcept = delete;

            Channel& operator=(const Channel&) = delete;

            Channel& operator=(Channel&&) noexcept = delete;

            ~Channel()
            {
                if (m_pAudioFile)
                {
                    m_pAudioFile->RemoveChannel(this);
                    m_pAudioFile = nullptr;
                }
                if (m_pAudioVoice)
                {
                    m_pAudioVoice->DestroyVoice();
                    m_pAudioVoice = nullptr;
                }
            }

            const WAVEFORMATEX* const GetFormatPtr() const { return m_pFormat; }

            void Play(AudioFile& s, bool repeat, float vol, float frequency, SoundInstanceID& instanceId)
			{
				assert(m_pAudioVoice && !m_pAudioFile);
				s.AddChannel(this, instanceId);
				// callback thread not running yet, so no sync necessary for m_pAudioFile
				m_pAudioFile = &s;
				m_XAudioBuffer.LoopCount = repeat ? XAUDIO2_LOOP_INFINITE : 0;
				m_XAudioBuffer.pAudioData = s.m_pData.data();
                m_XAudioBuffer.AudioBytes = static_cast<UINT32>(s.m_pData.size());
                m_pAudioVoice->SubmitSourceBuffer(&m_XAudioBuffer, nullptr);
                const float freq = std::max(XAUDIO2_MIN_FREQ_RATIO, std::min(XAUDIO2_MAX_FREQ_RATIO, frequency));
                m_pAudioVoice->SetFrequencyRatio(freq);
				m_pAudioVoice->SetVolume(vol);
				m_pAudioVoice->Start();
			}
			void Stop()
			{
				if(m_pAudioVoice && m_pAudioFile)
				{
					m_pAudioVoice->Stop();
					m_pAudioFile->RemoveChannel(this);
                    m_pAudioFile = nullptr;
                    m_pAudioSystem->DeactivateChannel(*this);
                    m_pAudioVoice->FlushSourceBuffers();
                    m_IsPaused = false;
                }
            }

            void Pause()
            {
                if (m_pAudioVoice && m_pAudioFile)
                {
                    m_pAudioVoice->Stop();
                    m_IsPaused = true;
				}
			}
			void Resume()
			{
				if (m_pAudioVoice && m_pAudioFile && m_IsPaused)
				{
					m_pAudioVoice->Start();
					m_IsPaused = false;
				}
			}
		private:
			class VoiceCallback final : public IXAudio2VoiceCallback
			{
			public:
				virtual ~VoiceCallback() = default;

				virtual void STDMETHODCALLTYPE OnStreamEnd() override {}
				virtual void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
				virtual void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 ) override {}
				virtual void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override
				{
					Channel& chan = *(static_cast<Channel *>(pBufferContext));
					chan.Stop();
				}
				virtual void STDMETHODCALLTYPE OnBufferStart(void* ) override {}
				virtual void STDMETHODCALLTYPE OnLoopEnd(void* ) override {}
				virtual void STDMETHODCALLTYPE OnVoiceError(void* , HRESULT ) override {}
			};

			XAUDIO2_BUFFER m_XAudioBuffer{};
			IXAudio2SourceVoice* m_pAudioVoice{ nullptr };
			// does this need to be synchronized?
			// (no--no overlap of callback thread and main thread here)
			AudioFile* m_pAudioFile{ nullptr };
			const WAVEFORMATEX* const m_pFormat { nullptr };
			AudioImpl* const m_pAudioSystem { nullptr };
			bool m_IsPaused{ false };

			inline static VoiceCallback m_Callback{};
		};
		//----------------------------------------------------------------------------------------------------------------------------



		WAVEFORMATEX* AddFormat(const WAVEFORMATEX& extractedFormat)
		{
			const auto itFormat = std::ranges::find_if(m_VecSupportedFormats, [&](const auto& internalFormat) { return extractedFormat == *internalFormat; });

			if (itFormat != m_VecSupportedFormats.cend())
				return itFormat->get();

			const auto pFormat = m_VecSupportedFormats.emplace_back(std::make_unique<WAVEFORMATEX>(extractedFormat)).get();

			std::lock_guard<std::mutex> lock{ m_ChannelMutex };
			m_ChannelPools.try_emplace(pFormat, FormatChannelPool{ this, pFormat });

			return pFormat;
		}
		void PlayAudioFile(class AudioFile& s, bool repeat, float vol, float frequency, SoundInstanceID& instanceId)
		{
			std::lock_guard<std::mutex> lock{ m_ChannelMutex };
			const WAVEFORMATEX* const pSoundFormat{ s.GetFormatPtr() };
			if (m_ChannelPools.contains(pSoundFormat))
			{
				auto& [idles, actives] = m_ChannelPools.at(pSoundFormat);
				if (idles.empty())
				{
					OutputDebugString(std::format(_T("WARNING! When trying to play {}, no channels were available.\n"), s.GetFileName()).c_str());
					return;
				}

				actives.emplace_back(std::move(idles.back()));
				idles.pop_back();
				actives.back()->Play(s, repeat, vol, frequency, instanceId);
			}
		}
		void DeactivateChannel(Channel& channel)
		{
			std::lock_guard<std::mutex> lock{ m_ChannelMutex };
			if (m_ChannelPools.contains(channel.GetFormatPtr()))
			{
				auto& [idles, actives] = m_ChannelPools.at(channel.GetFormatPtr());
				auto itChannel = std::ranges::find_if(actives, [&](const auto& channelPtr) { return channelPtr.get() == &channel; });
				if (itChannel != actives.cend())
				{
					idles.emplace_back(std::move(*itChannel));
					actives.erase(itChannel);
				}
			}
		}

	private:

		IXAudio2* m_pAudioEngine{ nullptr };
		IXAudio2MasteringVoice* m_pMasteringVoice{ nullptr };

		struct FormatChannelPool final
		{
			FormatChannelPool(AudioImpl* const pAudioSystem, const WAVEFORMATEX* const pFormat)
			{
				idleChannelPtrs.reserve(amountOfChannels);
				activeChannelPtrs.reserve(amountOfChannels);

				for (size_t i = 0; i < amountOfChannels; i++)
					idleChannelPtrs.emplace_back(std::make_unique<Channel>(pAudioSystem, pFormat));

				m_ChannelPoolCreated = true;
			}

			static constexpr size_t amountOfChannels{ 64 };

			// We store unique ptrs in order for the Channel objects to stay in the same place in memory
			// This is necessary for the async callback (in the Channel class) that points to a channel object somewhere in memory
			std::vector<std::unique_ptr<AudioImpl::Channel>> idleChannelPtrs{};
			std::vector<std::unique_ptr<AudioImpl::Channel>> activeChannelPtrs{};
		};

		std::map<SoundID, AudioFile> m_MapAudioFiles{};
		std::vector<std::unique_ptr<WAVEFORMATEX>> m_VecSupportedFormats{};
		std::unordered_map<const WAVEFORMATEX*, FormatChannelPool> m_ChannelPools{};
		std::mutex m_ChannelMutex{};

		bool m_IsMute{ false };
		uint8_t m_LatestVolume{ 100 };
	};



	//Audio
	XAudio::XAudio() :
		m_pImpl{ new AudioImpl{} }
	{
	}

	XAudio::~XAudio()
	{
		delete m_pImpl;
	}
	void XAudio::AddSound(const tstring& filename, SoundID id)
	{
		m_pImpl->AddSoundImpl(filename, id);
	}
	void XAudio::RemoveSound(SoundID id)
	{
		m_pImpl->RemoveSoundImpl(id);
	}
	void XAudio::PlaySoundClip(SoundID id, bool repeat, uint8_t volume, float frequency) const
	{
		m_pImpl->PlaySoundClipImpl(id, repeat, volume, frequency);
	}
	void XAudio::PlaySoundInstance(SoundID id, bool repeat, SoundInstanceID& instanceId, uint8_t volume, float frequency) const
	{
		m_pImpl->PlaySoundInstanceImpl(id, repeat, volume, instanceId, frequency);
	}
	uint8_t XAudio::GetMasterVolume() const
	{
		return m_pImpl->GetMasterVolumeImpl();
	}
	void XAudio::SetMasterVolume(uint8_t newVolume)
	{
		m_pImpl->SetMasterVolumeImpl(newVolume);
	}
	void XAudio::IncrementMasterVolume()
	{
		m_pImpl->IncrementMasterVolumeImpl();
	}
	void XAudio::DecrementMasterVolume()
	{
		m_pImpl->DecrementMasterVolumeImpl();
	}
	void XAudio::ToggleMute()
	{
		m_pImpl->ToggleMuteImpl();
	}
	void XAudio::PauseSound(SoundID id) const
	{
		m_pImpl->PauseSoundImpl(id);
	}
	void XAudio::PauseSound(SoundID id, const SoundInstanceID& instanceId) const
	{
		m_pImpl->PauseSoundImpl(id, instanceId);
	}
	void XAudio::PauseAllSounds() const
	{
		m_pImpl->PauseAllSoundsImpl();
	}
	void XAudio::ResumeSound(SoundID id) const
	{
		m_pImpl->ResumeSoundImpl(id);
	}
	void XAudio::ResumeSound(SoundID id, const SoundInstanceID& instanceId) const
	{
		m_pImpl->ResumeSoundImpl(id, instanceId);
	}
	void XAudio::ResumeAllSounds() const
	{
		m_pImpl->ResumeAllSoundsImpl();
	}
	void XAudio::StopSound(SoundID id) const
	{
		m_pImpl->StopSoundImpl(id);
	}
	void XAudio::StopSound(SoundID id, const SoundInstanceID& instanceId) const
	{
		m_pImpl->StopSoundImpl(id, instanceId);
	}
	void XAudio::StopAllSounds() const
	{
		m_pImpl->StopAllSoundsImpl();
	}
}