#include "script.h"

SCRIPTCALL Script::op4_drawInverseBox()
{
	parameter = getParameter();

	word coord_x0 = parameter->get(0);
	word coord_y0 = parameter->get(1);
	word coord_x1 = parameter->get(2);
	word coord_y1 = parameter->get(3);

	deleteParameter();

	video->drawBox(DRAWBOX_INVERSE, coord_x0, coord_y0, coord_x1, coord_y1);

	return RETURN_NORMAL;
}
