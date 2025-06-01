#include "RE_Window.hpp"

namespace RE {
	
	Vector2u windowSize(600, 400);
	Window* Window::pInstance = nullptr;

	Window::Window() : pcTitle("Untitled game window"), bVisible(false), bMinimized(false), bMaximized(false), bCloseFlag(false), bValid(false) {
		if (pInstance) {
			RE_FATAL_ERROR("A window already exists. New window has been discarded");
			return;
		}
		pInstance = this;
	}

	Window::~Window() {
		if (pInstance == this)
			pInstance = nullptr;
	}

	void Window::update_window_size(uint32_t u32NewWidth, uint32_t u32NewHeight) {
		windowSize[0] = u32NewWidth;
		windowSize[1] = u32NewHeight;
		CATCH_SIGNAL(mark_swapchain_dirty());
	}

	void Window::show_window(bool bShowWindow) {
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

	bool Window::should_close() const {
		return bCloseFlag;
	}

	bool Window::should_render() const {
		return !bMinimized && bVisible;
	}

	bool Window::is_valid() const {
		return bValid;
	}

}
