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
		uint64_t hits, misses;

		Playy() : GameObject(1, 1), hits(0), misses(0) {
			pPlayy = this;
			transform.position[0] = 0.7f;
			transform.position[1] = 0.7f;
			transform.position[2] = 0.7f;
			transform.scale[0] = 0.6f;
			transform.scale[1] = 0.6f;
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
				hits++;
			else
				misses++;
			spriteRenderer.color.set_red(std::fmod(std::abs(transform.position[0]), 1.0f));
			spriteRenderer.color.set_green(std::fmod(std::abs(transform.position[1]), 1.0f));
			spriteRenderer.color.set_blue(std::fmod(std::abs(transform.position[2]), 1.0f));
		}
		void end() {
			PRINT_LN(append_to_string(hits, ", ", misses).c_str());
		}
};

class Imagy : public GameObject {
	public:
		Imagy() : GameObject(3, 1) {
			transform.position[0] = 0.8f;
			transform.position[1] = 0.8f;
			transform.position[2] = 0.8f;
			transform.scale[0] = 0.3f;
			transform.scale[1] = 0.3f;
		}
		~Imagy() {}
		void start() {
			/*const Texture hTexture = alloc_texture_loading_from_file("Image.png");
			const SpriteLayoutSettings spriteLayoutSettings;
			const SpriteLayout hSpriteLayout = create_sprite_layout(spriteLayoutSettings);
			spriteRenderer.hSprite = create_sprite(hTexture, hSpriteLayout);*/
		}
		void update() {
			transform.position[0] = pPlayy->transform.position[0];
			transform.position[1] = pPlayy->transform.position[1];
			spriteRenderer.textureCoordinates[0] = pPlayy->transform.position[0];
			spriteRenderer.textureCoordinates[1] = pPlayy->transform.position[1];
			spriteRenderer.textureOffset[0] = pPlayy->transform.position[0];
			spriteRenderer.textureOffset[1] = pPlayy->transform.position[1];
		}
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
			transform.position[2] = 0.5f;
			transform.scale[0] = 2.0f;
			transform.scale[1] = 2.0f;
			spriteRenderer.color.set_green(0.0f);
			spriteRenderer.color.set_blue(0.0f);
			spriteRenderer.color.set_alpha(0.4f);
		}
};

class PlayerCamera : public Camera {
	private:

	public:
		PlayerCamera() {
			transform.position[2] = -10.0f;
			view[0] = 1.33f;
			view[1] = view[0];
		}
		~PlayerCamera() {}
		void update_before_render() {
			if (pPlayy) {
				transform.position[0] = pPlayy->transform.position[0];
				transform.position[1] = pPlayy->transform.position[1];
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
