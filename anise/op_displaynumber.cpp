#include "script.h"

#define NUMBER_SCRIPT_SIZE	12
#define FULLSIZE_CHARACTER	0
#define ZERO_CONVERT_CODE	0x2000
#define JIS_ZERO			(0x824F - ZERO_CONVERT_CODE)
#define GAMEBOX_ZERO		(0x9450 - ZERO_CONVERT_CODE)
#define ASCII_ZERO			0x30

int power(int base, int exponent)
{
	int number = 1;
	for (int i = 0; i < exponent; i++) {
		number = number * base;
	}

	return number;
}


SCRIPTCALL Script::op_displayNumber()
{
	parameter = getParameter();
	word number = parameter->get(0);
	deleteParameter();

	byte flag = memory->b_SystemVariable->queryByte(ibf_OP_DisplayNumber);
	byte length = flag & MASK_LOWER_BYTE;
	byte font_size = flag & MASK_UPPER_BYTE;

	MemoryBlock *b_DisplayNumberScript = new MemoryBlock(0, NUMBER_SCRIPT_SIZE);

	if (length == 0) {
		if (number >= 10000) {
			length = 5;
		}
		else if (number >= 1000) {
			length = 4;
		}
		else if (number >= 100) {
			length = 3;
		}
		else if (number >= 10) {
			length = 2;
		}
		else if (number >= 1) {
			length = 1;
		}
	}

	word index = 0;

	if (font_size != FULLSIZE_CHARACTER) {
		b_DisplayNumberScript->writeByte(index++, CODE_OP_HALFSIZECHARACTER);
	}

	for (int i = length; i > 0; i--) {
		int digit = power(10, length - 1);
		int count = number / digit;
		number = number % digit;

		if (font_size == FULLSIZE_CHARACTER) {
			word code;
			if (option->font_type == FONT_GAMEBOX) {
				code = count + GAMEBOX_ZERO;
			}
			else {
				code = count + JIS_ZERO;
			}

			b_DisplayNumberScript->writeByte(index++, (code & MASK_UPPER_WORD) >> 8);
			b_DisplayNumberScript->writeByte(index++, code & MASK_LOWER_WORD);
		}
		else {
			byte code = count + ASCII_ZERO;

			b_DisplayNumberScript->writeByte(index++, code);
		}
	}

	if (font_size != FULLSIZE_CHARACTER) {
		b_DisplayNumberScript->writeByte(index++, CODE_OP_HALFSIZECHARACTER);
	}

	b_DisplayNumberScript->writeByte(index, CODE_BLOCK_END);

	MemoryBlock *b_MainScript = memory->b_Script;
	memory->b_Script = b_DisplayNumberScript;

	parse();

	memory->b_Script = b_MainScript;

	delete b_DisplayNumberScript;

	return RETURN_NORMAL;
}