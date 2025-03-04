#include "RE_Window.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Window* Window::pInstance = nullptr;

	Window::Window() : pcTitle("Untitled game window"), size(600, 400), bCloseFlag(false), bValid(false) {
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

	void Window::updateWindowSize(REushort u16NewWidth, REushort u16NewHeight) {
		size[0] = u16NewWidth;
		size[1] = u16NewHeight;
		CATCH_SIGNAL(RenderSystem::pInstance->windowResize(size));
	}

	void Window::show(bool bShowWindow) {
		if (bWindowVisible == bShowWindow || !bValid)
			return;
		bWindowVisible = bShowWindow;
		showInternal();
	}

	void Window::setTitle(const char* pNewTitle) {
		if (std::strcmp(pcTitle, pNewTitle) == 0 || !bValid)
			return;
		pcTitle = pNewTitle;
		updateTitleInternal();
	}

	void Window::update() {
		CATCH_SIGNAL(inputMgr.preInputEvent());
		CATCH_SIGNAL(processLoop());
	}

	bool Window::shouldClose() {
		return bCloseFlag;
	}

	bool Window::isValid() {
		return bValid;
	}

	Vector<REushort, 2> Window::getSize() {
		Vector<REushort, 2> copySize;
		copySize = size;
		return copySize;
	}

}
