#include "script.h"

enum OverlapFunction {
	OVERLAP_SET = 0,
	OVERLAP_CHECK = 1,
	OVERLAP_INITIALIZE = 2
};


SCRIPTCALL Script::oput_overlapScreen()
{
	word mode = parameter->get(1);
	word parameter_2 = parameter->get(2);
	word parameter_3 = parameter->get(3);

	deleteParameter();

	word return_value = 0;
	switch (mode) {
		case OVERLAP_SET:
			{
				video->overlap_inuse = false;

				if (parameter_2 == 0) {
					video->overlap_left_level = video->overlap_level;
				}
				else {
					video->overlap_level = 1;
					video->overlap_left_level = 1;
				}

				video->initializeOverlapScreen();
			}
			break;

		case OVERLAP_CHECK:
			{
				if (parameter_2 == 1) {
					return_value = timer->checkOverlapFrameTimer();
				}
				else {
					return_value = video->overlap_inuse ? 1 : 0;
				}
			}
			break;

		case OVERLAP_INITIALIZE:
			{
				word parameter_value;

				if (parameter_2 == 0) {
					timer->resetOverlapFrameTimer();
					break;
				}
				else if (parameter_2 == 1) {
					video->overlap_parameter = timer->checkOverlapFrameTimer();
					parameter_value = video->overlap_parameter & MASK_LOWER_WORD;
					return_value = video->overlap_parameter & MASK_LOWER_WORD;
				}
				else {
					return_value = video->overlap_parameter;
					video->overlap_parameter = parameter_3 & MASK_LOWER_WORD;
					parameter_value = parameter_3 & MASK_LOWER_WORD;
				}

				word level = 1;
				word delay = 1;

				//TODO: make more simple implementation
				if (parameter_value > 1) {
					level = level << 1;
				}
				if (parameter_value > 2) {
					level = level << 1;
				}
				if (parameter_value > 4) {
					level = level << 1;
				}
				if (parameter_value > 8) {
					level = level << 1;
				}
				if (parameter_value > 16) {
					delay = parameter_value >> 4;
				}

				video->overlap_level = level;
				video->overlap_delay = delay;
			}
			break;
	}

	memory->b_BaseVariable->writeWord(iw_Selection, return_value);

	return RETURN_NORMAL;
}
