#include "script.h"

#define NUMBER_SCRIPT_SIZE	12
#define FULLSIZE_CHARACTER	0
#define SJIS_ZERO			0x824F
#define SJIS_CONVERTCODE	0x2000
#define ASCII_ZERO			0x30

void save(MemoryBlock *b_DisplayNumberScript, word index, word *value, word digit, byte size);

SCRIPTCALL Script::op_displayNumber()
{
	parameter = getParameter();
	word value = parameter->get(0);
	deleteParameter();

	byte flag = memory->b_SystemVariable->queryByte(ibf_OP_DisplayNumber);
	byte length = flag & 0x0F;
	byte size = flag & 0xF0;

	MemoryBlock *b_DisplayNumberScript = new MemoryBlock(0, NUMBER_SCRIPT_SIZE);
	word index = 0;
	b_DisplayNumberScript->set(index);

	if (size != FULLSIZE_CHARACTER) {
		b_DisplayNumberScript->writeByte(index++, CODE_OP_HALFSIZECHARACTER);
	}

	if (length == 0) {
		if (value >= 10000) {
			length = 5;
		}
		if (value >= 1000) {
			length = 4;
		}
		if (value >= 100) {
			length = 3;
		}
		if (value >= 10) {
			length = 2;
		}
		if (value >= 1) {
			length = 1;
		}
	}

	switch (length) {
		case 5:
		default:
			save(b_DisplayNumberScript, index, &value, 10000, size);
		case 4:
			save(b_DisplayNumberScript, index, &value, 1000, size);
		case 3:
			save(b_DisplayNumberScript, index, &value, 100, size);
		case 2:
			save(b_DisplayNumberScript, index, &value, 10, size);
		case 1:
			save(b_DisplayNumberScript, index, &value, 1, size);
			break;
	}

	MemoryBlock *b_MainScript = memory->b_Script;
	memory->b_Script = b_DisplayNumberScript;

	parse();

	memory->b_Script = b_MainScript;

	return RETURN_NORMAL;
}


void save(MemoryBlock *b_DisplayNumberScript, word index, word *value, word digit, byte size)
{
	word number = 0;
	while (true) {
		number = number - digit;
		if (number < 0) {
			break;
		}
		number++;
	}
	number = number + digit;

	if (size == FULLSIZE_CHARACTER) {
		word code = number + SJIS_ZERO;
		code = code - SJIS_CONVERTCODE;

		b_DisplayNumberScript->writeWord(index, code);
		index += 2;
		b_DisplayNumberScript->writeByte(index, CODE_BLOCK_END);
	}
	else {
		byte code = (byte) number + ASCII_ZERO;
		b_DisplayNumberScript->writeByte(index++, code);
		b_DisplayNumberScript->writeByte(index++, CODE_OP_HALFSIZECHARACTER);
		b_DisplayNumberScript->writeByte(index, CODE_BLOCK_END);
	}
}