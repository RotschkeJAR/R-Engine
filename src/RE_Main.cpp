#include "RE_Ext Header.hpp"
#include "RE_Window_Win64.hpp"

#include <chrono>
#include <thread>

namespace RE {

	bool errorOccured = false;
	bool running = false;
	Window* window = nullptr;

	void renderThread() {
		while (running) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	
	void execute() {
#ifdef RE_OS_WINDOWS
		window = new Window_Win64();
#elif defined RE_OS_LINUX

#else
		RE_ERROR("Window couldn't be created, because the OS is unknown");
		return;
#endif
		if (!window->isValid()) {
			delete window;
			return;
		}
		running = true;
		std::thread renderLogic(renderThread);
		while (running) {
			window->processLoop();
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			window->show(true);
			running = !window->shouldClose() && !errorOccured;
		}
		window->show(false);
		delete window;
		renderLogic.join();
	}

}
