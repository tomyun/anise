#ifndef SCRIPT_H
#define SCRIPT_H

#include "memory.h"
#include "video.h"
#include "timer.h"
#include "input.h"
#include "file.h"
#include "image.h"
#include "animation.h"
#include "dialogue.h"
#include "field.h"
#include "config.h"
#include "script_code.h"
#include "script_parameter.h"
#include "script_stack.h"
#include "debug.h"

#define OPCODE(x)		{&Script::x, #x}
#define OPCODE_OP		17
#define OPCODE_OPUT		14
#define OPCODE_OP4		24
#define OPCODE_OPRE		16

#define RANDOM_SEED		0x8801

typedef int SCRIPTCALL;

enum ScriptCallReturnType {
	RETURN_SUCCESS = 0x0000,
	RETURN_CONTINUE = 0x0001,
	RETURN_BREAK = 0x0002,
	RETURN_EXIT = 0xFF00,
	RETURN_NORMAL = 0xFF01,
	RETURN_ERROR = 0xFFFF
};


class Script {
private:
	Memory *memory;
	Video *video;
	Timer *timer;
	Input *input;
	File *file;
	Image *image;
	Animation *animation;
	Dialogue *dialogue;
	Field *field;
	Config *config;

	int parse_level;

	// methods
public:
	byte fetch();
	byte fetchAdvance();
	void advance();
	word skipScriptBlock();
	word getOffset();
	void setOffset(word offset);

	// value reading related stuffs (stack.*, script_value.cpp)
private:
	ScriptStack *stack;
	word random_seed;

public:
	word readConstant();
	word readConstant1Op();
	word readConstant2Op();
	word readConstant3Op();
	word readExpression();

    // parameter related stuffs (script_parameter.cpp)
private:
	ScriptParameter *parameter;
	ScriptParameter* getParameter();
	void deleteParameter();

	// opcode related stuffs
	typedef SCRIPTCALL (Script::*OpcodeFunction)();
	struct Opcode {
		OpcodeFunction function;
		const char *description;
	} *opcode_op_list, *opcode_op4_list, *opcode_opre_list, *opcode_oput_list;

	void setOpcodes();	//TODO: extend to support other games (initialize(GameType))

	SCRIPTCALL excuteOpcodeOp(byte code);
	SCRIPTCALL excuteOpcodeOp4(byte code);
	SCRIPTCALL excuteOpcodeOpre(byte code);
	const char* getOpcodeDescriptionOp(byte code);
	const char* getOpcodeDescriptionOp4(byte code);
	const char* getOpcodeDescriptionOpre(byte code);

	SCRIPTCALL op_saveConstant();
	SCRIPTCALL op_saveExpression();
	SCRIPTCALL op_setBaseVariable();
	SCRIPTCALL op_makeArray();
	SCRIPTCALL op_makeByteArray();
	SCRIPTCALL op_if();
	SCRIPTCALL op_setDialogueColor();
	SCRIPTCALL op_wait();
	SCRIPTCALL op_defineProcedure();
	SCRIPTCALL op_callProcedure();
	SCRIPTCALL op_call();
	SCRIPTCALL op_displayNumber();
	SCRIPTCALL op_delay();
	SCRIPTCALL op_clearScreen();
	SCRIPTCALL op_setColor();
	SCRIPTCALL op_utility();
	SCRIPTCALL op_animate();
	SCRIPTCALL op_null();

	SCRIPTCALL op4_while();
	SCRIPTCALL op4_continue();
	SCRIPTCALL op4_break();
	SCRIPTCALL op4_displaySelection();
	SCRIPTCALL op4_initializeSelection();
	SCRIPTCALL op4_mouse();
	SCRIPTCALL op4_palette();
	SCRIPTCALL op4_drawSolidBox();
	SCRIPTCALL op4_drawInverseBox();
	SCRIPTCALL op4_blitDirect();
	SCRIPTCALL op4_blitSwapped();
	SCRIPTCALL op4_blitMasked();
	SCRIPTCALL op4_loadFile();
	SCRIPTCALL op4_loadImage();
	SCRIPTCALL op4_jumpScript();
	SCRIPTCALL op4_callScript();
	//SCRIPTCALL op4_loadParserFlag();
	SCRIPTCALL op4_manipulateFlag();
	SCRIPTCALL op4_checkClick();
	SCRIPTCALL op4_sound();
	//SCRIPTCALL op4_null();
	SCRIPTCALL op4_field();
	//SCRIPTCALL op4_setSegment();
	SCRIPTCALL op4_dummy();

	SCRIPTCALL opre_add();
	SCRIPTCALL opre_subtract();
	SCRIPTCALL opre_multiply();
	SCRIPTCALL opre_divide();
	SCRIPTCALL opre_modular();
	SCRIPTCALL opre_or();
	SCRIPTCALL opre_and();
	SCRIPTCALL opre_isEqual();
	SCRIPTCALL opre_isNotEqual();
	SCRIPTCALL opre_isAbove();
	SCRIPTCALL opre_isBelow();
	SCRIPTCALL opre_getWordFromMemory();
	SCRIPTCALL opre_getByteFromMemory();
	SCRIPTCALL opre_getConstantFromOperand();
	SCRIPTCALL opre_getConstantFromStack();
	SCRIPTCALL opre_random();
	SCRIPTCALL opre_null();

	SCRIPTCALL oput_unpackAniHeader();
	SCRIPTCALL oput_overlapScreen();
	SCRIPTCALL oput_swapScreen();
	SCRIPTCALL oput_sprayScreen();
	SCRIPTCALL oput_dummy();

public:
	// constructor & destructor
	Script(Memory *memory, Video *video, Timer *timer, Input *input, Config *config);
	~Script();

	// methods
	void load(const char *filename);

	SCRIPTCALL parse();
private:
	SCRIPTCALL parseNested();
};

#endif