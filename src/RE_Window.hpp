#ifndef __RE_WINDOW_H__
#define __RE_WINDOW_H__

#include "RE_Internal Header.hpp"
#include "RE_Input.hpp"
#include "RE_Vulkan_Device.hpp"
#ifdef RE_OS_LINUX
# include <xdg-shell-client-protocol.h>
#endif

namespace RE {

#define MIN_WINDOW_WIDTH 100
#define MIN_WINDOW_HEIGHT 100
#define MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR -100
#define MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR -100

	enum WindowType {
		Windows,
		X11,
		Wayland
	};

	extern Vector2u windowSize;
	
	class Window {

		protected:
			InputMgr inputMgr;
			const char* pcTitle;
			bool bVisible, bMinimized, bMaximized;
			bool bCloseFlag;
			bool bValid;

			virtual void internal_window_proc() = 0;
			virtual void internal_show_window() = 0;
			virtual void internal_update_title() = 0;
			void update_window_size(const uint32_t usNewWidth, const uint32_t usNewHeight);

		public:
			static Window* pInstance;

			Window();
			virtual ~Window();
			void show_window(const bool bShowWindow);
			void set_window_title(const char* pNewTitle);
			virtual bool create_vulkan_surface(VkSurfaceKHR &vk_rhSurface) const = 0;
			virtual const char* get_vulkan_required_surface_extension_name() const = 0;
			void window_proc();
			virtual void post_rendering_window_proc() = 0;
			bool should_close() const;
			virtual bool should_render();
			bool is_valid() const;
			virtual WindowType get_window_type() const = 0;

		friend class InputMgr;
	};

#ifdef RE_OS_WINDOWS
	class Window_Win64 final : public Window {
		private:
			HWND win_hWindow;
			HCURSOR win_hCursor;

		protected:
			void internal_window_proc();
			void internal_show_window();
			void internal_update_title();

		public:
			const HKL win_keyboardLayout;
			static HINSTANCE win_hInstance;
			
			Window_Win64();
			~Window_Win64();
			bool create_vulkan_surface(VkSurfaceKHR &vk_rhSurface) const;
			const char* get_vulkan_required_surface_extension_name() const;
			void post_rendering_window_proc();
			HWND get_hwindow() const;
			WindowType get_window_type() const;

		friend LRESULT CALLBACK windows_window_proc(HWND win_hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

	void set_hinstance(HINSTANCE win_hInstance);
#elif (defined RE_OS_LINUX)
	class Window_X11 final : public Window {
		private:
			XColormap x11_colormap;
			XWindow x11_hWindow;
			XAtom x11_hClose;
			XAtom x11_hUTF8, x11_hWindowName;
			XInputMethod x11_hInputMethod;
			XInputContext x11_hInputContext;
			XSizeHints* x11_pSizes;

		protected:
			void internal_window_proc();
			void internal_show_window();
			void internal_update_title();

		public:
			XDisplay* const x11_pDisplay;
			
			Window_X11();
			~Window_X11();
			bool create_vulkan_surface(VkSurfaceKHR &vk_rhSurface) const;
			const char* get_vulkan_required_surface_extension_name() const;
			void post_rendering_window_proc();
			WindowType get_window_type() const;
	};

	class Window_Wayland final : public Window {
		private:
			wl_registry *wl_pRegistry;
			wl_compositor *wl_pCompositor;
			wl_surface *wl_pSurface;
			xdg_wm_base *xdg_pWindowBase;
			xdg_surface *xdg_pSurface;
			xdg_toplevel *xdg_pToplevel;
			bool bShouldRenderFrame;
			
			void register_next_ready_frame_callback();
			void commit_surface();
			
		protected:
			void internal_window_proc();
			void internal_show_window();
			void internal_update_title();

		public:
			wl_display *const wl_pDisplay;

			Window_Wayland();
			~Window_Wayland();
			bool create_vulkan_surface(VkSurfaceKHR &vk_rhSurface) const;
			const char* get_vulkan_required_surface_extension_name() const;
			void post_rendering_window_proc();
			bool should_render() override;
			WindowType get_window_type() const;

		friend void registry_handle_global(void *pData, wl_registry *wl_pRegistry, uint32_t u32Name, const char *pcInterface, uint32_t u32Version);
		friend void wayland_frame_ready(void *pData, wl_callback *wl_pCallback, uint32_t u32Serial);
	};
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

}

#endif /* __RE_WINDOW_H__ */
