#ifndef MFILE_H
#define MFILE_H

#include "file.h"
#include "option.h"
#include "memory_block.h"

#include "SDL.h"

class MFile {
private:
	File *file;
	Option *option;
	MemoryBlock *memory;

	struct ChannelInfo{
		unsigned char status;
		unsigned short filePos;
		unsigned char delay;
		unsigned char keepCnt;
		unsigned char note;
		unsigned char keep;
		unsigned char spkch;
		unsigned char volume;
		unsigned char subVol[4];
		unsigned char byte[100];
		unsigned char port;
		unsigned char op;
		short int word[100];
	} chInfo[6];

	unsigned char *Music;
	unsigned char *MusicPreset;
	int mType;

	int ch;
	unsigned char regBD;
	unsigned char baseNote;
	unsigned char playStatus;
	int tempo;
	bool active;
	int TempoBase;

	void OutPort(unsigned char reg, unsigned char val);
	void OutPortOp(unsigned char reg, unsigned char val);

	unsigned char getByte(void);
	char getChar(void);
	unsigned short getWord(void);
	short getInt16(void);

	void initChip(void);
	void initChannel(void);
	void setPreset(int id);
	void setVolume(void);
	void setSpeaker(unsigned char spk);
	void setDepth(unsigned char depth);
	void OpCode(unsigned char op);
	void NoteToFreq(void);
	void setupMusic(void);
	void NoteOn(void);
	void NoteOnSub(void);
	void NoteOff(void);
	void PlaySound(void);

	bool getbuf(void);
	static bool callback(void *pointer);

public:
	MFile(File *file, Option *option);
	~MFile();

	bool load(const char *file);
	void play(void);
	void stop(void);

	void getpcm(short *pcm, unsigned int samples);
};

#endif // MFILE_H
