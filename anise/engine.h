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