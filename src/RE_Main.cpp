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
		DEFINE_SIGNAL_GUARD(sigGuardMainLoop);
		std::setlocale(LC_ALL, "");

		// Create window
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(Window::pInstance = new Window_Win64());
#elif defined RE_OS_LINUX
		{
			const bool bX11Exists = !is_string_empty(std::getenv("DISPLAY")), bWaylandExists = !is_string_empty(std::getenv("WAYLAND_DISPLAY"));
			if (bX11Exists) {
				CATCH_SIGNAL(Window::pInstance = new Window_X11());
				if (!Window::pInstance->is_valid()) {
					CATCH_SIGNAL(delete Window::pInstance);
					if (bWaylandExists) {
						RE_NOTE("Failed creating X11 window. Using Wayland instead");
						CATCH_SIGNAL(Window::pInstance = new Window_Wayland());
					} else {
						RE_FATAL_ERROR("Failed creating X11 window. No other known window compositor exists");
						return;
					}
				}
			} else if (bWaylandExists) {
				CATCH_SIGNAL(Window::pInstance = new Window_Wayland());
				if (!Window::pInstance->is_valid()) {
					CATCH_SIGNAL(delete Window::pInstance);
					if (bX11Exists) {
						RE_NOTE("Failed creating Wayland window. Using X11 instead");
						CATCH_SIGNAL(Window::pInstance = new Window_X11());
					} else {
						RE_FATAL_ERROR("Failed creating Wayland window. No other known window compositor exists");
						return;
					}
				}
			} else {
				RE_FATAL_ERROR("There aren't any window compositors installed, that is supported by the engine");
				return;
			}
		}
#else
# warning The targeted OS is unknown, so the engine will terminate immediatly upon execution
		RE_ERROR("The OS is unknown. The engine can't initialize");
		return;
#endif
		if (CATCH_SIGNAL_AND_RETURN(Window::pInstance->is_valid() && init_vulkan_instance(), bool)) {
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
