#include "dialogue.h"

Dialogue::Dialogue(Memory *memory, Timer *timer, Input *input, Video *video, Animation *animation, Option *option)
{
	this->memory = memory;
	this->timer = timer;
	this->input = input;
	this->video = video;
	this->animation = animation;
	this->option = option;

	previous_code = 0x00;

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			josa_table[i][j] = 0;
		}
	}
	josa_array = NULL;

	if (option->font_type == FONT_JIS) {
		initializeJisFont();
	}
	else if (option->font_type == FONT_JISHAN) {
		initializeJisHanFont();
	}
	else if (option->font_type == FONT_GAMEBOX) {
		initializeGameboxFont();
	}
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
		offset = getJisFontOffset(first_code, second_code);
	}
	else if (option->font_type == FONT_JISHAN) {
		offset = getJisHanFontOffset(first_code, second_code);
	}
	else if (option->font_type == FONT_GAMEBOX) {
		offset = getGameboxFontOffset(first_code, second_code);
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


void Dialogue::setPosition(word coord_xb, word coord_y)
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

			timer->delay();
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
