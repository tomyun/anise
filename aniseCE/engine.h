#ifndef ENGINE_H
#define ENGINE_H

#include "anise.h"
#include "memory.h"
#include "timer.h"
#include "input.h"
#include "sound.h"
#include "video.h"
#include "script.h"
#include "option.h"
#include "SDL.h"

#ifdef _WIN32
#include <windows.h>
#include "resource.h"
#include "SDL_syswm.h"
#include "SDL_version.h"
#endif

class Engine {
private:
	// connected objects
	Memory *memory;
	Timer *timer;
	Input *input;
	Sound *sound;
	Video *video;
	Script *script;
	Option *option;

public:
	// constructor & destructor
	Engine(Option *option);
	~Engine();
	
	// methods
	bool on();
	bool off();
	bool run();
};

#endif
