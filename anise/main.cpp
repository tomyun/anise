#include "engine.h"

int main(int argc, char *argv[])
{
	Config *config = new Config();
	if (config->initialize(argc, argv) == false) {
		//TODO: process error
		exit(1);
	}

	Engine *engine = new Engine(config);
	engine->on();
	engine->run();
	engine->off();

	delete engine;
	delete config;

	return 0;
}