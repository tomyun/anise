#include "script.h"

SCRIPTCALL Script::op4_loadImage()
{
	char filename[PARAMETER_LENGTH];

	parameter = getParameter();
	parameter->load(0, filename);
	deleteParameter();

	animation->clearSlot();
	image->load(filename);

	return RETURN_NORMAL;
}