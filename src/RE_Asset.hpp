#ifndef __RE_ASSET_H__
#define __RE_ASSET_H__

#include "RE_Internal Header.hpp"

namespace RE {

	struct AssetFile final {
	#ifdef RE_OS_WINDOWS
		HANDLE win_hFile;
	#elif defined RE_OS_LINUX
		int iFileDesc;
	#endif
		void *pBuffer;
		size_t sSize;
	};

	bool asset_open(const char *pacFilename, AssetFile &rFile);
	void asset_close(AssetFile &rFile);
	void* asset_get_content(AssetFile &rFile);
	size_t asset_get_size(AssetFile &rFile);

	struct AssetImage final {
		AssetFile file;
		void *pBuffer;
		int iWidth,
			iHeight,
			iChannels;
	};

	bool asset_image_load(AssetImage &rImage, int iDesiredChannels = 0);
	void asset_image_free(AssetImage &rImage);
	void* asset_image_get(AssetImage &rImage);
	int asset_image_get_width(AssetImage &rImage);
	int asset_image_get_height(AssetImage &rImage);
	int asset_image_get_channels(AssetImage &rImage);

}

#endif /* __RE_ASSET_H__ */
