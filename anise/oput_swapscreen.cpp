#include "script.h"

SCRIPTCALL Script::oput_swapScreen()
{
	word source_coord_x0b = 8;
	word source_coord_y0 = parameter->get(1);
	word source_coord_x1b = 72;
	word source_coord_y1 = parameter->get(1);
	byte source_type = SURFACE_SCREEN;
	word destination_coord_xb = 8;
	word destination_coord_y = parameter->get(1);
	byte destination_type = SURFACE_BUFFER1;

	video->blit(BLIT_SWAPPED, source_coord_x0b, source_coord_y0, source_coord_x1b, source_coord_y1, source_type, destination_coord_xb, destination_coord_y, destination_type);

	deleteParameter();

	return RETURN_NORMAL;
}