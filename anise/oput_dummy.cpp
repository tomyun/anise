#include "script.h"

SCRIPTCALL Script::oput_dummy()
{
	deleteParameter();

	return RETURN_NORMAL;
}