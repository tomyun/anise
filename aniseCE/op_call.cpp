#include "script.h"

SCRIPTCALL Script::op_call()
{
	parameter = getParameter();
	word callee_offset = parameter->get(0);
	deleteParameter();

	//TODO: check this out
	MemoryBlock *caller = memory->b_Script;
	word caller_offset = getOffset();

	MemoryBlock *callee = memory->s_Core->get(&callee_offset);
	memory->b_Script = callee;
	setOffset(callee_offset);

	parse();

	memory->b_Script = caller;
	setOffset(caller_offset);

	return RETURN_NORMAL;
}
