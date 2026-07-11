#include "RE_Asset.hpp"

#ifdef RE_OS_WINDOWS
#	include <fileapi.h>
#	include <handleapi.h>
#elif defined RE_OS_LINUX
#	include <fcntl.h>
#	include <sys/mman.h>
#	include <unistd.h>
#endif

namespace RE {
	
	bool asset_open(const char *pacFilename, AssetFile &rFile) {
	#ifdef RE_OS_WINDOWS
		PRINT_DEBUG("Converting file path \"", pacFilename, "\" to wide chars");
		const std::wstring std_wtFilename = convert_chars_to_wide(pacFilename);
		PRINT_DEBUG("Opening asset file \"", pacFilename, "\" with read-only access");
		/* CreateFile has no path length limit unlike OpenFile */
		rFile.win_hFile = CreateFileW(
				std_wtFilename.c_str(),
				GENERIC_READ,
				FILE_SHARE_READ,
				nullptr,
				OPEN_EXISTING,
				0,
				nullptr);
		if (rFile.win_hFile) {
			PRINT_DEBUG("Fetching size of opened asset file");
			LARGE_INTEGER win_fileSize;
			if (GetFileSizeEx(rFile.win_hFile, &win_fileSize) != 0) {
				PRINT_DEBUG("Reading content of asset file");
				rFile.sSize = static_cast<size_t>(win_fileSize.QuadPart);
				rFile.pBuffer = safe_malloc(rFile.sSize);
				if (ReadFile(rFile.win_hFile,
						rFile.pBuffer,
						rFile.sSize,
						nullptr,
						nullptr)) {
					return true;
				} else
					RE_ERROR("Failed to read content of open file ", rFile.win_hFile);
				PRINT_DEBUG("Freeing memory of asset file due to failure reading it");
				std::free(rFile.pBuffer);
			} else
				RE_ERROR("Failed to get size of open file ", rFile.win_hFile);
			PRINT_DEBUG("Closing asset file due to fetching details and its content");
			if (CloseHandle(rFile.win_hFile) == 0)
				RE_ERROR("Failed to close file handle ", rFile.win_hFile);
		} else
			RE_ERROR("Failed to open read-only file \"", pacFilename, "\"");
	#elif defined RE_OS_LINUX
		PRINT_DEBUG("Opening asset file \"", pacFilename, "\" with read-only access");
		rFile.iFileDesc = open(pacFilename, O_RDONLY);
		if (rFile.iFileDesc != -1) {
			PRINT_DEBUG("Setting pointer to end of asset file");
			const off_t lx_size = lseek(rFile.iFileDesc, 0, SEEK_END);
			if (lx_size != static_cast<off_t>(-1)) {
				rFile.sSize = static_cast<size_t>(lx_size);
				PRINT_DEBUG("Setting pointer to beginning of asset file");
				if (lseek(rFile.iFileDesc, 0, SEEK_SET) != static_cast<off_t>(-1)) {
					PRINT_DEBUG("Mapping content of asset file to memory");
					rFile.pBuffer = mmap(nullptr, rFile.sSize, PROT_READ, MAP_PRIVATE, rFile.iFileDesc, 0);
					if (rFile.pBuffer != MAP_FAILED) {
						return true;
					} else
						RE_ERROR("Failed to map content of file descriptor ", rFile.iFileDesc, " to memory");
				} else
					RE_ERROR("Failed to set pointer to content of file descriptor ", rFile.iFileDesc, " to zero");
			} else
				RE_ERROR("Failed to set pointer to content of file descriptor ", rFile.iFileDesc, " to the end");
			PRINT_DEBUG("Closing asset file due to failure fetching its details and mapping its content to memory");
			if (close(rFile.iFileDesc) == -1)
				RE_ERROR("Failed to close file descriptor ", rFile.iFileDesc);
		} else
			RE_ERROR("Failed to open read-only file \"", pacFilename, "\"");
	#else
		RE_ERROR("No file can be loaded due to the platform being unknown");
	#endif
		return false;
	}

	void asset_close(AssetFile &rFile) {
	#ifdef RE_OS_WINDOWS
		PRINT_DEBUG("Freeing buffer of asset file content");
		std::free(rFile.pBuffer);
		PRINT_DEBUG("Closing asset file");
		if (CloseHandle(rFile.win_hFile) == 0)
			RE_ERROR("Failed to close file handle ", rFile.win_hFile);
	#elif defined RE_OS_LINUX
		PRINT_DEBUG("Unmapping asset file content");
		if (munmap(rFile.pBuffer, rFile.sSize) == -1)
			RE_ERROR("Failed to unmap pages of file descriptor ", rFile.iFileDesc);
		PRINT_DEBUG("Closing asset file");
		if (close(rFile.iFileDesc) == -1)
			RE_ERROR("Failed to close file descriptor ", rFile.iFileDesc);
	#else
		RE_ERROR("No file to close due to the platform being unknown");
	#endif
	}

	void* asset_get_content(AssetFile &rFile) {
		return rFile.pBuffer;
	}

	size_t asset_get_size(AssetFile &rFile) {
		return rFile.sSize;
	}

}
