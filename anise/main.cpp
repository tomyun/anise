#include "engine.h"

#if defined(_WIN32_WCE) && (_WIN32_WCE <= 300)
static FILE *stdout_file;
static FILE *stderr_file;
#endif

#if defined(_WIN32)||defined(_WIN32_WCE)
int SDL_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
#if defined(_WIN32_WCE) && (_WIN32_WCE <= 300)
	stdout_file = fopen("\\anise_stdout.txt", "w");
	stderr_file = fopen("\\anise_stderr.txt", "w");
#endif

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

#if defined(_WIN32_WCE) && (_WIN32_WCE <= 300)
	fclose(stdout_file);
	fclose(stderr_file);
#endif

	return 0;
}
