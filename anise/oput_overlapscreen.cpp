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

	word return_value = NULL;
	switch (mode) {
		case OVERLAP_SET:
			{
				video->overlap_inuse = false;

				if (parameter_2 == 0) {
					video->overlap_current_level = 0;
				}
				else {
					video->overlap_level = 1;
					video->overlap_current_level = 0;
				}

				video->initializeOverlapScreen();
			}
			break;

		case OVERLAP_CHECK:
			{
				if (parameter_2 == 1) {
					return_value = video->overlap_current_level;
				}
				else {
					return_value = video->overlap_inuse ? 1 : 0;
				}
			}
			break;

		case OVERLAP_INITIALIZE:
			{
				word parameter_value;
				word current_parameter;

				if (parameter_2 == 0) {
					video->overlap_current_level = 0;
					break;
				}
				else if (parameter_2 == 1) {
					parameter_value = video->overlap_current_level;
					current_parameter = parameter_value & MASK_LOWER_WORD;
					video->overlap_parameter = parameter_value;
				}
				else {
					parameter_value = parameter_3 & MASK_LOWER_WORD;
					current_parameter = video->overlap_parameter;
					video->overlap_parameter = parameter_value;
				}

				word level = 1;
				word delay = 1;

				//TODO: make more simple implementation
				if (parameter_value >= 2) {
					level = level << 1;
				}
				if (parameter_value >= 3) {
					level = level << 1;
				}
				if (parameter_value >= 5) {
					level = level << 1;
				}
				if (parameter_value >= 9) {
					level = level << 1;
				}
				if (parameter_value >= 17) {
					delay = parameter_value >> 4;
				}

				video->overlap_level = level;
				video->overlap_delay = delay;

				return_value = current_parameter;
			}
			break;
	}

	memory->b_BaseVariable->writeWord(iw_Selection, return_value);

	deleteParameter();

	return RETURN_NORMAL;
}