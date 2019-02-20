// vim: set noet:

#include <WaveSabreCore/GmDls.h>

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

static const char *gmDlsPaths[2] =
{
	"drivers/gm.dls",
	"drivers/etc/gm.dls"
};

namespace WaveSabreCore
{
	unsigned char *GmDls::Load()
	{
		unsigned char* gmDls;

#ifdef _MSC_VER
		HANDLE gmDlsFile = INVALID_HANDLE_VALUE;
		for (int i = 0; gmDlsFile == INVALID_HANDLE_VALUE; i++)
		{
			OFSTRUCT reOpenBuff;
			gmDlsFile = (HANDLE)OpenFile(gmDlsPaths[i], &reOpenBuff, OF_READ);
		}

		auto gmDlsFileSize = GetFileSize(gmDlsFile, NULL);
		gmDls = new unsigned char[gmDlsFileSize];
		unsigned int bytesRead;
		ReadFile(gmDlsFile, gmDls, gmDlsFileSize, (LPDWORD)&bytesRead, NULL);
		CloseHandle(gmDlsFile);
#else
		int fd = -1;
		for (int i = 0; fd < 0 && i < sizeof(gmDlsPaths)/sizeof(*gmDlsPaths); ++i) {
			fd = open(gmDlsPaths[i], O_RDONLY);
		}
		if (fd < 0) return NULL;

		struct stat st;
		fstat(fd, &st);

		gmDls = new unsigned char[static_cast<size_t>(st.st_size)];
		read(fd, gmDls, sizeof(gmDls));
		close(fd);
#endif

		return gmDls;
	}
}
