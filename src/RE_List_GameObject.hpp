#ifndef __RE_LIST_GAME_OBJECT_H__
#define __RE_LIST_GAME_OBJECT_H__

#include "RE_Internal Header.hpp"
#include "RE_ListBatch_GameObject.hpp"

namespace RE {

	extern std::vector<GameObject*> newGameObjects, deletableGameObjects;
	extern std::list<ListBatch_GameObject*> gameObjectBatchList;
	
	void add_new_game_objects();
	void delete_marked_game_objects();
	void add_game_object(GameObject *pGameObject);
	void remove_game_object(const GameObject *pGameObject);
	void mark_game_object_deletable(GameObject *pGameObject);

	void start_game_objects();
	void update_game_objects();
	void end_game_objects();

	bool init_game_object_render_batches();
	void destroy_game_object_render_batches();
	void load_game_object_vertices(bool &rbNeedsRender);
	void render_opaque_game_objects();
	void render_transparent_game_objects();

}

#endif /* __RE_LIST_GAME_OBJECT_H__ */
