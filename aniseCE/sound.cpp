#include "sound.h"
#include "opl3.h"

enum {
	NONE_FILE = 0,
	WAV_FILE = 1,
	M_FILE = 2
};


Sound::Sound(Option *option)
{
	this->option = option;

	is_effect = false;

	is_playing = false;

	buffer = NULL;
	length = 0;
	current_length = 0;
	song_type = NONE_FILE;

#ifdef _WIN32_WCE
	int				i;
	WAVEFORMATEX	wfex;

	int samples = 2048 * int(option->sound_freq / 11025);
	wave_buf = (BYTE *)malloc(samples * 4);
	if (wave_buf == NULL) {
		//TODO: process error
		PRINT_ERROR("[Sound::Sound()] unable to malloc sound buffer\n");
		exit(1);
	}

	ZeroMemory(wave_buf, samples * 4);
	for (i=0; i<4; i++) {
		ZeroMemory(wh + i, sizeof(WAVEHDR));
		wh[i].lpData = (char *)wave_buf + (samples * i);
		wh[i].dwBufferLength = samples;
		wh[i].dwUser = i;
	}

	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nSamplesPerSec = option->sound_freq;
	wfex.wBitsPerSample = 16;
	wfex.nChannels = 2;
	wfex.nBlockAlign = wfex.nChannels * (wfex.wBitsPerSample / 8);
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
	wfex.cbSize = 0;
	hwave = 0;
	if (waveOutOpen(&hwave, WAVE_MAPPER, &wfex, (DWORD)callback, (DWORD)this, CALLBACK_FUNCTION) == MMSYSERR_NOERROR)
	{
		for (i=0; i<4; i++) {
			waveOutPrepareHeader(hwave, wh + i, sizeof(WAVEHDR));
			waveOutWrite(hwave, wh + i, sizeof(WAVEHDR));
		}
	} else {
		free(wave_buf);
		PRINT_ERROR("[Sound::Sound()] unable to open audio: %s\n", SDL_GetError());
		exit(1);
	}
#else
	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.freq = option->sound_freq;
	spec.samples = 1024 * int(option->sound_freq / 11025);

	spec.callback = callback;
	spec.userdata = this;

	if (SDL_OpenAudio(&spec, NULL) < 0) {
		//TODO: process error
		PRINT_ERROR("[Sound::Sound()] unable to open audio: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_PauseAudio(0);
#endif

	OPL3_init(option->sound_freq);
}


Sound::~Sound()
{
	stop();

	if(buffer)
		free(buffer);
	OPL3_destroy();

#ifdef _WIN32_WCE
	int		i;
	int		retry = 10;

	for (i=0; i<4; i++) {
		waveOutUnprepareHeader(hwave, wh + i, sizeof(WAVEHDR));
		wh[i].lpData = NULL;
	}

	//waveOutPause(hwave);
	//waveOutReset(hwave);

	do {
		if (waveOutClose(hwave) == MMSYSERR_NOERROR) {
			break;
		}
		Sleep(500);
	} while(--retry);
	free(wave_buf);
#else
	SDL_CloseAudio();
#endif
}

#ifdef _WIN32_WCE
void CALLBACK Sound::callback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	if ((uMsg == WOM_DONE) && (dwParam1)) {
		waveOutUnprepareHeader(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
		if (((WAVEHDR *)dwParam1)->lpData) {
			((Sound*)dwInstance)->mix((Uint8 *)((WAVEHDR *)dwParam1)->lpData, ((WAVEHDR *)dwParam1)->dwBufferLength);
			waveOutPrepareHeader(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
			waveOutWrite(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
		}
	}
}
#else
void Sound::callback(void *pointer, Uint8 *stream, int stream_length)
{
	((Sound*)pointer)->mix(stream, stream_length);
}
#endif

void Sound::mix(Uint8 *stream, int stream_length)
{
	switch(is_playing ? song_type : NONE_FILE) {
	case WAV_FILE:{
		Uint8 *current_buffer = buffer + current_length;
		int remain_length = length - current_length;

		while (remain_length <= stream_length) {
			memcpy(stream, current_buffer, remain_length);

			stream += remain_length;
			stream_length -= remain_length;
			current_length = 0;

			if (is_effect) {
				memset(stream, 0, stream_length);

				stream += stream_length;
				stream_length = 0;

				song_type = NONE_FILE;
				break;
			} else {
				current_buffer = buffer;
				remain_length = length;
			}

		}

		memcpy(stream, current_buffer, stream_length);

		current_length += stream_length;
		break;
		}
	case M_FILE:
		OPL3_getpcm((short *)stream, stream_length >> 2);
		break;
	case NONE_FILE:
		memset(stream, 0, stream_length);
		break;
	}
}

void Sound::load()
{
	stop();
	song_type = NONE_FILE;

	if (buffer != NULL) {
		free(buffer);
		buffer = NULL;
	}

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
	}

	string file = file_name + ".WAV";
	FILE* fp = fopen(file.c_str(), "rb");

	if(!fp){
		file = file_name + ".M";
		fp = fopen(file.c_str(), "rb");
	}

	if(fp){
		char header[0x10];
		fread(header, 1, sizeof(header), fp);
		fclose(fp);

		if((strncmp(header, "RIFF", 4) == 0)&&(strncmp(&header[8], "WAVEfmt", 7) == 0)){
			song_type = WAV_FILE;
			PRINT("Load Wave File %s\n", file.c_str());
		} else if((strncmp(header, "MUSIC DRV", 9) == 0)||(strncmp(header, "OPL3 DATA", 9) == 0)){
			song_type = M_FILE;
			PRINT("Load M File %s\n", file.c_str());
		}
	}

	switch(song_type){
	case WAV_FILE:{
		SDL_AudioSpec wave;
		Uint8 *data;
		Uint32 dlen;
		SDL_AudioCVT cvt;

		if (SDL_LoadWAV(file.c_str(), &wave, &data, &dlen) == NULL) {
			PRINT_ERROR("[Sound::load()] unable to load wave file(%s): %s\n", file.c_str(), SDL_GetError());
			song_type = NONE_FILE;
		}
		SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq,
								AUDIO_S16,   2,             option->sound_freq);
		cvt.buf = (Uint8 *)malloc(dlen*cvt.len_mult);
		if(!cvt.buf){
			PRINT_ERROR("[Sound::Sound()] unable to malloc wave buffer\n");
			SDL_FreeWAV(data);
			exit(1);
		}
		cvt.len = dlen;
		memcpy(cvt.buf, data, dlen);
		SDL_FreeWAV(data);
		SDL_ConvertAudio(&cvt);

		if(!cvt.buf){
			song_type = NONE_FILE;
		} else {
			buffer = cvt.buf;
			length = cvt.len_cvt;
			current_length = 0;
		}
		break;
		}
	case M_FILE:
		OPL3_load(file.c_str());
		break;
	}
}


void Sound::play()
{
	switch(song_type){
	case WAV_FILE:
		if (buffer) {
			is_playing = true;
		}
		break;
	case M_FILE:
		OPL3_play();
		is_playing = true;
		break;
	case NONE_FILE:
		stop();
		break;
	}

	if(is_playing) {
#ifdef _WIN32_WCE
		waveOutRestart(hwave);
#else
		SDL_PauseAudio(0);
#endif
	}
}


void Sound::stop()
{
	if(is_playing){
		switch(song_type){
		case WAV_FILE:
			break;
		case M_FILE:
			OPL3_stop();
			break;
		case NONE_FILE:
			break;
		}

#ifdef _WIN32_WCE
		//waveOutReset(hwave);
#else
		SDL_PauseAudio(1);
#endif

		is_playing = false;
	}
}
