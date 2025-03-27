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
		REulong hits, misses;

		Objy() : GameObject(0, 1), hits(0UL), misses(0UL) {}
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
		}
		void end(Scene* pEndingScene) {
			PRINT_LN(append_to_string(hits, ", ", misses).c_str());
		}
};

class First : public Scene {
	public:
		Objy objy;
		InputAction trigger, refresh;

		First() : Scene(1) {}
		~First() {}
		void start() {
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

int main() {
	show_message_box_on_error(false);
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
	return main();
}
#endif
