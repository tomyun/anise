#include "script.h"

SCRIPTCALL Script::op_wait()
{
	//TODO: make this separate for those silky's games
	//parameter = getParameter();
	//deleteParameter();

	dialogue->wait();
    video->clearScreen();
	dialogue->setPosition();

	return RETURN_NORMAL;
}