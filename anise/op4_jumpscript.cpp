#include "script.h"

SCRIPTCALL Script::op4_jumpScript()
{
	char filename[SCRIPT_NAME_SIZE];

	parameter = getParameter();
	parameter->load(0, filename);
	deleteParameter();

	memory->b_ScriptName->writeString(0, filename, SCRIPT_NAME_SIZE);

	load(filename);

	animation->clearSlot();

	return RETURN_NORMAL;
}