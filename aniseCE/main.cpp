#include "engine.h"

#if defined(_WIN32_WCE)
//#ifdef DEBUG
static FILE *stdout_file;
static FILE *stderr_file;
#endif

#if defined(_WIN32)||defined(_WIN32_WCE)
int SDL_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
#if defined(_WIN32_WCE)
//	char *new_argv[] = { "anise.exe", "-p\\Storage Card\\Nanpa2", "-lK", "-f", "nanpa2" };
//	int new_argc = 5;
//	#define _ARG
//	char *new_argv[] = { "anise.exe", "-p=PDAGame\\ANISE\\Nanpa", "-s2", "-f", "nanpa1" };
	char *new_argv[] = { "anise.exe", "-p=PDAGame\\ANISE\\Nanpa2JK", "-s1", "-f", "nanpa2" };
	int new_argc = 5;
	#define _ARG
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

	Engine *engine = new Engine(option);
	engine->on();
	engine->run();
	engine->off();

	delete engine;
	delete option;

#if defined(_WIN32_WCE)
//#ifdef DEBUG
	fclose(stdout_file);
	fclose(stderr_file);
#endif

	return 0;
}
