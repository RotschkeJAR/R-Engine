#include "RE_Manager.hpp"

namespace RE {

	std::vector<GameObject*> Manager::gameObjects;
	REuint Manager::u32ObjCount = 0;
	Manager* Manager::pInstance = nullptr;
	
	Manager::Manager() : pCurrentScene(nullptr), pNextScene(nullptr) {
		if (Manager::pInstance) {
			RE_FATAL_ERROR("Another instance of the class \"Manager\" has been constructed");
			return;
		}
		Manager::pInstance = this;
	}

	Manager::~Manager() {
		if (Manager::pInstance != this)
			return;
		Manager::pInstance = nullptr;
	}

	void Manager::gameLogicUpdate() {

	}

}
