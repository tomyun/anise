#include "script.h"

SCRIPTCALL Script::op4_mouse()
{
	parameter = getParameter();

	word function = parameter->get(0);
	switch (function) {
		case 0:
			input->showCursor();
			break;
		case 1:
			input->hideCursor();
			break;
		case 2:
			break;
		case 3:
			input->setCursorPosition(parameter->get(1), parameter->get(2));
			break;
		case 4:
		default:
			input->loadCursorImage(parameter->get(1));
			input->setCursorImage(CURSOR_FIRSTFRAME);
	}

	deleteParameter();

	return RETURN_NORMAL;
}
