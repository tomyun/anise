#include "script.h"

SCRIPTCALL Script::op_setColor()
{
	parameter = getParameter();

	memory->b_SystemVariable->writeWord(iw_Video_Color, parameter->get(0));

	deleteParameter();

	return RETURN_NORMAL;
}