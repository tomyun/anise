#include "script.h"

SCRIPTCALL Script::op4_changeSlot()
{
	parameter = getParameter();

	word slot_index = parameter->get(0);
	byte packed_index = (byte) parameter->get(1);

	deleteParameter();

	switch (slot_index) {
		case 0:
			memory->b_SystemVariable->writeByte(ib_PackedFile_Slot0, packed_index);
			break;

		case 1:
			memory->b_SystemVariable->writeByte(ib_PackedFile_Slot1, packed_index);
			break;
	}

	//TODO: it is only true for DAT based games like nanpa1 or aisimai
	if (!option->is_unpacked) {
		file->initializeDAT(0, memory->b_SystemVariable->queryByte(ib_PackedFile_Slot0));
		file->initializeDAT(1, memory->b_SystemVariable->queryByte(ib_PackedFile_Slot1));
	}

	return RETURN_NORMAL;
}
