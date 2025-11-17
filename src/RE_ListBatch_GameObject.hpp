#ifndef __RE_BATCH_GAME_OBJECT_H__
#define __RE_BATCH_GAME_OBJECT_H__

#include "RE_Renderer.hpp"

namespace RE {

#define MAXIMUM_GAME_OBJECTS_PER_BATCH 1000
	
	class ListBatch_GameObject final {
		private:
			std::array<GameObject*, MAXIMUM_GAME_OBJECTS_PER_BATCH> apGameObjects;
			uint16_t u16Count;

		public:
			ListBatch_GameObject();
			~ListBatch_GameObject();

			void add(GameObject *pGameObject);
			bool remove(const GameObject *pGameObject);
			bool contains(const GameObject *pGameObject) const;
			GameObject* at(uint16_t u16Index) const;

			uint16_t size() const;
			bool empty() const;
			bool has_space() const;

			void start();
			void update();
			void end();

			GameObject* operator[](uint16_t u16Index) const;
	};

}

#endif /* __RE_BATCH_GAME_OBJECT_H__ */
