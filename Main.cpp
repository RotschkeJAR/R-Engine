#include "RE.hpp"
#include <csignal>

using namespace RE;

class Nexie : public GameObject {
	public:
		Nexie() : GameObject(0, 2) {}
		~Nexie() {}
		void start(Scene* pStartingScene) {
			println("start nexie");
		}
		void update(Scene* pCurrentScene) {
			println("update nexie");
		}
		void end(Scene* pEndingScene) {
			println("end nexie");
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
			println("start clonus");
		}
		void update(Scene* pCurrentScene) {
			println("update clonus");
		}
		void end(Scene* pEndingScene) {
			println("end clonus");
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
			println("start objy");
		}
		void update(Scene* pCurrentScene) {
			//println("update objy");
			bool randomResult = rng.random_bool(0.2f);
			if (randomResult)
				hits++;
			else
				misses++;
		}
		void end(Scene* pEndingScene) {
			println(hits, ", ", misses);
		}
};

class First : public Scene {
	public:
		Objy objy;

		First() : Scene(1) {}
		~First() {}
		void start() {}
		void update() {}
		void end() {}
};

int main() {
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
