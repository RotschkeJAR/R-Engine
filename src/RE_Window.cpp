#include "RE_Window.hpp"
#include "RE_GPU.hpp"
#include "RE_Window_Win64.hpp"
#include "RE_Window_X11.hpp"
#include "RE_Window_Wayland.hpp"

namespace RE {

#ifdef RE_OS_LINUX
	LinuxWindowType eLinuxWindowType = RE_LINUX_WINDOW_TYPE_X11;
#endif
	
	Vector2u windowSize(600, 400), largestMonitorSize(1920, 1080);
	const char* pacWindowTitle = "Untitled game window";
	uint8_t u8WindowFlagBits = 1 << WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT;

	bool create_window() {
		PRINT_DEBUG("Creating window");
		set_bits<uint8_t>(u8WindowFlagBits, false, WINDOW_CREATED_BIT, WINDOW_VISIBLE_BIT, WINDOW_MINIMIZED_BIT, WINDOW_MAXIMIZED_BIT, WINDOW_CLOSE_FLAG_BIT);
		bool bSuccess;
#ifdef RE_OS_WINDOWS
		bSuccess = win64_create_window();
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case RE_LINUX_WINDOW_TYPE_X11:
				bSuccess = x11_create_window();
				break;
			case RE_LINUX_WINDOW_TYPE_WAYLAND:
				set_bits<uint8_t>(u8WindowFlagBits, false, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT);
				bSuccess = wayland_create_window();
				break;
			default:
				ABORT("Window compositor is unknown");
		}
#endif
		set_bits<uint8_t>(u8WindowFlagBits, bSuccess, WINDOW_CREATED_BIT);
		return bSuccess;
	}

	void destroy_window() {
		PRINT_DEBUG("Destroying window");
#ifdef RE_OS_WINDOWS
		win64_destroy_window();
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case RE_LINUX_WINDOW_TYPE_X11:
				x11_destroy_window();
				break;
			case RE_LINUX_WINDOW_TYPE_WAYLAND:
				wayland_destroy_window();
				break;
			default:
				ABORT("Window compositor is unknown");
		}
#endif
		set_bits<uint8_t>(u8WindowFlagBits, false, WINDOW_CREATED_BIT, WINDOW_VISIBLE_BIT, WINDOW_MINIMIZED_BIT, WINDOW_MAXIMIZED_BIT, WINDOW_CLOSE_FLAG_BIT);
	}

	void window_resize_event(const uint32_t u32NewWidth, const uint32_t u32NewHeight) {
		PRINT_DEBUG("Updating window dimensions after resize");
		windowSize[0] = u32NewWidth;
		windowSize[1] = u32NewHeight;
		mark_swapchain_dirty();
	}

	void show_window(const bool bShowWindow) {
		PRINT_DEBUG("Showing/Hiding window");
		set_bits<uint8_t>(u8WindowFlagBits, bShowWindow, WINDOW_VISIBLE_BIT);
#ifdef RE_OS_WINDOWS
		win64_show_window();
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case RE_LINUX_WINDOW_TYPE_X11:
				x11_show_window();
				break;
			case RE_LINUX_WINDOW_TYPE_WAYLAND:
				wayland_show_window();
				break;
			default:
				ABORT("Window compositor is unknown");
		}
#endif
	}

	void set_fullscreen(const bool bNewFullscreen) {
		if (is_fullscreen() == bNewFullscreen)
			return;
		PRINT_DEBUG("Setting fullscreen to ", bNewFullscreen);
		set_bits<uint8_t>(u8WindowFlagBits, bNewFullscreen, WINDOW_FULLSCREEN_BIT);
		if (!are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_CREATED_BIT))
			return;
#ifdef RE_OS_WINDOWS
		win64_update_fullscreen();
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case RE_LINUX_WINDOW_TYPE_X11:
				x11_update_fullscreen();
				break;
			case RE_LINUX_WINDOW_TYPE_WAYLAND:
				wayland_update_fullscreen();
				break;
			default:
				ABORT("Window compositor is unknown");
		}
#endif
	}

	bool is_fullscreen() {
		return are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_FULLSCREEN_BIT);
	}

	void window_proc() {
		PRINT_DEBUG("General window procedure called");
		update_input_buffers();
#ifdef RE_OS_WINDOWS
		win64_window_proc();
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case RE_LINUX_WINDOW_TYPE_X11:
				x11_window_proc();
				break;
			case RE_LINUX_WINDOW_TYPE_WAYLAND:
				wayland_window_proc();
				break;
			default:
				ABORT("Window compositor is unknown");
		}
#endif
	}

	void post_rendering_window_proc() {
		PRINT_DEBUG("General window procedure after rendering called");
#ifdef RE_OS_LINUX
		if (eLinuxWindowType == RE_LINUX_WINDOW_TYPE_WAYLAND)
			wayland_post_rendering_window_proc();
#endif
	}

	[[nodiscard]]
	bool should_window_close() {
		return are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_CLOSE_FLAG_BIT);
	}

	[[nodiscard]]
	bool should_render() {
		return !are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_MINIMIZED_BIT) &&
				are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_VISIBLE_BIT, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT);
	}

	bool create_vulkan_surface(VkSurfaceKHR &vk_rhSurface) {
#ifdef RE_OS_WINDOWS
		PRINT_DEBUG("Creating Vulkan surface linked to Windows");
		const VkWin32SurfaceCreateInfoKHR vk_win32SurfaceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.hinstance = win_hInstance,
			.hwnd = win_hWindow
		};
		return vkCreateWin32SurfaceKHR(vk_hInstance, &vk_win32SurfaceCreateInfo, nullptr, &vk_rhSurface) == VK_SUCCESS;
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case RE_LINUX_WINDOW_TYPE_X11:
				{
					PRINT_DEBUG("Creating Vulkan surface linked to RE_LINUX_WINDOW_TYPE_X11");
					const VkXlibSurfaceCreateInfoKHR vk_x11SurfaceCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
						.dpy = x11_pDisplay,
						.window = x11_hWindow
					};
					return vkCreateXlibSurfaceKHR(vk_hInstance, &vk_x11SurfaceCreateInfo, nullptr, &vk_rhSurface) == VK_SUCCESS;
				}
			case RE_LINUX_WINDOW_TYPE_WAYLAND:
				{
					PRINT_DEBUG("Creating Vulkan surface linked to Wayland");
					const VkWaylandSurfaceCreateInfoKHR vk_waylandSurfaceCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
						.display = wl_pDisplay,
						.surface = wl_pSurface
					};
					return vkCreateWaylandSurfaceKHR(vk_hInstance, &vk_waylandSurfaceCreateInfo, nullptr, &vk_rhSurface) == VK_SUCCESS;
				}
			default:
				ABORT("Window compositor is unknown");
				return false;
		}
#endif
	}

	[[nodiscard]]
	const char* get_vulkan_required_surface_extension_name() {
#ifdef RE_OS_WINDOWS
		return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case RE_LINUX_WINDOW_TYPE_X11:
				return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
			case RE_LINUX_WINDOW_TYPE_WAYLAND:
				return VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
			default:
				ABORT("Window compositor is unknown");
				return nullptr;
		}
#endif
	}

	void set_window_title(const char *const pacNewTitle) {
		if (are_string_contents_equal(pacWindowTitle, pacNewTitle))
			return;
		PRINT_DEBUG("Updating window title");
		pacWindowTitle = pacNewTitle;
		if (!are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_CREATED_BIT))
			return;
#ifdef RE_OS_WINDOWS
		win64_update_window_title();
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case RE_LINUX_WINDOW_TYPE_X11:
				x11_update_window_title();
				break;
			case RE_LINUX_WINDOW_TYPE_WAYLAND:
				wayland_update_window_title();
				break;
			default:
				ABORT("Window compositor is unknown");
		}
#endif
	}

}
