#include "script.h"

SCRIPTCALL Script::op4_callScript()
{
	char filename[SCRIPT_NAME_SIZE];

	parameter = getParameter();
	parameter->load(0, filename);
	deleteParameter();

	word script_offset = getOffset();

	MemoryBlock *b_Procedure = new MemoryBlock(0, PROCEDURE_SIZE);
	b_Procedure->writeBlock(memory->b_Procedure, 0);

	char *script_name = memory->b_ScriptName->queryString(0, SCRIPT_NAME_SIZE);
	memory->b_ScriptName->writeString(0, filename, SCRIPT_NAME_SIZE);
	load(filename);

	parse();

	memory->b_Procedure->writeBlock(b_Procedure, 0);
	delete b_Procedure;

	memory->b_ScriptName->writeString(0, script_name, SCRIPT_NAME_SIZE);
	load(script_name);
	delete script_name;

	setOffset(script_offset);

	return RETURN_NORMAL;
}
