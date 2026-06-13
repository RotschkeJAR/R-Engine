#include "RE_Main.hpp"
#include "RE_Window.hpp"
#include "RE_Renderer.hpp"
#include "RE_GPU.hpp"
#include "RE_Manager.hpp"

#include <stdlib.h>

namespace RE {

#define LOWEST_SMOOTH_FPS 15
#define DELTATIME_FOR_LOWEST_SMOOTH_FPS (1.0f / LOWEST_SMOOTH_FPS)

	static float f32MaxDeltatime = DELTATIME_FOR_LOWEST_SMOOTH_FPS;
	float f32Deltaseconds = 0.0f;
	bool bRunning = false,
		bErrorOccured = false;

#define CALCULATE_FPS(DELTATIME) static_cast<uint32_t>(std::roundf(1.0f / DELTATIME))

	bool execute() {
#if !(defined RE_OS_WINDOWS) && !(defined RE_OS_LINUX)
# warning The targeted OS is unknown, so the engine will terminate immediatly upon execution
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
		if (getenv("WAYLAND_DISPLAY")) {
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
		if (init_logical_gpu()) {
			if (init_renderer()) {
				PRINT_DEBUG("Starting game loop");
				std::chrono::steady_clock::time_point currentFrameTime = std::chrono::steady_clock::now(),
					lastFrameTime = currentFrameTime;
				show_window(true);
				bRunning = true;

				// Game loop
				while (!should_window_close() && bRunning && are_scenes_present() && !bErrorOccured) {
					window_proc();
					game_logic_update();
					if (should_render())
						render();

					PRINT_DEBUG("Calculating deltatime for next frame");
					currentFrameTime = std::chrono::steady_clock::now();
					f32Deltaseconds = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastFrameTime).count();
					lastFrameTime = currentFrameTime;
					f32Deltaseconds = std::min(f32Deltaseconds, f32MaxDeltatime);
				}

				bRunning = false;
				show_window(false);
				vkDeviceWaitIdle(vk_hDevice);
				last_game_logic_update();
				f32Deltaseconds = 0.0f;
				destroy_renderer();
			}
			destroy_logical_gpu();
		}
		destroy_window();
		print_error_count();
		return !bErrorOccured;
	}

	float get_deltaseconds() {
		return f32Deltaseconds;
	}

	float get_fps_rate() {
		return f32Deltaseconds > 0.0f ? (1.0f / f32Deltaseconds) : 0.0f;
	}

	void set_max_deltatime(const float f32NewMaxDeltatime) {
		f32MaxDeltatime = f32NewMaxDeltatime;
	}

	float get_max_deltatime() {
		return f32MaxDeltatime;
	}

}
