#include "script.h"

SCRIPTCALL Script::op4_blitMasked()
{
	parameter = getParameter();

	word source_coord_x0b;
	word source_coord_y0;
	word source_coord_x1b;
	word source_coord_y1;
	byte source_type;
	word destination_coord_xb;
	word destination_coord_y;
	byte destination_type;

	if (parameter->getCount() != 0) {
		source_coord_x0b = parameter->get(0);
		source_coord_y0 = parameter->get(1);
		source_coord_x1b = parameter->get(2);
		source_coord_y1 = parameter->get(3);
		source_type = (byte) parameter->get(4);
		destination_coord_xb = parameter->get(5);
		destination_coord_y = parameter->get(6);
		destination_type = (byte) parameter->get(7);
	}
	else {
		source_coord_x0b = memory->b_SystemVariable->queryWord(iw_GP4File_CoordXb);
		source_coord_y0 = memory->b_SystemVariable->queryWord(iw_GP4File_CoordY);
		source_coord_x1b = memory->b_SystemVariable->queryWord(iw_GP4File_Widthb) - source_coord_x0b;
		source_coord_y1 = memory->b_SystemVariable->queryWord(iw_GP4File_Height) - source_coord_y0;
		source_type = SURFACE_BUFFER1;
		destination_coord_xb = memory->b_SystemVariable->queryWord(iw_GP4File_CoordXb);
		destination_coord_y = memory->b_SystemVariable->queryWord(iw_GP4File_CoordY);
		destination_type = SURFACE_BUFFER2;
	}

	deleteParameter();

	video->blit(BLIT_MASKED, source_coord_x0b, source_coord_y0, source_coord_x1b, source_coord_y1, source_type, destination_coord_xb, destination_coord_y, destination_type);

	return RETURN_NORMAL;
}