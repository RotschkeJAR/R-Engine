#include "RE_Ext Header.hpp"
#include "RE_Window_Win64.hpp"
#include "RE_Window_X11.hpp"
#include "RE_Renderer.hpp"
#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"
#include "RE_Manager.hpp"
#include "RE_Signals.hpp"

namespace RE {

	bool bErrorOccured = false;
	bool bRunning = false;
	
	void execute() {
		DEFINE_SIGNAL_GUARD(sigGuardMainLoop);
		std::setlocale(LC_ALL, "");
		SignalCatcher signalCatcher;
		Window* pWindow = nullptr;
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(pWindow = new Window_Win64());
#elif defined RE_OS_LINUX
		CATCH_SIGNAL(pWindow = new Window_X11());
#else
# warning The targeted OS is unknown, so the engine will terminate immediatly upon execution
		RE_FATAL_ERROR("The OS is unknown. The engine can't initialize");
		return;
#endif
		if (!pWindow->isValid() || bErrorOccured) {
			delete pWindow;
			return;
		}
		{
			Manager gameMgr;
			Vulkan vulkan;
			if (!vulkan.isValid() || bErrorOccured)
				return;
			RenderSystem renderSystem;
			if (!renderSystem.isValid() || bErrorOccured)
				return;
			//Renderer renderer;
			bRunning = true;
			while (bRunning) {
				CATCH_SIGNAL(pWindow->update());
				CATCH_SIGNAL(gameMgr.gameLogicUpdate());
				CATCH_SIGNAL(renderSystem.drawFrame());
				//renderer.render();
				pWindow->show(true);
				bRunning = !pWindow->shouldClose() && gameMgr.isGameValid() && !bErrorOccured;
			}
			pWindow->show(false);
			CATCH_SIGNAL(gameMgr.lastGameLogicUpdate());
			CATCH_SIGNAL(vkDeviceWaitIdle(RE_VK_HANDLE_DEVICE));
		}
		delete pWindow;
	}

}
