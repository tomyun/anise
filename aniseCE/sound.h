#ifndef SOUND_H
#define SOUND_H

#include "anise.h"
#include "file.h"
#include "option.h"

#ifdef _WIN32_WCE
	#include "WAVEMNG.H"
#else
	#include "SDL.h"
	#include "SDL_audio.h"
#endif

#include "ymf262.h"

class Sound {
private:
	Option *option;

	string file_name;

#ifndef _WIN32_WCE
	SDL_AudioSpec spec;

	Uint8 *buffer;
	Uint32 length;
	int current_length;
#endif

	bool is_playing;

	bool is_effect;

#ifndef _WIN32_WCE
	static void callback(void *unused, Uint8 *stream, int stream_length);
	void mix(Uint8 *stream, int stream_length);
#endif

public:
	Sound(Option *option);
	~Sound();

	void load();
	void play();
	void stop();
};

#endif
