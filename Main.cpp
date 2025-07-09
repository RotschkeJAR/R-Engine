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
			transform.position[2] = -0.5f;
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
			transform.position[0] += (right.is_down() - left.is_down()) * 0.5f * get_deltaseconds();
			transform.position[1] += (up.is_down() - down.is_down()) * 0.5f * get_deltaseconds();
		}
		void end(Scene* pEndingScene) {
			PRINT_LN(append_to_string(hits, ", ", misses).c_str());
		}
};

class OC : public GameObject {
	public:
		OC() : GameObject(3, 1) {
			transform.position[0] = 2.0f;
			transform.position[1] = 2.0f;
			transform.position[2] = 1.8f;
			transform.scale[0] = 4.0f;
			transform.scale[1] = 4.0f;
			spriteRenderer.color.set_red(0.0f);
			spriteRenderer.color.set_green(0.0f);
		}
		~OC() {}
		void start(Scene *pStartingScene) {}
		void update(Scene *pCurrentScene) {}
		void end(Scene *pEndingScene) {}
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
		PlayerCamera() : zoomerIn(RE_INPUT_SCROLL_UP), zoomerOut(RE_INPUT_SCROLL_DOWN) {}
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
		InputAction trigger, refresh, camActivator;
		bool bCamActive;

		First() : Scene(1), bCamActive(true) {}
		~First() {}
		void start() {
			playerCam.view[0] = 1.0f;
			playerCam.view[1] = 1.0f;
			playerCam.activate();
			trigger.change_input(RE_INPUT_KEY_NUMPAD_ENTER);
			refresh.change_input(RE_INPUT_KEY_ESCAPE);
			camActivator.change_input(RE_INPUT_KEY_Q);
		}
		void update() {
			if (trigger.is_pressed())
				set_next_scene(second);
			else if (refresh.is_pressed())
				trigger.update_input();
			else if (camActivator.is_pressed()) {
				if (!bCamActive)
					playerCam.activate();
				else
					playerCam.deactivate();
				bCamActive = !bCamActive;
			}
			//PRINT_LN(get_fps_rate());
		}
		void end() {}
};

int main_func() {
	show_message_box_on_error(false);
	set_fps_limit(60U);
	SignalCatcher sigCatcher;
	reset_screen_size();
	First first;
	second = new Second();
	set_next_scene(&first);
	RE::execute();
	delete second;
	if (clonus)
		delete clonus;
	return 0;
}

#ifdef RE_OS_WINDOWS
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pwcCmdLine, int i32CmdShow) {
	set_hinstance(hInstance);
	return main_func();
}
#endif

int main() {
	return main_func();
}
