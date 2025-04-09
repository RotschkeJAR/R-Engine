#include "RE_Internal Header.hpp"
#include "RE_Window_Win64.hpp"
#include "RE_Window_X11.hpp"
#include "RE_Window_Wayland.hpp"
#include "RE_Renderer.hpp"
#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"
#include "RE_Manager.hpp"

namespace RE {

	bool bErrorOccured = false;
	bool bRunning = false;
	float fDeltaseconds = 0.0f, fMinDeltatime = -1.0f;
	WindowingSystem eUsingWindowingSystem = RE_WINDOWING_SYSTEM_MAX_ENUM;

	void execute() {
		DEFINE_SIGNAL_GUARD(sigGuardMainLoop);
		std::setlocale(LC_ALL, "");
		Window* pWindow;
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(pWindow = new Window_Win64());
		eUsingWindowingSystem = RE_WINDOWING_SYSTEM_WIN32;
#elif defined RE_OS_LINUX
		{
			const char *const pcSessionType = CATCH_SIGNAL_AND_RETURN(std::getenv("XDG_SESSION_TYPE"), const char*);
			if (std::strcmp(pcSessionType, "wayland") == 0) {
				CATCH_SIGNAL(pWindow = new Window_Wayland());
				if (!pWindow->is_valid() || bErrorOccured) {
					DELETE_SAFELY(pWindow);
					if (std::strlen(std::getenv("DISPLAY")) > 0) {
						RE_NOTE("Failed to create Wayland window. Attempting to create in X11");
						CATCH_SIGNAL(pWindow = new Window_X11());
						if (!pWindow->is_valid() || bErrorOccured) {
							RE_FATAL_ERROR("Failed to create window in X11 either");
							DELETE_SAFELY(pWindow);
							return;
						}
						eUsingWindowingSystem = RE_WINDOWING_SYSTEM_X11;
					} else {
						RE_FATAL_ERROR("Failed to create Wayland window. No other alternative known window managers exist");
						return;
					}
				} else
					eUsingWindowingSystem = RE_WINDOWING_SYSTEM_WAYLAND;
			} else if (std::strcmp(pcSessionType, "x11") == 0) {
				CATCH_SIGNAL(pWindow = new Window_X11());
				if (!pWindow->is_valid() || bErrorOccured) {
					DELETE_SAFELY(pWindow);
					if (std::strlen(std::getenv("WAYLAND_DISPLAY")) > 0) {
						RE_NOTE("Failed to create X11 window. Attempting to create in Wayland");
						CATCH_SIGNAL(pWindow = new Window_Wayland());
						if (!pWindow->is_valid() || bErrorOccured) {
							RE_FATAL_ERROR("Failed to create window in Wayland either");
							DELETE_SAFELY(pWindow);
							return;
						}
						eUsingWindowingSystem = RE_WINDOWING_SYSTEM_WAYLAND;
					} else {
						RE_FATAL_ERROR("Failed to create X11 window. No other alternative known window managers exist");
						return;
					}
				} else
					eUsingWindowingSystem = RE_WINDOWING_SYSTEM_X11;
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
			std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now(), lastFrameTime;
			bRunning = true;
			while (bRunning) {
				CATCH_SIGNAL(pWindow->window_proc());
				CATCH_SIGNAL(gameMgr.game_logic_update());
				CATCH_SIGNAL(renderSystem.draw_frame());
				lastFrameTime = currentFrameTime;
				currentFrameTime = std::chrono::high_resolution_clock::now();
				fDeltaseconds = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastFrameTime).count();
				CATCH_SIGNAL(pWindow->show_window(true));
				bRunning = !pWindow->should_close() && gameMgr.is_game_valid() && !bErrorOccured;
			}
			pWindow->show_window(false);
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
