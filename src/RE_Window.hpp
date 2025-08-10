#ifndef __RE_WINDOW_H__
#define __RE_WINDOW_H__

#include "RE_Internal Header.hpp"
#include "RE_Input.hpp"
#include "RE_Vulkan_Device.hpp"

namespace RE {

#define MIN_WINDOW_WIDTH 100
#define MIN_WINDOW_HEIGHT 100
#define MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR -100
#define MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR -100

#define WINDOW_CREATED_BIT 0
#define WINDOW_VISIBLE_BIT 1
#define WINDOW_MINIMIZED_BIT 2
#define WINDOW_MAXIMIZED_BIT 3
#define WINDOW_CLOSE_FLAG_BIT 4
#define WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT 5

	extern Vector2u windowSize;
	extern const char* pacWindowTitle;
	extern uint8_t u8WindowFlagBits;

	bool create_window();
	void destroy_window();
	void window_resize_event(const uint32_t u32NewWidth, const uint32_t u32NewHeight);
	void show_window(const bool bShowWindow);
	void window_proc();
	void post_rendering_window_proc();
	[[nodiscard]]
	bool should_window_close();
	[[nodiscard]]
	bool should_render();
	bool create_vulkan_surface(VkSurfaceKHR &vk_rhSurface);
	[[nodiscard]]
	const char* get_vulkan_required_surface_extension_name();

}

#endif /* __RE_WINDOW_H__ */
