#include "engine.h"

int main(int argc, char *argv[])
{
	Option *option = new Option();
	if (option->initialize(argc, argv) == false) {
		//TODO: process error
		exit(1);
	}

	Engine *engine = new Engine(option);
	engine->on();
	engine->run();
	engine->off();

	delete engine;
	delete option;

	return 0;
}