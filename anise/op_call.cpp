#include "script.h"

SCRIPTCALL Script::op_call()
{
	parameter = getParameter();
	word callee_offset = parameter->get(0);
	deleteParameter();

	//TODO: check this out
	MemoryBlock *b_Script = memory->b_Script;
	word script_offset = getOffset();

	MemoryBlock *calle = memory->s_Core->get(&callee_offset);
	memory->b_Script = calle;
	setOffset(callee_offset);

	parse();

	memory->b_Script = b_Script;
	setOffset(script_offset);

	return RETURN_NORMAL;
}