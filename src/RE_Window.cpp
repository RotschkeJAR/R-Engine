#include "RE_Window.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vector2u windowSize(600, 400);
	Window* Window::pInstance = nullptr;

	Window::Window() : pcTitle("Untitled game window"), bVisible(false), bMinimized(false), bMaximized(false), bCloseFlag(false), bValid(false) {}

	Window::~Window() {}

	void Window::update_window_size(const uint32_t u32NewWidth, const uint32_t u32NewHeight) {
		windowSize[0] = u32NewWidth;
		windowSize[1] = u32NewHeight;
		mark_swapchain_dirty();
	}

	void Window::show_window(const bool bShowWindow) {
		if (bVisible == bShowWindow || !bValid)
			return;
		bVisible = bShowWindow;
		CATCH_SIGNAL(internal_show_window());
	}

	void Window::set_window_title(const char* pNewTitle) {
		if (std::strcmp(pcTitle, pNewTitle) == 0 || !bValid)
			return;
		pcTitle = pNewTitle;
		CATCH_SIGNAL(internal_update_title());
	}

	void Window::window_proc() {
		CATCH_SIGNAL(inputMgr.update_input_buffers());
		CATCH_SIGNAL(internal_window_proc());
	}

	[[nodiscard]]
	bool Window::should_close() const {
		return bCloseFlag;
	}

	[[nodiscard]]
	bool Window::should_render() {
		return !bMinimized && bVisible;
	}

	[[nodiscard]]
	bool Window::is_valid() const {
		return bValid;
	}

}
