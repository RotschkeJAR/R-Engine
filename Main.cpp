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

class Objy : public GameObject {
	public:
		RandomNumberGenerator rng;
		InputAction left, right, up, down;
		uint64_t hits, misses;

		Objy() : GameObject(1, 1), left(RE_INPUT_KEY_ARROW_LEFT), right(RE_INPUT_KEY_ARROW_RIGHT), up(RE_INPUT_KEY_ARROW_UP), down(RE_INPUT_KEY_ARROW_DOWN), hits(0UL), misses(0UL) {
			transform.scale[0] = 0.1f;
			transform.scale[1] = 0.1f;
		}
		~Objy() {}
		void start(Scene* pStartingScene) {
			PRINT_LN("start objy");
		}
		void update(Scene* pCurrentScene) {
			bool randomResult = rng.random_bool(0.2f);
			if (randomResult)
				hits++;
			else
				misses++;
			transform.position[0] += (right.is_down() - left.is_down()) * 0.1f * get_deltaseconds();
			transform.position[1] += (up.is_down() - down.is_down()) * 0.1f * get_deltaseconds();
			//PRINT_LN(get_fps_rate());
		}
		void end(Scene* pEndingScene) {
			PRINT_LN(append_to_string(hits, ", ", misses).c_str());
		}
};

class Background : public GameObject {
	public:
		Background() : GameObject(2, 1) {
			transform.scale[0] = 2.0f;
			transform.scale[1] = 2.0f;
			spriteRenderer.color.set_channel(1U, 0.0f);
			spriteRenderer.color.set_channel(2U, 0.0f);
		}
};

class PlayerCamera : public Camera {
	public:
		PlayerCamera() {
			scale[0] = 400.0f;
			scale[1] = 300.0f;
		}
		~PlayerCamera() {}
		void update() {}
};

class First : public Scene {
	public:
		Background background;
		Objy objy;
		PlayerCamera playerCam;
		InputAction trigger, refresh;

		First() : Scene(1) {}
		~First() {}
		void start() {
			playerCam.activate();
			trigger.change_input(RE_INPUT_KEY_NUMPAD_ENTER);
			refresh.change_input(RE_INPUT_KEY_ESCAPE);
		}
		void update() {
			if (trigger.is_pressed())
				set_next_scene(second);
			else if (refresh.is_pressed())
				trigger.update_input();
		}
		void end() {}
};

int main_func() {
	show_message_box_on_error(false);
	bind_fps_to_vsync(true);
	SignalCatcher sigCatcher;
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
