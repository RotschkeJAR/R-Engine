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

class Objy;
Objy *pObjy = nullptr;

class Objy : public GameObject {
	public:
		RandomNumberGenerator rng;
		InputAction left, right, up, down;
		uint64_t hits, misses;

		Objy() : GameObject(1, 1), left(RE_INPUT_KEY_ARROW_LEFT), right(RE_INPUT_KEY_ARROW_RIGHT), up(RE_INPUT_KEY_ARROW_UP), down(RE_INPUT_KEY_ARROW_DOWN), hits(0UL), misses(0UL) {
			pObjy = this;
			transform.position[2] = 4.5f;
			transform.scale[0] = 0.1f;
			transform.scale[1] = 0.1f;
		}
		~Objy() {
			pObjy = nullptr;
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
		}
		void end(Scene* pEndingScene) {
			PRINT_LN(append_to_string(hits, ", ", misses).c_str());
		}
};

class OC : public GameObject {
	public:
		OC() : GameObject(3, 1) {
			transform.position[0] = 0.0f;
			transform.position[1] = 0.0f;
			transform.position[2] = 0.0f;
			transform.scale[0] = 2.0f;
			transform.scale[1] = 2.0f;
		}
		~OC() {}
		void start(Scene *pStartingScene) {
			spriteRenderer.sprite.hTexture = alloc_texture_loading_from_file("Image.png");
			spriteRenderer.sprite.hSpriteLayout = create_sprite_layout();
		}
		void update(Scene *pCurrentScene) {
			transform.position[0] = pObjy->transform.position[0];
			transform.position[1] = pObjy->transform.position[1];
			spriteRenderer.textureCoordinates[0] = pObjy->transform.position[0];
			spriteRenderer.textureCoordinates[1] = pObjy->transform.position[1];
			spriteRenderer.textureOffset[0] = pObjy->transform.position[0];
			spriteRenderer.textureOffset[1] = pObjy->transform.position[1];
		}
		void end(Scene *pEndingScene) {
			free_texture(spriteRenderer.sprite.hTexture);
			destroy_sprite_layout(spriteRenderer.sprite.hSpriteLayout);
		}
};

class Background : public GameObject {
	public:
		Background() : GameObject(2, 1) {
			transform.position[2] = 0.5f;
			transform.scale[0] = 1.0f;
			transform.scale[1] = 1.0f;
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
			view[0] = 1.33f;
			view[1] = view[0];
		}
		~PlayerCamera() {}
		void update() {
			if (pObjy) {
				position[0] = pObjy->transform.position[0];
				position[1] = pObjy->transform.position[1];
			}
			if (zoomerIn.is_down())
				view[0] -= get_deltaseconds();
			else if (zoomerOut.is_down())
				view[0] += get_deltaseconds();
		}
};

class First : public Scene {
	public:
		Background background;
		Objy objy;
		OC another;
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
				set_msaa_mode(bMsaaEight ? RE_MSAA_MODE_8 : RE_MSAA_MODE_1);
				if (bMsaaEight) {
					const ScreenPercentageSettings constSized(Vector2u{50, 50});
					set_screen_percentage_settings(constSized);
				} else {
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
	show_message_box_on_error(false);
	if (init_engine()) {
		set_fps_limit(60);
		First first;
		Second secondInStack;
		second = &secondInStack;
		set_next_scene(&first);
		start_game_loop();
		if (clonus)
			delete clonus;
	}
	return 0;
}

#ifdef RE_OS_WINDOWS
int WINAPI wWinMain(HINSTANCE win_hInstance, HINSTANCE win_hPrevInstance, PWSTR pawcCmdLine, int i32CmdShow) {
	win64_set_hinstance(win_hInstance);
	return main_func();
}
#endif

int main() {
	return main_func();
}
