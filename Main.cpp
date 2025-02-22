#include "RE.hpp"
#include <csignal>

#ifdef RE_OS_WINDOWS
# define NOGDI
# include <windows.h>
#endif

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
		Objy() : GameObject(0, 1) {}
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
			else if (isKeyPressed(Keyboard::Enter) && !clonus)
				clonus = new Clonus();
			else if (isKeyPressed(Keyboard::Backspace) && clonus)
				MARK_SAFE_DELETE(clonus);
			else if (isKeyPressed(Keyboard::Right_Shift))
				CATCH_SIGNAL(raise(SIGSEGV));
		}
		void end() {}
};

int main() {
	First first;
	second = new Second();
	setNextScene(&first);
	RE::execute();
	delete second;
	if (clonus)
		delete clonus;
	return 0;
}

#ifdef RE_OS_WINDOWS
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pwcCmdLine, int i32CmdShow) {
	return main();
}
#endif
