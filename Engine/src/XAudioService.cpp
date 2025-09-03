#include "Audio.h"
#include "Engine.h"
#include <xaudio2.h>
#include <filesystem>

namespace jela
{
	//Implementation
	class XAudioService::AudioImpl final
	{
	public:
		AudioImpl() 
		{ 
			m_Format.wFormatTag = WAVE_FORMAT_PCM;
			m_Format.nChannels = 2; // 1 == mono, 2 == stereo
			m_Format.wBitsPerSample = 16;
			m_Format.nSamplesPerSec = 44100; // 44.1 kHz
			m_Format.nBlockAlign = m_Format.nChannels * m_Format.wBitsPerSample / 8; // Divided by 8 bits
			m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * m_Format.nBlockAlign;
			m_Format.cbSize = 0;
			

			HRESULT hr = XAudio2Create(&m_pAudioEngine, 0, XAUDIO2_DEFAULT_PROCESSOR); 
			if (FAILED(hr)) OutputDebugString(_T("ERROR! Unable to create the XAudio2 Engine!"));

			hr = m_pAudioEngine->CreateMasteringVoice(&m_pMasteringVoice);
			if (FAILED(hr)) OutputDebugString(_T("ERROR! Unable to create the XAudio2 Mastering Voice!"));
		}
		~AudioImpl() 
		{
			SafeRelease(&m_pAudioEngine);
			m_pMasteringVoice = nullptr;
		}

		AudioImpl(const AudioImpl&) = delete;
		AudioImpl(AudioImpl&&) noexcept = delete;
		AudioImpl& operator= (const AudioImpl&) = delete;
		AudioImpl& operator= (AudioImpl&&) noexcept = delete;

		void AddSoundImpl(const tstring& filename, SoundID)
		{
			audioFile = std::make_unique<AudioFile>(filename);
		}
		void RemoveSoundImpl(SoundID )
		{
		}
		void PlaySoundClipImpl(SoundID , bool )
		{
		}
		uint8_t GetMasterVolumeImpl() const
		{
			return 0;
		}
		void SetMasterVolumeImpl(uint8_t )
		{
		}
		void IncrementMasterVolumeImpl()
		{
		}
		void DecrementMasterVolumeImpl()
		{
		}
		void ToggleMuteImpl()
		{
		}
		void PauseSoundImpl(SoundID )
		{
		}
		void PauseAllSoundsImpl()
		{
		}
		void ResumeSoundImpl(SoundID )
		{
		}
		void ResumeAllSoundsImpl()
		{
		}
		void StopSoundImpl(SoundID )
		{
		}
		void StopAllSoundsImpl()
		{
		}
		static WAVEFORMATEX GetWaveFormat() { return m_Format; }

	private:

		//// PRIVATE DATA
		//enum class Event
		//{
		//	Add,
		//	Remove,
		//	Play,
		//	Pause,
		//	Resume,
		//	Stop
		//};

		//struct AudioInfo
		//{
		//	std::unique_ptr<AudioFile> pAudioFile;
		//	bool repeat{};
		//};

		//struct QueueInfo
		//{
		//	bool repeat{ false };
		//	bool allSounds{ false };
		//	SoundID id{};
		//	Event playBackEvent{};
		//	tstring filename{};
		//};

		IXAudio2* m_pAudioEngine{ nullptr };
		IXAudio2MasteringVoice* m_pMasteringVoice{ nullptr };
		inline static WAVEFORMATEX m_Format{};

		std::unique_ptr<AudioFile> audioFile{};
	};

	class XAudioService::AudioFile final
	{
	public:

		AudioFile(const tstring& filename) :
			m_FileName{ filename }
		{
			OutputDebugString(OpenFile(m_FileName).c_str());
		}

		~AudioFile() = default;

		AudioFile(const AudioFile& other) = delete;
		AudioFile(AudioFile&& other) noexcept = delete;
		AudioFile& operator=(const AudioFile& other) = delete;
		AudioFile& operator=(AudioFile&& other) noexcept = delete;

	private:

		enum class WaveCode
		{

#ifdef _XBOX //Big-Endian
			RIFF = 'RIFF',
			DATA = 'data',
			FMT = 'fmt ',
			WAVE = 'WAVE',
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

		tstring OpenFile(const tstring& fileName)
		{
			constexpr static unsigned int nrOfFourccChars{ 4 };
			std::streampos filePosition{ 0 }; // Debug Purposes

			tstring pathString{ ENGINE.ResourceMngr()->GetDataPath() + fileName };

			if (std::filesystem::path filePath{ pathString }; std::filesystem::exists(filePath))
			{
				if (std::ifstream file{ filePath, std::ios_base::binary }; file.is_open())
				{
					int fourccResult{};

					filePosition = file.read(reinterpret_cast<char*>(&fourccResult), nrOfFourccChars).tellg();
					if (fourccResult != static_cast<int>(WaveCode::RIFF))
						return std::format(_T("ERROR! Expected {} (WAVE_CODE_RIFF) when reading Sound file. Got {} instead.\n"), static_cast<int>(WaveCode::RIFF), fourccResult);

					unsigned int fileSize{ 0 };
					filePosition = file.read(reinterpret_cast<char*>(&fileSize), nrOfFourccChars).tellg();
					if (fileSize <= 16)
						return _T("ERROR! Expected a filesize larger than 16 bytes when reading Sound file.\n");

					filePosition = file.read(reinterpret_cast<char*>(&fourccResult), nrOfFourccChars).tellg();
					if (fourccResult != static_cast<int>(WaveCode::WAVE))
						return std::format(_T("ERROR! Expected {} (WAVE_CODE_WAVE) when reading Sound file. Got {} instead.\n"), static_cast<int>(WaveCode::WAVE), fourccResult);

					unsigned int chunkSize{};

					auto findChunk = [&](WaveCode waveCode) -> bool
					{
						filePosition = file.seekg(nrOfFourccChars * 3, file.beg).tellg();
						bool bFilledData{ false };

						while (!bFilledData && !file.eof())
						{
							filePosition = file.read(reinterpret_cast<char*>(&fourccResult), nrOfFourccChars).tellg();

							if (fourccResult != static_cast<int>(waveCode)) // did not find chuck
							{
								filePosition = file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize)).tellg();
								filePosition = file.seekg(chunkSize, file.cur).tellg();
							}
							else // found chunk
							{
								filePosition = file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize)).tellg();

								if (waveCode == WaveCode::FMT) filePosition = file.read(reinterpret_cast<char*>(&m_Format), sizeof(m_Format)).tellg();
								else if(waveCode == WaveCode::DATA)
								{
									m_pData.assign(chunkSize, 0);
									filePosition = file.read(reinterpret_cast<char*>(m_pData.data()), m_pData.size()).tellg();
								}

								bFilledData = true;
							}
						}

						return bFilledData;
					};

					if (!findChunk(WaveCode::FMT))
						return std::format(_T("ERROR! Expected {} (WAVE_CODE_FMT) format not found when reading Sound file.\n"), static_cast<int>(WaveCode::FMT));

					if (!findChunk(WaveCode::DATA))
						return std::format(_T("ERROR! Expected {} (WAVE_CODE_DATA) data not found when reading Sound file.\n"), static_cast<int>(WaveCode::DATA));;
				}
				else return std::format(_T("ERROR! File path ({}) was found but file ({}) could not be opened."), pathString, fileName);
			}
			else return std::format(_T("ERROR! File path ({}) could not be found."), pathString);

			m_Exists = true;
			return _T("Audio File was succesfully loaded!\n");
		}

		std::vector<BYTE> m_pData{};
		WAVEFORMATEX m_Format;
		tstring m_FileName;
		bool m_Exists{ false };

	};


	//Audio
	XAudioService::XAudioService() :
		m_pImpl{ new AudioImpl{} }
	{
	}

	XAudioService::~XAudioService()
	{
		delete m_pImpl;
	}
	void XAudioService::AddSound(const tstring& filename, SoundID id)
	{
		m_pImpl->AddSoundImpl(filename, id);
	}
	void XAudioService::RemoveSound(SoundID id)
	{
		m_pImpl->RemoveSoundImpl(id);
	}
	void XAudioService::PlaySoundClip(SoundID id, bool repeat) const
	{
		m_pImpl->PlaySoundClipImpl(id, repeat);
	}
	uint8_t XAudioService::GetMasterVolume() const
	{
		return m_pImpl->GetMasterVolumeImpl();
	}
	void XAudioService::SetMasterVolume(uint8_t newVolume)
	{
		m_pImpl->SetMasterVolumeImpl(newVolume);
	}
	void XAudioService::IncrementMasterVolume()
	{
		m_pImpl->IncrementMasterVolumeImpl();
	}
	void XAudioService::DecrementMasterVolume()
	{
		m_pImpl->DecrementMasterVolumeImpl();
	}
	void XAudioService::ToggleMute()
	{
		m_pImpl->ToggleMuteImpl();
	}
	void XAudioService::PauseSound(SoundID id) const
	{
		m_pImpl->PauseSoundImpl(id);
	}
	void XAudioService::PauseAllSounds() const
	{
		m_pImpl->PauseAllSoundsImpl();
	}
	void XAudioService::ResumeSound(SoundID id) const
	{
		m_pImpl->ResumeSoundImpl(id);
	}
	void XAudioService::ResumeAllSounds() const
	{
		m_pImpl->ResumeAllSoundsImpl();
	}
	void XAudioService::StopSound(SoundID id) const
	{
		m_pImpl->StopSoundImpl(id);
	}
	void XAudioService::StopAllSounds() const
	{
		m_pImpl->StopAllSoundsImpl();
	}
}