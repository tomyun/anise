#ifndef SOUND_H
#define SOUND_H

#include "anise.h"
#include "file.h"
#include "option.h"
#include "SDL.h"
#include "SDL_audio.h"

class Sound {
private:
	Option *option;

	string file_name;

	SDL_AudioSpec spec;
	Uint8 *buffer;
	Uint32 length;
	int current_length;

	bool is_effect;
	bool is_playing;

	static void callback(void *unused, Uint8 *stream, int stream_length);
	void mix(Uint8 *stream, int stream_length);

public:
	Sound(Option *option);
	~Sound();

	void load();
	void play();
	void stop();
};

#endif
