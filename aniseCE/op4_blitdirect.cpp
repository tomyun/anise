#include "script.h"

SCRIPTCALL Script::op4_blitDirect()
{
	parameter = getParameter();

	word source_coord_x0b = parameter->get(0);
	word source_coord_y0 = parameter->get(1);
	word source_coord_x1b = parameter->get(2);
	word source_coord_y1 = parameter->get(3);
	byte source_type = (byte) parameter->get(4);
	word destination_coord_xb = parameter->get(5);
	word destination_coord_y = parameter->get(6);
	byte destination_type = (byte) parameter->get(7);

	deleteParameter();

	video->blit(BLIT_DIRECT, source_coord_x0b, source_coord_y0, source_coord_x1b, source_coord_y1, source_type, destination_coord_xb, destination_coord_y, destination_type);

	return RETURN_NORMAL;
}
