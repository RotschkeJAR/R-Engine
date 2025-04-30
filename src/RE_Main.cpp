#include "RE_Internal Header.hpp"
#include "RE_Window.hpp"
#include "RE_Renderer.hpp"
#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"
#include "RE_Manager.hpp"

namespace RE {

	bool bErrorOccured = false;
	bool bRunning = false;
	float fDeltaseconds = 0.0f, fMinDeltatime = -1.0f;

	void execute() {
		DEFINE_SIGNAL_GUARD(sigGuardMainLoop);
		std::setlocale(LC_ALL, "");

		// Create window
		Window* pWindow = nullptr;
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(pWindow = new Window_Win64());
#elif defined RE_OS_LINUX
		{
			const bool bX11Exists = std::strlen(std::getenv("DISPLAY")) > 0, bWaylandExists = std::strlen(std::getenv("WAYLAND_DISPLAY")) > 0;
			if (bX11Exists) {
				CATCH_SIGNAL(pWindow = new Window_X11());
				if (!pWindow->is_valid()) {
					if (bWaylandExists) {
						CATCH_SIGNAL(delete pWindow);
						RE_NOTE("Failed creating X11 window. Using Wayland instead");
						CATCH_SIGNAL(pWindow = new Window_Wayland());
					} else {
						RE_FATAL_ERROR("Failed creating X11 window. No other alternative window compositor exists");
						return;
					}
				}
			} else if (bWaylandExists) {
				CATCH_SIGNAL(pWindow = new Window_Wayland());
				if (!pWindow->is_valid()) {
					if (bX11Exists) {
						CATCH_SIGNAL(delete pWindow);
						RE_NOTE("Failed creating Wayland window. Using X11 instead");
						CATCH_SIGNAL(pWindow = new Window_X11());
					} else {
						RE_FATAL_ERROR("Failed creating Wayland window. No other alternative window compositor exists");
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
		if (!pWindow->is_valid() || bErrorOccured) {
			CATCH_SIGNAL(delete pWindow);
			return;
		}

		{
			Manager gameMgr;
			Vulkan vulkan;
			if (!vulkan.is_valid() || bErrorOccured)
				return;
			RenderSystem renderSystem;
			if (!renderSystem.is_valid() || bErrorOccured)
				return;
			Renderer renderer;
			if (!renderer.is_valid() || bErrorOccured)
				return;
			std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now(), lastFrameTime;
			bRunning = true;

			// Game loop
			while (bRunning) {
				CATCH_SIGNAL(pWindow->window_proc());
				CATCH_SIGNAL(gameMgr.game_logic_update());
				CATCH_SIGNAL(renderSystem.refresh());
				CATCH_SIGNAL(renderer.render());
				lastFrameTime = currentFrameTime;
				currentFrameTime = std::chrono::high_resolution_clock::now();
				fDeltaseconds = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastFrameTime).count();
				CATCH_SIGNAL(pWindow->show_window(true));
				bRunning = !pWindow->should_close() && gameMgr.is_game_valid() && !bErrorOccured;
			}

			// Termination
			CATCH_SIGNAL(pWindow->show_window(false));
			CATCH_SIGNAL(gameMgr.last_game_logic_update());
			WAIT_FOR_IDLE_VULKAN_DEVICE();
			fDeltaseconds = 0.0f;
		}
		CATCH_SIGNAL(delete pWindow);
	}

	float get_deltaseconds() {
		return fDeltaseconds;
	}

	float get_fps_rate() {
		DEFINE_SIGNAL_GUARD(sigGuardGetFpsRate);
		if (fDeltaseconds > 0.0f)
			return 1.0f / fDeltaseconds;
		RE_ERROR("FPS rate couldn't be calculated, because the deltatime is still in its default value and would crash the game");
		return 0.0f;
	}

}
