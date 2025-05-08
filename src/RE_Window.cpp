#include "RE_Window.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Window* Window::pInstance = nullptr;

	Window::Window() : pcTitle("Untitled game window"), size(600, 400), bVisible(false), bMinimized(false), bCloseFlag(false), bValid(false) {
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

	void Window::update_window_size(uint32_t u16NewWidth, uint32_t u16NewHeight) {
		size[0] = u16NewWidth;
		size[1] = u16NewHeight;
		if (RenderSystem::pInstance)
			CATCH_SIGNAL(RenderSystem::pInstance->window_resize_event());
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

	Vector<uint32_t, 2> Window::get_size() const {
		return size;
	}

}
