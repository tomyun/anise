#include "script.h"

#define PALETTE_CHANGE_DELAY	2

word temporary_palette[VIDEO_COLOR];

void savePalette(Memory *memory);
void restorePalette(Memory *memory);
void getPalette(Memory *memory, ScriptParameter *parameter);

enum PaletteFunction {
	PALETTE_SET = 2,
	PALETTE_CHANGE = 3
};


SCRIPTCALL Script::op4_palette()
{
	parameter = getParameter();

	word function = parameter->get(0);
	switch (function) {
		case PALETTE_SET:
			{
				savePalette(memory);
				getPalette(memory, parameter);

				for (int i = 0; i < VIDEO_COLOR; i++) {
					word color = memory->b_SystemVariable->queryWord(iw_Video_Palette0 + (i * 2));
					video->setIntermediateColor(i, color);
				}

				video->setPalette();

				restorePalette(memory);
			}
			break;

		case PALETTE_CHANGE:
			{
				savePalette(memory);
				getPalette(memory, parameter);

				for (int i = 0; i < VIDEO_COLOR; i++) {
					word color = memory->b_SystemVariable->queryWord(iw_Video_Palette0 + (i * 2));
					video->setIntermediateColor(i, color);

					//ACK: original algorithm, but too slow
					/*
					word color_to, color_from;

					for (int j = 0; j < COLOR_LEVEL; j++) {
						color_to = memory->b_SystemVariable->queryWord(iw_Video_Palette0 + (i * 2));
						color_from = video->getIntermediateColor(i);

						word blue_to = color_to & COLOR_BLUE;
						word blue_from = color_from & COLOR_BLUE;
						if (blue_to < blue_from) {
							color_from -= 0x0001;
						}
						else if (blue_to > blue_from) {
							color_from += 0x0001;
						}

						word red_to = color_to & COLOR_RED;
						word red_from = color_from & COLOR_RED;
						if (red_to < red_from) {
							color_from -= 0x0010;
						}
						else if (red_to > red_from) {
							color_from += 0x0010;
						}

						word green_to = color_to & COLOR_GREEN;
						word green_from = color_from & COLOR_GREEN;
						if (green_to < green_from) {
							color_from -= 0x0100;
						}
						else if (green_to > green_from) {
							color_from += 0x0100;
						}

						video->setIntermediateColor(i, color_from);
					}

					input->delay(PALETTE_CHANGE_DELAY);
					video->setPalette();
					input->refresh();		//TODO: temporary

					*/
				}

				video->fadeScreen();

				restorePalette(memory);
			}
			break;
	}

	deleteParameter();

	return RETURN_NORMAL;
}


void savePalette(Memory *memory)
{
	if (memory->b_SystemVariable->testByte(ibf_DisabledStatus, DISABLE_PRESERVEPALETTE) == false) {
		for (int i = 0; i < VIDEO_COLOR; i++) {
			temporary_palette[i] = memory->b_SystemVariable->queryWord(iw_Video_Palette0 + (i * 2));
		}
	}
}


void restorePalette(Memory *memory)
{
	if (memory->b_SystemVariable->testByte(ibf_DisabledStatus, DISABLE_PRESERVEPALETTE) == false) {
		for (int i = 0; i < VIDEO_COLOR; i++) {
			memory->b_SystemVariable->writeWord(iw_Video_Palette0 + (i * 2), temporary_palette[i]);
		}
	}
}


void getPalette(Memory *memory, ScriptParameter *parameter)
{
	if (parameter->getType(1) == PARAMETER_TYPE_CONSTANT) {
		for (int i = 0; i < VIDEO_COLOR; i++) {
			memory->b_SystemVariable->writeWord(iw_Video_Palette0 + (i * 2), parameter->get(1));
		}
	}
}
