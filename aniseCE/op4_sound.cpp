#include "script.h"

enum SoundFunction {
	SOUND_PLAY = 0,
	SOUND_STOP = 1,
	SOUND_RESET = 2
};


SCRIPTCALL Script::op4_sound()
{
	parameter = getParameter();
	word function = parameter->get(0);
	deleteParameter();

	switch (function) {
		case SOUND_PLAY:
			{
				sound->stop();
				sound->load();
				sound->play();
			}
			break;

		case SOUND_STOP:
			{
				sound->stop();
			}
			break;

		case SOUND_RESET:
			{
				//TODO: need implementation
				sound->stop();
			}
			break;
	}

	return RETURN_NORMAL;
}
