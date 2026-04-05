#include "RE_Main.hpp"
#include "RE_Window.hpp"
#include "RE_Renderer.hpp"
#include "RE_GPU.hpp"
#include "RE_Manager.hpp"

namespace RE {

#define LOWEST_SMOOTH_FPS 15
#define DELTATIME_FOR_LOWEST_SMOOTH_FPS (1.0f / LOWEST_SMOOTH_FPS)
#define MIN_LAG_TIME 0.5f
#define MIN_EXHAUSTION_TIME 10.0f

	static float f32MinDeltatime = 1.0f / 60,
		f32MaxDeltatime = DELTATIME_FOR_LOWEST_SMOOTH_FPS,
		f32MaxExhaustionTime = f32MaxDeltatime + MIN_EXHAUSTION_TIME;
	float f32Deltaseconds = 0.0f;
	bool bRunning = false, bErrorOccured = false;

#define CALCULATE_FPS(DELTATIME) static_cast<uint32_t>(std::roundf(1.0f / DELTATIME))

	bool execute() {
#if !(defined RE_OS_WINDOWS) && !(defined RE_OS_LINUX)
# warning The targeted OS is unknown, so the engine will terminate immediatly upon execution
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
		if (create_window()) {
			if (init_logical_gpu()) {
				if (init_renderer()) {
					PRINT_DEBUG("Starting game loop");
					std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now(),
						lastFrameTime,
						lastExecutionTimepoint;
					show_window(true);
					bRunning = true;

					// Game loop
					while (!should_window_close() && bRunning && are_scenes_present() && !bErrorOccured) {
						window_proc();
						if (f32Deltaseconds > 0.0f) {
							game_logic_update();
							if (should_render())
								render();
							lastExecutionTimepoint = std::chrono::high_resolution_clock::now();
							currentFrameTime = lastExecutionTimepoint;
						} else
							currentFrameTime = std::chrono::high_resolution_clock::now();

						PRINT_DEBUG("Calculating deltatime for next frame");
						lastFrameTime = currentFrameTime;
						f32Deltaseconds = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastFrameTime).count();
						if (f32Deltaseconds <= f32MaxDeltatime) {
							if (f32Deltaseconds < f32MinDeltatime) {
								const float fTimeToWait = f32MinDeltatime - f32Deltaseconds;
								PRINT_DEBUG("Main thread sleeps for ", fTimeToWait, " seconds for FPS limit");
								std::this_thread::sleep_for(std::chrono::duration<float>(f32MinDeltatime - f32Deltaseconds));
							}
						} else {
							if (std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastExecutionTimepoint).count() >= f32MaxExhaustionTime) {
								RE_ERROR("The main thread has been exhausted for more than ", f32MaxExhaustionTime, " seconds. Exiting normally");
								break;
							}
							std::this_thread::sleep_for(std::chrono::duration<float>(f32MinDeltatime - (f32Deltaseconds - std::truncf(f32Deltaseconds / f32MinDeltatime) * f32MinDeltatime)));
							f32Deltaseconds = 0.0f;
						}
					}

					PRINT_DEBUG("Exiting game loop");
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
		}
		print_error_count();
		PRINT_DEBUG("Finished execution");
		return !bErrorOccured;
#endif
	}

	[[nodiscard]]
	float get_deltaseconds() {
		return f32Deltaseconds;
	}

	[[nodiscard]]
	float get_fps_rate() {
		return f32Deltaseconds > 0.0f ? (1.0f / f32Deltaseconds) : 0.0f;
	}

	void set_fps_limit(const uint32_t u32MaxFramesPerSecond) {
		if (u32MaxFramesPerSecond < LOWEST_SMOOTH_FPS && u32MaxFramesPerSecond > 0)
			RE_WARNING("The new FPS limit (", u32MaxFramesPerSecond, ") might be too low and cause visual stutters");
		if (u32MaxFramesPerSecond > 0) {
			PRINT_DEBUG("Setting FPS limit to ", u32MaxFramesPerSecond);
			f32MinDeltatime = 1.0f / u32MaxFramesPerSecond;
		} else {
			PRINT_DEBUG("Removing the FPS limit (", CALCULATE_FPS(f32MinDeltatime), ")");
			f32MinDeltatime = -1.0f;
		}
		if (f32MinDeltatime > f32MaxDeltatime - MIN_LAG_TIME) {
			RE_NOTE("The new FPS limit (", u32MaxFramesPerSecond, " => ", f32MinDeltatime, " seconds) is to close to the maximum lag time (", f32MaxDeltatime, " seconds => ", CALCULATE_FPS(f32MaxDeltatime), " frames per second). Maximum lag time will be increased by ", MIN_LAG_TIME, " seconds.");
			set_max_lag_time(std::max<float>(f32MinDeltatime + MIN_LAG_TIME, DELTATIME_FOR_LOWEST_SMOOTH_FPS));
		}
	}

	[[nodiscard]]
	uint32_t get_fps_limit() {
		return f32MinDeltatime > 0.0f ? CALCULATE_FPS(f32MinDeltatime) : 0;
	}

	void set_max_lag_time(const float f32MaxSecondsOfLag) {
		if (f32MaxSecondsOfLag <= 0.0f) {
			RE_ERROR("Maximum lag time cannot be zero or negative. Maximum lag time won't be changed");
			return;
		} else if (f32MaxSecondsOfLag < DELTATIME_FOR_LOWEST_SMOOTH_FPS)
			RE_WARNING("The new maximum lag time (", f32MaxSecondsOfLag, " seconds => ", CALCULATE_FPS(f32MaxSecondsOfLag), " frames per second) might be too low causing the simulation to stutter or freeze. Common fixed deltatime for ", LOWEST_SMOOTH_FPS, " FPS is ", DELTATIME_FOR_LOWEST_SMOOTH_FPS, " seconds");
		if (f32MaxSecondsOfLag <= f32MinDeltatime) {
			RE_ERROR("New maximum lag time (", f32MaxSecondsOfLag, " seconds => ", CALCULATE_FPS(f32MaxSecondsOfLag), " frames per second) is lower than the FPS limit (", f32MinDeltatime, " seconds => ", CALCULATE_FPS(f32MinDeltatime), " frames per second). Maximum lag time won't be changed");
			return;
		}
		PRINT_DEBUG("Setting maximum deltatime to ", f32MaxSecondsOfLag, " seconds");
		f32MaxDeltatime = f32MaxSecondsOfLag;
		if (f32MaxDeltatime > f32MaxExhaustionTime - MIN_EXHAUSTION_TIME) {
			RE_NOTE("New maximum lag time (", f32MaxDeltatime, " seconds) is too close to the maximum exhaustion time (", f32MaxExhaustionTime, " seconds). Maximum exhaustion time will be increased by ", MIN_EXHAUSTION_TIME, " seconds");
			set_max_exhaustion_time(f32MaxDeltatime + MIN_EXHAUSTION_TIME);
		}
	}

	[[nodiscard]]
	float get_max_lag_time() {
		return f32MaxDeltatime;
	}

	void set_max_exhaustion_time(const float f32MaxSecondsOfExhaustion) {
		if (f32MaxSecondsOfExhaustion < MIN_EXHAUSTION_TIME) {
			RE_ERROR("New maximum exhaustion time is not allowed to be smaller than ", MIN_EXHAUSTION_TIME, " seconds. The change will not be applied");
			return;
		}
		if (f32MaxSecondsOfExhaustion < f32MaxDeltatime + MIN_EXHAUSTION_TIME) {
			RE_ERROR("There should be at least ", MIN_EXHAUSTION_TIME, " seconds between the maximum lag time (", f32MaxDeltatime, " seconds) and new maximum exhaustion time (", f32MaxSecondsOfExhaustion, " seconds)");
			return;
		}
		PRINT_DEBUG("Setting maximum exhaustion time to ", f32MaxSecondsOfExhaustion, " seconds");
		f32MaxExhaustionTime = f32MaxSecondsOfExhaustion;
	}

	[[nodiscard]]
	float get_max_exhaustion_time() {
		return f32MaxExhaustionTime;
	}

}
