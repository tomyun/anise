#include "script.h"

SCRIPTCALL Script::op_setDialogueColor()
{
	parameter = getParameter();
	word color = memory->b_SystemVariable->queryWord(iw_Video_Color0 + (parameter->get(0) * 2));
	deleteParameter();

	memory->b_SystemVariable->writeWord(iw_Video_Palette8, color);
	video->setIntermediateColor(8, color);
	video->setColor(8, color);

	//HACK: it's too slow and needless
	//video->updateScreen();

	animation->setSlot(SLOT_ACTIVATE, 1);

	return RETURN_NORMAL;
}
