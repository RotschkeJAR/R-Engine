#include "RE_Internal Header.hpp"
#include "RE_Window.hpp"
#include "RE_Renderer.hpp"
#include "RE_Render System.hpp"
#include "RE_Manager.hpp"

namespace RE {

	bool bErrorOccured = false;
	bool bRunning = false;
	float fDeltaseconds = 0.0f, fMinDeltatime = 1.0f / 60;

	void execute() {
#if !(defined RE_OS_WINDOWS) && !(defined RE_OS_LINUX)
# warning The targeted OS is unknown, so the engine will terminate immediatly upon execution
		RE_ERROR("The OS is unknown. The engine can't initialize");
		return;
#endif
		std::setlocale(LC_ALL, "");
		if (!are_scenes_present()) {
			RE_ERROR("The aren't any scenes yet. The engine won't launch");
			return;
		}
		
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_window(), bool)) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_vulkan_instance(), bool)) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_render_system(), bool)) {
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_renderer(), bool)) {
						std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now(), lastFrameTime;
						PUSH_TO_CALLSTACKTRACE(show_window(true));
						bRunning = true;

						// Game loop
						while (!should_window_close() && are_scenes_present() && !bErrorOccured) {
							PUSH_TO_CALLSTACKTRACE(window_proc());
							PUSH_TO_CALLSTACKTRACE(game_logic_update());
							if (should_render()) {
								PUSH_TO_CALLSTACKTRACE(refresh_swapchain());
								PUSH_TO_CALLSTACKTRACE(render());
							}

							lastFrameTime = currentFrameTime;
							currentFrameTime = std::chrono::high_resolution_clock::now();
							fDeltaseconds = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastFrameTime).count();
							const float fTimeToWait = fMinDeltatime - fDeltaseconds;
							if (fTimeToWait > 0.0f)
								std::this_thread::sleep_for(std::chrono::duration<float>(fTimeToWait));
						}

						// Termination
						bRunning = false;
						PUSH_TO_CALLSTACKTRACE(show_window(false));
						PUSH_TO_CALLSTACKTRACE(last_game_logic_update());

						WAIT_FOR_IDLE_VULKAN_DEVICE();
						PUSH_TO_CALLSTACKTRACE(destroy_renderer());
					}
					PUSH_TO_CALLSTACKTRACE(destroy_render_system());
				}
				PUSH_TO_CALLSTACKTRACE(destroy_vulkan_instance());
			}
			PUSH_TO_CALLSTACKTRACE(destroy_window());
		}
		fDeltaseconds = 0.0f;
	}

	[[nodiscard]]
	float get_deltaseconds() {
		return fDeltaseconds;
	}

	void set_fps_limit(const uint32_t u32MaxFramesPerSecond) {
		fMinDeltatime = u32MaxFramesPerSecond ? (1.0f / static_cast<float>(u32MaxFramesPerSecond)) : -1.0f;
	}

	[[nodiscard]]
	float get_fps_rate() {
		return fDeltaseconds > 0.0f ? (1.0f / fDeltaseconds) : -1.0f;
	}

}
