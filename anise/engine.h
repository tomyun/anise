#ifndef ENGINE_H
#define ENGINE_H

#include "memory.h"
#include "video.h"
#include "sound.h"
#include "timer.h"
#include "input.h"
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
	Video *video;
	Sound *sound;
	Timer *timer;
	Input *input;
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
