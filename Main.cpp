#include "RE.hpp"

using namespace RE;

class Nexie : public GameObject {
	public:
		Nexie() : GameObject(0, 2) {}
		~Nexie() {}
		void start(Scene* pStartingScene) {
			PRINT_LN("start nexie");
		}
		void update(Scene* pCurrentScene) {}
		void end(Scene* pEndingScene) {
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
		void start(Scene* pStartingScene) {
			PRINT_LN("start clonus");
		}
		void update(Scene* pCurrentScene) {}
		void end(Scene* pEndingScene) {
			PRINT_LN("end clonus");
		}
};

Clonus* clonus = nullptr;

class Playy;
Playy *pPlayy = nullptr;

class Playy : public GameObject {
	public:
		RandomNumberGenerator rng;
		InputAction left, right, up, down;
		uint64_t hits, misses;

		Playy() : GameObject(1, 1), left(RE_INPUT_KEY_ARROW_LEFT), right(RE_INPUT_KEY_ARROW_RIGHT), up(RE_INPUT_KEY_ARROW_UP), down(RE_INPUT_KEY_ARROW_DOWN), hits(0), misses(0) {
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
		void start(Scene* pStartingScene) {
			PRINT_LN("start objy");
		}
		void update(Scene* pCurrentScene) {
			bool randomResult = rng.random_bool(0.2);
			if (randomResult)
				hits++;
			else
				misses++;
			transform.position[0] += (right.is_down() - left.is_down()) * 0.62f * get_deltaseconds();
			transform.position[1] += (up.is_down() - down.is_down()) * 0.62f * get_deltaseconds();
			spriteRenderer.color.set_red(std::fmodf(std::abs(transform.position[0]), 1.0f));
			spriteRenderer.color.set_green(std::fmodf(std::abs(transform.position[1]), 1.0f));
			spriteRenderer.color.set_blue(std::fmodf(std::abs(transform.position[2]), 1.0f));
		}
		void end(Scene* pEndingScene) {
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
		void start(Scene *pStartingScene) {
			const Texture hTexture = alloc_texture_loading_from_file("Image.png");
			const SpriteLayoutSettings spriteLayoutSettings;
			const SpriteLayout hSpriteLayout = create_sprite_layout(spriteLayoutSettings);
			spriteRenderer.hSprite = create_sprite(hTexture, hSpriteLayout);
		}
		void update(Scene *pCurrentScene) {
			transform.position[0] = pPlayy->transform.position[0];
			transform.position[1] = pPlayy->transform.position[1];
			spriteRenderer.textureCoordinates[0] = pPlayy->transform.position[0];
			spriteRenderer.textureCoordinates[1] = pPlayy->transform.position[1];
			spriteRenderer.textureOffset[0] = pPlayy->transform.position[0];
			spriteRenderer.textureOffset[1] = pPlayy->transform.position[1];
		}
		void end(Scene *pEndingScene) {
			const Texture hTexture = get_texture_from_sprite(spriteRenderer.hSprite);
			const SpriteLayout hSpriteLayout = get_sprite_layout_from_sprite(spriteRenderer.hSprite);
			destroy_sprite(spriteRenderer.hSprite);
			destroy_sprite_layout(hSpriteLayout);
			free_texture(hTexture);
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
		InputAction zoomerIn, zoomerOut;

	public:
		PlayerCamera() : zoomerIn(RE_INPUT_SCROLL_UP), zoomerOut(RE_INPUT_SCROLL_DOWN) {
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
			if (zoomerIn.is_down())
				view[0] -= get_deltaseconds();
			else if (zoomerOut.is_down())
				view[0] += get_deltaseconds();
		}
};

class First : public Scene {
	public:
		Playy objy;
		Imagy imagy;
		Background background;
		PlayerCamera playerCam;
		InputAction trigger;
		bool bCamActive, bMsaaEight;

		First() : Scene(1), trigger(RE_INPUT_KEY_NUMPAD_ENTER), bCamActive(true), bMsaaEight(false) {}
		~First() {}
		void start() {
			playerCam.activate();
			ERROR("test message\ntesty");
			WARNING("test message\ntesty");
			NOTE("test message\ntesty");
		}
		void update() {
			if (trigger.is_pressed())
				set_next_scene();
			else if (is_pressed(RE_INPUT_KEY_ESCAPE, 0))
				trigger.update_input();
			else if (is_pressed(RE_INPUT_KEY_Q, 0)) {
				if (!bCamActive)
					playerCam.activate();
				else
					playerCam.deactivate();
				bCamActive = !bCamActive;
			} else if (is_pressed(RE_INPUT_KEY_SPACE, 0)) {
				bMsaaEight = !bMsaaEight;
				if (bMsaaEight) {
					set_msaa_mode(RE_MSAA_MODE_8);
					const ScreenPercentageSettings constSized(Vector2u{50, 50});
					set_screen_percentage_settings(constSized);
				} else {
					set_msaa_mode(RE_MSAA_MODE_1);
					const ScreenPercentageSettings normal;
					set_screen_percentage_settings(normal);
				}
			}
			//PRINT_LN(get_fps_rate());
		}
		void end() {}
};

int main_func() {
	set_signal_handlers();
	set_fps_limit(60);
	{
		First first;
		Second secondInStack;
		second = &secondInStack;
		set_next_scene(&first);
		execute();
		if (clonus)
			delete clonus;
	}
	return 0;
}

#ifdef RE_OS_WINDOWS
int WINAPI wWinMain(const HINSTANCE win_hInstance, const HINSTANCE win_hPrevInstance, const PWSTR pawcCmdLine, const int i32CmdShow) {
	win64_set_hinstance(win_hInstance);
	return main_func();
}
#endif

int main() {
	return main_func();
}
