#include "script.h"

SCRIPTCALL Script::op_animate()
{
	parameter = getParameter();

	word function = parameter->get(0);
	word slot_index = parameter->get(1);
	word parameter_2 = parameter->get(2);
	word parameter_3 = parameter->get(3);

	deleteParameter();

	word script_offset = getOffset();

	if (animation->setSlot(function, slot_index, parameter_2, parameter_3) == false) {
		return RETURN_ERROR;	
	}

	setOffset(script_offset);

	return RETURN_NORMAL;
}
