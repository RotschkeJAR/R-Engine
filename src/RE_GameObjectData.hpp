#ifndef __RE_GAME_OBJECT_DATA_H__
#define __RE_GAME_OBJECT_DATA_H__

#include "RE_Internal.hpp"

namespace RE {

	struct GameObjectData final {
		Transform transform;
		SpriteRenderer spriteRenderer;
	};

}

#endif /* __RE_GAME_OBJECT_DATA_H__ */
