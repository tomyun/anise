#include "engine.h"

#if defined(_WIN32_WCE)
//#ifdef DEBUG
static FILE *stdout_file;
static FILE *stderr_file;
#endif

int SDL_main(int argc, char *argv[])
{
#if defined(_WIN32_WCE)
//	char *new_argv[] = { "anise.exe", "-p\\Storage Card\\Nanpa2", "-lK", "-f", "nanpa2" };
//	int new_argc = 5;
//	#define _ARG
#elif defined(_WIN32)
//	char *new_argv[] = { "anise.exe", "-pC:\\Games\\Nanpa2", "-lK", "nanpa2" };
//	int new_argc = 4;
//	#define _ARG
#endif

#if defined(_WIN32_WCE)
//#ifdef DEBUG
	stdout_file = fopen("\\anise_stdout.txt", "w");
	stderr_file = fopen("\\anise_stderr.txt", "w");
#endif

	PRINT("ANISE Project On\n");

	Option *option = new Option();
#ifdef _ARG
	if (argc == 1) {
		if (option->initialize(new_argc, new_argv) == false) {
			//TODO: process error
			exit(1);
		}
	} else {
		if (option->initialize(argc, argv) == false) {
			//TODO: process error
			exit(1);
		}
	}
#else
	if (option->initialize(argc, argv) == false) {
		//TODO: process error
		exit(1);
	}
#endif

	PRINT("Generation Engine\n");
	Engine *engine = new Engine(option);
	PRINT("Engine On\n");
	engine->on();
	PRINT("Engine Run\n");
	engine->run();
	PRINT("Engine Off\n");
	engine->off();

	delete engine;
	delete option;

	PRINT("ANISE Terminated.\n");

#if defined(_WIN32_WCE)
//#ifdef DEBUG
	fclose(stdout_file);
	fclose(stderr_file);
#endif

	return 0;
}
