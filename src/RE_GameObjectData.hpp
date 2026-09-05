#ifndef __RE_GAME_OBJECT_DATA_H__
#define __RE_GAME_OBJECT_DATA_H__ 1

#include "RE_Renderer_ShaderStructs.hpp"

namespace RE {

	struct GameObjectData final {
		Transform transform;
		SpriteRenderer spriteRenderer;
	};

	static_assert(sizeof(GameObjectData) == sizeof(GameObjectShaderData));
	static_assert(offsetof(GameObjectData, transform) + offsetof(Transform, position) == offsetof(GameObjectShaderData, a3fPosition));
	static_assert(sizeof(Transform::position) == sizeof(GameObjectShaderData::a3fPosition));
	static_assert(offsetof(GameObjectData, transform) + offsetof(Transform, rotation) == offsetof(GameObjectShaderData, a3fRotation));
	static_assert(sizeof(Transform::rotation) == sizeof(GameObjectShaderData::a3fRotation));
	static_assert(offsetof(GameObjectData, transform) + offsetof(Transform, scale) == offsetof(GameObjectShaderData, a3fScale));
	static_assert(sizeof(Transform::scale) == sizeof(GameObjectShaderData::a3fScale));
	static_assert(offsetof(GameObjectData, spriteRenderer) + offsetof(SpriteRenderer, color) == offsetof(GameObjectShaderData, a4fColor));
	static_assert(sizeof(SpriteRenderer::color) == sizeof(GameObjectShaderData::a4fColor));
	static_assert(offsetof(GameObjectData, spriteRenderer) + offsetof(SpriteRenderer, hTexture) == offsetof(GameObjectShaderData, u64TextureId));
	static_assert(sizeof(SpriteRenderer::hTexture) == sizeof(GameObjectShaderData::u64TextureId));

}

#endif /* __RE_GAME_OBJECT_DATA_H__ */
