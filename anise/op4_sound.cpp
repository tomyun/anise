#include "script.h"

enum SoundFunction {
	SOUND_PLAY = 0,
	SOUND_STOP = 1
};


SCRIPTCALL Script::op4_sound()
{
	parameter = getParameter();
	word function = parameter->get(0);
	deleteParameter();

	switch (function) {
		case SOUND_PLAY:
			{
			}
			break;

		case SOUND_STOP:
			{
			}
			break;
	}

	return RETURN_NORMAL;
}