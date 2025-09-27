#include "RE_Internal Header.hpp"
#include "RE_Vulkan_Device.hpp"
#include "RE_GPU.hpp"

namespace RE {
	
	[[nodiscard]]
	SpriteLayout create_sprite_layout() {
		const SpriteLayoutSettings initialSpriteLayoutSettings;
		return create_sprite_layout(initialSpriteLayoutSettings);
	}

	[[nodiscard]]
	SpriteLayout create_sprite_layout(const SpriteLayoutSettings &rSettings) {
		return nullptr;
	}

	void change_sprite_layout_settings(SpriteLayout &rSpriteLayout, const SpriteLayoutSettings &rNewSettings) {
		if (vk_hDevice == VK_NULL_HANDLE) {
			RE_ERROR("Sprite layouts aren't valid anymore, when the engine doesn't run, so their settings cannot be changed either");
			return;
		}
		destroy_sprite_layout(rSpriteLayout);
		rSpriteLayout = create_sprite_layout(rNewSettings);
	}

	void destroy_sprite_layout(const SpriteLayout spriteLayout) {
		if (!spriteLayout)
			return;
		else if (vk_hDevice == VK_NULL_HANDLE) {
			RE_ERROR("Sprite layouts aren't valid anymore, when the engine doesn't run, so they cannot be destroyed either");
			return;
		}
	}

}
