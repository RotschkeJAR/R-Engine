#include "RE_Internal.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_GameObjectData.hpp"

namespace RE {
	
	GameObject::GameObject(uint32_t u32OwnId, uint32_t u32SceneParentId) : bNew(true),
			pData(std::malloc(sizeof(GameObjectData))),
			u32OwnId(u32OwnId),
			u32SceneParentId(u32SceneParentId) {
		add_game_object(this);
	}
	
	GameObject::~GameObject() {
		remove_game_object(this);
		std::free(pData);
	}

	void GameObject::start() {}
	void GameObject::update() {}
	void GameObject::end() {}

	Transform GameObject::get_transform() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform;
	}

	Vector3f GameObject::get_transform_position() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.position;
	}

	float GameObject::get_transform_position_x() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.position[0];
	}
	
	float GameObject::get_transform_position_y() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.position[1];
	}
	
	float GameObject::get_transform_position_z() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.position[2];
	}

	Quaternion GameObject::get_transform_rotation() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.rotation;
	}

	float GameObject::get_transform_rotation_pitch() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.rotation.get_pitch();
	}
	
	float GameObject::get_transform_rotation_yaw() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.rotation.get_yaw();
	}
	
	float GameObject::get_transform_rotation_roll() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.rotation.get_roll();
	}

	Vector3f GameObject::get_transform_scale() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.scale;
	}

	float GameObject::get_transform_scale_width() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.scale[0];
	}
	
	float GameObject::get_transform_scale_height() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.scale[1];
	}
	
	float GameObject::get_transform_scale_depth() const {
		return reinterpret_cast<GameObjectData*>(pData)->transform.scale[2];
	}
	
	SpriteRenderer GameObject::get_sprite_renderer() const {
		return reinterpret_cast<GameObjectData*>(pData)->spriteRenderer;
	}
	
	Color GameObject::get_sprite_renderer_color() const {
		return reinterpret_cast<GameObjectData*>(pData)->spriteRenderer.color;
	}

	float GameObject::get_sprite_renderer_color_red() const {
		return reinterpret_cast<GameObjectData*>(pData)->spriteRenderer.color.get_red();
	}
	
	float GameObject::get_sprite_renderer_color_green() const {
		return reinterpret_cast<GameObjectData*>(pData)->spriteRenderer.color.get_green();
	}
	
	float GameObject::get_sprite_renderer_color_blue() const {
		return reinterpret_cast<GameObjectData*>(pData)->spriteRenderer.color.get_blue();
	}
	
	float GameObject::get_sprite_renderer_color_alpha() const {
		return reinterpret_cast<GameObjectData*>(pData)->spriteRenderer.color.get_alpha();
	}

	void GameObject::set_transform(const Transform &rTransform) {
		reinterpret_cast<GameObjectData*>(pData)->transform = rTransform;
	}
	
	void GameObject::set_transform_position(const Vector3f &rPosition) {
		reinterpret_cast<GameObjectData*>(pData)->transform.position = rPosition;
	}

	void GameObject::set_transform_position_x(const float fX) {
		reinterpret_cast<GameObjectData*>(pData)->transform.position[0] = fX;
	}
	
	void GameObject::set_transform_position_y(const float fY) {
		reinterpret_cast<GameObjectData*>(pData)->transform.position[1] = fY;
	}
	
	void GameObject::set_transform_position_z(const float fZ) {
		reinterpret_cast<GameObjectData*>(pData)->transform.position[2] = fZ;
	}
	
	void GameObject::set_transform_rotation(const Quaternion &rRotation) {
		reinterpret_cast<GameObjectData*>(pData)->transform.rotation = rRotation;
	}

	void GameObject::set_transform_rotation_pitch(const float fPitch) {
		reinterpret_cast<GameObjectData*>(pData)->transform.rotation.set_pitch(fPitch);
	}
	
	void GameObject::set_transform_rotation_yaw(const float fYaw) {
		reinterpret_cast<GameObjectData*>(pData)->transform.rotation.set_yaw(fYaw);
	}
	
	void GameObject::set_transform_rotation_roll(const float fRoll) {
		reinterpret_cast<GameObjectData*>(pData)->transform.rotation.set_roll(fRoll);
	}
	
	void GameObject::set_transform_scale(const Vector3f &rScale) {
		reinterpret_cast<GameObjectData*>(pData)->transform.scale = rScale;
	}

	void GameObject::set_transform_scale_width(const float fWidth) {
		reinterpret_cast<GameObjectData*>(pData)->transform.scale[0] = fWidth;
	}
	
	void GameObject::set_transform_scale_height(const float fHeight) {
		reinterpret_cast<GameObjectData*>(pData)->transform.scale[1] = fHeight;
	}
	
	void GameObject::set_transform_scale_depth(const float fDepth) {
		reinterpret_cast<GameObjectData*>(pData)->transform.scale[2] = fDepth;
	}
	
	void GameObject::set_sprite_renderer(const SpriteRenderer &rSpriteRenderer) {
		reinterpret_cast<GameObjectData*>(pData)->spriteRenderer = rSpriteRenderer;
	}
	
	void GameObject::set_sprite_renderer_color(const Color &rColor) {
		reinterpret_cast<GameObjectData*>(pData)->spriteRenderer.color = rColor;
	}

	void GameObject::set_sprite_renderer_color_red(const float fRed) {
		reinterpret_cast<GameObjectData*>(pData)->spriteRenderer.color.set_red(fRed);
	}
	
	void GameObject::set_sprite_renderer_color_green(const float fGreen) {
		reinterpret_cast<GameObjectData*>(pData)->spriteRenderer.color.set_green(fGreen);
	}
	
	void GameObject::set_sprite_renderer_color_blue(const float fBlue) {
		reinterpret_cast<GameObjectData*>(pData)->spriteRenderer.color.set_blue(fBlue);
	}
	
	void GameObject::set_sprite_renderer_color_alpha(const float fAlpha) {
		reinterpret_cast<GameObjectData*>(pData)->spriteRenderer.color.set_alpha(fAlpha);
	}

}
