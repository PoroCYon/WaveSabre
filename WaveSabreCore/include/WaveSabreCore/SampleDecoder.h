// vim: set noet:

#ifndef __WAVESABRECORE_SAMPLEDECODER_H__
#define __WAVESABRECORE_SAMPLEDECODER_H__

#ifdef _MSC_VER
#include <Windows.h>
#include <mmreg.h>
#include <MSAcm.h>
#else
// ...
#endif

namespace WaveSabreCore
{
	struct AudioFormat;

	class ISampleDecoder
	{
	public:
		virtual ~ISampleDecoder();

		virtual void LoadSample(char *data, int compressedSize,
				int uncompressedSize, AudioFormat* waveFormat) = 0;

		char *chunkData;

		char *waveFormatData;
		int compressedSize, uncompressedSize;

		char *compressedData;
		float *sampleData;

		int sampleLength;

		static ISampleDecoder* NewPlatformSampleDecoder();
	};

#ifdef _MSC_VER
	struct AudioFormat {
		WAVEFORMATEX* data;
	};

	class AcmSampleDecoder : public ISampleDecoder {
	private:
		static BOOL __stdcall driverEnumCallback(HACMDRIVERID driverId, DWORD dwInstance, DWORD fdwSupport);
		static BOOL __stdcall formatEnumCallback(HACMDRIVERID driverId, LPACMFORMATDETAILS formatDetails, DWORD dwInstance, DWORD fdwSupport);

		static HACMDRIVERID driverId;
	};
#else
	struct AudioFormat {
		// TODO
	};

	class OpusSampleDecoder : public ISampleDecoder {
		// TODO
	};
#endif
}

#endif

