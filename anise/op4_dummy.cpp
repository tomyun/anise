#include "script.h"

SCRIPTCALL Script::op4_dummy()
{
	parameter = getParameter();

	deleteParameter();

	return RETURN_NORMAL;
}