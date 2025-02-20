#include "RE.hpp"

using namespace RE;

class Nexie : public GameObject {
	public:
		Nexie() : GameObject(2, 0) {}
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

class Objy : public GameObject {
	public:
		Objy() : GameObject(1, 0) {}
		~Objy() {}
		void start(Scene* pStartingScene) {
			println("start objy");
		}
		void update(Scene* pCurrentScene) {
			println("update objy");
		}
		void end(Scene* pEndingScene) {
			println("end objy");
		}
};

class First : public Scene {
	public:
		Objy objy;

		First() : Scene(1) {}
		~First() {}
		void start() {}
		void update() {
			if (isKeyPressed(Keyboard::Space))
				setNextScene(second);
		}
		void end() {}
};

int main() {
	First first;
	second = new Second();
	setNextScene(&first);
	RE::execute();
	delete second;
	return 0;
}
