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
		return nullptr;
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile) {
		Vector2i textureSize;
		return alloc_texture_loading_from_file(pacPathToTextureFile, textureSize);
	}

	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, Vector2i &rSize) {
		return alloc_texture_loading_from_file(pacPathToTextureFile, rSize[0], rSize[1]);
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, int32_t &ri32Width, int32_t &ri32Height) {
		int32_t i32Channels;
		return alloc_texture_loading_from_file(pacPathToTextureFile, ri32Width, ri32Height, i32Channels);
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, Vector2i &rSize, int32_t &ri32Channels) {
		return alloc_texture_loading_from_file(pacPathToTextureFile, rSize[0], rSize[1], ri32Channels);
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, int32_t &ri32Width, int32_t &ri32Height, int32_t &ri32Channels) {
		return nullptr;
	}

	void free_texture(const Texture hTexture) {
	}

	void free_texture_and_fix_dangling_pointers(const Texture hTexture) {
	}

}
