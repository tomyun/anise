#include "sound.h"

Sound::Sound(Option *option)
{
	this->option = option;

	buffer = NULL;
	length = 0;
	current_length = 0;

	is_effect = false;
	is_playing = false;

	if ((option->game_type == GAME_NANPA2) && (option->font_type == FONT_GAMEBOX)) {
		spec.format = AUDIO_U8;
	}
	else {
		spec.format = AUDIO_S16;
	}

	spec.channels = 1;
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
	stop();

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
	if (option->sound_file_extension != WAV_FILE_EXTENSION) {
		if (option->sound_file_extension == M_FILE_EXTENSION) {
			if (((option->game_type == GAME_NANPA2) && (option->font_type == FONT_GAMEBOX)) == false) {
				return;
			}
		}
		else {
			return;
		}
	}

	if (buffer != NULL) {
		SDL_FreeWAV(buffer);
		buffer = NULL;
	}

	if (file_name != option->sound_file_name) {
		file_name = option->sound_file_name + option->sound_file_extension;

		//HACK: check music file whether it is an effect file that should be played just one time
		is_effect = false;
		string raw_name = option->sound_file_name.substr(option->path_name.size());
		if (option->game_type == GAME_NANPA2) {
			if ((raw_name == "chime") ||
				(raw_name == "cockoo") ||
				(raw_name == "damage") ||
				(raw_name == "elf") ||
				(raw_name == "gcrash3") ||
				(raw_name == "halley") ||
				(raw_name == "se7_1") ||
				(raw_name == "se7_2") ||
				(raw_name == "se11") ||
				(raw_name == "shot_me") ||
				(raw_name == "spoon") ||
				(raw_name == "tennis")) {
					is_effect = true;
				}
		}
		else if (option->game_type == GAME_NANPA1) {
			if ((raw_name == "punch") ||
				(raw_name == "chaime") ||
				(raw_name == "cockoo") ||
				(raw_name == "effe3") ||
				(raw_name == "effe4") ||
				(raw_name == "jo_se") ||
				(raw_name == "window")) {
					is_effect = true;
				}
		}

		SDL_AudioSpec dummy_spec;
		if (SDL_LoadWAV(file_name.c_str(), &dummy_spec, &buffer, &length) == NULL) {
			PRINT_ERROR("[Sound::load()] unable to load wave file(%s): %s\n", file_name.c_str(), SDL_GetError());
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
