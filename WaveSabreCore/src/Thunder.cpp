// vim: set noet:

#include <WaveSabreCore/Thunder.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
	Thunder::Thunder()
		: SynthDevice(0)
	{
		for (int i = 0; i < maxVoices; i++) voices[i] = new ThunderVoice(this);

		this->decoder = ISampleDecoder::NewPlatformSampleDecoder();
	}

	Thunder::~Thunder()
	{
		delete this->decoder;
	}

	typedef struct
	{
		int CompressedSize;
		int UncompressedSize;
	} ChunkHeader;

	void Thunder::SetChunk(void *data, int size)
	{
		if (!size) return;
		auto h = (ChunkHeader *)data;
		auto waveFormat = (WAVEFORMATEX *)((char *)data + sizeof(ChunkHeader));
		auto compressedData = (char *)waveFormat + sizeof(WAVEFORMATEX) + waveFormat->cbSize;
		LoadSample(compressedData, h->CompressedSize, h->UncompressedSize, waveFormat);
	}

	int Thunder::GetChunk(void **data)
	{
		if (!compressedData) return 0;
		ChunkHeader h;
		h.CompressedSize = compressedSize;
		h.UncompressedSize = uncompressedSize;
		if (chunkData) delete [] chunkData;
		int chunkSize = sizeof(ChunkHeader) + sizeof(WAVEFORMATEX) + ((WAVEFORMATEX *)waveFormatData)->cbSize + compressedSize + sizeof(int);
		chunkData = new char[chunkSize];
		memcpy(chunkData, &h, sizeof(ChunkHeader));
		memcpy(chunkData + sizeof(ChunkHeader), waveFormatData, sizeof(WAVEFORMATEX) + ((WAVEFORMATEX *)waveFormatData)->cbSize);
		memcpy(chunkData + sizeof(ChunkHeader) + sizeof(WAVEFORMATEX) + ((WAVEFORMATEX *)waveFormatData)->cbSize, compressedData, compressedSize);
		*(int *)(chunkData + chunkSize - sizeof(int)) = chunkSize;
		*data = chunkData;
		return chunkSize;
	}

	void Thunder::LoadSample(void* data, int compressedSize,
			int uncompressedSize, AudioFormat* waveFormat)
	{
		decoder->LoadSample(data, compressedSize, uncompressedSize,
				waveFormat);
	}

	Thunder::ThunderVoice::ThunderVoice(Thunder *thunder)
	{
		this->thunder = thunder;
	}

	SynthDevice *Thunder::ThunderVoice::SynthDevice() const
	{
		return thunder;
	}

	void Thunder::ThunderVoice::Run(double songPosition, float **outputs, int numSamples)
	{
		for (int i = 0; i < numSamples; i++)
		{
			if (samplePos >= thunder->sampleLength)
			{
				IsOn = false;
				break;
			}
			float sample = thunder->sampleData[samplePos];
			outputs[0][i] += sample;
			outputs[1][i] += sample;
			samplePos++;
		}
	}

	void Thunder::ThunderVoice::NoteOn(int note, int velocity, float detune, float pan)
	{
		Voice::NoteOn(note, velocity, detune, pan);
		samplePos = 0;
	}
}
