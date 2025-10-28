#include "RE_Texture.hpp"
#include "RE_Vulkan_Device.hpp"
#include "RE_GPU.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_List_GameObject.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#define STBI_ASSERT(x)
#include "stb_image.h"

namespace RE {

	Texture alloc_texture_from_binary_data(const uint8_t *const pau8ImageBinaryData, const Vector2u imageSize, const uint32_t u32Channels) {
		return alloc_texture_from_binary_data(pau8ImageBinaryData, imageSize[0], imageSize[1], u32Channels);
	}

	Texture alloc_texture_from_binary_data(const uint8_t *const pau8ImageBinaryData, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Channels) {
		PRINT_DEBUG("Allocating texture from binary data ", pau8ImageBinaryData, ", sized ", u32Width, "x", u32Height, " containing ", u32Channels, " channels");
		return nullptr;
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile) {
		Vector2u textureSize;
		return alloc_texture_loading_from_file(pacPathToTextureFile, textureSize);
	}

	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, Vector2u &rSize) {
		return alloc_texture_loading_from_file(pacPathToTextureFile, rSize[0], rSize[1]);
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, uint32_t &ru32Width, uint32_t &ru32Height) {
		uint32_t u32Channels;
		return alloc_texture_loading_from_file(pacPathToTextureFile, ru32Width, ru32Height, u32Channels);
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, Vector2i &rSize, uint32_t &ru32Channels) {
		return alloc_texture_loading_from_file(pacPathToTextureFile, rSize[0], rSize[1], ru32Channels);
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, uint32_t &ru32Width, uint32_t &ru32Height, uint32_t &ru32Channels) {
		PRINT_DEBUG("Loading image from \"", pacPathToTextureFile, "\" and allocating texture from it");
		return nullptr;
	}

	void free_texture(const Texture hTexture) {
		PRINT_DEBUG("Freeing texture ", hTexture);
	}

	void free_texture_and_fix_dangling_pointers(const Texture hTexture) {
		PRINT_DEBUG("Freeing texture ", hTexture, " and removing dangling pointers pointing to it");
	}

}
