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
				sound->load();
				sound->play();
			}
			break;

		case SOUND_STOP:
			{
				sound->stop();
			}
			break;
	}

	return RETURN_NORMAL;
}