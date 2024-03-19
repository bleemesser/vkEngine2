#include "src/app.hpp"

int main() {

	App* vkApp = new App(1920, 1080);

	vkApp->run();

	delete vkApp;
	
	return 0;
}