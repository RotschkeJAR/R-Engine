#include "RE_Internal Header.hpp"
#include "RE_Window.hpp"
#include "RE_Renderer.hpp"
#include "RE_GPU.hpp"
#include "RE_Manager.hpp"

namespace RE {

#define LOWEST_SMOOTH_FPS 10
#define DELTATIME_FOR_LOWEST_SMOOTH_FPS (1.0f / LOWEST_SMOOTH_FPS)

	float fDeltaseconds = 0.0f, fMinDeltatime = 1.0f / 60, fMaxLagTime = DELTATIME_FOR_LOWEST_SMOOTH_FPS;
	bool bErrorOccured = false, bRunning = false;

	bool execute() {
#if !(defined RE_OS_WINDOWS) && !(defined RE_OS_LINUX)
# warning The targeted OS is unknown, so the engine will terminate immediatly upon execution
		RE_ERROR("The OS is unknown. The engine can't initialize");
		return false;
#else
		if (bErrorOccured)
			RE_ERROR("A fatal error occured before and further attempts to run the engine will be dropped");
		else if (bRunning)
			RE_ERROR("The engine is already running");
		else if (create_window()) {
			if (init_vulkan_instance()) {
				if (init_render_system()) {
					if (init_renderer()) {
						PRINT_DEBUG("Starting game loop");
						std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now(), lastFrameTime;
						show_window(true);
						bRunning = true;

						// Game loop
						while (!should_window_close() && bRunning && are_scenes_present() && !bErrorOccured) {
							window_proc();
							game_logic_update();
							if (should_render() && fDeltaseconds > 0.0f) {
								refresh_swapchain();
								render();
							}

							PRINT_DEBUG("Calculating deltatime for next frame");
							lastFrameTime = currentFrameTime;
							currentFrameTime = std::chrono::high_resolution_clock::now();
							fDeltaseconds = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastFrameTime).count();
							if (fDeltaseconds <= fMaxLagTime) {
								const float fTimeToWait = fMinDeltatime - fDeltaseconds;
								if (fTimeToWait > 0.0f) {
									PRINT_DEBUG("Main thread sleeps for ", fTimeToWait);
									std::this_thread::sleep_for(std::chrono::duration<float>(fTimeToWait));
								}
							} else
								fDeltaseconds = 0.0f;
						}

						PRINT_DEBUG("Exiting game loop");
						bRunning = false;
						show_window(false);
						WAIT_FOR_IDLE_VULKAN_DEVICE();
						last_game_logic_update();
						fDeltaseconds = 0.0f;
						destroy_renderer();
					}
					destroy_render_system();
				}
				destroy_vulkan_instance();
			}
			destroy_window();
		}
		PRINT_DEBUG("Finished execution");
		print_error_count();
		return !bErrorOccured;
#endif
	}

	[[nodiscard]]
	float get_deltaseconds() {
		return fDeltaseconds;
	}

	[[nodiscard]]
	float get_fps_rate() {
		return fDeltaseconds > 0.0f ? (1.0f / fDeltaseconds) : -1.0f;
	}

	void set_fps_limit(const uint32_t u32MaxFramesPerSecond) {
		fMinDeltatime = u32MaxFramesPerSecond ? (1.0f / static_cast<float>(u32MaxFramesPerSecond)) : -1.0f;
	}

	[[nodiscard]]
	uint32_t get_fps_limit() {
		return fMinDeltatime > 0.0f ? static_cast<uint32_t>(std::round(1.0f / fMinDeltatime)) : 0;
	}

	void set_max_lag_time(const float fSecondsOfLag) {
		if (fSecondsOfLag <= 0.0f) {
			RE_WARNING("Maximum lag time cannot be zero or negative. Old value will be kept");
			return;
		} else if (fSecondsOfLag < DELTATIME_FOR_LOWEST_SMOOTH_FPS)
			RE_WARNING("The new maximum lag time (", fSecondsOfLag, " seconds = ", static_cast<uint32_t>(std::round(1.0f / fSecondsOfLag)), " FPS) might be too low causing the simulation to stutter or freeze. Common fixed deltatime for ", LOWEST_SMOOTH_FPS, " FPS is ", DELTATIME_FOR_LOWEST_SMOOTH_FPS, " seconds");
		fMaxLagTime = fSecondsOfLag;
	}

	[[nodiscard]]
	float get_max_lag_time() {
		return fMaxLagTime;
	}

}
