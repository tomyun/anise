#ifndef INPUT_H
#define INPUT_H

#include "memory.h"
#include "timer.h"
#include "SDL_events.h"

//#define WAIT_DURATION	1		//TODO: deprecate this

#define CURSOR_SIZE		32
#define CURSOR_HEIGHT	CURSOR_SIZE
#define CURSOR_BPB		8
#define CURSOR_WIDTH	(CURSOR_HEIGHT / CURSOR_BPB)
#define CURSOR_FRAME	30		//ACK: original value was 10

enum InputType {
	INPUT_OK = 0x0010,
	INPUT_CANCEL = 0x0020,
	INPUT_UP = 0x0001,
	INPUT_DOWN = 0x0002,
	INPUT_LEFT = 0x0004,
	INPUT_RIGHT = 0x0008,
	INPUT_CTRL = 0x0040,
	INPUT_END = 0x0080,
	INPUT_SHIFT = 0x0100
};

enum CursorFrame {
	CURSOR_FIRSTFRAME = 0,
	CURSOR_SECONDFRAME = 1
};


class Input {
private:
	Memory *memory;
	Timer *timer;

	word keyboard_status;
	word mouse_status;

	bool is_quit;

	SDL_Cursor *cursor[2];
	byte cursor_image[2][CURSOR_WIDTH * CURSOR_HEIGHT];
	byte cursor_mask[2][CURSOR_WIDTH * CURSOR_HEIGHT];
	int current_cursor_frame;
	
public:
	// constructor & destructor
	Input(Memory *memory, Timer *timer);
	~Input();

	// methods
	bool refreshKeyboard();
	void refreshMouse(word *coord_x, word *coord_y);
	void refreshMouse();
	bool refresh();
	bool check(word type);

	bool isKeyPressed();
	bool isLeftClicked();
	bool isRightClicked();
	bool isCursorVisible();

	void showCursor();
	void hideCursor();
	void setCursorPosition(word coord_x, word coord_y);
	void loadCursorImage(word offset);
	void setCursorImage(int cursor_frame);
};

#endif
