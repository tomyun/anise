#include "sound.h"

Sound::Sound(Config *config)
{
	this->config = config;

	buffer = NULL;
	length = 0;
	current_length = 0;

	is_loaded = false;
	is_playing = false;

	spec.channels = 1;
	spec.format = AUDIO_U8;
	spec.freq = 22050;
	spec.padding = 0;
	spec.samples = 4096;
	spec.silence = 0;
	spec.size = 0;

	spec.callback = callback;
	spec.userdata = this;

	if (SDL_OpenAudio(&spec, NULL) < 0) {
		//TODO: process error
		ERROR("[Sound::Sound()] unable to open audio: %s\n", SDL_GetError());
		exit(1);
	}
}


Sound::~Sound()
{
	if (buffer != NULL) {
		SDL_FreeWAV(buffer);
	}
	SDL_CloseAudio();
}


void Sound::callback(void *pointer, Uint8 *stream, int stream_length)
{
	((Sound*)pointer)->mix(stream, stream_length);
}


void Sound::mix(Uint8 *stream, int stream_length)
{
	Uint8 *current_buffer = buffer + current_length;
	int remain_length = length - current_length;

	while (remain_length <= stream_length) {
		SDL_MixAudio(stream, current_buffer, remain_length, SDL_MIX_MAXVOLUME);

		stream += remain_length;
		stream_length -= remain_length;

		current_buffer = buffer;
		remain_length = length;

		current_length = 0;
	}

	SDL_MixAudio(stream, current_buffer, stream_length, SDL_MIX_MAXVOLUME);

	current_length += stream_length;
}


void Sound::load()
{
	if (buffer != NULL) {
		SDL_FreeWAV(buffer);
		buffer = NULL;
	}

	SDL_AudioSpec dummy_spec;

	if (SDL_LoadWAV(config->sound_file_name.data(), &dummy_spec, &buffer, &length) == NULL) {
		//TODO: process error
		//ERROR("[Sound::load()] unable to load wave file(%s): %s\n", config->sound_file_name, SDL_GetError());
		//exit(1);

		is_loaded = false;
	}
	else {
		is_loaded = true;
	}

	current_length = 0;
}


void Sound::play()
{
	if (is_loaded) {
		is_playing = true;

		SDL_PauseAudio(0);
	}
}


void Sound::stop()
{
	if (is_loaded) {
		SDL_PauseAudio(1);

		is_playing = false;
	}
}