#include "RE_Main.hpp"
#include "RE_Window.hpp"
#include "RE_Renderer.hpp"
#include "RE_GPU.hpp"
#include "RE_Manager.hpp"
#include "RE_Settings.hpp"

namespace RE {

	float fDeltaseconds = 0.0f;
	bool bRunning = false,
		bErrorOccured = false;

	bool execute() {
	#if !(defined RE_OS_WINDOWS) && !(defined RE_OS_LINUX)
	#	warning The targeted OS is unknown, so the engine will terminate immediatly upon execution
		RE_ERROR("The OS is unknown. The engine can't initialize");
		return false;
	#else
		if (bErrorOccured) {
			RE_ERROR("A fatal error occured before and further attempts to run the engine will be dropped");
			return false;
		} else if (bRunning) {
			RE_ERROR("The engine is already running");
			return false;
		}
		PRINT_DEBUG("Starting the engine");
		if (!load_settings())
			return false;
	#	ifdef RE_OS_WINDOWS
		if (!create_window())
			return false;
	#	elif defined RE_OS_LINUX
		if (std::getenv("WAYLAND_DISPLAY")) {
			PRINT_DEBUG("Creating a window in Wayland");
			eLinuxWindowType = LINUX_WINDOW_TYPE_WAYLAND;
			if (!create_window()) {
				RE_NOTE("Failed to create a window in Wayland. Using X11 instead");
				eLinuxWindowType = LINUX_WINDOW_TYPE_X11;
				if (!create_window()) {
					RE_FATAL_ERROR("Failed both attempts to create a window in X11 and Wayland");
					return false;
				}
			}
		} else {
			PRINT_DEBUG("Creating a window in X11");
			eLinuxWindowType = LINUX_WINDOW_TYPE_X11;
			if (!create_window()) {
				RE_FATAL_ERROR("Failed to create a window in X11");
				return false;
			}
		}
	#	endif
		if (init_input()) {
			if (init_manager()) {
				if (init_logical_gpu()) {
					if (init_renderer()) {
						if (init_settings()) {
							PRINT_DEBUG("Starting game loop");
							show_window(true);
							bRunning = true;

							// Game loop
							while (!should_window_close() && bRunning && are_scenes_present() && !bErrorOccured) {
								const std::chrono::steady_clock::time_point std_workStart = std::chrono::steady_clock::now();
								window_proc();
								if ((mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
									settings_update();
								else
									game_logic_update();
								render();

								PRINT_DEBUG("Calculating deltatime for next frame");
								if (fMinDeltatime > 0.0f) {
									const float fWorktime = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - std_workStart).count();
									if (fWorktime < fMinDeltatime)
										std::this_thread::sleep_for(std::chrono::duration<float>(fMinDeltatime - fWorktime));
								}
								fDeltaseconds = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - std_workStart).count();
								fDeltaseconds = std::min(fDeltaseconds, fMaxDeltatime);
							}

							bRunning = false;
							show_window(false);
							vkDeviceWaitIdle(vk_hDevice);
							last_game_logic_update();
							fDeltaseconds = 0.0f;
							destroy_settings();
						}
						destroy_renderer();
					}
					destroy_logical_gpu();
				}
				destroy_manager();
			}
			destroy_input();
		}
		destroy_window();
		print_error_count();
		return !bErrorOccured;
	#endif
	}

	float get_deltaseconds() {
		return fDeltaseconds;
	}

	float get_fps_rate() {
		return fDeltaseconds > 0.0f ? (1.0f / fDeltaseconds) : 0.0f;
	}

}
