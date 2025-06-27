#include "RE_Internal Header.hpp"
#include "RE_Window.hpp"
#include "RE_Renderer.hpp"
#include "RE_Render System.hpp"
#include "RE_Manager.hpp"

namespace RE {

	bool bErrorOccured = false;
	bool bRunning = false;
	float fDeltaseconds = 0.0f, fMinDeltatime = 1.0f / 60.0f;

	void execute() {
		std::setlocale(LC_ALL, "");

		// Create window
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(Window::pInstance = new Window_Win64());
#elif defined RE_OS_LINUX
		CATCH_SIGNAL(Window::pInstance = new Window_X11());
#else
# warning The targeted OS is unknown, so the engine will terminate immediatly upon execution
		RE_FATAL_ERROR("The OS is unknown. The engine can't initialize");
		return;
#endif
		if (CATCH_SIGNAL_AND_RETURN(Window::pInstance && Window::pInstance->is_valid() && init_vulkan_instance(), bool)) {
			if (CATCH_SIGNAL_AND_RETURN(init_render_system(), bool)) {
				if (CATCH_SIGNAL_AND_RETURN(init_renderer(), bool)) {
					std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now(), lastFrameTime;

					// Game loop
					do {
						CATCH_SIGNAL(Window::pInstance->window_proc());
						CATCH_SIGNAL(game_logic_update());
						if (Window::pInstance->should_render()) {
							CATCH_SIGNAL(refresh_swapchain());
							CATCH_SIGNAL(render());
						} else {
							// branch avoiding excessive CPU usage, when rendering is ambigious
							float fTimeToWait = fMinDeltatime - fDeltaseconds;
							if (fTimeToWait > 0.0f)
								std::this_thread::sleep_for(std::chrono::duration<float>(fTimeToWait));
						}
						lastFrameTime = currentFrameTime;
						currentFrameTime = std::chrono::high_resolution_clock::now();
						fDeltaseconds = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastFrameTime).count();
						CATCH_SIGNAL(Window::pInstance->show_window(true));
						bRunning = !Window::pInstance->should_close() && is_game_valid() && !bErrorOccured;
					} while (bRunning);

					// Termination
					CATCH_SIGNAL(Window::pInstance->show_window(false));
					CATCH_SIGNAL(last_game_logic_update());
					WAIT_FOR_IDLE_VULKAN_DEVICE();
					CATCH_SIGNAL(destroy_renderer());
				}
				CATCH_SIGNAL(destroy_render_system());
			}
			CATCH_SIGNAL(destroy_vulkan_instance());
		}
		DELETE_SAFELY(Window::pInstance);
		fDeltaseconds = 0.0f;
	}

	float get_deltaseconds() {
		return fDeltaseconds;
	}

	float get_fps_rate() {
		return fDeltaseconds > 0.0f ? (1.0f / fDeltaseconds) : -1.0f;
	}

}
