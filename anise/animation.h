#ifndef ANIMATION_H
#define ANIMATION_H

#include "anise.h"
#include "memory.h"
#include "timer.h"
#include "input.h"
#include "video.h"

#define SLOTS		10
#define SLOT_SIZE	16
//HACK: for Kakyusei
//#define SLOT_SIZE	18
#define FRAME_RATE	10
#define FRAME_DELAY	2
#define OPERATION_SIZE	33

enum AnimationSetSlotFunction {
	SLOT_INITIALIZE = 0,
	SLOT_ACTIVATE = 1,
	SLOT_FINALIZE = 2,
	SLOT_DEACTIVATE = 3,
	SLOT_PLAY = 4,
	SLOT_SETDISPLAYBUFFER = 5,
	SLOT_FINALIZEALL = 6,
	SLOT_CLEAR = 7
};


enum AnimationSlotStatus {
	STATUS_DEACTIVATED = 0,
	STATUS_1 = 1,
	STATUS_ACTIVATED = 0xFF
};


enum AnimationScriptOpcode {
	CODE_ANIMATION_01 = 0x01,
	CODE_ANIMATION_SETDELAY = 0x02,
	CODE_ANIMATION_REWIND = 0x03,
	CODE_ANIMATION_STOP_04 = 0x04,
	CODE_ANIMATION_STOP_FF = 0xFF,
	CODE_ANIMATION_05 = 0x05,
	CODE_ANIMATION_06 = 0x06,
	CODE_ANIMATION_07 = 0x07,
	CODE_ANIMATION_08 = 0x08,
	CODE_ANIMATION_OPERATION = 0x14,
	CODE_ANIMATION_BLITDIRECT = 0x10,
	CODE_ANIMATION_BLITMASKED = 0x20,
	CODE_ANIMATION_BLITSWAPPED = 0x30,
	CODE_ANIMATION_SETCOLOR = 0x40,
	CODE_ANIMATION_BLITMERGED = 0x50,
	CODE_ANIMATION_ERROR = 0x60,
	CODE_ANIMATION_SETPALETTE = 0x80
};


enum AnimationOperationScriptBlitStruct {
	SOURCE_COORD_X0B = 1,
	SOURCE_COORD_Y0 = 2,
	SOURCE_COORD_X1B = 4,
	SOURCE_COORD_Y1 = 5,
	DESTINATION_COORD_XB = 7,
	DESTINATION_COORD_Y = 8
};


class Animation {
private:
	Memory *memory;
	Timer *timer;
	Input *input;
	Video *video;

	MemoryBlock *b_Slot;
	MemoryBlock *b_Script;

	word getSlotOffset(word slot_index);

	byte getSlotStatus(word slot_offset);
	word getSlotEntryOffset(word slot_offset);
	word getSlotCurrentOffset(word slot_offset);
	byte getSlotDelayFrame(word slot_offset);

	void setSlotStatus(word slot_offset, byte status);
	void setSlotEntryOffset(word slot_offset, word entry_offset);
	void setSlotCurrentOffset(word slot_offset, word current_offset);
	void setSlotDelayFrame(word slot_offset, byte delay_frame);
	void decreaseSlotDelayFrame(word slot_offset);

	void initializeBlit(word script_offset, BlitStruct *order);

	bool parse(word slot_offset);

public:
	Animation(Memory *memory, Timer *timer, Input *input, Video *video);
	~Animation();

	bool setSlot(word function, word slot_index, word parameter_2 = 0, word parameter_3 = 0);
	void clearSlot();
	bool show();
};

#endif
