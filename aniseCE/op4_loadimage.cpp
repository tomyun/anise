#include "script.h"

SCRIPTCALL Script::op4_loadImage()
{
	char filename[PARAMETER_LENGTH];

	parameter = getParameter();
	//HACK: support empty parameter
	bool is_parameter;
	if (parameter->getCount() != 0) {
		is_parameter = true;
		parameter->load(0, filename);
	}
	else {
		is_parameter = false;
	}
	deleteParameter();

	animation->clearSlot();

	if (is_parameter) {
		image->load(filename);
	}
	else {
		image->load(NULL);
	}

	return RETURN_NORMAL;
}
