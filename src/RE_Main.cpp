#include "RE_Main.hpp"
#include "RE_Window.hpp"
#include "RE_Renderer.hpp"
#include "RE_GPU.hpp"
#include "RE_Manager.hpp"

#include <stdlib.h>

namespace RE {

#define LOWEST_SMOOTH_FPS                  15
#define DELTATIME_FOR_LOWEST_SMOOTH_FPS    (1.0f / LOWEST_SMOOTH_FPS)

#define STANDARD_FPS                 60
#define DELTATIME_FOR_STANDARD_FPS   (1.0f / STANDARD_FPS)

#define CALCULATE_FPS(DELTATIME) static_cast<uint32_t>(std::roundf(1.0f / DELTATIME))

	static float fMaxDeltatime = DELTATIME_FOR_LOWEST_SMOOTH_FPS,
		fMinDeltatime = DELTATIME_FOR_STANDARD_FPS;
	float fDeltaseconds = 0.0f;
	bool bRunning = false,
		bErrorOccured = false;

	bool execute() {
	#if !(defined RE_OS_WINDOWS) && !(defined RE_OS_LINUX)
	#warning The targeted OS is unknown, so the engine will terminate immediatly upon execution
		RE_ERROR("The OS is unknown. The engine can't initialize");
		return false;
	#endif
		if (bErrorOccured) {
			RE_ERROR("A fatal error occured before and further attempts to run the engine will be dropped");
			return false;
		} else if (bRunning) {
			RE_ERROR("The engine is already running");
			return false;
		}
		PRINT_DEBUG("Starting the engine");
	#ifdef RE_OS_LINUX
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
	#elif defined RE_OS_WINDOWS
		if (!create_window())
			return false;
	#endif
		if (init_manager()) {
			if (init_logical_gpu()) {
				if (init_renderer()) {
					PRINT_DEBUG("Starting game loop");
					show_window(true);
					bRunning = true;

					// Game loop
					while (!should_window_close() && bRunning && are_scenes_present() && !bErrorOccured) {
						const std::chrono::steady_clock::time_point std_workStart = std::chrono::steady_clock::now();
						window_proc();
						game_logic_update();
						render();

						PRINT_DEBUG("Calculating deltatime for next frame");
						const float fWorktime = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - std_workStart).count();
						if (fWorktime < fMinDeltatime)
							std::this_thread::sleep_for(std::chrono::duration<float>(fMinDeltatime - fWorktime));
						fDeltaseconds = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - std_workStart).count();
						fDeltaseconds = std::min(fDeltaseconds, fMaxDeltatime);
					}

					bRunning = false;
					show_window(false);
					vkDeviceWaitIdle(vk_hDevice);
					last_game_logic_update();
					fDeltaseconds = 0.0f;
					destroy_renderer();
				}
				destroy_logical_gpu();
			}
			destroy_manager();
		}
		destroy_window();
		print_error_count();
		return !bErrorOccured;
	}

	float get_deltaseconds() {
		return fDeltaseconds;
	}

	float get_fps_rate() {
		return fDeltaseconds > 0.0f ? (1.0f / fDeltaseconds) : 0.0f;
	}

	void set_max_deltatime(float fNewMaxDeltatime) {
		fMaxDeltatime = fNewMaxDeltatime;
	}

	float get_max_deltatime() {
		return fMaxDeltatime;
	}

	void set_fps_limit(uint32_t u32NewFpsLimit) {
		if (u32NewFpsLimit > 0) {
			fMinDeltatime = 1.0f / u32NewFpsLimit;
			if (fMaxDeltatime < fMinDeltatime) {
				RE_WARNING("New FPS limit (", fMinDeltatime, ") conflicts with maximum deltatime (", fMaxDeltatime, "). Setting maximum deltatime to half of the FPS limit");
				set_max_deltatime(1.0f / (CALCULATE_FPS(fMinDeltatime) / 2.0f));
			}
		} else
			fMinDeltatime = 0.0f;
	}

	uint32_t get_fps_limit() {
		if (fMinDeltatime > 0.0f)
			return CALCULATE_FPS(fMinDeltatime);
		else
			return 0;
	}

	void disable_fps_limit() {
		set_fps_limit(0);
	}

	bool is_fps_limit_enabled() {
		return fMinDeltatime == 0.0f;
	}

}
