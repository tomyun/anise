#include "script.h"

SCRIPTCALL Script::op_makeArray()
{
	word base_index = fetchAdvance();

	MemorySegment *segment;
	if (base_index == CODE_BASEVARIABLE_FIRST) {
		segment = memory->s_Core;
	}
	else {
		segment = memory->getCurrentSegment();
	}

	base_index = (base_index - CODE_BASEVARIABLE_FIRST) * 2;
	word base = memory->b_BaseVariable->queryWord(base_index);
	word array_index = readExpression();
	array_index = base + (array_index * 2);

	MemoryBlock *memory_block = segment->get(&array_index);

	while (true) {
		word element = readExpression();
		memory_block->writeWord(array_index, element);

		byte code = fetch();
		if (code == CODE_CONTINUE) {
			array_index += 2;
			advance();
		}
		else {
			break;
		}
	}

	return RETURN_NORMAL;
}