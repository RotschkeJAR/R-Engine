#ifndef __RE_MANAGER_H__
#define __RE_MANAGER_H__

#include "RE_Ext Header.hpp"

namespace RE {

	class Manager {
		private:
			bool shouldUpdateObject(GameObject* pGameObject);
			void startProc();
			void updateProc();
			void endProc();
			void deleteProc();
			void addProc();

		public:
			static Scene *pCurrentScene, *pNextScene;
			std::vector<GameObject*> deletableGameObjects, newGameObjects;
			static std::vector<GameObject*> gameObjects;
			static Manager* pInstance;

			Manager();
			~Manager();
			void gameLogicUpdate();
			void lastGameLogicUpdate();
			bool isGameValid();
	};

	void markDelete(GameObject* pGameObject);

	void setNextScene(Scene* pNextScene);
	bool isNextSceneSet();
	Scene* getCurrentScene();
	REuint getCurrentSceneId();
	bool isSceneCurrent(REuint u32SceneId);
	Scene* getNextScene();
	REuint getNextSceneId();
	bool isSceneNext(REuint u32SceneId);

}

#endif /* __RE_MANAGER_H__ */
