// #define	WAVEMNG_CBMAIN
//#include "compiler.h"

#include <windows.h>

BOOL wavemng_create(UINT hz, UINT channels);
void wavemng_destroy(void);
void wavemng_cb(UINT msg, HWAVEOUT hwo, WAVEHDR *whd);

void wavemng_reopen(void);
void wavemng_awake(void);
int wavemng_getdelay(void);
void wavemng_mastervolume(DWORD value);

