#include "script.h"

SCRIPTCALL Script::op4_loadFile()
{
	char filename[PARAMETER_LENGTH];

	parameter = getParameter();
	parameter->load(0, filename);

	word offset = parameter->get(1);
	MemorySegment *segment = memory->getCurrentSegment();
	MemoryBlock *memory_block = segment->get(&offset);

	file->open(filename, FILE_READ);
	file->load(memory_block, offset);
	file->close();

	deleteParameter();

	return RETURN_NORMAL;
}