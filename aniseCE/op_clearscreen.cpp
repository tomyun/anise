#include "script.h"

SCRIPTCALL Script::op_clearScreen()
{
	parameter = getParameter();

	word color = 0;
	if (parameter->getCount() > 0) {
		color = memory->b_SystemVariable->queryWord(iw_Video_Color);
		memory->b_SystemVariable->writeWord(iw_Video_Color, parameter->get(0) << 4);
	}

	video->clearScreen();
	dialogue->setPosition();
	
	if (parameter->getCount() > 0) {
		memory->b_SystemVariable->writeWord(iw_Video_Color, color);
	}

	deleteParameter();

	return RETURN_NORMAL;
}
