#include "script.h"

SCRIPTCALL Script::op_setBaseVariable()
{
	word base_index = fetchAdvance();
	base_index = (base_index - CODE_BASEVARIABLE_FIRST) * 2;
	word base = readExpression();
	memory->b_BaseVariable->writeWord(base_index, base);

	return RETURN_NORMAL;
}
