#include "script.h"

SCRIPTCALL Script::op_delay()
{
	parameter = getParameter();

	if (parameter->getCount() == 0) {
		dialogue->wait();
	}
	else {
		dialogue->delay(parameter->get(0));
	}

	deleteParameter();

	return RETURN_NORMAL;
}
