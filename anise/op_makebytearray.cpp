#include "script.h"

SCRIPTCALL Script::op_makeByteArray()
{
	word base_index = fetchAdvance();
	base_index = (base_index - CODE_BASEVARIABLE_FIRST) * 2;
	word base = memory->b_BaseVariable->queryWord(base_index);
	word array_index = readExpression();
	array_index = base + array_index;

	MemorySegment *segment = memory->getCurrentSegment();
	MemoryBlock *memory_block = segment->get(&array_index);

	while (true) {
		byte element = (byte) readExpression();
		memory_block->writeByte(array_index, element);

		byte code = fetch();
		if (code == CODE_CONTINUE) {
			array_index++;
			advance();
		}
		else {
			break;
		}
	}

	return RETURN_NORMAL;
}
