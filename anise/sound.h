#ifndef SOUND_H
#define SOUND_H

#include "config.h"
#include "SDL.h"
#include "SDL_audio.h"

class Sound {
private:
	Config *config;

	SDL_AudioSpec spec;
	Uint8 *buffer;
	Uint32 length;
	int current_length;

	bool is_loaded;
	bool is_playing;

	static void callback(void *unused, Uint8 *stream, int stream_length);
	void mix(Uint8 *stream, int stream_length);

public:
	Sound(Config *config);
	~Sound();

	void load();
	void play();
	void stop();
};
#endif