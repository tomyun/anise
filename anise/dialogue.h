#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <string>
#include "memory.h"
#include "video.h"
#include "timer.h"
#include "input.h"
#include "animation.h"
#include "script_code.h"

using std::string;

#define FONT_HALF_WIDTH			8
#define FONT_HALF_HEIGHT		16
#define FONT_FULL_WIDTH			16
#define FONT_FULL_HEIGHT		16
#define FONT_MARGIN				2

#define FONT_FACTOR				0x20
#define FONT_PREDEFINED_FACTOR	0x80

#define WAIT_DURATION	1

class Dialogue {
private:
	Memory *memory;
	Video *video;
	Timer *timer;
	Input *input;
	Animation *animation;
	Config *config;

	byte *font;
	long int size;

	byte code_newline_first;
	byte code_newline_second;

	word previous_code;

public:
	Dialogue(Memory *memory, Video *video, Timer *timer,  Input *input, Animation *animation, Config *config);
	~Dialogue();

	bool initialize();
	void putHalfWidthCharacters();
	void putStandardText();
	void putPredefinedText();
	void putFullWidthCharacter(byte first_code, byte second_code);
	void updatePosition();
	void breakNewLine();
	void setPosition(word coord_xb, word coord_y);
	void setPosition();
	void getPosition(word *coord_xb, word *coord_y);

	void delay(word duration);
	void wait();
};

#endif