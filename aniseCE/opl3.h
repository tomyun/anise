#ifndef _OPL3_H_
#define _OPL3_H_

bool OPL3_load(const char *file);

void OPL3_play(void);
void OPL3_stop(void);

void OPL3_init(void);
void OPL3_destroy(void);

void OPL3_getpcm(short *pcm, unsigned int samples);

#endif // _OPL3_H_