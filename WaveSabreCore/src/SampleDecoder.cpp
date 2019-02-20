// vim: set noet:

#include <WaveSabreCore/SampleDecoder.h>
#include <WaveSabreCore/Helpers.h>

#include <math.h>

namespace WaveSabreCore
{
#ifdef _MSC_VER
	ISampleDecoder* ISampleDecoder::NewPlatformSampleDecoder()
	{
		return new AcmSampleDecoder();
	}

	HACMDRIVERID AcmSampleDecoder::driverId = NULL;

	AcmSampleDecoder::AcmSampleDecoder()
		: ISampleDecoder()
	{
		chunkData = nullptr;

		waveFormatData = nullptr;
		compressedSize = uncompressedSize = 0;
		compressedData = nullptr;
		sampleData     = nullptr;

		sampleLength = 0;
	}

	AcmSampleDecoder::~AcmSampleDecoder()
	{
		if (chunkData     ) delete [] chunkData;
		if (waveFormatData) delete [] waveFormatData;
		if (compressedData) delete [] compressedData;
		if (sampleData    ) delete [] sampleData;
	}

	void AcmSampleDecoder::LoadSample(char* data, int compressedSize,
			int uncompressedSize, AudioFormat* audioFormat)
	{
		WAVEFORMATEX* waveFormat = audioFormat->data;

		this->compressedSize = compressedSize;
		this->uncompressedSize = uncompressedSize;

		if (waveFormatData) delete [] waveFormatData;
		waveFormatData = new char[sizeof(WAVEFORMATEX) + waveFormat->cbSize];
		memcpy(waveFormatData, waveFormat, sizeof(WAVEFORMATEX) + waveFormat->cbSize);
		if (compressedData) delete [] compressedData;
		compressedData = new char[compressedSize];
		memcpy(compressedData, data, compressedSize);

		acmDriverEnum(driverEnumCallback, NULL, NULL);
		HACMDRIVER driver = NULL;
		acmDriverOpen(&driver, driverId, 0);

		WAVEFORMATEX dstWaveFormat =
		{
			WAVE_FORMAT_PCM,
			1,
			waveFormat->nSamplesPerSec,
			waveFormat->nSamplesPerSec * 2,
			sizeof(short),
			sizeof(short) * 8,
			0
		};

		HACMSTREAM stream = NULL;
		acmStreamOpen(&stream, driver, waveFormat, &dstWaveFormat, NULL, NULL, NULL, ACM_STREAMOPENF_NONREALTIME);

		ACMSTREAMHEADER streamHeader;
		memset(&streamHeader, 0, sizeof(ACMSTREAMHEADER));
		streamHeader.cbStruct = sizeof(ACMSTREAMHEADER);
		streamHeader.pbSrc = (LPBYTE)compressedData;
		streamHeader.cbSrcLength = compressedSize;
		auto uncompressedData = new short[uncompressedSize * 2];
		streamHeader.pbDst = (LPBYTE)uncompressedData;
		streamHeader.cbDstLength = uncompressedSize * 2;
		acmStreamPrepareHeader(stream, &streamHeader, 0);

		acmStreamConvert(stream, &streamHeader, 0);

		acmStreamClose(stream, 0);
		acmDriverClose(driver, 0);

		sampleLength = streamHeader.cbDstLengthUsed / sizeof(short);
		if (sampleData) delete [] sampleData;
		sampleData = new float[sampleLength];
		for (int i = 0; i < sampleLength; i++) sampleData[i] = (float)((double)uncompressedData[i] / 32768.0);

		delete [] uncompressedData;
	}

	BOOL __stdcall AcmSampleDecoder::driverEnumCallback(HACMDRIVERID driverId,
			DWORD dwInstance, DWORD fdwSupport)
	{
		if (AcmSampleDecoder::driverId) return 1;

		HACMDRIVER driver = NULL;
		acmDriverOpen(&driver, driverId, 0);

		int waveFormatSize = 0;
		acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &waveFormatSize);
		auto waveFormat = (WAVEFORMATEX *)(new char[waveFormatSize]);
		memset(waveFormat, 0, waveFormatSize);
		ACMFORMATDETAILS formatDetails;
		memset(&formatDetails, 0, sizeof(formatDetails));
		formatDetails.cbStruct = sizeof(formatDetails);
		formatDetails.pwfx = waveFormat;
		formatDetails.cbwfx = waveFormatSize;
		formatDetails.dwFormatTag = WAVE_FORMAT_UNKNOWN;
		acmFormatEnum(driver, &formatDetails, formatEnumCallback, NULL, NULL);

		delete [] (char *)waveFormat;

		acmDriverClose(driver, 0);

		return 1;
	}

	BOOL __stdcall AcmSampleDecoder::formatEnumCallback(HACMDRIVERID driverId,
			LPACMFORMATDETAILS formatDetails, DWORD dwInstance,
			DWORD fdwSupport)
	{
		if (formatDetails->pwfx->wFormatTag == WAVE_FORMAT_GSM610 &&
			formatDetails->pwfx->nChannels == 1 &&
			formatDetails->pwfx->nSamplesPerSec == SampleRate)
		{
			AcmSampleDecoder::driverId = driverId;
		}
		return 1;
	}
#else
	ISampleDecoder* ISampleDecoder::NewPlatformSampleDecoder()
	{
		return new OpusSampleDecoder();
	}

	OpusSampleDecoder::OpusSampleDecoder()
	{
		chunkData = nullptr;

		waveFormatData = nullptr;
		compressedSize = uncompressedSize = 0;
		compressedData = nullptr;
		sampleData     = nullptr;

		sampleLength = 0;
	}

	OpusSampleDecoder::~OpusSampleDecoder()
	{
		if (chunkData     ) delete [] chunkData;
		if (waveFormatData) delete [] waveFormatData;
		if (compressedData) delete [] compressedData;
		if (sampleData    ) delete [] sampleData;
	}

	void OpusSampleDecoder::LoadSample(char* data, int compressedSize,
			int uncompressedSize, AudioFormat* audioFormat)
	{
		// TODO
	}
#endif
}

