#include	"common.h"
//#include	<mmsystem.h>
#include	"wavemng.h"


#define	BUFFER_COUNT	4
#define	BUFFER_SAMPLES	(576*2)

#if !defined(_WIN32_WCE)
#pragma comment(lib, "winmm.lib")
#endif

typedef struct {
	HWAVEOUT	hwave;
	BYTE		*buffer;
	DWORD		samples;
	DWORD		bufalign;
	WAVEHDR		wh[BUFFER_COUNT];
} WAVEMNGT;

static	BOOL		waveopened = FALSE;
static	WAVEMNGT	w_ctrl;
static	BOOL		buffersent = FALSE;
static	DWORD		lastbuffer = 0;
static	DWORD		lasttick = 0;
static	DWORD		retry = 0;
static	UINT		curhz = 22050;
static	UINT		curch = 2;

static const DWORD capsfmt[] = {
			WAVE_FORMAT_1S16, WAVE_FORMAT_1M16,
			WAVE_FORMAT_2S16, WAVE_FORMAT_2M16,
			WAVE_FORMAT_4S16, WAVE_FORMAT_4M16};

extern DWORD _soundmix_getpcm(short *pcm, DWORD samples);

#if defined(WAVEMNG_CBMAIN)

// wparam -> Handle...
// lparam -> WAVEHDR

extern HWND hMainWnd;

void wavemng_cb(UINT msg, HWAVEOUT hwo, WAVEHDR *whd) {

	if ((msg == MM_WOM_DONE) && (whd)) {
		waveOutUnprepareHeader(hwo, whd, sizeof(WAVEHDR));
		if (whd->lpData) {
			_soundmix_getpcm((short *)whd->lpData, w_ctrl.samples);
			waveOutPrepareHeader(hwo, whd, sizeof(WAVEHDR));
			waveOutWrite(hwo, whd, sizeof(WAVEHDR));
			buffersent = TRUE;
		}
	}
}
#else
static void CALLBACK hwavecb(HWAVEOUT hwo, UINT uMsg,
						DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {

	if ((uMsg == WOM_DONE) && (dwParam1)) {
		waveOutUnprepareHeader(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
		if (((WAVEHDR *)dwParam1)->lpData) {
			_soundmix_getpcm((short *)((WAVEHDR *)dwParam1)->lpData,
															w_ctrl.samples);
			waveOutPrepareHeader(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
			waveOutWrite(hwo, (WAVEHDR *)dwParam1, sizeof(WAVEHDR));
			buffersent = TRUE;
		}
	}
}
#endif

int wavemng_getdelay(void)
{
	return (BUFFER_SAMPLES*BUFFER_COUNT)*1000/curhz;
}

BOOL wavemng_create(UINT hz, UINT channels) {

	int				devs;
	int				num;
	int				i;
	WAVEOUTCAPS		woc;
	WAVEFORMATEX	wfex;
	DWORD			type;

	if (waveopened) {
		return(FAILURE);
	}
	switch(hz) {
		case 11025:
			type = 0;
			break;
		case 44100:
			type = 4;
			break;
		default:
			type = 2;
			break;
	}
	if (channels != 2) {
		type++;
	}
	type = capsfmt[type];

	devs = waveOutGetNumDevs();
	for (num=0; num<devs; num++) {
		if (waveOutGetDevCaps(num, &woc, sizeof(woc)) == MMSYSERR_NOERROR) {
			if (woc.dwFormats & type) {
				break;
			}
		}
	}
	if ((num >= devs) && (type != WAVE_FORMAT_2S16)) {
		for (num=0; num<devs; num++) {
			if (waveOutGetDevCaps(num, &woc, sizeof(woc))
												== MMSYSERR_NOERROR) {
				if (woc.dwFormats & WAVE_FORMAT_2S16) {
					break;
				}
			}
		}
	}
	if (num < devs) {
		ZeroMemory(&w_ctrl, sizeof(w_ctrl));
		ZeroMemory(&wfex, sizeof(wfex));

		w_ctrl.samples = BUFFER_SAMPLES;
		w_ctrl.bufalign = w_ctrl.samples * 2 * channels;
		w_ctrl.buffer = (BYTE *)_MALLOC(w_ctrl.bufalign * BUFFER_COUNT,
																"wave mng");
		if (w_ctrl.buffer == NULL) {
			return(FAILURE);
		}
		ZeroMemory(w_ctrl.buffer, w_ctrl.bufalign * BUFFER_COUNT);
		for (i=0; i<BUFFER_COUNT; i++) {
			w_ctrl.wh[i].lpData = (char *)w_ctrl.buffer +
													(w_ctrl.bufalign * i);
			w_ctrl.wh[i].dwBufferLength = w_ctrl.bufalign;
			w_ctrl.wh[i].dwUser = i;
		}
		wfex.wFormatTag = WAVE_FORMAT_PCM;
		wfex.nSamplesPerSec = hz;
		wfex.wBitsPerSample = 16;
		wfex.nChannels = channels;
		wfex.nBlockAlign = wfex.nChannels * (wfex.wBitsPerSample / 8);
		wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
#if defined(WAVEMNG_CBMAIN)
		if (waveOutOpen(&w_ctrl.hwave, num, &wfex, (DWORD)hMainWnd, 0,
										CALLBACK_WINDOW) == MMSYSERR_NOERROR)
#else
		if (waveOutOpen(&w_ctrl.hwave, num, &wfex, (DWORD)hwavecb, 0,
									CALLBACK_FUNCTION) == MMSYSERR_NOERROR)
#endif
		{
			_HANDLE_ADD(w_ctrl.hwave, "WaveOut");
			for (i=0; i<BUFFER_COUNT; i++) {
				waveOutPrepareHeader(w_ctrl.hwave, w_ctrl.wh + i,
														sizeof(WAVEHDR));
				waveOutWrite(w_ctrl.hwave, w_ctrl.wh + i, sizeof(WAVEHDR));
			}
			curhz = hz;
			curch = channels;
			waveopened = TRUE;
			return(SUCCESS);
		}
		_MFREE(w_ctrl.buffer);
	}
	return(FAILURE);
}

void wavemng_destroy(void) {

	int		i;
	int		retry = 10;

	if (waveopened) {
		for (i=0; i<BUFFER_COUNT; i++) {
			waveOutUnprepareHeader(w_ctrl.hwave, w_ctrl.wh + i,
															sizeof(WAVEHDR));
			w_ctrl.wh[i].lpData = NULL;
		}
#if (defined(WIN32_PLATFORM_PSPC)) || (!defined(ARM))
		waveOutPause(w_ctrl.hwave);
		waveOutReset(w_ctrl.hwave);
#endif
		do {
			if (waveOutClose(w_ctrl.hwave) == MMSYSERR_NOERROR) {
				_HANDLE_REM(w_ctrl.hwave);
				break;
			}
			Sleep(500);
		} while(--retry);
		_MFREE(w_ctrl.buffer);
		waveopened = FALSE;
	}
}

void wavemng_reopen(void) {

	wavemng_destroy();
	wavemng_create(curhz, curch);
}

void wavemng_awake(void) {

	if (waveopened) {
		DWORD now = GetTickCount();
		if ((now - lasttick) >= 500) {
			lasttick = now;
			if (buffersent) {
				buffersent = FALSE;
			}
			else if (++retry >= 3) {
				wavemng_reopen();
				retry = 0;
			}
		}
	}
}

void wavemng_mastervolume(DWORD value)
{
	if (waveopened) {
		value &= 0xffff;
		value |= (value<<16);
		waveOutSetVolume(w_ctrl.hwave, value);
	}
}
