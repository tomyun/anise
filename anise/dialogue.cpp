#include "dialogue.h"

Dialogue::Dialogue(Memory *memory, Video *video, Timer *timer, Input *input, Animation *animation, Option *option)
{
	this->memory = memory;
	this->video = video;
	this->timer = timer;
	this->input = input;
	this->animation = animation;
	this->option = option;

	if (option->font_type == FONT_JIS) {
		initializeFontJis();
	}
	else if (option->font_type == FONT_JISHAN) {
		initializeFontJisHan();
	}
	else if (option->font_type == FONT_GAMEBOX) {
		initializeFontGamebox();
	}

	previous_code = 0x00;
}


Dialogue::~Dialogue()
{
}


void Dialogue::putHalfWidthCharacters()
{
	memory->b_Script->advance();

	while (true) {
		word coord_xb;
		word coord_y;
		getPosition(&coord_xb, &coord_y);

		byte code = memory->b_Script->readByte();
		if (code == CODE_OP_HALFSIZECHARACTER) {
			break;
		}

		int offset = code * (FONT_HALF_WIDTH / FONT_BPB) * (FONT_HALF_HEIGHT);
		if (offset < size) {
			video->drawFont(coord_xb << 3, coord_y, font, offset, FONT_HALF_WIDTH, FONT_HALF_HEIGHT);
		}
		else {
			PRINT_ERROR("[Dialogue::putHalfWidthCharacters()] out of bound: offset = %d, size = %d\n", offset, size);
		}

		word duration = memory->b_SystemVariable->queryWord(iw_Dialogue_Delay);
		delay(duration);

		updatePosition();
	}
}


void Dialogue::putStandardText()
{
	byte first_code = memory->b_Script->readByte() + FONT_FACTOR;
	byte second_code = memory->b_Script->readByte();

	if ((first_code == code_newline_first) && (second_code == code_newline_second)) {
		breakNewLine();
	}
	else {
		putFullWidthCharacter(first_code, second_code);
	}
}


void Dialogue::putPredefinedText()
{
	byte first_code = memory->b_Script->readByte();
	byte second_code;

	if (memory->b_SystemVariable->testByte(ibf_DisabledStatus, DISABLE_PREDEFINEDTEXT)) {
		second_code = memory->b_Script->readByte();
	}
	else {
		word index = (word) ((first_code - FONT_PREDEFINED_FACTOR) * 2);
		word offset = memory->b_SystemVariable->queryWord(iwpo_Script) + index + 2;
		
		MemoryBlock *memory_block = memory->s_Core->get(&offset);
		first_code = memory_block->queryByte(offset);
		second_code = memory_block->queryByte(offset + 1);
	}

	if ((first_code == code_newline_first) && (second_code == code_newline_second)) {
		breakNewLine();
	}
	else {
		putFullWidthCharacter(first_code, second_code);
	}
}


void Dialogue::putFullWidthCharacter(byte first_code, byte second_code)
{
	word coord_xb = memory->b_SystemVariable->queryWord(iw_Dialogue_CoordXb);
	word coord_y = memory->b_SystemVariable->queryWord(iw_Dialogue_CoordY);
	int offset = 0;
	if (option->font_type == FONT_JIS) {
		// calculate font code
		if ((first_code >= 0x81) && (first_code <= 0x9F)) {
			first_code = first_code - 0x81;
		}
		else if ((first_code >= 0xE0) && (first_code <= 0xFF)) {
			first_code = first_code - 0x81 - 0x40;
		}
		else {
			//TODO: process error
			PRINT_ERROR("[Dialogue::putFullWidthCharacter()] invalid first_code: %2x\n", first_code);
		}

		if ((second_code >= 0x40) && (second_code <= 0x7F)) {
			second_code = second_code - 0x40;
		}
		else if ((second_code >= 0x80) && (second_code <= 0xFC)) {
			second_code = second_code - 0x40 - 1;
		}
		else {
			//TODO: process error
			PRINT_ERROR("[Dialogue::putFullWidthCharacter()] invalid second_code: %2x\n", second_code);
		}

		offset = ((first_code * 0xBC) + second_code + 0x70) * (FONT_FULL_WIDTH / FONT_BPB) * FONT_FULL_HEIGHT;
	}
	else if (option->font_type == FONT_JISHAN) {
		//HACK: adjust space size to improve readablity
		if (first_code == 0x81 && second_code == 0x40) {
			if (coord_y != 295) {
				setPosition(coord_xb - 1, NONE);
			}
		}

		// calculate font code
		word code = (first_code << 8) + second_code;
		code = code - 0x8398;

		//TODO: clean it up
		if (code <= 4) {
			int josa_type = (int) code;
			if ((previous_code != (0x82F1 - 0x8140)) && (previous_code != (0x8393 - 0x8140))) {
				int table_index = (int) (previous_code & MASK_LOWER_WORD);
				previous_code = previous_code - (0x8A40 - 0x8140);
				if (previous_code < (0x9691 - 0x8A40 + 1)) {
					byte table_offset_index = (previous_code & MASK_UPPER_WORD) >> 8;
					int table_offset = (table_index << 3) + (table_offset_index * 24);

				}

				// bx1
			}

			// bx0
		}

		// g16
		code = code + 0x8398 - 0x8140;
		previous_code = code;

		first_code = (byte) ((code & MASK_UPPER_WORD) >> 8);
		second_code = (byte) (code & MASK_LOWER_WORD);

		// joi10
		if (first_code >= 7) {
			first_code = first_code - 3;
		}

		// joi10b
		if (second_code < 0xBD) {
			code = (first_code * 0xBD) + second_code;
			if (code >= (4841 - 128)) {
				code = 0x00;
			}
		}
		else {
			code = 0x00;
		}

		// joi10a
		offset = (code + 0x80) * (FONT_FULL_WIDTH / FONT_BPB) * FONT_FULL_HEIGHT;
	}
	else if (option->font_type == FONT_GAMEBOX) {
		//HACK: adjust space size to improve readablity
		if (first_code == 0x91 && second_code == 0x40) {
			setPosition(coord_xb - 1, NONE);
		}
		else if (first_code == 0x91 && second_code == 0x41) {
			if (coord_y != 295) {
				setPosition(coord_xb - 1, NONE);
			}
		}

		// calculate font code
		if ((first_code == 0x91) && (second_code >= 0x82) && (second_code <= 0x86)) {
			PRINT("[Dialogue::putFullWidthCharacter()] for nanpa2teen\n");
		}

		word code = (first_code << 8) + second_code;
		code = code - 0x8140;
		first_code = (byte) ((code & MASK_UPPER_WORD) >> 8);
		second_code = (byte) (code & MASK_LOWER_WORD);

		code = first_code * 0xBB;
		if (code >= 0x0DE1) {
			code = code - 0x01CD;
		}
		if (code >= 0x0BB0) {
			code = code - 0x012C;
		}
		if (code >= 0x02EC) {
			code = code - 0x0118;
		}

		if (second_code >= 0x3F) {
			second_code = second_code - 1;
		}

		code = code + second_code;

		if (second_code >= 0x1C) {
			code = code + 0x7F;
		}
		else {
			code = code + 0x80;
		}

		if (code <= 0x09EB) {
			previous_code = code;
		}

		offset = code * (FONT_FULL_WIDTH / FONT_BPB) * FONT_FULL_HEIGHT;
	}
	
	if (offset < size) {
		video->drawFont(coord_xb << 3, coord_y, font, offset, FONT_FULL_WIDTH, FONT_FULL_HEIGHT);
	}
	else {
		PRINT_ERROR("[Dialogue::putFullWidthCharacter()] out of bound: offset = %d, size = %d\n", offset, size);
	}

	word duration = memory->b_SystemVariable->queryWord(iw_Dialogue_Delay);
	delay(duration);

	updatePosition();
}


void Dialogue::updatePosition()
{
	word dialogue_coord_xb = memory->b_SystemVariable->queryWord(iw_Dialogue_CoordXb);
	byte font_widthb = memory->b_SystemVariable->queryByte(ib_Dialogue_FontWidthb);

	setPosition(dialogue_coord_xb + font_widthb, NONE);

	word video_max_coord_xb = memory->b_SystemVariable->queryWord(iw_Video_MaxCoordXb);
	if ((dialogue_coord_xb + (font_widthb * 2) - FONT_MARGIN) >= video_max_coord_xb) {
		breakNewLine();
	}
}


void Dialogue::breakNewLine()
{
	
	word video_coord_xb = memory->b_SystemVariable->queryWord(iw_Video_CoordXb);
	word dialogue_coord_y = memory->b_SystemVariable->queryWord(iw_Dialogue_CoordY);
	byte font_height = memory->b_SystemVariable->queryByte(ib_Dialogue_FontHeight);

	setPosition(video_coord_xb, dialogue_coord_y + font_height);

	word video_height = memory->b_SystemVariable->queryWord(iw_Video_MaxCoordY);
	if ((dialogue_coord_y + (font_height * 2) - FONT_MARGIN) >= video_height) {
		dialogue_coord_y = memory->b_SystemVariable->queryWord(iw_Video_CoordY);
		memory->b_SystemVariable->writeWord(iw_Dialogue_CoordY, dialogue_coord_y);
	}
}


inline void Dialogue::setPosition(word coord_xb, word coord_y)
{
	if (coord_xb != NONE) {
		memory->b_SystemVariable->writeWord(iw_Dialogue_CoordXb, coord_xb);
	}
	if (coord_y != NONE) {
		memory->b_SystemVariable->writeWord(iw_Dialogue_CoordY, coord_y);
	}
}


void Dialogue::setPosition()
{
	word coord_xb = memory->b_SystemVariable->queryWord(iw_Video_CoordXb);
	word coord_y = memory->b_SystemVariable->queryWord(iw_Video_CoordY);

	setPosition(coord_xb, coord_y);
}


void Dialogue::getPosition(word *coord_xb, word *coord_y)
{
	*coord_xb = memory->b_SystemVariable->queryWord(iw_Dialogue_CoordXb);
	*coord_y = memory->b_SystemVariable->queryWord(iw_Dialogue_CoordY);
}


void Dialogue::delay(word duration)
{
	if (duration != 0) {
		duration = (duration >> 2) + 1;

		timer->resetDelayTimer();

		word tick = 0;
		while (tick < duration) {
			//TODO: debugmode
			animation->show();
			if (input->refresh() == false) {
				break;
			}

			if (input->check(INPUT_CTRL)) {
				break;
			}

			tick = (word) timer->checkDelayTimer();
		}
	}
}


void Dialogue::wait()
{
	while (true) {
		if (input->refresh() == false) {
			return;
		}

		if (input->check(INPUT_CTRL) == true) {
			return;
		}

		delay(WAIT_DURATION);

		if (input->check(INPUT_OK) == true) {
			while (true) {
				if (input->check(INPUT_CTRL) == true) {
					return;
				}
				else {
					delay(WAIT_DURATION);

					if (input->check(INPUT_OK) == false) {
						return;
					}
				}
			}
		}
	}
}
