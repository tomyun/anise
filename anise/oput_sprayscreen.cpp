#include "script.h"

SCRIPTCALL Script::oput_sprayScreen()
{
	deleteParameter();

	word coord_x = memory->b_SystemVariable->queryWord(iw_GP4File_CoordXb) << 3;
	word coord_y = memory->b_SystemVariable->queryWord(iw_GP4File_CoordY);
	word width = memory->b_SystemVariable->queryWord(iw_GP4File_Widthb) << 3;
	word height = memory->b_SystemVariable->queryWord(iw_GP4File_Height);
	word scaled_height = height >> 4;

	byte mask_index = 0;
	byte mask = 0x80;
	byte current_mask = mask;
	word masks[16];
	for (int i = 0; i < 16; i++) {
		masks[i] = 0;
	}

	for (int i = 0; i < 128; i++) {
		timer->resetSprayTimer();

		for (int sy = 0; sy < scaled_height; sy++) {
			int y = (sy * 16) + mask_index;

			for (int x = 0; x < width; x++) {
				if (((current_mask >> (7 - (x % 8))) & 1) == 1) {
					byte pixel = video->getPoint(SURFACE_BUFFER2, coord_x + x, coord_y + y);
					video->putPoint(SURFACE_SCREEN, coord_x + x, coord_y + y, pixel);
				}
			}
		}

		video->updateScreen(coord_x, coord_y, width, height);

		byte old_mask = mask;
		mask = mask << 1;
		if ((old_mask & 0x80) == 0x80) {
			mask++;
		}

		if (mask < old_mask) {
			mask_index++;
		}
		mask_index = (mask_index + 5) & MASK_LOWER_BYTE;

		current_mask = mask | (byte) (masks[mask_index]);
		masks[mask_index] = (word) current_mask;

		input->refresh();

		word spray_timer = 0;
		while (spray_timer < 2) {
			//TODO: debugmode
			animation->show();
			input->refresh();

			spray_timer = timer->checkSprayTimer();
		}
	}

	return RETURN_NORMAL;
}