#include "RE_Ext Header.hpp"
#include "RE_Window_Win64.hpp"
#include "RE_Window_X11.hpp"
#include "RE_Renderer.hpp"

#include <chrono>
#include <thread>

namespace RE {

	bool errorOccured = false;
	bool running = false;
	Window* window = nullptr;

	void waitForTrue(bool& value) {
		REubyte distraction;
		while (!value)
			distraction ++;
	}

	void gameThread() {
		while (running) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	
	void execute() {
		std::setlocale(LC_ALL, "");
#ifdef RE_OS_WINDOWS
		window = new Window_Win64();
#elif defined RE_OS_LINUX
		window = new Window_X11();
#else
# warning The OS is unknown, so the engine will terminate immediatly upon execution
		RE_ERROR("Window couldn't be created, because the OS is unknown");
		return;
#endif
		if (!window->isValid()) {
			delete window;
			return;
		}
		Renderer renderer;
		running = true;
		std::thread gameLogicThread(gameThread);
		while (running) {
			window->update();
			renderer.render();
			window->show(true);
			running = !window->shouldClose() && !errorOccured;
		}
		window->show(false);
		gameLogicThread.join();
		delete window;
	}

}
