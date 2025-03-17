#include "RE_Internal Header.hpp"
#include "RE_Window_Win64.hpp"
#include "RE_Window_X11.hpp"
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
		//SignalCatcher signalCatcher;
		Window* pWindow;
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(pWindow = new Window_Win64());
#elif defined RE_OS_LINUX
		CATCH_SIGNAL(pWindow = new Window_X11());
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
			//Renderer renderer;
			std::chrono::high_resolution_clock::time_point currentFrameTime = std::chrono::high_resolution_clock::now(), lastFrameTime;
			bRunning = true;
			while (bRunning) {
				CATCH_SIGNAL(pWindow->window_proc());
				CATCH_SIGNAL(gameMgr.game_logic_update());
				CATCH_SIGNAL(renderSystem.draw_frame());
				//renderer.render();
				lastFrameTime = currentFrameTime;
				currentFrameTime = std::chrono::high_resolution_clock::now();
				fDeltaseconds = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastFrameTime).count();
				CATCH_SIGNAL(pWindow->show_window(true));
				bRunning = !pWindow->should_close() && gameMgr.is_game_valid() && !bErrorOccured;
			}
			pWindow->show_window(false);
			CATCH_SIGNAL(gameMgr.last_game_logic_update());
			CATCH_SIGNAL(vkDeviceWaitIdle(RE_VK_HANDLE_DEVICE));
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
