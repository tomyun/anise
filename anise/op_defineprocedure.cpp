#include "script.h"

SCRIPTCALL Script::op_defineProcedure()
{
	parameter = getParameter();

	word procedure_index = parameter->get(0) * 2;
	memory->b_Procedure->writeWord(procedure_index, parameter->get(1));

	deleteParameter();

	return RETURN_NORMAL;
}
