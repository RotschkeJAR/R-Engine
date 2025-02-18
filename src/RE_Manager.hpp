#ifndef __RE_MANAGER_H__
#define __RE_MANAGER_H__

#include "RE_Ext Header.hpp"

namespace RE {

	class Manager {
		private:
			Scene *pCurrentScene, *pNextScene;
			static std::vector<GameObject*> gameObjects;
			static REuint u32ObjCount;

		public:
			static Manager* pInstance;

			Manager();
			~Manager();
			void gameLogicUpdate();

		friend class Scene;
		friend class GameObject;
	};

}

#endif /* __RE_MANAGER_H__ */
