#include "script.h"

#define SELECTION_NOTFOUND	NONE

SCRIPTCALL Script::op4_checkClick()
{
	parameter = getParameter();

	word a6_offset = parameter->get(0);
	word coord_x = parameter->get(1);
	word coord_y = parameter->get(2);

	deleteParameter();

	MemorySegment *segment = memory->getCurrentSegment();
	MemoryBlock *memory_block = segment->get(&a6_offset);

	word selection;
	while (true) {
		selection = memory_block->queryWord(a6_offset);
		if (selection == SELECTION_NOTFOUND) {
			break;
		}
		else {
			word coord_x_start = memory_block->queryWord(a6_offset + 2);
			word coord_x_end = memory_block->queryWord(a6_offset + 6);
			word coord_y_start = memory_block->queryWord(a6_offset + 4);
			word coord_y_end = memory_block->queryWord(a6_offset + 8);

			if ((coord_x >= coord_x_start) && (coord_x < coord_x_end) && (coord_y >= coord_y_start) && (coord_y < coord_y_end)) {
				break;
			}
			else {
				a6_offset += 10;
				continue;
			}
		}
	}

	memory->b_BaseVariable->writeWord(iw_Selection, selection);

	return RETURN_NORMAL;
}
