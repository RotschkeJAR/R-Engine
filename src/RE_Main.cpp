#include "RE_Ext Header.hpp"
#include "RE_Window_Win64.hpp"
#include "RE_Window_X11.hpp"
#include "RE_Renderer.hpp"
#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"
#include "RE_Manager.hpp"

namespace RE {

	bool bErrorOccured = false;
	bool bRunning = false;
	
	void execute() {
		std::setlocale(LC_ALL, "");
		Window* pWindow = nullptr;
#ifdef RE_OS_WINDOWS
		pWindow = new Window_Win64();
#elif defined RE_OS_LINUX
		pWindow = new Window_X11();
#else
# warning The targeted OS is unknown, so the engine will terminate immediatly upon execution
		RE_FATAL_ERROR("The OS is unknown. The engine can't initialize");
		return;
#endif
		if (!pWindow->isValid()) {
			delete pWindow;
			return;
		}
		{
			Manager gameMgr;
			Vulkan vulkan;
			if (!vulkan.isValid())
				return;
			RenderSystem renderSystem;
			if (!renderSystem.isValid())
				return;
			//Renderer renderer;
			bRunning = true;
			while (bRunning) {
				pWindow->update();
				gameMgr.gameLogicUpdate();
				renderSystem.drawFrame();
				//renderer.render();
				pWindow->show(true);
				bRunning = !pWindow->shouldClose() && !bErrorOccured;
			}
			pWindow->show(false);
			vkDeviceWaitIdle(RE_VK_HANDLE_DEVICE);
		}
		delete pWindow;
	}

}
