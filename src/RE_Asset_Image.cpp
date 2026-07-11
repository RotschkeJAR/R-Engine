#include "RE_Asset.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ASSERT(x)
#include "stb_image.h"

namespace RE {
	
	bool asset_image_load(AssetImage &rImage, int iDesiredChannels) {
		PRINT_DEBUG("Loading asset image");
		rImage.pBuffer = reinterpret_cast<void*>(stbi_load_from_memory(
				reinterpret_cast<stbi_uc*>(rImage.file.pBuffer),
				static_cast<int>(rImage.file.sSize),
				&rImage.iWidth,
				&rImage.iHeight,
				&rImage.iChannels,
				iDesiredChannels));
		if (rImage.pBuffer) {
			return true;
		} else
			RE_ERROR("Failed to load image");
		return false;
	}

	void asset_image_free(AssetImage &rImage) {
		PRINT_DEBUG("Freeing asset image");
		stbi_image_free(rImage.pBuffer);
	}

	void* asset_image_get(AssetImage &rImage) {
		return rImage.pBuffer;
	}

	int asset_image_get_width(AssetImage &rImage) {
		return rImage.iWidth;
	}

	int asset_image_get_height(AssetImage &rImage) {
		return rImage.iHeight;
	}

	int asset_image_get_channels(AssetImage &rImage) {
		return rImage.iChannels;
	}

}
