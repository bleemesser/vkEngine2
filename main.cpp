#include "src/app.hpp"

int main() {

	App* vkApp = new App(800, 600);

	vkApp->run();

	delete vkApp;
	
	return 0;
}