#include "sound.h"
#include "opl3.h"

#ifdef _WIN32_WCE
DWORD _soundmix_getpcm(short *pcm, DWORD samples) {
	OPL3_getpcm(pcm, samples);
	//samples = soundmix_getpcm(pcm, samples);
	return samples;
}
#endif

Sound::Sound(Option *option)
{
	this->option = option;

	is_effect = false;

	is_playing = false;

	OPL3_init();

#ifdef _WIN32_WCE

	wavemng_create(22050, 2);

#else
	buffer = NULL;
	length = 0;
	current_length = 0;

	if ((option->game_type == GAME_NANPA2) && (option->font_type == FONT_GAMEBOX)) {
		spec.format = AUDIO_U8;
	} else {
		spec.format = AUDIO_S16;
	}

	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.freq = 22050;
	spec.samples = 2048;

	spec.callback = callback;
	spec.userdata = this;

	if (SDL_OpenAudio(&spec, NULL) < 0) {
		//TODO: process error
		PRINT_ERROR("[Sound::Sound()] unable to open audio: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_PauseAudio(0);
#endif
}


Sound::~Sound()
{
	is_effect = true;stop();
	is_effect = false;stop();

	OPL3_destroy();

#ifdef _WIN32_WCE
	//wavemng_destroy();
#else
	if (buffer != NULL) {
		SDL_FreeWAV(buffer);
	}

	SDL_CloseAudio();
#endif
}

#ifndef _WIN32_WCE
void Sound::callback(void *pointer, Uint8 *stream, int stream_length)
{
	OPL3_getpcm((short *)stream, stream_length >> 2);
	//((Sound*)pointer)->mix(stream, stream_length);
}

void Sound::mix(Uint8 *stream, int stream_length)
{
	if(true){
		OPL3_getpcm((short *)stream, stream_length >> 2);
	} else {
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
}
#endif

void Sound::load()
{
#ifndef _WIN32_WCE
/*
	if (!(option->sound_file_extension == WAV_FILE_EXTENSION)) {
		PRINT("Not wav File\n");
		if (option->sound_file_extension == M_FILE_EXTENSION) {
			if (((option->game_type == GAME_NANPA2) && (option->font_type == FONT_GAMEBOX)) == false) {
				return;
			}
		}
		else {
			return;
		}
	}
*/
	if (buffer != NULL) {
		SDL_FreeWAV(buffer);
		buffer = NULL;
	}
#endif

	if (file_name != option->sound_file_name) {
		file_name = option->sound_file_name; // + option->sound_file_extension;

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

#ifndef _WIN32_WCE
		/*
		SDL_AudioSpec dummy_spec;
		if (SDL_LoadWAV(file_name.c_str(), &dummy_spec, &buffer, &length) == NULL) {
			PRINT_ERROR("[Sound::load()] unable to load wave file(%s): %s\n", file_name.c_str(), SDL_GetError());
		}
		*/
#endif
	}

	//char file[MAX_PATH];
	//MMAPPEDSTREAMARG arg;
	FILE* fp;
	int result = 0;

	stop();

	//sprintf(file, "%s.M", file_name.c_str());
	string file = file_name + ".M";
	PRINT("Check Sound File %s\n", file.c_str());
	fp = fopen(file.c_str(), "rb");

	if ( fp ) {
		fclose(fp);
		result = 1;
		PRINT("Load Sound File %s\n", file.c_str());
		OPL3_load(file.c_str());
	}
/*
	//sprintf(file, "%s%s.MP3", option->path_name, option->sound_file_name.substr(option->path_name.size()));
	sprintf(file, "%s.MP3", file_name.c_str());
	PRINT("Check Sound File %s\n", file);
#ifdef _WIN32_WCE
	TCHAR wfile[MAX_PATH];
	static TCHAR ofile[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, file, -1, ofile, MAX_PATH);
	wcscpy(wfile, base_path);
	wcscat(wfile, ofile);

	fp = _wfopen(wfile, _T("rb"));
#else
	fp = fopen(file, "rb");
#endif
	if ( fp ) {
		fclose(fp);
		result = 1;
	}
	if ( !result ) {
		sprintf(file, "%s.WAV", file_name.c_str());
		PRINT("Chck Sound File %s\n", file);
#ifdef _WIN32_WCE
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, file, -1, ofile, MAX_PATH);
		wcscpy(wfile, base_path);
		wcscat(wfile, ofile);

		fp = _wfopen(wfile, _T("rb"));
#else
		fp = fopen(file, "rb");
#endif
		if ( fp ) {
			fclose(fp);
			result = 1;
		}
	}
	if ( result ) {
		if ( !is_effect ) {
			arg.type = MMMAPPEDSTREAM_NEWMAP;
		} else {
			arg.type = MMMAPPEDSTREAM_ONMEMORY;
		}
#ifdef _WIN32_WCE
		arg.fname = wfile;
#else
		arg.fname = file;
#endif
		PRINT("Load Sound File %s\n", file);
		fflush(0);

		if(is_effect)
			soundmix_load(SOUNDTRK_SE, &mmapped_stream, &arg);
		else
			soundmix_load(SOUNDTRK_SOUND, &mmapped_stream, &arg);
		//soundmix_play(ch, loop, delay);
	}

	//return result;
	//current_length = 0;
*/
}


void Sound::play()
{
	OPL3_play();
/*
	if(is_effect)
		soundmix_play(SOUNDTRK_SE, 0, 0);
	else
		soundmix_play(SOUNDTRK_SOUND, 1, 0);
*/
/*
	if (buffer) {
		is_playing = true;

//		SDL_PauseAudio(0);
	}
*/
}


void Sound::stop()
{
	OPL3_stop();
/*
	if(is_effect)
		soundmix_stop(SOUNDTRK_SE, 0);
	else
		soundmix_stop(SOUNDTRK_SOUND, 0);
*/
/*
	if (buffer && is_playing) {
//		SDL_PauseAudio(1);

		is_playing = false;
	}
*/
}
