#include "sound.h"

Sound::Sound(Option *option)
{
	this->option = option;

	buffer = NULL;
	length = 0;
	current_length = 0;

	is_effect = false;
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
		PRINT_ERROR("[Sound::Sound()] unable to open audio: %s\n", SDL_GetError());
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

		if (!is_effect) {
			current_buffer = buffer;
			remain_length = length;

			current_length = 0;
		}
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

	if (file_name != option->sound_file_name) {
		file_name = option->sound_file_name;

		//HACK: check music file whether it is an effect file that should be played just one time
		string raw_name = file_name.substr(option->path_name.size());
		if ((raw_name == "chime.") ||
			(raw_name == "cockoo.m") ||
			(raw_name == "damage.m") ||
			(raw_name == "elf.m") ||
			(raw_name == "gcrash3.m") ||
			(raw_name == "halley.m") ||
			(raw_name == "se7_1.m") ||
			(raw_name == "se7_2.m") ||
			(raw_name == "se11.m") ||
			(raw_name == "shot_me.m") ||
			(raw_name == "spoon.m") ||
			(raw_name == "tennis.m")) {
				is_effect = true;
			}
		else {
			is_effect = false;
		}

		if (SDL_LoadWAV(option->sound_file_name.c_str(), &dummy_spec, &buffer, &length) == NULL) {
			//TODO: process error
			//PRINT_ERROR("[Sound::load()] unable to load wave file(%s): %s\n", option->sound_file_name, SDL_GetError());
			//exit(1);
		}
	}

	current_length = 0;
}


void Sound::play()
{
	if (buffer) {
		is_playing = true;

		SDL_PauseAudio(0);
	}
}


void Sound::stop()
{
	if (buffer && is_playing) {
		SDL_PauseAudio(1);

		is_playing = false;
	}
}