#include "script.h"

SCRIPTCALL Script::op_callProcedure()
{
	parameter = getParameter();

	word script_offset = getOffset();

	word procedure_index = parameter->get(0) * 2;
	word procedure_offset = memory->b_Procedure->queryWord(procedure_index);
	setOffset(procedure_offset);

	SCRIPTCALL condition = parseNested();

	setOffset(script_offset);

	deleteParameter();

	if ((condition == RETURN_EXIT) || (condition == RETURN_ERROR)) {
		return condition;
	}
	else {
		return RETURN_NORMAL;
	}
}
