#include "script.h"

SCRIPTCALL Script::op4_loadFile()
{
	char filename[PARAMETER_LENGTH];
	word offset;

	parameter = getParameter();
	//HACK: for kakyusei
	if (parameter->getType(0) == PARAMETER_TYPE_CONSTANT) {
		parameter->load(1, filename);
		offset = parameter->get(2);
	}
	else {
		parameter->load(0, filename);
		offset = parameter->get(1);
	}
	deleteParameter();

	MemorySegment *segment = memory->getCurrentSegment();
	MemoryBlock *memory_block = segment->get(&offset);

	file->open(filename);
	file->load(memory_block, offset);
	file->close();

	return RETURN_NORMAL;
}
