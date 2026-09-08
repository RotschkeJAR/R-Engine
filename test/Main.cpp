#include "RE.hpp"

using namespace RE;

class Nexie : public GameObject {
	public:
		Nexie() : GameObject(0, 2) {}
		~Nexie() {}
		void start() {
			PRINT_LN("start nexie");
		}
		void update() {}
		void end() {
			PRINT_LN("end nexie");
		}
};

class Second : public Scene {
	public:
		Nexie nexie;

		Second() : Scene(2) {}
		~Second() {}
};

Second* second = nullptr;

class Clonus : public GameObject {
	public:
		Clonus() : GameObject(0, 0) {}
		~Clonus() {}
		void start() {
			PRINT_LN("start clonus");
		}
		void update() {}
		void end() {
			PRINT_LN("end clonus");
		}
};

Clonus* clonus = nullptr;

class Playy;
Playy *pPlayy = nullptr;

class Playy : public GameObject {
	public:
		RandomNumberGenerator rng;
		unsigned long ulHits,
			ulMisses;

		Playy() : GameObject(1, 1), ulHits(0), ulMisses(0) {
			pPlayy = this;
			set_transform_position_x(0.7f);
			set_transform_position_y(0.7f);
			set_transform_position_z(0.7f);
			set_transform_scale_width(0.6f);
			set_transform_scale_height(0.6f);
		}
		~Playy() {
			pPlayy = nullptr;
		}
		void start() {
			PRINT_LN("start objy");
		}
		void update() {
			bool randomResult = rng.random_bool(0.2);
			if (randomResult)
				ulHits++;
			else
				ulMisses++;
			set_sprite_renderer_color_red(std::fmod(std::abs(get_transform_position_x()), 1.0f));
			set_sprite_renderer_color_green(std::fmod(std::abs(get_transform_position_y()), 1.0f));
			set_sprite_renderer_color_blue(std::fmod(std::abs(get_transform_position_z()), 1.0f));
		}
		void end() {
			PRINT_LN(append_to_string(ulHits, ", ", ulMisses).c_str());
		}
};

class Imagy : public GameObject {
	public:
		Imagy() : GameObject(3, 1) {
			set_transform_position_x(0.8f);
			set_transform_position_y(0.8f);
			set_transform_position_z(0.8f);
			set_transform_scale_width(0.3f);
			set_transform_scale_height(0.3f);
		}
		~Imagy() {}
		void start() {
			/*const Texture hTexture = alloc_texture_loading_from_file("Image.png");
			const SpriteLayoutSettings spriteLayoutSettings;
			const SpriteLayout hSpriteLayout = create_sprite_layout(spriteLayoutSettings);
			spriteRenderer.hSprite = create_sprite(hTexture, hSpriteLayout);*/
		}
		void update() {}
		void end() {
			/*const Texture hTexture = get_texture_from_sprite(spriteRenderer.hSprite);
			const SpriteLayout hSpriteLayout = get_sprite_layout_from_sprite(spriteRenderer.hSprite);
			destroy_sprite(spriteRenderer.hSprite);
			destroy_sprite_layout(hSpriteLayout);
			free_texture(hTexture);*/
		}
};

class Background : public GameObject {
	public:
		Background() : GameObject(2, 1) {
			set_transform_position_z(0.5f);
			set_transform_scale_width(2.0f);
			set_transform_scale_height(2.0f);
			set_sprite_renderer_color_green(0.0f);
			set_sprite_renderer_color_blue(0.0f);
			set_sprite_renderer_color_alpha(0.4f);
		}
};

class PlayerCamera : public Camera {
	private:

	public:
		PlayerCamera() {
			//transform.position[2] = -10.0f;
			view[0] = 1.33f;
			view[1] = view[0];
		}
		~PlayerCamera() {}
		void update_before_render() {
			if (pPlayy) {
				transform.position[0] = pPlayy->get_transform_position_x();
				transform.position[1] = pPlayy->get_transform_position_y();
			}
		}
};

class First : public Scene {
	public:
		Playy objy;
		Background background;
		Imagy imagy;
		PlayerCamera playerCam;
		bool bCamActive,
			bNormalScreen;

		First() : Scene(1), bCamActive(true), bNormalScreen(true) {}
		~First() {}
		void start() {
			playerCam.activate();
			ERROR("test message\ntesty");
			WARNING("test message\ntesty");
			NOTE("test message\ntesty");
		}
		void update() {
			//PRINT_LN(get_fps_rate());
		}
		void end() {}
};

int main_func() {
	First first;
	Second secondInStack;
	second = &secondInStack;
	set_next_scene(&first);
	execute();
	if (clonus)
		delete clonus;
	return 0;
}

#ifdef RE_OS_WINDOWS
int WINAPI wWinMain(HINSTANCE win_hInstance, HINSTANCE win_hPrevInstance, PWSTR pawcCmdLine, int iCmdShow) {
	win64_set_hinstance(win_hInstance);
	return main_func();
}
#endif

int main() {
	return main_func();
}
