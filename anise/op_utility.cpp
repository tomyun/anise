#include "script.h"

SCRIPTCALL Script::op_utility()
{
	parameter = getParameter();
	word type = parameter->get(0);

	OpcodeFunction operation = opcode_oput_list[type].function;
	SCRIPTCALL condition = (this->*operation) ();

	return condition;
}